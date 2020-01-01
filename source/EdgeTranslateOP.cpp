#include "draft3/EdgeTranslateOP.h"

#include <polymesh3/Polytope.h>
#include <model/Model.h>

namespace draft3
{

EdgeTranslateOP::EdgeTranslateOP(const std::shared_ptr<pt0::Camera>& camera,
	                             const pt3::Viewport& vp,
	                             const ee0::SubjectMgrPtr& sub_mgr,
	                             const MeshPointQuery::Selected& selected,
	                             const ee0::SelectionSet<pm3::Polytope::EdgePtr>& selection,
	                             std::function<void()> update_cb)
	: MeshTranslateBaseOP<pm3::Polytope::EdgePtr>(camera, vp, sub_mgr, selected, selection, update_cb)
{
}

bool EdgeTranslateOP::QueryByPos(const sm::vec2& pos, const pm3::Polytope::EdgePtr& edge,
	                             const sm::mat4& cam_mat) const
{
    auto brush = m_selected.GetBrush();
    if (!brush || !brush->impl) {
        return nullptr;
    }

	auto b3 = brush->impl->Points()[edge->first]->pos;
	auto e3 = brush->impl->Points()[edge->second]->pos;
	auto mid3 = (b3 + e3) * 0.5f;
	auto b2 = m_vp.TransPosProj3ToProj2(b3, cam_mat);
	auto e2 = m_vp.TransPosProj3ToProj2(e3, cam_mat);
	auto mid2 = m_vp.TransPosProj3ToProj2(mid3, cam_mat);
	if (sm::dis_pos_to_pos(mid2, pos) < NODE_QUERY_RADIUS) {
		m_last_pos3 = mid3;
		return true;
	} else {
		return false;
	}
}

void EdgeTranslateOP::TranslateSelected(const sm::vec3& offset)
{
    auto brush = m_selected.GetBrush();
    if (!brush || !brush->impl) {
        return;
    }

	auto& faces = m_selected.poly->GetLoops();
	m_selection.Traverse([&](const pm3::Polytope::EdgePtr& edge)->bool
	{
		// update helfedge geo
        auto f = faces.Head();
        do {
			auto start = f->edge;
			auto curr = start;
			do {
				auto d0 = brush->impl->Points()[edge->first]->pos - curr->vert->position;
				if (fabs(d0.x) < SM_LARGE_EPSILON &&
					fabs(d0.y) < SM_LARGE_EPSILON &&
					fabs(d0.z) < SM_LARGE_EPSILON) {
					curr->vert->position += offset;
					break;
				}
                auto d1 = brush->impl->Points()[edge->second]->pos - curr->vert->position;
                if (fabs(d1.x) < SM_LARGE_EPSILON &&
                    fabs(d1.y) < SM_LARGE_EPSILON &&
                    fabs(d1.z) < SM_LARGE_EPSILON) {
                    curr->vert->position += offset;
                    break;
                }
				curr = curr->next;
			} while (curr != start);

            f = f->linked_next;
        } while (f != faces.Head());

		// update polymesh3 brush
        brush->impl->Points()[edge->first]->pos  += offset;
        brush->impl->Points()[edge->second]->pos += offset;

		return true;
	});

	// update helfedge geo
	m_selected.poly->UpdateAABB();

	// update model aabb
	sm::cube model_aabb;
	model_aabb.Combine(brush->impl->GetTopoPoly()->GetAABB());
	m_selected.model->aabb = model_aabb;

	// update vbo
	model::BrushBuilder::UpdateVBO(*m_selected.model, *brush);
}

}