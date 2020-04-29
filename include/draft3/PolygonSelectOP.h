#pragma once

#include <ee0/EditOP.h>
#include <ee0/typedef.h>

namespace ee0 { class WxStagePage; }
namespace pt3 { class Viewport; }

namespace draft3
{

class PolygonSelectOP : public ee0::EditOP
{
public:
    PolygonSelectOP(const std::shared_ptr<pt0::Camera>& camera,
        ee0::WxStagePage& stage, const pt3::Viewport& vp);

    virtual bool OnMouseLeftDown(int x, int y) override;

    virtual bool OnDraw(const ur::Device& dev, ur::Context& ctx) const override;

private:
    ee0::WxStagePage&    m_stage;

    const pt3::Viewport& m_vp;
    ee0::SubjectMgrPtr   m_sub_mgr;

}; // PolygonSelectOP

}