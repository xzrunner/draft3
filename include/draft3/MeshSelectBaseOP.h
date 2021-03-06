#pragma once

#include "draft3/MeshPointQuery.h"

#include <ee0/EditOP.h>
#include <ee0/typedef.h>

#include <SM_Rect.h>

#include <set>

namespace pt2 { class OrthoCamera; }
namespace pt3 { class Viewport; }
namespace pm3 { class Polytope; }

namespace draft3
{

template <typename T>
class MeshSelectBaseOP : public ee0::EditOP
{
public:
	MeshSelectBaseOP(const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp,
		const ee0::SubjectMgrPtr& sub_mgr, const MeshPointQuery::Selected& selected);

	virtual bool OnMouseLeftDown(int x, int y) override;
	virtual bool OnMouseLeftUp(int x, int y) override;
	virtual bool OnMouseMove(int x, int y) override;
	virtual bool OnMouseDrag(int x, int y) override;

	virtual bool OnDraw(const ur::Device& dev, ur::Context& ctx) const override;

	auto& GetSelected() const { return m_selected; }

	void SetCamera(const std::shared_ptr<pt0::Camera>& camera) {
		m_camera = camera;
	}

protected:
	virtual void DrawImpl(const ur::Device& dev, ur::Context& ctx,
        const pm3::Polytope& poly, const sm::mat4& cam_mat) const = 0;

	virtual T QueryByPos(int x, int y) const = 0;
	virtual void QueryByRect(const sm::irect& rect, std::vector<T>& selection) const = 0;

private:
	void SelectByPos(const sm::ivec2& pos);
	void SelectByRect(const sm::irect& rect);

protected:
	const pt3::Viewport& m_vp;
	ee0::SubjectMgrPtr   m_sub_mgr;

	const MeshPointQuery::Selected& m_base_selected;

	std::shared_ptr<pt2::OrthoCamera> m_cam2d;

	ee0::SelectionSet<T> m_selected;
	T                    m_selecting;

	sm::ivec2 m_first_pos;

	std::unique_ptr<ee0::EditOpState> m_draw_state = nullptr;
	mutable bool m_draw_state_enable = true;

}; // MeshSelectBaseOP

}

#include "draft3/MeshSelectBaseOP.inl"
