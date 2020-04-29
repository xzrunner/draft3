#include "draft3/PolygonSelectOP.h"
#include "draft3/MeshEditStyle.h"

#include <ee0/WxStagePage.h>
#include <ee0/MsgHelper.h>
#include <ee0/SubjectMgr.h>

#include <SM_Ray.h>
#include <SM_RayIntersect.h>
#include <tessellation/Painter.h>
#include <unirender/RenderState.h>
#include <painting0/Camera.h>
#include <painting2/RenderSystem.h>
#include <painting3/Viewport.h>
#include <painting3/PerspCam.h>
#include <node0/SceneNode.h>
#include <node3/CompShape.h>
#include <geoshape/Polygon3D.h>

namespace draft3
{

PolygonSelectOP::PolygonSelectOP(const std::shared_ptr<pt0::Camera>& camera,
                                 ee0::WxStagePage& stage, const pt3::Viewport& vp)
    : ee0::EditOP(camera)
    , m_stage(stage)
    , m_vp(vp)
    , m_sub_mgr(stage.GetSubjectMgr())
{
}

bool PolygonSelectOP::OnMouseLeftDown(int x, int y)
{
    if (ee0::EditOP::OnMouseLeftDown(x, y)) {
        return true;
    }

    m_sub_mgr->NotifyObservers(ee0::MSG_NODE_SELECTION_CLEAR);

    if (m_camera->TypeID() == pt0::GetCamTypeID<pt3::PerspCam>())
    {
        auto p_cam = std::dynamic_pointer_cast<pt3::PerspCam>(m_camera);

        sm::vec3 ray_dir = m_vp.TransPos3ScreenToDir(
            sm::vec2(static_cast<float>(x), static_cast<float>(y)), *p_cam);
        sm::Ray ray(p_cam->GetPos(), ray_dir);
        m_stage.Traverse([&](const ee0::GameObj& obj)->bool
        {
            if (!obj->HasUniqueComp<n3::CompShape>()) {
                return true;
            }

            auto& cshape = obj->GetUniqueComp<n3::CompShape>();
            for (auto& shape : cshape.GetShapes())
            {
                if (shape->get_type() != rttr::type::get<gs::Polygon3D>()) {
                    continue;
                }

                auto polygon = std::static_pointer_cast<gs::Polygon3D>(shape);
                auto& verts = polygon->GetVertices();
                sm::vec3 cross;
                if (sm::ray_polygon_intersect_both_faces(sm::mat4(), verts.data(), verts.size(), ray, &cross))
                {
                    // insert to selection set
                    std::vector<n0::NodeWithPos> nwps;
                    nwps.push_back(n0::NodeWithPos(obj, obj, 0));
                    ee0::MsgHelper::InsertSelection(*m_sub_mgr, nwps);
                    break;
                }
            }

            return true;
        });
    }

    return false;
}

bool PolygonSelectOP::OnDraw(const ur::Device& dev, ur::Context& ctx) const
{
    if (ee0::EditOP::OnDraw(dev, ctx)) {
        return true;
    }

	tess::Painter pt;

	auto cam_mat = m_camera->GetProjectionMat() * m_camera->GetViewMat();
	m_stage.GetSelection().Traverse([&](const ee0::GameObjWithPos& nwp)->bool
	{
        if (!nwp.GetNode()->HasUniqueComp<n3::CompShape>()) {
            return true;
        }

        auto& cshape = nwp.GetNode()->GetUniqueComp<n3::CompShape>();
        for (auto& shape : cshape.GetShapes())
        {
            if (shape->get_type() != rttr::type::get<gs::Polygon3D>()) {
                continue;
            }

            auto polygon = std::static_pointer_cast<gs::Polygon3D>(shape);
            auto& verts = polygon->GetVertices();
            auto cam_mat = m_camera->GetProjectionMat() * m_camera->GetViewMat();
            pt.AddPolygonFilled3D(verts.data(), verts.size(), [&](const sm::vec3& pos3)->sm::vec2 {
                return m_vp.TransPosProj3ToProj2(pos3, cam_mat);
            }, LIGHT_SELECT_COLOR);
        }

		return true;
	});

    ur::RenderState rs;
	pt2::RenderSystem::DrawPainter(dev, ctx, rs, pt);

    return false;
}

}