#include "drawing3/EdgeSelectOP.h"

#include <tessellation/Painter.h>
#include <painting2/RenderSystem.h>

namespace dw3
{
namespace mesh
{

EdgeSelectOP::EdgeSelectOP(const std::shared_ptr<pt0::Camera>& camera,
	                       const pt3::Viewport& vp,
	                       const ee0::SubjectMgrPtr& sub_mgr,
	                       const MeshPointQuery::Selected& selected)
	: MeshSelectBaseOP<BrushEdge>(camera, vp, sub_mgr, selected)
{
}

void EdgeSelectOP::DrawImpl(const pm3::Brush& brush, const sm::mat4& cam_mat) const
{
	tess::Painter pt;

	// all edges
	for (auto& face : brush.faces)
	{
		auto& vs = face->vertices;
		for (int i = 0, n = vs.size(); i < n; ++i) {
			BrushEdge(vs[i], vs[(i + 1) % n]).Project(m_vp, cam_mat,
				[&](const sm::vec2& b, const sm::vec2& e, const sm::vec2& mid) {
				pt.AddLine(b, e, UNSELECT_COLOR);
				pt.AddCircleFilled(mid, NODE_DRAW_RADIUS, UNSELECT_COLOR);
			});
		}
	}
	// selecting
	if (m_selecting)
	{
		m_selecting.Project(m_vp, cam_mat,
			[&](const sm::vec2& b, const sm::vec2& e, const sm::vec2& mid) {
			pt.AddLine(b, e, SELECT_COLOR);
			pt.AddCircle(mid, NODE_QUERY_RADIUS, SELECT_COLOR);
		});
	}
	// selected
	m_selected.Traverse([&](const BrushEdge& edge)->bool {
		edge.Project(m_vp, cam_mat,
			[&](const sm::vec2& b, const sm::vec2& e, const sm::vec2& mid) {
			pt.AddLine(b, e, SELECT_COLOR);
			pt.AddCircleFilled(mid, NODE_DRAW_RADIUS, SELECT_COLOR);
		});
		return true;
	});

	pt2::RenderSystem::DrawPainter(pt);
}

BrushEdge EdgeSelectOP::QueryByPos(int x, int y) const
{
	auto brush = m_base_selected.GetBrush();
	if (!brush) {
		return BrushEdge();
	}

	auto pos = m_cam2d->TransPosScreenToProject(x, y,
		static_cast<int>(m_vp.Width()), static_cast<int>(m_vp.Height()));

	auto cam_mat = m_camera->GetProjectionMat() * m_camera->GetViewMat();
	for (auto& face : brush->faces)
	{
		auto& vs = face->vertices;
		for (int i = 0, n = vs.size(); i < n; ++i)
		{
			BrushEdge edge;
			BrushEdge(vs[i], vs[(i + 1) % n]).Project(m_vp, cam_mat,
				[&](const sm::vec2& b, const sm::vec2& e, const sm::vec2& mid) {
				if (sm::dis_pos_to_pos(mid, pos) < NODE_QUERY_RADIUS) {
					edge = BrushEdge(vs[i], vs[(i + 1) % n]);
				}
			});
			if (edge) {
				return edge;
			}
		}
	}

	return BrushEdge();
}

void EdgeSelectOP::QueryByRect(const sm::irect& rect, std::vector<BrushEdge>& selection) const
{
	auto brush = m_base_selected.GetBrush();
	if (!brush) {
		return;
	}

	auto r_min = m_cam2d->TransPosScreenToProject(rect.xmin, rect.ymin,
		static_cast<int>(m_vp.Width()), static_cast<int>(m_vp.Height()));
	auto r_max = m_cam2d->TransPosScreenToProject(rect.xmax, rect.ymax,
		static_cast<int>(m_vp.Width()), static_cast<int>(m_vp.Height()));
	sm::rect s_rect(r_min, r_max);

	auto cam_mat = m_camera->GetProjectionMat() * m_camera->GetViewMat();
	for (auto& face : brush->faces)
	{
		auto& vs = face->vertices;
		for (int i = 0, n = vs.size(); i < n; ++i)
		{
			BrushEdge(vs[i], vs[(i + 1) % n]).Project(m_vp, cam_mat,
				[&](const sm::vec2& b, const sm::vec2& e, const sm::vec2& mid) {
				if (sm::is_point_in_rect(mid, s_rect)) {
					selection.push_back(BrushEdge(vs[i], vs[(i + 1) % n]));
				}
			});
		}
	}
}

}
}