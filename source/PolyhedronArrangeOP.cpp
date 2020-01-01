#include "draft3/PolyhedronArrangeOP.h"
#include "draft3/PolyTranslateState.h"

namespace draft3
{
namespace mesh
{

PolyhedronArrangeOP::PolyhedronArrangeOP(const std::shared_ptr<pt0::Camera>& camera,
	                         const pt3::Viewport& vp,
	                         const ee0::SubjectMgrPtr& sub_mgr,
	                         const MeshPointQuery::Selected& selected,
	                         std::function<void()> update_cb)
	: ee0::EditOP(camera)
	, m_vp(vp)
	, m_sub_mgr(sub_mgr)
{
	m_poly_trans_state = std::make_shared<PolyTranslateState>(
		camera, vp, sub_mgr, selected, update_cb);

	ChangeEditOpState(m_poly_trans_state);
}

bool PolyhedronArrangeOP::OnMouseLeftDown(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftDown(x, y)) {
		return true;
	}
	if (m_op_state->OnMousePress(x, y)) {
		return true;
	}

	return false;
}

bool PolyhedronArrangeOP::OnMouseLeftUp(int x, int y)
{
	if (ee0::EditOP::OnMouseLeftUp(x, y)) {
		return true;
	}
	if (m_op_state->OnMouseRelease(x, y)) {
		return true;
	}

	return false;
}

bool PolyhedronArrangeOP::OnMouseDrag(int x, int y)
{
	if (ee0::EditOP::OnMouseDrag(x, y)) {
		return true;
	}
	if (m_op_state->OnMouseDrag(x, y)) {
		return true;
	}

	return false;
}

bool PolyhedronArrangeOP::OnDraw() const
{
	if (ee0::EditOP::OnDraw()) {
		return true;
	}
	if (m_op_state->OnDraw()) {
		return true;
	}

	return false;
}

}
}