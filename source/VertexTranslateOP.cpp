#include "drawing3/VertexTranslateOP.h"

#include <polymesh3/Brush.h>
#include <model/Model.h>
#include <model/BrushModel.h>

namespace dw3
{
namespace mesh
{

VertexTranslateOP::VertexTranslateOP(const std::shared_ptr<pt0::Camera>& camera,
	                                 const pt3::Viewport& vp,
	                                 const ee0::SubjectMgrPtr& sub_mgr,
	                                 const MeshPointQuery::Selected& selected,
	                                 const ee0::SelectionSet<pm3::BrushVertexPtr>& selection,
	                                 std::function<void()> update_cb)
	: MeshTranslateBaseOP<pm3::BrushVertexPtr>(camera, vp, sub_mgr, selected, selection, update_cb)
{
}

bool VertexTranslateOP::QueryByPos(const sm::vec2& pos, const pm3::BrushVertexPtr& vert,
	                               const sm::mat4& cam_mat) const
{
	auto p3 = vert->pos * model::BrushBuilder::VERTEX_SCALE;
	auto p2 = m_vp.TransPosProj3ToProj2(p3, cam_mat);
	if (sm::dis_pos_to_pos(p2, pos) < NODE_QUERY_RADIUS) {
		m_last_pos3 = p3;
		return true;
	} else {
		return false;
	}
}

void VertexTranslateOP::TranslateSelected(const sm::vec3& offset)
{
	auto& vertices = m_selected.poly->GetVertices();
	auto _offset = offset / model::BrushBuilder::VERTEX_SCALE;
	m_selection.Traverse([&](const pm3::BrushVertexPtr& vert)->bool
	{
		// update helfedge geo
		for (auto& v : vertices)
		{
			auto d = vert->pos * model::BrushBuilder::VERTEX_SCALE - v->position;
			if (fabs(d.x) < SM_LARGE_EPSILON &&
				fabs(d.y) < SM_LARGE_EPSILON &&
				fabs(d.z) < SM_LARGE_EPSILON) {
				v->position += offset;
				break;
			}
		}

		// update polymesh3 brush
		vert->pos += _offset;

		return true;
	});

	// update helfedge geo
	m_selected.poly->UpdateAABB();

	// update model aabb
    auto brush = m_selected.GetBrush();
    assert(brush);
	sm::cube model_aabb;
	model_aabb.Combine(brush->impl->geometry->GetAABB());
	m_selected.model->aabb = model_aabb;

	// update vbo
	model::BrushBuilder::UpdateVBO(*m_selected.model, *brush->impl, brush->desc);
}

}
}