#pragma once

#include "drawing3/MeshTranslateBaseOP.h"

#include <model/BrushModel.h>

namespace dw3
{
namespace mesh
{

class VertexTranslateOP : public MeshTranslateBaseOP<model::BrushModel::BrushVertexPtr>
{
public:
	VertexTranslateOP(const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp,
		const ee0::SubjectMgrPtr& sub_mgr, const MeshPointQuery::Selected& selected,
		const ee0::SelectionSet<model::BrushModel::BrushVertexPtr>& selection, std::function<void()> update_cb);

protected:
	virtual bool QueryByPos(const sm::vec2& pos, const model::BrushModel::BrushVertexPtr& vert,
		const sm::mat4& cam_mat) const override;
	virtual void TranslateSelected(const sm::vec3& offset) override;

}; // VertexTranslateOP

}
}