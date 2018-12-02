#pragma once

#include <ee0/typedef.h>

#include <painting0/Camera.h>
#include <node0/typedef.h>

namespace ee0 { class WxStagePage; }
namespace ee3 { class CameraMgr; }
namespace dw3 { class CameraMgr; }
namespace pt0 { class Camera; }
namespace pt3 { class Viewport; }

namespace dw3
{

class WxStagePage;

class EditOpMgr
{
public:
	enum class Operator
	{
		UNKNOWN,

		ROTATE,
		TRANSLATE,

		VERTEX,
		EDGE,
		FACE,

		ARRANGE,
		DRAW,
	};

public:
	EditOpMgr(ee0::WxStagePage& stage);

	void Init(const ee3::CameraMgr& cam_mgr,
		const std::shared_ptr<pt0::Camera>& cam,
		const pt3::Viewport& vp);

	void SetCamera(const pt0::CameraPtr& cam);

	Operator GetCurrOp() const { return m_curr_op; }
	void SetCurrOp(Operator op);

	void SetSelectedNode(const n0::SceneNodePtr& node);

private:
	void ChangedCamOP(const ee0::EditOPPtr& cam_op);

private:
	ee0::WxStagePage& m_stage;

	Operator m_curr_op;

	ee0::EditOPPtr m_cam_drive_op = nullptr;
	ee0::EditOPPtr m_cam_fly_op   = nullptr;
	ee0::EditOPPtr m_camera_op    = nullptr;
	ee0::EditOPPtr m_select_op    = nullptr;

	ee0::EditOPPtr m_arrange_op   = nullptr;
	ee0::EditOPPtr m_draw_op      = nullptr;

	ee0::EditOPPtr m_rotate_op    = nullptr;
	ee0::EditOPPtr m_translate_op = nullptr;

	ee0::EditOPPtr m_vertex_op    = nullptr;
	ee0::EditOPPtr m_edge_op      = nullptr;
	ee0::EditOPPtr m_face_op      = nullptr;

}; // EditOpMgr

}