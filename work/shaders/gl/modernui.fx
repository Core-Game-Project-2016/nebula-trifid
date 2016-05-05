//------------------------------------------------------------------------------
//  modernui.fx
//
//	Basic GUI shader for use with Awesomium
//
//  (C) 2013 Gustav Sterbrant
//------------------------------------------------------------------------------

#include "lib/std.fxh"
#include "lib/techniques.fxh"

sampler2D Texture;

samplerstate TextureSampler
{
	Samplers = { Texture };
};

mat4 Model;

state DefaultGUIState
{
	BlendEnabled[0] = true;
	SrcBlend[0] = SrcAlpha;
	DstBlend[0] = OneMinusSrcAlpha;
	CullMode = Back;
	DepthEnabled = false;
};

//------------------------------------------------------------------------------
/**
*/
shader
void
vsMain(
	[slot=0] in vec2 position,
	[slot=2] in vec2 uv,
	out vec2 UV) 
{
	gl_Position = Model * vec4(position, 1, 1);
	UV = uv;
}

//------------------------------------------------------------------------------
/**
*/
shader
void
psMain(
	in vec2 UV,
	[color0] out vec4 FinalColor) 
{
	FinalColor = texture(Texture, UV).bgra;
}

//------------------------------------------------------------------------------
/**
*/
SimpleTechnique(Default, "Static", vsMain(), psMain(), DefaultGUIState);