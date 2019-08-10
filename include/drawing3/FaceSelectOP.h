#pragma once

#include "drawing3/MeshSelectBaseOP.h"

#include <polymesh3/typedef.h>

namespace tess { class Painter; }

namespace dw3
{
namespace mesh
{

class FaceSelectOP : public MeshSelectBaseOP<pm3::FacePtr>
{
public:
	FaceSelectOP(const std::shared_ptr<pt0::Camera>& camera, const pt3::Viewport& vp,
		const ee0::SubjectMgrPtr& sub_mgr, const MeshPointQuery::Selected& selected);

protected:
	virtual void DrawImpl(const pm3::Polytope& poly, const sm::mat4& cam_mat) const override;

	virtual pm3::FacePtr QueryByPos(int x, int y) const override;
	virtual void QueryByRect(const sm::irect& rect, std::vector<pm3::FacePtr>& selection) const override;

private:
	sm::vec2 CalcFaceCenter(const pm3::Face& face, const sm::mat4& cam_mat) const;

	void DrawFace(tess::Painter& pt, const pm3::Face& face, uint32_t color, const sm::mat4& cam_mat) const;

}; // FaceSelectOP

}
}