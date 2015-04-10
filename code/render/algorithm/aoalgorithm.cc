//------------------------------------------------------------------------------
//  aoalgorithm.cc
//  (C) 2013 gscept
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "aoalgorithm.h"
#include "coregraphics/rendertarget.h"
#include "coregraphics/shaderserver.h"
#include "coregraphics/memorytextureloader.h"
#include "resources/resourceloader.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/shadersemantics.h"
#include "graphics/graphicsserver.h"
#include "resources/resourcemanager.h"
#include "coregraphics/base/rendertargetbase.h"

#define MAX_RADIUS_PIXELS 0.5f
#define DivAndRoundUp(a, b) (a % b != 0) ? (a / b + 1) : (a / b)
//#define BLUR_PS

using namespace Math;
using namespace CoreGraphics;
using namespace Graphics;
namespace Algorithm
{
__ImplementClass(Algorithm::AOAlgorithm, 'ALAO', Algorithm::AlgorithmBase);


//------------------------------------------------------------------------------
/**
*/
AOAlgorithm::AOAlgorithm() :
	downSample(1)
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AOAlgorithm::~AOAlgorithm()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
AOAlgorithm::Setup()
{
	n_assert(this->inputs.Size() == 1);
	n_assert(this->outputNames.Size() == 1);
	n_assert(this->inputs[0].isvalid());

	AlgorithmBase::Setup();

	// setup output render target
	this->output = RenderTarget::Create();
	this->output->SetResolveTextureResourceId(this->outputNames[0]);
    this->output->SetRelativeWidth(1);
    this->output->SetRelativeHeight(1);
	this->output->SetAntiAliasQuality(AntiAliasQuality::None);
	this->output->SetColorBufferFormat(PixelFormat::R16F);
	this->output->SetClearColor(float4(0,0,0,0));
	//this->output->SetClearFlags(Base::RenderTargetBase::ClearColor);
	this->output->Setup();

    // setup internal targets
    IndexT target;
    for (target = 0; target < 2; target++)
    {
        this->internalTargets[target] = RenderTarget::Create();
        this->internalTargets[target]->SetResolveTextureResourceId("SSAOInternalBuffer" + Util::String::FromInt(target));
        this->internalTargets[target]->SetRelativeWidth(1);
        this->internalTargets[target]->SetRelativeHeight(1);
        this->internalTargets[target]->SetAntiAliasQuality(AntiAliasQuality::None);
        this->internalTargets[target]->SetColorBufferFormat(PixelFormat::G16R16F);
        this->internalTargets[target]->SetClearColor(float4(0,0,0,0));
        this->internalTargets[target]->Setup();
    }

#ifdef HBAO_COMPUTE
	// setup shaders
	this->hbao = ShaderServer::Instance()->CreateShaderInstance("shd:hbao_cs");
	this->blur = ShaderServer::Instance()->CreateShaderInstance("shd:hbaoblur_cs");

    this->xDirection = ShaderServer::Instance()->FeatureStringToMask("X");
    this->yDirection = ShaderServer::Instance()->FeatureStringToMask("Y");

	this->hbao0Var = this->hbao->GetVariableByName("HBAO0");
	this->hbao1Var = this->hbao->GetVariableByName("HBAO1");
	this->hbaoBlur0Var = this->blur->GetVariableByName("HBAO0");
	this->hbaoBlur1Var = this->blur->GetVariableByName("HBAO0");
#else
	// setup shaders
	this->hbao = ShaderServer::Instance()->CreateShaderInstance("shd:hbao");
	this->blur = ShaderServer::Instance()->CreateShaderInstance("shd:hbaoblur");
	this->hbaoTextureVar = this->blur->GetVariableByName("HBAOBuffer");
	this->randomTextureVar = this->hbao->GetVariableByName("RandomMap");
	this->rVar = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_R);
	this->focalLengthVar = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_PIXELFOCALLENGTH);
	this->maxRadiusPixelsVar = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_MAXRADIUSPIXELS);
	this->negInvR2Var = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_NEGINVR2);

