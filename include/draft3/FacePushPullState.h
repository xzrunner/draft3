#pragma once

#include "draft3/MeshPointQuery.h"

#include <ee0/EditOpState.h>
#include <ee0/typedef.h>

#include <SM_Matrix.h>
#include <SM_Plane.h>
#include <halfedge/HalfEdge.h>

#include <functional>

namespace pt2 { class OrthoCamera; }
namespace pt3 { class Viewport; }

namespace draft3
{

class FacePushPullState : public ee0::EditOpState
{
public:
	FacePushPullState(const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp,
		const ee0::SubjectMgrPtr& sub_mgr, const MeshPointQuery::Selected& selected,
		std::function<void()> update_cb);

	virtual bool OnMousePress(int x, int y) override;
	virtual bool OnMouseDrag(int x, int y) override;

	virtual bool OnDraw(const ur::Device& dev, ur::Context& ctx) const override;

private:
	void TranslateFace(const sm::vec3& offset);

private:
	const pt3::Viewport& m_vp;

	ee0::SubjectMgrPtr m_sub_mgr;

	const MeshPointQuery::Selected& m_selected;

	std::function<void()> m_update_cb;

	std::shared_ptr<pt2::OrthoCamera> m_cam2d;

	sm::mat4 m_cam_mat;

	// move path 2d
	sm::vec2 m_first_pos2;
	sm::vec2 m_first_dir2;

	sm::Ray m_move_path3d;

	sm::vec3 m_last_pos3d;

}; // FacePushPullState

}