// ------------------------------------------------------------------------------------------------------------------------
// 
// File:  awesomiumrenderer.h
// 
// Author: Timmy Silesmo
// Creation Date: 9:42 PM - 03/05/2016
// 
// Contact: Timmy.Silesmo@gmail.com
// 
// 
// ------------------------------------------------------------------------------------------------------------------------
#pragma once

#include "core/rttimacros.h"
#include "core/refcounted.h"
#include <coregraphics/shader.h>
#include <coregraphics/renderdevice.h>

namespace AwesomiumUI
{
class AwesomiumLayout;

struct NebulaGeometry
{
	Ptr<CoreGraphics::VertexBuffer> vb;
	Ptr<CoreGraphics::IndexBuffer> ib;
	CoreGraphics::PrimitiveGroup primGroup;
};

class AwesomiumRenderer : public Core::RefCounted
{
	__DeclareClass(AwesomiumRenderer);
public:
	void Render(AwesomiumLayout* view);
	void InstancedRender(AwesomiumLayout* view, const Util::Array<Math::matrix44>& matrices);

private:
	AwesomiumRenderer();
	~AwesomiumRenderer();
	void GenerateMesh();

private:
	struct NebulaVertex
	{
		float x, y;
		float u, v;
	};

	Ptr<CoreGraphics::RenderDevice> renderDevice;

	NebulaGeometry* geometry;

	Ptr<CoreGraphics::Shader> uiShader;
	Ptr<CoreGraphics::ShaderVariable> uiDiffMap;
	Ptr<CoreGraphics::ShaderVariable> uiModelVar;

	Ptr<CoreGraphics::Shader> hologramShader;
	Ptr<CoreGraphics::ShaderVariable> hologramDiffMap;
	Ptr<CoreGraphics::ShaderVariable> hologramModelVar;
	Ptr<CoreGraphics::ShaderVariable> hologramViewProjVar;

	Ptr<CoreGraphics::Shader> hologramInstancedShader;
	Ptr<CoreGraphics::ShaderVariable> hologramInstancedDiffMap;
	Ptr<CoreGraphics::ShaderVariable> hologramInstancedViewProjVar;
	Ptr<CoreGraphics::ShaderVariable> hologramInstancedModelArray;
};
}
