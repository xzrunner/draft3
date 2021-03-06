#include "draft3/DrawRectFaceState.h"

#include <ee0/SubjectMgr.h>
#include <ee0/MessageID.h>
#include <ee0/MsgHelper.h>

#include <SM_Ray.h>
#include <SM_RayIntersect.h>
#include <tessellation/Painter.h>
#include <unirender/RenderState.h>
#include <painting2/RenderSystem.h>
#include <painting3/PerspCam.h>
#include <painting3/Viewport.h>
#include <model/Model.h>
#include <model/BrushBuilder.h>
#include <node0/SceneNode.h>
#include <node0/CompIdentity.h>
#include <node3/CompModel.h>
#include <node3/CompModelInst.h>
#include <node3/CompTransform.h>
#include <node3/CompAABB.h>

namespace draft3
{

DrawRectFaceState::DrawRectFaceState(const ur::Device& dev,
                                     const std::shared_ptr<pt0::Camera>& camera,
	                                 const pt3::Viewport& vp,
	                                 const ee0::SubjectMgrPtr& sub_mgr)
	: ee0::EditOpState(camera)
    , m_dev(dev)
	, m_vp(vp)
	, m_sub_mgr(sub_mgr)
	, m_y(0)
{
	m_first_pos.MakeInvalid();
}

bool DrawRectFaceState::OnMousePress(int x, int y)
{
	sm::vec3 cross;
	if (RayPlaneIntersect(x, y, m_y, cross)) {
		m_first_pos = cross;
	}

	return false;
}

bool DrawRectFaceState::OnMouseRelease(int x, int y)
{
	if (m_first_pos.IsValid() && m_last_pos.IsValid()) {
		ee0::MsgHelper::InsertNode(*m_sub_mgr, CreateModelObj(), true);
	}

	m_first_pos.MakeInvalid();
	m_last_pos.MakeInvalid();

	return false;
}

bool DrawRectFaceState::OnMouseDrag(int x, int y)
{
	sm::vec3 cross;
	if (RayPlaneIntersect(x, y, m_y, cross)) {
		m_last_pos = cross;
		m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
	}

	return false;
}

bool DrawRectFaceState::OnDraw(const ur::Device& dev, ur::Context& ctx) const
{
	if (m_first_pos.IsValid() && m_last_pos.IsValid())
	{
		tess::Painter pt;
		auto cam_mat = m_camera->GetProjectionMat() * m_camera->GetViewMat();
		pt.AddCube(sm::cube(m_first_pos, m_last_pos), [&](const sm::vec3& pos3)->sm::vec2 {
			return m_vp.TransPosProj3ToProj2(pos3, cam_mat);
		}, 0xffffffff);

        ur::RenderState rs;
		pt2::RenderSystem::DrawPainter(dev, ctx, rs, pt);
	}
	return false;
}

bool DrawRectFaceState::RayPlaneIntersect(int x, int y, float plane_y, sm::vec3& cross) const
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

n0::SceneNodePtr DrawRectFaceState::CreateModelObj()
{
	std::vector<sm::vec3> face;
	face.reserve(4);
	face.push_back(m_first_pos);
	face.push_back({ m_last_pos.x, m_y, m_first_pos.z });
	face.push_back(m_last_pos);
	face.push_back({ m_first_pos.x, m_y, m_last_pos.z });

    std::shared_ptr<model::Model> model =
        model::BrushBuilder::PolymeshFromPolygon(m_dev, face);

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