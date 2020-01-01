#pragma once

#include "draft3/MeshSelectBaseOP.h"

#include <polymesh3/Polytope.h>

namespace draft3
{

class EdgeSelectOP : public MeshSelectBaseOP<pm3::Polytope::EdgePtr>
{
public:
	EdgeSelectOP(const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp,
		const ee0::SubjectMgrPtr& sub_mgr, const MeshPointQuery::Selected& selected);

protected:
	virtual void DrawImpl(const pm3::Polytope& poly, const sm::mat4& cam_mat) const override;

	virtual pm3::Polytope::EdgePtr QueryByPos(int x, int y) const override;
	virtual void QueryByRect(const sm::irect& rect, std::vector<pm3::Polytope::EdgePtr>& selection) const override;

private:
	void OnBrushEdge(const pm3::Polytope::EdgePtr& edge, const sm::mat4& cam_mat,
		std::function<void(const sm::vec2& b, const sm::vec2& e, const sm::vec2& mid)> func) const;

    static void ProjectBrushEdge(const pm3::Polytope& poly, const pm3::Polytope::EdgeIndex& edge, const pt3::Viewport& vp, const sm::mat4& cam_mat,
        std::function<void(const sm::vec2& b, const sm::vec2& e, const sm::vec2& mid)> func);

}; // EdgeSelectOP

}