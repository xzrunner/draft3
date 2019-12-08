#include "drawing3/FaceSelectOP.h"

#include <polymesh3/Polytope.h>
#include <tessellation/Painter.h>
#include <painting2/OrthoCamera.h>
#include <painting2/RenderSystem.h>

namespace dw3
{
namespace mesh
{

FaceSelectOP::FaceSelectOP(const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp,
	                       const ee0::SubjectMgrPtr& sub_mgr,
	                       const MeshPointQuery::Selected& selected)
	: MeshSelectBaseOP<pm3::Polytope::FacePtr>(camera, vp, sub_mgr, selected)
{
	m_selecting = nullptr;
}

void FaceSelectOP::DrawImpl(const pm3::Polytope& poly, const sm::mat4& cam_mat) const
{
	tess::Painter pt;
	// all nodes
	for (auto& f : poly.Faces())
	{
		auto center = CalcFaceCenter(*f, cam_mat);
		DrawFace(pt, *f, LIGHT_UNSELECT_COLOR, cam_mat);
		pt.AddCircleFilled(center, NODE_DRAW_RADIUS, UNSELECT_COLOR);
	}
	// selecting
	if (m_selecting)
	{
		auto center = CalcFaceCenter(*m_selecting, cam_mat);
		pt.AddCircle(center, NODE_QUERY_RADIUS, SELECT_COLOR);
	}
	// selected
	m_selected.Traverse([&](const pm3::Polytope::FacePtr& face)->bool
	{
		DrawFace(pt, *face, LIGHT_SELECT_COLOR, cam_mat);
		auto center = CalcFaceCenter(*face, cam_mat);
		pt.AddCircleFilled(center, NODE_DRAW_RADIUS, SELECT_COLOR);
		return true;
	});
	pt2::RenderSystem::DrawPainter(pt);
}

pm3::Polytope::FacePtr FaceSelectOP::QueryByPos(int x, int y) const
{
	auto brush = m_base_selected.GetBrush();
    if (!brush || !brush->impl) {
		return nullptr;
	}

	auto pos = m_cam2d->TransPosScreenToProject(x, y,
		static_cast<int>(m_vp.Width()), static_cast<int>(m_vp.Height()));

	auto cam_mat = m_camera->GetProjectionMat() * m_camera->GetViewMat();
	for (auto& f : brush->impl->Faces()) {
		auto center = CalcFaceCenter(*f, cam_mat);
		if (sm::dis_pos_to_pos(center, pos) < NODE_QUERY_RADIUS) {
			return f;
		}
	}

	return nullptr;
}

void FaceSelectOP::QueryByRect(const sm::irect& rect, std::vector<pm3::Polytope::FacePtr>& selection) const
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
	for (auto& f : brush->impl->Faces()) {
		auto center = CalcFaceCenter(*f, cam_mat);
		if (sm::is_point_in_rect(center, s_rect)) {
			selection.push_back(f);
		}
	}
}

sm::vec2 FaceSelectOP::CalcFaceCenter(const pm3::Polytope::Face& face, const sm::mat4& cam_mat) const
{
    auto brush = m_base_selected.GetBrush();
    if (!brush || !brush->impl) {
        return sm::vec2(0, 0);
    }

	if (face.points.empty()) {
		return sm::vec2(0, 0);
	}

	sm::vec3 center;
	for (auto& v : face.points) {
		center += brush->impl->Points()[v]->pos;
	}
	center /= face.points.size();

	return m_vp.TransPosProj3ToProj2(center, cam_mat);
}

void FaceSelectOP::DrawFace(tess::Painter& pt, const pm3::Polytope::Face& face, uint32_t color, const sm::mat4& cam_mat) const
{
    auto brush = m_base_selected.GetBrush();
    if (!brush || !brush->impl) {
        return;
    }

	std::vector<sm::vec2> polygon;
	polygon.reserve(face.points.size());
	for (auto& v : face.points) {
		auto p3 = brush->impl->Points()[v]->pos;
		polygon.push_back(m_vp.TransPosProj3ToProj2(p3, cam_mat));
	}
	pt.AddPolygonFilled(polygon.data(), polygon.size(), color);
}

}
}