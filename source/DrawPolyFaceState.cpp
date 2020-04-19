#include "draft3/DrawPolyFaceState.h"
#include "draft3/MeshRayIntersect.h"

#include <ee0/SubjectMgr.h>
#include <ee0/MessageID.h>
#include <ee0/MsgHelper.h>

#include <SM_Ray.h>
#include <SM_RayIntersect.h>
#include <tessellation/Painter.h>
#include <unirender2/RenderState.h>
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

#include <wx/defs.h>

namespace draft3
{

DrawPolyFaceState::DrawPolyFaceState(const ur2::Device& dev,
                                     const std::shared_ptr<pt0::Camera>& camera,
	                                 const pt3::Viewport& vp,
	                                 const ee0::SubjectMgrPtr& sub_mgr)
	: ee0::EditOpState(camera)
    , m_dev(dev)
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
    if (m_camera->TypeID() != pt0::GetCamTypeID<pt3::PerspCam>()) {
        return false;
    }

    auto p_cam = std::dynamic_pointer_cast<pt3::PerspCam>(m_camera);

    sm::vec3 cross;
    if (ray_yplane_intersect(*p_cam, m_vp, x, y, m_y, cross))
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

    if (m_camera->TypeID() != pt0::GetCamTypeID<pt3::PerspCam>()) {
        return false;
    }
    auto p_cam = std::dynamic_pointer_cast<pt3::PerspCam>(m_camera);

    sm::vec3 cross;
    if (ray_yplane_intersect(*p_cam, m_vp, x, y, m_y, cross)) {
        m_polygon.back() = cross;
        m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
    }

	return false;
}

bool DrawPolyFaceState::OnDraw(const ur2::Device& dev, ur2::Context& ctx) const
{
	if (m_polygon.empty()) {
		return false;
	}

	tess::Painter pt;
	auto cam_mat = m_camera->GetProjectionMat() * m_camera->GetViewMat();
	pt.AddPolygon3D(m_polygon.data(), m_polygon.size(), [&](const sm::vec3& pos3)->sm::vec2 {
		return m_vp.TransPosProj3ToProj2(pos3, cam_mat);
	}, 0xffffffff);

    ur2::RenderState rs;
	pt2::RenderSystem::DrawPainter(dev, ctx, rs, pt);

	return false;
}

n0::SceneNodePtr DrawPolyFaceState::CreateModelObj()
{
	if (m_polygon.size() < 4) {
		return nullptr;
	}

	auto polygon = m_polygon;
	polygon.pop_back();

    std::shared_ptr<model::Model> model =
        model::BrushBuilder::PolymeshFromPolygon(m_dev, polygon);

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