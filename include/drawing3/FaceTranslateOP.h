#pragma once

#include "drawing3/MeshTranslateBaseOP.h"

#include <polymesh3/typedef.h>

namespace dw3
{
namespace mesh
{

class FaceTranslateOP : public MeshTranslateBaseOP<pm3::FacePtr>
{
public:
	FaceTranslateOP(const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp,
		const ee0::SubjectMgrPtr& sub_mgr, const MeshPointQuery::Selected& selected,
		const ee0::SelectionSet<pm3::FacePtr>& selection, std::function<void()> update_cb);

protected:
	virtual bool QueryByPos(const sm::vec2& pos, const pm3::FacePtr& face,
		const sm::mat4& cam_mat) const override;
	virtual void TranslateSelected(const sm::vec3& offset) override;

}; // FaceTranslateOP

}
}