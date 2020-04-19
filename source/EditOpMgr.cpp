#include "draft3/EditOpMgr.h"
#include "draft3/VertexSelectOP.h"
#include "draft3/VertexTranslateOP.h"
#include "draft3/EdgeSelectOP.h"
#include "draft3/EdgeTranslateOP.h"
#include "draft3/FaceSelectOP.h"
#include "draft3/FaceTranslateOP.h"
#include "draft3/PolyhedronSelectOP.h"
#include "draft3/PolyhedronArrangeOP.h"
#include "draft3/PolyhedronBuildOP.h"

#include <ee0/WxStagePage.h>
#include <ee0/WxStageCanvas.h>

#include <ee3/CameraMgr.h>
#include <ee3/MessageID.h>
#include <ee3/NodeRotateOP.h>
#include <ee3/NodeTranslateOP.h>
#include <ee3/NodeSelectOP.h>
#include <ee3/NodeArrangeOP.h>
#include <ee3/CameraDriveOP.h>
#include <ee3/CameraFlyOP.h>

#if defined(__WXGTK__)
   #define HIDE_CURSOR wxSetCursor(wxCURSOR_BLANK)
   #define SHOW_CURSOR wxSetCursor(*wxSTANDARD_CURSOR)
#elif defined(__WXMSW__)
   #define HIDE_CURSOR ShowCursor(0)
   #define SHOW_CURSOR ShowCursor(1)
#endif

