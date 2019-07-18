#pragma once

#include <SM_Matrix.h>
#include <polymesh3/Brush.h>

#include <functional>

namespace pt3 { class Viewport; }

namespace dw3
{
namespace mesh
{

struct BrushEdge
{
	BrushEdge() : begin(nullptr), end(nullptr) {}
	BrushEdge(const pm3::BrushVertexPtr& begin, const pm3::BrushVertexPtr& end)
		: begin(begin), end(end) {}

	bool operator == (const BrushEdge& e) const {
		return begin == e.begin && end == e.end;
	}
	operator bool() const {
		return begin && end;
	}

	void Project(const pt3::Viewport& vp, const sm::mat4& cam_mat,
		std::function<void(const sm::vec2& b, const sm::vec2& e, const sm::vec2& mid)> func) const;

	pm3::BrushVertexPtr begin;
	pm3::BrushVertexPtr end;

}; // BrushEdge

}
}