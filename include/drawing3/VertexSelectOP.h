#pragma once

#include "drawing3/MeshSelectBaseOP.h"

#include <model/BrushModel.h>

namespace dw3
{
namespace mesh
{

class VertexSelectOP : public MeshSelectBaseOP<model::BrushModel::BrushVertexPtr>
{
public:
	VertexSelectOP(const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp,
		const ee0::SubjectMgrPtr& sub_mgr, const MeshPointQuery::Selected& selected);

protected:
	virtual void DrawImpl(const pm3::Brush& brush, const sm::mat4& cam_mat) const override;

	virtual model::BrushModel::BrushVertexPtr QueryByPos(int x, int y) const override;
	virtual void QueryByRect(const sm::irect& rect, std::vector<model::BrushModel::BrushVertexPtr>& selection) const override;

}; // VertexSelectOP

}
}