namespace draft3
{

EditOpMgr::EditOpMgr(ee0::WxStagePage& m_stage)
	: m_stage(m_stage)
{
}

void EditOpMgr::Init(const ee3::CameraMgr& cam_mgr,
	                 const std::shared_ptr<pt0::Camera>& cam,
	                 const pt3::Viewport& vp)
{
	auto sub_mgr = m_stage.GetSubjectMgr();

	auto& cam3d = cam_mgr.GetCamera(ee3::CameraMgr::CAM_3D);
	m_cam_drive_op = std::make_shared<ee3::CameraDriveOP>(cam3d, vp, sub_mgr);
	m_cam_fly_op   = std::make_shared<ee3::CameraFlyOP>(&m_stage, cam3d, sub_mgr);
	m_camera_op = m_cam_drive_op;

	auto select_op = std::make_shared<PolyhedronSelectOP>(cam, m_stage, vp);
	m_select_op = select_op;
	auto& selected = select_op->GetSelected();
	std::function<void()> update_cb = [select_op]() {
		select_op->UpdateCachedPolyBorder();
	};

    auto& dev = m_stage.GetImpl().GetCanvas()->GetRenderDevice();

	// arrange op with select, default
	m_arrange_op = std::make_shared<PolyhedronArrangeOP>(cam, vp, sub_mgr, selected, update_cb);
	m_arrange_op->SetPrevEditOP(m_select_op);
	m_draw_op = std::make_shared<PolyhedronBuildOP>(dev, cam, vp, sub_mgr, selected, update_cb);
	m_draw_op->SetPrevEditOP(m_select_op);

	// rotate
	m_rotate_op = std::make_shared<ee3::NodeRotateOP>(cam, m_stage, vp);
	// translate
	m_translate_op = std::make_shared<ee3::NodeTranslateOP>(cam, m_stage, vp);
	// vertex
	auto select_vert_op = std::make_shared<VertexSelectOP>(cam, vp, sub_mgr, selected);
	select_vert_op->SetPrevEditOP(m_select_op);
	m_vertex_op = std::make_shared<VertexTranslateOP>(
		cam, vp, sub_mgr, selected, select_vert_op->GetSelected(), update_cb);
	m_vertex_op->SetPrevEditOP(select_vert_op);
	// edge
	auto select_edge_op = std::make_shared<EdgeSelectOP>(cam, vp, sub_mgr, selected);
	select_edge_op->SetPrevEditOP(m_select_op);
	m_edge_op = std::make_shared<EdgeTranslateOP>(
		cam, vp, sub_mgr, selected, select_edge_op->GetSelected(), update_cb);
	m_edge_op->SetPrevEditOP(select_edge_op);
	// face
	auto select_face_op = std::make_shared<FaceSelectOP>(cam, vp, sub_mgr, selected);
	select_face_op->SetPrevEditOP(m_select_op);
	m_face_op = std::make_shared<FaceTranslateOP>(
		cam, vp, sub_mgr, selected, select_face_op->GetSelected(), update_cb);
	m_face_op->SetPrevEditOP(select_face_op);

	ChangedCamOP(m_camera_op);

	auto& impl = m_stage.GetImpl();
	SetCurrOp(Operator::ARRANGE);
	impl.SetOnKeyDownFunc([&, sub_mgr](int key_code)
	{
		auto select_op = std::dynamic_pointer_cast<PolyhedronSelectOP>(m_select_op);
		switch (key_code)
		{
		case 'R':
			if (!m_stage.GetSelection().IsEmpty()) {
				select_op->SetCanSelectNull(false);
				SetCurrOp(Operator::ROTATE);
				sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			}
			break;
		case 'T':
			if (!m_stage.GetSelection().IsEmpty()) {
				select_op->SetCanSelectNull(false);
				SetCurrOp(Operator::TRANSLATE);
				sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			}
			break;

		case 'V':
			select_op->SetCanSelectNull(false);
			SetCurrOp(Operator::VERTEX);
			sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			break;
		case 'E':
			select_op->SetCanSelectNull(false);
			SetCurrOp(Operator::EDGE);
			sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			break;
		case 'F':
			select_op->SetCanSelectNull(false);
			SetCurrOp(Operator::FACE);
			sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			break;

		case 'Y':
			if (m_camera_op == m_cam_drive_op) {
				m_camera_op = m_cam_fly_op;
				HIDE_CURSOR;
			} else {
				m_camera_op = m_cam_drive_op;
				SHOW_CURSOR;
			}
			ChangedCamOP(m_camera_op);
			sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			break;

		//case WXK_ESCAPE:
		//	select_op->SetCanSelectNull(true);
		//	SetCurrOp(Operator::ARRANGE);
		//	sub_mgr->NotifyObservers(ee0::MSG_SET_CANVAS_DIRTY);
			break;
		case WXK_SPACE:
			sub_mgr->NotifyObservers(ee3::MSG_SWITCH_TO_NEXT_VIEWPORT);
			break;
		}
	});
}

void EditOpMgr::SetCamera(const pt0::CameraPtr& cam)
{
	m_cam_drive_op->SetCamera(cam);
	m_cam_fly_op->SetCamera(cam);
	m_select_op->SetCamera(cam);

	m_arrange_op->SetCamera(cam);
	m_draw_op->SetCamera(cam);

	m_rotate_op->SetCamera(cam);
	m_translate_op->SetCamera(cam);

	m_vertex_op->SetCamera(cam);
	m_edge_op->SetCamera(cam);
	m_face_op->SetCamera(cam);
}

void EditOpMgr::SetCurrOp(Operator op)
{
	if (m_curr_op == op) {
		return;
	}

	m_curr_op = op;

	auto& impl = m_stage.GetImpl();
	switch (m_curr_op)
	{
	case Operator::ROTATE:
		impl.SetEditOP(m_rotate_op);
		break;
	case Operator::TRANSLATE:
		impl.SetEditOP(m_translate_op);
		break;
	case Operator::VERTEX:
		impl.SetEditOP(m_vertex_op);
		break;
	case Operator::EDGE:
		impl.SetEditOP(m_edge_op);
		break;
	case Operator::FACE:
		impl.SetEditOP(m_face_op);
		break;
	case Operator::ARRANGE:
		impl.SetEditOP(m_arrange_op);
		break;
	case Operator::DRAW:
		impl.SetEditOP(m_draw_op);
		break;
	}
}

void EditOpMgr::SetSelectedNode(const n0::SceneNodePtr& node)
{
	std::static_pointer_cast<PolyhedronSelectOP>(m_select_op)->SetSelected(node);
}

void EditOpMgr::ChangedCamOP(const ee0::EditOPPtr& cam_op)
{
	m_select_op->SetPrevEditOP(cam_op);
	m_rotate_op->SetPrevEditOP(cam_op);
	m_translate_op->SetPrevEditOP(cam_op);
}

}