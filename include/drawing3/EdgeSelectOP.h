#pragma once

#include "drawing3/MeshSelectBaseOP.h"
#include "drawing3/BrushEdge.h"

namespace dw3
{
namespace mesh
{

class EdgeSelectOP : public MeshSelectBaseOP<BrushEdge>
{
public:
	EdgeSelectOP(const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp,
		const ee0::SubjectMgrPtr& sub_mgr, const MeshPointQuery::Selected& selected);

protected:
	virtual void DrawImpl(const pm3::Brush& brush, const sm::mat4& cam_mat) const override;

	virtual BrushEdge QueryByPos(int x, int y) const override;
	virtual void QueryByRect(const sm::irect& rect, std::vector<BrushEdge>& selection) const override;

private:
	void OnBrushEdge(const BrushEdge& edge, const sm::mat4& cam_mat,
		std::function<void(const sm::vec2& b, const sm::vec2& e, const sm::vec2& mid)> func) const;

}; // EdgeSelectOP

}
}