#endif

	// setup variables
	this->depthTextureVar = this->hbao->GetVariableByName("DepthBuffer");
	
    //this->hbaoBlurredTextureVar = this->blur->GetVariableByName("HBAOBlurred");
    //this->hbaoFinalTextureVar = this->blur->GetVariableByName("HBAOFinal");

	this->fullWidth = (float)this->output->GetWidth();
	this->fullHeight = (float)this->output->GetHeight();
	this->radius = 12.0f;

	vars.maxRadiusPixels = MAX_RADIUS_PIXELS * n_min(this->fullWidth, this->fullHeight);
	vars.tanAngleBias = tanf(n_deg2rad(10.0));
	vars.strength = 2.0f;

	this->uvToViewAVar = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_UVTOVIEWA);
	this->uvToViewBVar = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_UVTOVIEWB);
	this->r2Var = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_R2);
	this->aoResolutionVar = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_AORESOLUTION);
	this->invAOResolutionVar = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_INVAORESOLUTION);
	this->strengthVar = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_STRENGHT);
	this->tanAngleBiasVar = this->hbao->GetVariableBySemantic(NEBULA3_SEMANTIC_TANANGLEBIAS);
	this->powerExponentVar = this->blur->GetVariableBySemantic(NEBULA3_SEMANTIC_POWEREXPONENT);
	this->blurFalloff = this->blur->GetVariableBySemantic(NEBULA3_SEMANTIC_FALLOFF);
	this->blurDepthThreshold = this->blur->GetVariableBySemantic(NEBULA3_SEMANTIC_DEPTHTHRESHOLD);
	this->powerExponentVar->SetFloat(1.5f);

	// setup fsq
	this->quad.Setup(this->inputs[0]->GetWidth(), this->inputs[0]->GetHeight());

