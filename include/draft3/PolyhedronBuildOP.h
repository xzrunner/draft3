#pragma once

#include "draft3/MeshPointQuery.h"

#include <ee0/EditOP.h>
#include <ee0/typedef.h>

namespace pt3 { class Viewport; }

namespace draft3
{

class PolyhedronBuildOP : public ee0::EditOP
{
public:
	PolyhedronBuildOP(const ur::Device& dev, const std::shared_ptr<pt0::Camera>& camera,
        const pt3::Viewport& vp, const ee0::SubjectMgrPtr& sub_mgr,
        const MeshPointQuery::Selected& selected, std::function<void()> update_cb);

	virtual bool OnKeyDown(int key_code) override;
	virtual bool OnKeyUp(int key_code) override;

	virtual bool OnMouseLeftDown(int x, int y) override;
	virtual bool OnMouseLeftUp(int x, int y) override;
	virtual bool OnMouseMove(int x, int y) override;
	virtual bool OnMouseDrag(int x, int y) override;

	virtual bool OnDraw(const ur::Device& dev, ur::Context& ctx) const override;

private:
	const pt3::Viewport& m_vp;
	ee0::SubjectMgrPtr   m_sub_mgr;

	const MeshPointQuery::Selected& m_selected;

	std::function<void()> m_update_cb;

	ee0::EditOpStatePtr m_default_state   = nullptr;

	ee0::EditOpStatePtr m_face_pp_state   = nullptr;
	ee0::EditOpStatePtr m_draw_rect_state = nullptr;
	ee0::EditOpStatePtr m_draw_poly_state = nullptr;

	sm::ivec2 m_first_pos2;

	bool m_mouse_pressing = false;

}; // PolyhedronBuildOP

}