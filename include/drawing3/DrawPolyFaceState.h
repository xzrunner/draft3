#pragma once

#include <ee0/EditOpState.h>
#include <ee0/typedef.h>

#include <SM_Vector.h>
#include <node0/typedef.h>

#include <vector>

namespace pt3 { class Viewport; }

namespace dw3
{
namespace mesh
{

class DrawPolyFaceState : public ee0::EditOpState
{
public:
	DrawPolyFaceState(const std::shared_ptr<pt0::Camera>& camera,
		const pt3::Viewport& vp, const ee0::SubjectMgrPtr& sub_mgr);

	virtual bool OnKeyPress(int key_code) override;
	virtual bool OnMousePress(int x, int y) override;
	virtual bool OnMouseMove(int x, int y) override;

	virtual bool OnDraw() const override;

private:
	bool RayPlaneIntersect(int x, int y, float plane_y, sm::vec3& cross) const;

	n0::SceneNodePtr CreateModelObj();

private:
	const pt3::Viewport& m_vp;

	ee0::SubjectMgrPtr m_sub_mgr;

	std::vector<sm::vec3> m_polygon;

	float m_y;

}; // DrawPolyFaceState

}
}