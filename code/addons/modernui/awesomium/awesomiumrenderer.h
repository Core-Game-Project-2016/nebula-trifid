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

class AwesomiumRenderer : public Core::RefCounted
{
	__DeclareClass(AwesomiumRenderer);
public:
	void Render(AwesomiumLayout* view);

private:
	AwesomiumRenderer();
	~AwesomiumRenderer();

private:
	Ptr<CoreGraphics::RenderDevice> renderDevice;
	Ptr<CoreGraphics::Shader> shader;

	Ptr<CoreGraphics::ShaderVariable> diffMap;
	Ptr<CoreGraphics::ShaderVariable> modelVar;

	Ptr<Resources::ManagedTexture> whiteTexture;
	Math::rectangle<int> scissor;

	CoreGraphics::ShaderFeature::Mask defaultVariation;
	CoreGraphics::ShaderFeature::Mask scissorVariation;
};
}