#ifndef HBAO_COMPUTE
	// Mersenne-Twister random numbers in [0,1).
	static const float MersenneTwisterNumbers[1024] = {
		0.463937f,0.340042f,0.223035f,0.468465f,0.322224f,0.979269f,0.031798f,0.973392f,0.778313f,0.456168f,0.258593f,0.330083f,0.387332f,0.380117f,0.179842f,0.910755f,
		0.511623f,0.092933f,0.180794f,0.620153f,0.101348f,0.556342f,0.642479f,0.442008f,0.215115f,0.475218f,0.157357f,0.568868f,0.501241f,0.629229f,0.699218f,0.707733f,
		0.556725f,0.005520f,0.708315f,0.583199f,0.236644f,0.992380f,0.981091f,0.119804f,0.510866f,0.560499f,0.961497f,0.557862f,0.539955f,0.332871f,0.417807f,0.920779f,
		0.730747f,0.076690f,0.008562f,0.660104f,0.428921f,0.511342f,0.587871f,0.906406f,0.437980f,0.620309f,0.062196f,0.119485f,0.235646f,0.795892f,0.044437f,0.617311f,
		0.891128f,0.263161f,0.245298f,0.276518f,0.786986f,0.059768f,0.424345f,0.433341f,0.052190f,0.699924f,0.139479f,0.402873f,0.741976f,0.557978f,0.127093f,0.946352f,
		0.205587f,0.092822f,0.422956f,0.715176f,0.711952f,0.926062f,0.368646f,0.286516f,0.241413f,0.831616f,0.232247f,0.478637f,0.366948f,0.432024f,0.268430f,0.619122f,
		0.391737f,0.056698f,0.067702f,0.509009f,0.920858f,0.298358f,0.701015f,0.044309f,0.936794f,0.485976f,0.271286f,0.108779f,0.325844f,0.682314f,0.955090f,0.658145f,
		0.295861f,0.562559f,0.867194f,0.810552f,0.487959f,0.869567f,0.224706f,0.962637f,0.646548f,0.003730f,0.228857f,0.263667f,0.365176f,0.958302f,0.606619f,0.901869f,
		0.757257f,0.306061f,0.633172f,0.407697f,0.443632f,0.979959f,0.922944f,0.946421f,0.594079f,0.604343f,0.864211f,0.187557f,0.877119f,0.792025f,0.954840f,0.976719f,
		0.350546f,0.834781f,0.945113f,0.155877f,0.411841f,0.552378f,0.855409f,0.741383f,0.761251f,0.896223f,0.782077f,0.266224f,0.128873f,0.645733f,0.591567f,0.247385f,
		0.260848f,0.811970f,0.653369f,0.976713f,0.221533f,0.957436f,0.294018f,0.159025f,0.820596f,0.569601f,0.934328f,0.467182f,0.763165f,0.835736f,0.240033f,0.389869f,
		0.998754f,0.783739f,0.758034f,0.614317f,0.221128f,0.502497f,0.978066f,0.247794f,0.619551f,0.658307f,0.769667f,0.768478f,0.337143f,0.370689f,0.084723f,0.510534f,
		0.594996f,0.994636f,0.181230f,0.868113f,0.312023f,0.480495f,0.177356f,0.367374f,0.741642f,0.202983f,0.229404f,0.108165f,0.098607f,0.010412f,0.727391f,0.942217f,
		0.023850f,0.110631f,0.958293f,0.208996f,0.584609f,0.491803f,0.238266f,0.591587f,0.297477f,0.681421f,0.215040f,0.587764f,0.704494f,0.978978f,0.911686f,0.692657f,
		0.462987f,0.273259f,0.802855f,0.651633f,0.736728f,0.986217f,0.402363f,0.524098f,0.740470f,0.799076f,0.918257f,0.705367f,0.477477f,0.102279f,0.809959f,0.860645f,
		0.118276f,0.009567f,0.280106f,0.948473f,0.025423f,0.458173f,0.512607f,0.082088f,0.536906f,0.472590f,0.835726f,0.078518f,0.357919f,0.797522f,0.570516f,0.162719f,
		0.815968f,0.874141f,0.915300f,0.392073f,0.366307f,0.766238f,0.462755f,0.087614f,0.402357f,0.277686f,0.294194f,0.392791f,0.504893f,0.263420f,0.509197f,0.518974f,
		0.738809f,0.965800f,0.003864f,0.976899f,0.292287f,0.837148f,0.525498f,0.743779f,0.359015f,0.060636f,0.595481f,0.483102f,0.900195f,0.423277f,0.981990f,0.154968f,
		0.085584f,0.681517f,0.814437f,0.105936f,0.972238f,0.207062f,0.994642f,0.989271f,0.646217f,0.330263f,0.432094f,0.139929f,0.908629f,0.271571f,0.539319f,0.845182f,
		0.140069f,0.001406f,0.340195f,0.582218f,0.693570f,0.293148f,0.733441f,0.375523f,0.676068f,0.130642f,0.606523f,0.441091f,0.113519f,0.844462f,0.399921f,0.551049f,
		0.482781f,0.894854f,0.188909f,0.431045f,0.043693f,0.394601f,0.544309f,0.798761f,0.040417f,0.022292f,0.681257f,0.598379f,0.069981f,0.255632f,0.174776f,0.880842f,
		0.412071f,0.397976f,0.932835f,0.979471f,0.244276f,0.488083f,0.313785f,0.858199f,0.390958f,0.426132f,0.754800f,0.360781f,0.862827f,0.526424f,0.090054f,0.673971f,
		0.715044f,0.237489f,0.210234f,0.952837f,0.448429f,0.738062f,0.077342f,0.260666f,0.590478f,0.127519f,0.628981f,0.136232f,0.860189f,0.596789f,0.524043f,0.897171f,
		0.648864f,0.116735f,0.666835f,0.536993f,0.811733f,0.854961f,0.857206f,0.945069f,0.434195f,0.602343f,0.823780f,0.109481f,0.684652f,0.195598f,0.213630f,0.283516f,
		0.387092f,0.182029f,0.834655f,0.948975f,0.373107f,0.249751f,0.162575f,0.587850f,0.192648f,0.737863f,0.777432f,0.651490f,0.562558f,0.918301f,0.094830f,0.260698f,
		0.629400f,0.751325f,0.362210f,0.649610f,0.397390f,0.670624f,0.215662f,0.925465f,0.908397f,0.486853f,0.141060f,0.236122f,0.926399f,0.416056f,0.781483f,0.538538f,
		0.119521f,0.004196f,0.847561f,0.876772f,0.945552f,0.935095f,0.422025f,0.502860f,0.932500f,0.116670f,0.700854f,0.995577f,0.334925f,0.174659f,0.982878f,0.174110f,
		0.734294f,0.769366f,0.917586f,0.382623f,0.795816f,0.051831f,0.528121f,0.691978f,0.337981f,0.675601f,0.969444f,0.354908f,0.054569f,0.254278f,0.978879f,0.611259f,
		0.890006f,0.712659f,0.219624f,0.826455f,0.351117f,0.087383f,0.862534f,0.805461f,0.499343f,0.482118f,0.036473f,0.815656f,0.016539f,0.875982f,0.308313f,0.650039f,
		0.494165f,0.615983f,0.396761f,0.921652f,0.164612f,0.472705f,0.559820f,0.675677f,0.059891f,0.295793f,0.818010f,0.769365f,0.158699f,0.648142f,0.228793f,0.627454f,
		0.138543f,0.639463f,0.200399f,0.352380f,0.470716f,0.888694f,0.311777f,0.571183f,0.979317f,0.457287f,0.115151f,0.725631f,0.620539f,0.629373f,0.850207f,0.949974f,
		0.254675f,0.142306f,0.688887f,0.307235f,0.284882f,0.847675f,0.617070f,0.207422f,0.550545f,0.541886f,0.173878f,0.474841f,0.678372f,0.289180f,0.528111f,0.306538f,
		0.869399f,0.040299f,0.417301f,0.472569f,0.857612f,0.917462f,0.842319f,0.986865f,0.604528f,0.731115f,0.607880f,0.904675f,0.397955f,0.627867f,0.533371f,0.656758f,
		0.627210f,0.223554f,0.268442f,0.254858f,0.834380f,0.131010f,0.838028f,0.613512f,0.821627f,0.859779f,0.405212f,0.909901f,0.036186f,0.643093f,0.187064f,0.945730f,
		0.319022f,0.709012f,0.852200f,0.559587f,0.865751f,0.368890f,0.840416f,0.950571f,0.315120f,0.331749f,0.509218f,0.468617f,0.119006f,0.541820f,0.983444f,0.115515f,
		0.299804f,0.840386f,0.445282f,0.900755f,0.633600f,0.304196f,0.996153f,0.844025f,0.462361f,0.314402f,0.850035f,0.773624f,0.958303f,0.765382f,0.567577f,0.722607f,
		0.001299f,0.189690f,0.364661f,0.192390f,0.836882f,0.783680f,0.026723f,0.065230f,0.588791f,0.937752f,0.993644f,0.597499f,0.851975f,0.670339f,0.360987f,0.755649f,
		0.571521f,0.231990f,0.425067f,0.116442f,0.321815f,0.629616f,0.701207f,0.716931f,0.146357f,0.360526f,0.498487f,0.846096f,0.307994f,0.323456f,0.288884f,0.477935f,
		0.236433f,0.876589f,0.667459f,0.977175f,0.179347f,0.479408f,0.633292f,0.957666f,0.343651f,0.871846f,0.452856f,0.895494f,0.327657f,0.867779f,0.596825f,0.907009f,
		0.417409f,0.530739f,0.547422f,0.141032f,0.721096f,0.587663f,0.830054f,0.460860f,0.563898f,0.673780f,0.035824f,0.755808f,0.331846f,0.653460f,0.926339f,0.724599f,
		0.978501f,0.495221f,0.098108f,0.936766f,0.139911f,0.851336f,0.889867f,0.376509f,0.661482f,0.156487f,0.671886f,0.487835f,0.046571f,0.441975f,0.014015f,0.440433f,
		0.235927f,0.163762f,0.075399f,0.254734f,0.214011f,0.554803f,0.712877f,0.795785f,0.471616f,0.105032f,0.355989f,0.834418f,0.498021f,0.018318f,0.364799f,0.918869f,
		0.909222f,0.858506f,0.928250f,0.946347f,0.755364f,0.408753f,0.137841f,0.247870f,0.300618f,0.470068f,0.248714f,0.521691f,0.009862f,0.891550f,0.908914f,0.227533f,
		0.702908f,0.596738f,0.581597f,0.099904f,0.804893f,0.947457f,0.080649f,0.375755f,0.890498f,0.689130f,0.600941f,0.382261f,0.814084f,0.258373f,0.278029f,0.907399f,
		0.625024f,0.016637f,0.502896f,0.743077f,0.247834f,0.846201f,0.647815f,0.379888f,0.517357f,0.921494f,0.904846f,0.805645f,0.671974f,0.487205f,0.678009f,0.575624f,
		0.910779f,0.947642f,0.524788f,0.231298f,0.299029f,0.068158f,0.569690f,0.121049f,0.701641f,0.311914f,0.447310f,0.014019f,0.013391f,0.257855f,0.481835f,0.808870f,
		0.628222f,0.780253f,0.202719f,0.024902f,0.774355f,0.783080f,0.330077f,0.788864f,0.346888f,0.778702f,0.261985f,0.696691f,0.212839f,0.713849f,0.871828f,0.639753f,
		0.711037f,0.651247f,0.042374f,0.236938f,0.746267f,0.235043f,0.442707f,0.195417f,0.175918f,0.987980f,0.031270f,0.975425f,0.277087f,0.752667f,0.639751f,0.507857f,
		0.873571f,0.775393f,0.390003f,0.415997f,0.287861f,0.189340f,0.837939f,0.186253f,0.355633f,0.803788f,0.029124f,0.802046f,0.248046f,0.354010f,0.420571f,0.109523f,
		0.731250f,0.700653f,0.716019f,0.651507f,0.250055f,0.884214f,0.364255f,0.244975f,0.472268f,0.080641f,0.309332f,0.250613f,0.519091f,0.066142f,0.037804f,0.865752f,
		0.767738f,0.617325f,0.537048f,0.743959f,0.401200f,0.595458f,0.869843f,0.193999f,0.670364f,0.018494f,0.743159f,0.979555f,0.382352f,0.191059f,0.992247f,0.946175f,
		0.306473f,0.793720f,0.687331f,0.556239f,0.958367f,0.390949f,0.357823f,0.110213f,0.977540f,0.831431f,0.485895f,0.148678f,0.847327f,0.733145f,0.397393f,0.376365f,
		0.398704f,0.463869f,0.976946f,0.844771f,0.075688f,0.473865f,0.470958f,0.548172f,0.350174f,0.727441f,0.123139f,0.347760f,0.839587f,0.562705f,0.036853f,0.564723f,
		0.960356f,0.220534f,0.906969f,0.677664f,0.841052f,0.111530f,0.032346f,0.027749f,0.468255f,0.229196f,0.508756f,0.199613f,0.298103f,0.677274f,0.526005f,0.828221f,
		0.413321f,0.305165f,0.223361f,0.778072f,0.198089f,0.414976f,0.007498f,0.464238f,0.785213f,0.534428f,0.060537f,0.572427f,0.693334f,0.865843f,0.034964f,0.586806f,
		0.161710f,0.203743f,0.656513f,0.604340f,0.688333f,0.257211f,0.246437f,0.338237f,0.839947f,0.268420f,0.913245f,0.759551f,0.289283f,0.347280f,0.508970f,0.361526f,
		0.554649f,0.086439f,0.024344f,0.661653f,0.988840f,0.110613f,0.129422f,0.405940f,0.781764f,0.303922f,0.521807f,0.236282f,0.277927f,0.699228f,0.733812f,0.772090f,
		0.658423f,0.056394f,0.153089f,0.536837f,0.792251f,0.165229f,0.592251f,0.228337f,0.147078f,0.116056f,0.319268f,0.293400f,0.872600f,0.842240f,0.306238f,0.228790f,
		0.745704f,0.821321f,0.778268f,0.611390f,0.969139f,0.297654f,0.367369f,0.815074f,0.985840f,0.693232f,0.411759f,0.366651f,0.345481f,0.609060f,0.778929f,0.640823f,
		0.340969f,0.328489f,0.898686f,0.952345f,0.272572f,0.758995f,0.111269f,0.613403f,0.864397f,0.607601f,0.357317f,0.227619f,0.177081f,0.773828f,0.318257f,0.298335f,
		0.679382f,0.454625f,0.976745f,0.244511f,0.880111f,0.046238f,0.451342f,0.709265f,0.784123f,0.488338f,0.228713f,0.041251f,0.077453f,0.718891f,0.454221f,0.039182f,
		0.614777f,0.538681f,0.856650f,0.888921f,0.184013f,0.487999f,0.880338f,0.726824f,0.112945f,0.835710f,0.943366f,0.340094f,0.167909f,0.241240f,0.125953f,0.460130f,
		0.789923f,0.313898f,0.640780f,0.795920f,0.198025f,0.407344f,0.673839f,0.414326f,0.185900f,0.353436f,0.786795f,0.422102f,0.133975f,0.363270f,0.393833f,0.748760f,
		0.328130f,0.115681f,0.253865f,0.526924f,0.672761f,0.517447f,0.686442f,0.532847f,0.551176f,0.667406f,0.382640f,0.408796f,0.649460f,0.613948f,0.600470f,0.485404f,
	};

	#define RANDOM_TEXTURE_WIDTH 4
	#define NUM_DIRECTIONS 8

	int randomNumberIndex = 0;
	short *data = new short[RANDOM_TEXTURE_WIDTH*RANDOM_TEXTURE_WIDTH*4];
	for (uint i = 0; i < RANDOM_TEXTURE_WIDTH*RANDOM_TEXTURE_WIDTH*4; i += 4)
	{
		n_assert(randomNumberIndex < 1024);
		float r1 = MersenneTwisterNumbers[randomNumberIndex++];
		float r2 = MersenneTwisterNumbers[randomNumberIndex++];

		// Use random rotation angles in [0,2P/NUM_HBAO_DIRECTIONS).
		// This looks the same as sampling [0,2PI), but is faster.
		float angle = 2.0f * N_PI * r1 / NUM_DIRECTIONS;
		data[i  ] = (short)((1<<15)*cos(angle));
		data[i+1] = (short)((1<<15)*sin(angle));
		data[i+2] = (short)((1<<15)*r2);
		data[i+3] = 0;
	}

	// setup random texture
	this->randomTexture = CoreGraphics::Texture::Create();
	Ptr<MemoryTextureLoader> loader = MemoryTextureLoader::Create();
	loader->SetImageBuffer(data, RANDOM_TEXTURE_WIDTH, RANDOM_TEXTURE_WIDTH, PixelFormat::A16B16G16R16);
	this->randomTexture->SetLoader(loader.upcast<Resources::ResourceLoader>());
	this->randomTexture->SetResourceId("HBAORandomTexture");
	this->randomTexture->SetAsyncEnabled(false);
	this->randomTexture->Load();
	n_assert(this->randomTexture->IsLoaded());
	this->randomTexture->SetLoader(0);

	delete [] data;

	// set textures
	this->randomTextureVar->SetTexture(this->randomTexture);
