#include "draft3/VertexSelectOP.h"

#include <tessellation/Painter.h>
#include <polymesh3/Polytope.h>
#include <unirender/RenderState.h>
#include <painting2/OrthoCamera.h>
#include <painting2/RenderSystem.h>

namespace draft3
{

VertexSelectOP::VertexSelectOP(const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp,
	                           const ee0::SubjectMgrPtr& sub_mgr,
	                           const MeshPointQuery::Selected& selected)
	: MeshSelectBaseOP<pm3::Polytope::PointPtr>(camera, vp, sub_mgr, selected)
{
	m_selecting = nullptr;
}

void VertexSelectOP::DrawImpl(const ur::Device& dev, ur::Context& ctx,
                              const pm3::Polytope& poly, const sm::mat4& cam_mat) const
{
	tess::Painter pt;

	// all nodes
	for (auto& p : poly.Points()) {
		auto pos = m_vp.TransPosProj3ToProj2(p->pos, cam_mat);
		pt.AddCircleFilled(pos, NODE_DRAW_RADIUS, UNSELECT_COLOR);
	}
	// selecting
	if (m_selecting)
	{
		auto pos = m_vp.TransPosProj3ToProj2(m_selecting->pos, cam_mat);
		pt.AddCircle(pos, NODE_QUERY_RADIUS, SELECT_COLOR);
	}
	// selected
	m_selected.Traverse([&](const pm3::Polytope::PointPtr& vert)->bool {
		auto pos = m_vp.TransPosProj3ToProj2(vert->pos, cam_mat);
		pt.AddCircleFilled(pos, NODE_DRAW_RADIUS, SELECT_COLOR);
		return true;
	});

    ur::RenderState rs;
	pt2::RenderSystem::DrawPainter(dev, ctx, rs, pt);
}

pm3::Polytope::PointPtr VertexSelectOP::QueryByPos(int x, int y) const
{
	auto brush = m_base_selected.GetBrush();
	if (!brush || !brush->impl) {
		return nullptr;
	}

	auto pos = m_cam2d->TransPosScreenToProject(x, y,
		static_cast<int>(m_vp.Width()), static_cast<int>(m_vp.Height()));

	auto cam_mat = m_camera->GetProjectionMat() * m_camera->GetViewMat();

    auto& points = brush->impl->Points();
    for (int i = 0, n = points.size(); i < n; ++i)
    {
        auto p = m_vp.TransPosProj3ToProj2(points[i]->pos, cam_mat);
        if (sm::dis_pos_to_pos(p, pos) < NODE_QUERY_RADIUS) {
            return points[i];
        }
    }

	return nullptr;
}

void VertexSelectOP::QueryByRect(const sm::irect& rect, std::vector<pm3::Polytope::PointPtr>& selection) const
{
	auto brush = m_base_selected.GetBrush();
    if (!brush || !brush->impl) {
		return;
	}

	auto r_min = m_cam2d->TransPosScreenToProject(rect.xmin, rect.ymin,
		static_cast<int>(m_vp.Width()), static_cast<int>(m_vp.Height()));
	auto r_max = m_cam2d->TransPosScreenToProject(rect.xmax, rect.ymax,
		static_cast<int>(m_vp.Width()), static_cast<int>(m_vp.Height()));
	sm::rect s_rect(r_min, r_max);

	auto cam_mat = m_camera->GetProjectionMat() * m_camera->GetViewMat();
    auto& points = brush->impl->Points();
    for (int i = 0, n = points.size(); i < n; ++i)
    {
		auto p = m_vp.TransPosProj3ToProj2(points[i]->pos, cam_mat);
        if (sm::is_point_in_rect(p, s_rect)) {
			selection.push_back(points[i]);
		}
	}
}

}