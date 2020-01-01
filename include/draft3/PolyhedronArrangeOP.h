#pragma once

#include "draft3/MeshPointQuery.h"

#include <ee0/EditOP.h>
#include <ee0/typedef.h>

namespace pt3 { class Viewport; }

namespace draft3
{

class PolyhedronArrangeOP : public ee0::EditOP
{
public:
	PolyhedronArrangeOP(const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp,
		const ee0::SubjectMgrPtr& sub_mgr, const MeshPointQuery::Selected& selected,
		std::function<void()> update_cb);

	virtual bool OnMouseLeftDown(int x, int y) override;
	virtual bool OnMouseLeftUp(int x, int y) override;
	virtual bool OnMouseDrag(int x, int y) override;

	virtual bool OnDraw() const override;

private:
	const pt3::Viewport& m_vp;
	ee0::SubjectMgrPtr   m_sub_mgr;

	ee0::EditOpStatePtr m_poly_trans_state = nullptr;

}; // PolyhedronArrangeOP

}