#endif

	this->depthTextureVar->SetTexture(this->inputs[0]);

	// we need to late setup this algorithm since we need the camera
	this->deferredSetup = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
AOAlgorithm::Discard()
{
	AlgorithmBase::Discard();

	// unload shaders
	this->hbao->Discard();
	this->hbao = 0;
	this->blur->Discard();
	this->blur = 0;

	// unload render targets
    IndexT i;
    for (i = 0; i < 2; i++)
    {
        this->internalTargets[i]->Discard();
        this->internalTargets[i] = 0;
    }
	this->output->Discard();
	this->output = 0;

	// unload quad
	this->quad.Discard();

	// free variables
	this->depthTextureVar = 0;
#ifdef HBAO_COMPUTE
	this->hbao0Var = 0;
	this->hbao1Var = 0;
	this->hbaoBlur0Var = 0;
	this->hbaoBlur1Var = 0;
#else
	this->hbaoTextureVar = 0;
	this->randomTextureVar = 0;

	// unload random texture
	this->randomTexture->Unload();
	this->randomTexture = 0;

	this->rVar = 0;
	this->focalLengthVar = 0;
	this->maxRadiusPixelsVar = 0;
	this->negInvR2Var = 0;

#endif
	
	this->uvToViewAVar = 0;
	this->uvToViewBVar = 0;
	this->r2Var = 0;
	this->aoResolutionVar = 0;
	this->invAOResolutionVar = 0;
	this->strengthVar = 0;
	this->tanAngleBiasVar = 0;
	this->powerExponentVar = 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
AOAlgorithm::Prepare()
{
	if (this->deferredSetup)
	{
		// calculate variables
		this->Calculate();

		// set variables
		this->uvToViewAVar->SetFloat2(vars.uvToViewA);
		this->uvToViewBVar->SetFloat2(vars.uvToViewB);

#ifndef HBAO_COMPUTE
		this->rVar->SetFloat(vars.r);
		this->focalLengthVar->SetFloat2(vars.focalLength);
		this->maxRadiusPixelsVar->SetFloat(vars.maxRadiusPixels);
		this->negInvR2Var->SetFloat(vars.negInvR2);
#endif

		this->r2Var->SetFloat(vars.r2);
		this->aoResolutionVar->SetFloat2(vars.aoResolution);
		this->invAOResolutionVar->SetFloat2(vars.invAOResolution);
		this->strengthVar->SetFloat(vars.strength);
		this->tanAngleBiasVar->SetFloat(vars.tanAngleBias);

		// set blur vars
		this->blurFalloff->SetFloat(vars.blurFalloff);
		this->blurDepthThreshold->SetFloat(vars.blurThreshold);

		// we have performed our deferred setup, so set flag to false to avoid this happening again
		this->deferredSetup = false;
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
AOAlgorithm::Execute()
{
	// if enabled, render AO
	if (this->enabled)
	{
		RenderDevice* renderDevice = RenderDevice::Instance();
		ShaderServer* shaderServer = ShaderServer::Instance();
	

#ifdef HBAO_COMPUTE
#define TILE_WIDTH 320
		// calculate execution dimensions
		uint numGroupsX1 = DivAndRoundUp(this->output->GetWidth(), TILE_WIDTH);
		uint numGroupsX2 = this->output->GetWidth();
		uint numGroupsY1 = DivAndRoundUp(this->output->GetHeight(), TILE_WIDTH);
		uint numGroupsY2 = this->output->GetHeight();       

		// render AO in X
		this->hbao->SelectActiveVariation(this->xDirection);
		this->hbao->Begin();
		this->hbao->BeginPass(0);
		this->depthTextureVar->SetTexture(this->inputs[0]);
		this->hbao0Var->SetTexture(this->internalTargets[0]->GetResolveTexture());
		this->hbao->Commit();
		renderDevice->Compute(numGroupsX1, numGroupsY2, 1);
		this->hbao->PostDraw();
		this->hbao->EndPass();
		this->hbao->End();

#else

		// first render ao
		shaderServer->SetActiveShaderInstance(this->hbao);
		renderDevice->BeginPass(this->internalTargets[0], this->hbao);
		this->quad.Draw();
		renderDevice->EndPass();

		// now render first x-blur
		//this->blur->SelectActiveVariation(this->xBlurShaderFeature);
		this->hbaoTextureVar->SetTexture(this->internalTargets[0]->GetResolveTexture());
		renderDevice->BeginPass(this->output, this->blur);
		this->quad.Draw();
		renderDevice->EndPass();
#endif


		/*
		// render AO in Y
		this->hbao->SelectActiveVariation(this->yDirection);
		this->hbao->Begin();
		this->hbao->BeginPass(0);
		this->hbao0Var->SetTexture(this->internalTargets[0]->GetResolveTexture());
		this->hbao1Var->SetTexture(this->internalTargets[1]->GetResolveTexture());
		this->hbao->Commit();
		renderDevice->Compute(numGroupsY1, numGroupsX2, 1);
		this->hbao->PostDraw();
		this->hbao->EndPass();
		this->hbao->End();
		*/

		/*
		// set output
		this->hbaoBlur1Var->SetTexture(this->output->GetResolveTexture());

		this->blur->SelectActiveVariation(this->xDirection);
		this->blur->Begin();
        this->blur->BeginPass(0);        
		this->hbaoBlur0Var->SetTexture(this->internalTargets[1]->GetResolveTexture());
        this->blur->Commit();		
        renderDevice->Compute(numGroupsX1, numGroupsY2, 1);
        this->blur->PostDraw();
        this->blur->EndPass();
        this->blur->End();		

		this->blur->SelectActiveVariation(this->yDirection);
		this->blur->Begin();
		this->blur->BeginPass(0);
		this->hbaoBlur0Var->SetTexture(this->internalTargets[1]->GetResolveTexture());
		this->blur->Commit();
		renderDevice->Compute(numGroupsY1, numGroupsX2, 1);
		this->blur->PostDraw();
		this->blur->EndPass();
		this->blur->End();
		*/
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
AOAlgorithm::OnDisplayResized(SizeT width, SizeT height)
{
    // resize internal targets
    IndexT i;
    for (i = 0; i < 2; i++)
    {
        this->internalTargets[i]->Clear(RenderTarget::ClearColor);
        this->internalTargets[i]->OnDisplayResized(width, height);
    }

	// clear output and reset it
    this->output->Clear(RenderTarget::ClearColor);
	this->output->OnDisplayResized(width, height);

#ifndef HBAO_COMPUTE
	// since our textures have changed, set them in the variable again
    this->randomTextureVar->SetTexture(this->randomTexture);
#endif

	this->depthTextureVar->SetTexture(this->inputs[0]);

	// calculate new dimensions for render target
	this->fullWidth = (float)output->GetWidth();
	this->fullHeight = (float)output->GetHeight();

    // resize quad
    this->quad.Discard();
    this->quad.Setup((SizeT)this->fullWidth, (SizeT)this->fullHeight);

	// calculate the maximum pixel radius with the new dimensions
	vars.maxRadiusPixels = MAX_RADIUS_PIXELS * n_min(this->fullWidth, this->fullHeight);

	// trigger a new setup
	this->deferredSetup = true;
}

//------------------------------------------------------------------------------
/**
*/
void 
AOAlgorithm::Calculate()
{
	// get camera settings
	const CameraSettings& cameraSettings = Graphics::GraphicsServer::Instance()->GetCurrentView()->GetCameraEntity()->GetCameraSettings();

	this->width = this->fullWidth / downSample;
	this->height = this->fullHeight / downSample;

	this->nearZ = cameraSettings.GetZNear();
	this->farZ = cameraSettings.GetZFar();

	this->sceneScale = 1.0f;
	vars.r = this->radius * 4.0f / 100.0f;
	vars.r2 = vars.r * vars.r;
	vars.negInvR2 = -1.0f / vars.r2;

	vars.aoResolution.x() = this->fullWidth;
	vars.aoResolution.y() = this->fullHeight;
	vars.invAOResolution.x() = 1.0f / this->width;
	vars.invAOResolution.y() = 1.0f / this->height;

	float fov = cameraSettings.GetFov();
	vars.focalLength.x() = 1.0f / tanf(fov * 0.5f) * (this->fullHeight / this->fullWidth);
	vars.focalLength.y() = 1.0f / tanf(fov * 0.5f);

	float2 invFocalLength;
	invFocalLength.x() = 1 / vars.focalLength.x();
	invFocalLength.y() = 1 / vars.focalLength.y();

	vars.uvToViewA.x() = 2.0f * invFocalLength.x();
	vars.uvToViewA.y() = -2.0f * invFocalLength.y();
	vars.uvToViewB.x() = -1.0f * invFocalLength.x();
	vars.uvToViewB.y() = 1.0f * invFocalLength.y();

#ifndef INV_LN2
#define INV_LN2 1.44269504f
#endif

#ifndef SQRT_LN2
#define SQRT_LN2 0.832554611f
#endif

#define BLUR_RADIUS 33
#define BLUR_SHARPNESS 8.0f

	float blurSigma = (BLUR_RADIUS + 1) * 0.5f;
	vars.blurFalloff = INV_LN2 / (2.0f * blurSigma * blurSigma);
	vars.blurThreshold = 2.0f * SQRT_LN2 * (this->sceneScale / BLUR_SHARPNESS);
}

//------------------------------------------------------------------------------
/**
*/
bool 
AOAlgorithm::HandleMessage( const Ptr<Messaging::Message>& msg )
{
	if (msg->CheckId(SetAmbientOcclusionParams::Id))
	{
		Ptr<SetAmbientOcclusionParams> rMsg = msg.downcast<SetAmbientOcclusionParams>();

		// update variables set directly
		this->strengthVar->SetFloat(rMsg->GetStrength());
		this->tanAngleBiasVar->SetFloat(tanf(n_deg2rad(rMsg->GetAngleBias())));
		this->powerExponentVar->SetFloat(rMsg->GetPowerExponent());		
        this->radius = rMsg->GetRadius();		
		
		// radius needs the r, r2 and NegInvR2 to be recalculated
		this->Calculate();

		// set the newly calculated variables
#ifndef HBAO_COMPUTE
		this->rVar->SetFloat(vars.r);
		this->negInvR2Var->SetFloat(vars.negInvR2);
#endif
		this->r2Var->SetFloat(vars.r2);

		return true;
	}
	else if (msg->CheckId(EnableAmbientOcclusion::Id))
	{
		Ptr<EnableAmbientOcclusion> rMsg = msg.downcast<EnableAmbientOcclusion>();
		this->SetEnabled(rMsg->GetEnabled());
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
AOAlgorithm::Disable()
{
    // clear output
    this->output->Clear(RenderTarget::ClearColor);
}

} // namespace Algorithm