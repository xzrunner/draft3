//#pragma once
//
//#include <unirender/Shader.h>
//
//namespace draft3
//{
//
//class DrawEdgeShader : public ur::Shader
//{
//public:
//	DrawEdgeShader(ur::RenderContext* rc, const char* vs, const char* fs,
//		const std::vector<std::string>& textures, const CU_VEC<ur::VertexAttrib>& va_list);
//
//	virtual ur::DRAW_MODE GetDrawMode() const { return ur::DRAW_LINES; }
//	virtual void DrawBefore(const ur::TexturePtr& tex);
//
//}; // DrawEdgeShader
//
//}