#include "drawing3/DrawPolyFaceState.h"

#include <ee0/SubjectMgr.h>
#include <ee0/MessageID.h>
#include <ee0/MsgHelper.h>

#include <SM_Ray.h>
#include <SM_RayIntersect.h>
#include <tessellation/Painter.h>
#include <painting2/RenderSystem.h>
#include <painting3/PerspCam.h>
#include <painting3/Viewport.h>
#include <model/Model.h>
#include <model/QuakeMapEntity.h>
#include <model/MapBuilder.h>
#include <node0/SceneNode.h>
#include <node0/CompIdentity.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <node3/CompTransform.h>
#include <node3/CompAABB.h>

#include <wx/defs.h>

namespace dw3
{
namespace mesh
{

DrawPolyFaceState::DrawPolyFaceState(const std::shared_ptr<pt0::Camera>& camera,
	                           const pt3::Viewport& vp,
	                           const ee0::SubjectMgrPtr& sub_mgr)
	: ee0::EditOpState(camera)
	, m_vp(vp)
	, m_sub_mgr(sub_mgr)
	, m_y(0)
{
}

bool DrawPolyFaceState::OnKeyPress(int key_code)
{
	switch (key_code)
	{
	case WXK_ESCAPE:
	{
		auto obj = CreateModelObj();
		if (obj) {
			ee0::MsgHelper::InsertNode(*m_sub_mgr, obj, true);
		}
		m_polygon.clear();
	}
		break;
	}

	return false;
}

bool DrawPolyFaceState::OnMousePress(int x, int y)
{
	sm::vec3 cross;
	if (RayPlaneIntersect(x, y, m_y, cross))
	{
		if (m_polygon.empty()) {
			m_polygon.push_back(cross);
		}
		m_polygon.push_back(cross);
	}

	return false;
}

bool DrawPolyFaceState::OnMouseMove(int x, int y)
{
	if (m_polygon.empty()) {
		return false;
	}

	sm::vec3 cross;
	if (RayPlaneIntersect(x, y, m_y, cross)) {
		m_polygon.back() = cross;
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	}

	return false;
}

bool DrawPolyFaceState::OnDraw() const
{
	if (m_polygon.empty()) {
		return false;
	}

	tess::Painter pt;
	auto cam_mat = m_camera->GetViewMat() * m_camera->GetProjectionMat();
	pt.AddPolygon3D(m_polygon.data(), m_polygon.size(), [&](const sm::vec3& pos3)->sm::vec2 {
		return m_vp.TransPosProj3ToProj2(pos3, cam_mat);
	}, 0xffffffff);
	pt2::RenderSystem::DrawPainter(pt);

	return false;
}

bool DrawPolyFaceState::RayPlaneIntersect(int x, int y, float plane_y, sm::vec3& cross) const
{
	if (m_camera->TypeID() != pt0::GetCamTypeID<pt3::PerspCam>()) {
		return false;
	}

	auto p_cam = std::dynamic_pointer_cast<pt3::PerspCam>(m_camera);

	sm::vec3 ray_dir = m_vp.TransPos3ScreenToDir(
		sm::vec2(static_cast<float>(x), static_cast<float>(y)), *p_cam);
	sm::Ray ray(p_cam->GetPos(), ray_dir);

	sm::Plane plane;
	if (ray_dir.y < 0) {
		plane.Build(sm::vec3(0, 1, 0), -plane_y);
	} else {
		plane.Build(sm::vec3(0, -1, 0), plane_y);
	}

	return sm::ray_plane_intersect(ray, plane, &cross);
}

n0::SceneNodePtr DrawPolyFaceState::CreateModelObj()
{
	if (m_polygon.size() < 4) {
		return nullptr;
	}

	auto polygon = m_polygon;
	polygon.pop_back();

	auto model = model::MapBuilder::Create(polygon);

	auto node = std::make_shared<n0::SceneNode>();

	// model
	auto& cmodel = node->AddSharedComp<n3::CompModel>();
	cmodel.SetModel(model);
	node->AddUniqueComp<n3::CompModelInst>(model, 0);

	// transform
	node->AddUniqueComp<n3::CompTransform>();

	// aabb
	node->AddUniqueComp<n3::CompAABB>(pt3::AABB(model->aabb));

	// id
	node->AddUniqueComp<n0::CompIdentity>();

	return node;
}

}
}