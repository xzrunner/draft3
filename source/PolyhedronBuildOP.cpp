#include "draft3/PolyhedronBuildOP.h"
#include "draft3/DrawRectFaceState.h"
#include "draft3/DrawPolyFaceState.h"
#include "draft3/FacePushPullState.h"

#include <painting3/PerspCam.h>

#include <wx/defs.h>

namespace draft3
{

PolyhedronBuildOP::PolyhedronBuildOP(const ur::Device& dev, const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp,
	                                 const ee0::SubjectMgrPtr& sub_mgr, const MeshPointQuery::Selected& selected,
	                                 std::function<void()> update_cb)
	: ee0::EditOP(camera)
	, m_vp(vp)
	, m_sub_mgr(sub_mgr)
	, m_selected(selected)
	, m_update_cb(update_cb)
{
	m_draw_rect_state = std::make_shared<DrawRectFaceState>(dev, camera, vp, sub_mgr);
	m_draw_poly_state = std::make_shared<DrawPolyFaceState>(dev, camera, vp, sub_mgr);
	m_default_state = m_draw_poly_state;
//	m_default_state = m_draw_rect_state;
	ChangeEditOpState(m_default_state);
}

bool PolyhedronBuildOP::OnKeyDown(int key_code)
{
	if (ee0::EditOP::OnKeyDown(key_code)) {
		return true;
	}

	if (m_op_state->OnKeyPress(key_code)) {
		return true;
	}

	// create FacePushPullState
	if (key_code == WXK_SHIFT &&
		!m_mouse_pressing &&
		m_camera->TypeID() == pt0::GetCamTypeID<pt3::PerspCam>())
	{
		auto p_cam = std::dynamic_pointer_cast<pt3::PerspCam>(m_camera);
		m_face_pp_state = std::make_shared<FacePushPullState>(
			p_cam, m_vp, m_sub_mgr, m_selected, m_update_cb);
		ChangeEditOpState(m_face_pp_state);
	}

	return false;
}

bool PolyhedronBuildOP::OnKeyUp(int key_code)
{
	if (ee0::EditOP::OnKeyUp(key_code)) {
		return true;
	}

	if (m_op_state->OnKeyRelease(key_code)) {
		return true;
	}

	if (key_code == WXK_SHIFT)
	{
		//if (!std::dynamic_pointer_cast<PolyTranslateState>(m_poly_trans_state)->IsMoveAnyDirection()) {
		//	m_poly_trans_state->OnKeyRelease(key_code);
		//} else {
			m_face_pp_state.reset();
			ChangeEditOpState(m_default_state);
		//}
	}

	return false;
}

bool PolyhedronBuildOP::OnMouseLeftDown(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftDown(x, y)) {
		return true;
	}
	if (m_op_state->OnMousePress(x, y)) {
		return true;
	}

	m_first_pos2.Set(x, y);

	m_mouse_pressing = true;

	return false;
}

bool PolyhedronBuildOP::OnMouseLeftUp(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftUp(x, y)) {
		return true;
	}
	if (m_op_state->OnMouseRelease(x, y)) {
		return true;
	}

	m_mouse_pressing = false;

	return false;
}

bool PolyhedronBuildOP::OnMouseMove(int x, int y)
{
	if (ee0::EditOP::OnMouseMove(x, y)) {
		return true;
	}

	if (m_op_state->OnMouseMove(x, y)) {
		return true;
	}

	return false;
}

bool PolyhedronBuildOP::OnMouseDrag(int x, int y)
{
	if (ee0::EditOP::OnMouseDrag(x, y)) {
		return true;
	}
	if (m_op_state->OnMouseDrag(x, y)) {
		return true;
	}

	if (m_op_state != m_default_state &&
		!m_selected.poly)
	{
		ChangeEditOpState(m_default_state);
		m_op_state->OnMousePress(m_first_pos2.x, m_first_pos2.y);
		m_op_state->OnMouseDrag(x, y);
	}

	return false;
}

bool PolyhedronBuildOP::OnDraw(const ur::Device& dev, ur::Context& ctx) const
{
	if (ee0::EditOP::OnDraw(dev, ctx)) {
		return true;
	}
	if (m_op_state->OnDraw(dev, ctx)) {
		return true;
	}

	return false;
}

}