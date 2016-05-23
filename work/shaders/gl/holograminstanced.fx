#include "lib/std.fxh"
#include "lib/util.fxh"
#include "lib/techniques.fxh"


sampler2D Texture;

samplerstate TextureSampler
{
	Samplers = { Texture };
};

mat4 ModelArray[64];
mat4 ViewProjection;

state DefaultHologramState
{
	BlendEnabled[0] = true;
	SrcBlend[0] = SrcAlpha;
	DstBlend[0] = OneMinusSrcAlpha;
	CullMode = None;
	DepthEnabled = true;
};

shader
void
vsMainInstanced(
	in vec3 position,
	[slot=2] in vec2 uv,
	out vec2 UV)
{
	gl_Position = ViewProjection * ModelArray[gl_InstanceID] * vec4(position, 1);
	UV = uv;
}

shader
void
psMain(
	in vec2 UV,
	[color0] out vec4 FinalColor)	
{
	vec2 uv = vec2(UV.x, 1.0 - UV.y);
	FinalColor = texture(Texture, uv).bgra;
}

SimpleTechnique(HologramInstanced, "Static|Instanced", vsMainInstanced(), psMain(), DefaultHologramState);