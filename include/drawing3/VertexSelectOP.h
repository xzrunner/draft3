#pragma once

#include "drawing3/MeshSelectBaseOP.h"

namespace dw3
{
namespace mesh
{

class VertexSelectOP : public MeshSelectBaseOP<quake::BrushVertexPtr>
{
public:
	VertexSelectOP(const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp,
		const ee0::SubjectMgrPtr& sub_mgr, const MeshPointQuery::Selected& selected);

protected:
	virtual void DrawImpl(const quake::MapBrush& brush, const sm::mat4& cam_mat) const override;

	virtual quake::BrushVertexPtr QueryByPos(int x, int y) const override;
	virtual void QueryByRect(const sm::irect& rect, std::vector<quake::BrushVertexPtr>& selection) const override;

}; // VertexSelectOP

}
}