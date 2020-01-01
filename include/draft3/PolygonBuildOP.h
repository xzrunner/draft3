#pragma once

#include <ee0/EditOP.h>
#include <ee0/typedef.h>

#include <SM_Vector.h>

#include <vector>

namespace pt3 { class Viewport; }

namespace draft3
{

class PolygonBuildOP : public ee0::EditOP
{
public:
    PolygonBuildOP(const std::shared_ptr<pt0::Camera>& camera,
        const pt3::Viewport& vp, const ee0::SubjectMgrPtr& sub_mgr);

    virtual bool OnMouseLeftDown(int x, int y) override;
    virtual bool OnMouseRightDown(int x, int y) override;
    virtual bool OnMouseMove(int x, int y) override;
    virtual bool OnMouseDrag(int x, int y) override;
    virtual bool OnMouseLeftDClick(int x, int y) override;

    virtual bool OnDraw() const override;
    virtual bool Clear() override;

private:
    const pt3::Viewport& m_vp;
    ee0::SubjectMgrPtr m_sub_mgr;

    std::vector<sm::vec3> m_polyline;
    sm::vec3 m_curr_pos;

    const float m_y = 0.0f;

}; // PolygonBuildOP

}