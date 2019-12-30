#include "draft3/EditPolylineOP.h"
#include "draft3/MeshRayIntersect.h"

#include <ee0/MessageID.h>
#include <ee0/SubjectMgr.h>
#include <ee0/MsgHelper.h>

#include <geoshape/Polyline3D.h>
#include <painting2/RenderSystem.h>
#include <painting3/PerspCam.h>
#include <painting3/Viewport.h>
#include <node0/SceneNode.h>
#include <node3/CompShape.h>
#include <ns/NodeFactory.h>
#include <tessellation/Painter.h>

namespace draft3
{

EditPolylineOP::EditPolylineOP(const std::shared_ptr<pt0::Camera>& camera,
                               const pt3::Viewport& vp, const ee0::SubjectMgrPtr& sub_mgr)
    : ee0::EditOP(camera)
    , m_vp(vp)
    , m_sub_mgr(sub_mgr)
{
}

bool EditPolylineOP::OnMouseLeftDown(int x, int y)
{
    if (ee0::EditOP::OnMouseLeftDown(x, y)) {
        return true;
    }

    if (m_camera->TypeID() != pt0::GetCamTypeID<pt3::PerspCam>()) {
        return false;
    }

    auto p_cam = std::dynamic_pointer_cast<pt3::PerspCam>(m_camera);

    sm::vec3 cross;
    if (mesh::ray_yplane_intersect(*p_cam, m_vp, x, y, m_y, cross))
    {
        if (m_polyline.empty()) {
            m_polyline.push_back(cross);
        }
        m_polyline.push_back(cross);
    }

	return false;
}

bool EditPolylineOP::OnMouseRightDown(int x, int y)
{
    if (ee0::EditOP::OnMouseRightDown(x, y)) {
        return true;
    }

    if (m_polyline.size() > 1) {
        m_polyline[m_polyline.size() - 2] = m_polyline.back();
        m_polyline.pop_back();
    }

    return false;
}

bool EditPolylineOP::OnMouseMove(int x, int y)
{
    if (ee0::EditOP::OnMouseMove(x, y)) {
        return true;
    }

	if (m_polyline.empty()) {
		return false;
	}

    if (m_camera->TypeID() != pt0::GetCamTypeID<pt3::PerspCam>()) {
        return false;
    }
    auto p_cam = std::dynamic_pointer_cast<pt3::PerspCam>(m_camera);

    sm::vec3 cross;
    if (mesh::ray_yplane_intersect(*p_cam, m_vp, x, y, m_y, cross)) {
        m_polyline.back() = cross;
        m_sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
    }

	return false;
}

bool EditPolylineOP::OnMouseDrag(int x, int y)
{
    if (ee0::EditOP::OnMouseDrag(x, y)) {
        return true;
    }

    return false;
}

bool EditPolylineOP::OnMouseLeftDClick(int x, int y)
{
    if (ee0::EditOP::OnMouseLeftDClick(x, y)) {
        return true;
    }

	if (m_polyline.size() < 4) {
		return false;
	}

    m_polyline.pop_back();

    auto node = ns::NodeFactory::Create3D();
    assert(node);
    auto& cshape = node->AddSharedComp<n3::CompShape>();
    cshape.SetShapes({ std::make_shared<gs::Polyline3D>(m_polyline, true) });
    ee0::MsgHelper::InsertNode(*m_sub_mgr, node, true);

    m_polyline.clear();

    return false;
}

bool EditPolylineOP::OnDraw() const
{
    if (ee0::EditOP::OnDraw()) {
        return true;
    }

	if (m_polyline.empty()) {
		return false;
	}

	tess::Painter pt;
	auto cam_mat = m_camera->GetProjectionMat() * m_camera->GetViewMat();
	pt.AddPolygon3D(m_polyline.data(), m_polyline.size(), [&](const sm::vec3& pos3)->sm::vec2 {
		return m_vp.TransPosProj3ToProj2(pos3, cam_mat);
	}, 0xffffffff);
	pt2::RenderSystem::DrawPainter(pt);

	return false;
}

bool EditPolylineOP::Clear()
{
    if (ee0::EditOP::Clear()) {
        return true;
    }

    m_polyline.clear();
    m_curr_pos.MakeInvalid();

    return false;
}

}