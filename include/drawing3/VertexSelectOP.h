#pragma once

#include "drawing3/MeshSelectBaseOP.h"

#include <polymesh3/Polytope.h>

namespace dw3
{
namespace mesh
{

class VertexSelectOP : public MeshSelectBaseOP<pm3::Polytope::PointPtr>
{
public:
	VertexSelectOP(const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp,
		const ee0::SubjectMgrPtr& sub_mgr, const MeshPointQuery::Selected& selected);

protected:
	virtual void DrawImpl(const pm3::Polytope& poly, const sm::mat4& cam_mat) const override;

	virtual pm3::Polytope::PointPtr QueryByPos(int x, int y) const override;
	virtual void QueryByRect(const sm::irect& rect, std::vector<pm3::Polytope::PointPtr>& selection) const override;

}; // VertexSelectOP

}
}