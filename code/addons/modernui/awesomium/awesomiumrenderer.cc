#include "stdneb.h"
#include "awesomiumrenderer.h"
#include "awesomiumlayout.h"

#include "coregraphics/vertexcomponent.h"
#include "coregraphics/memoryvertexbufferloader.h"
#include "coregraphics/memoryindexbufferloader.h"
#include "coregraphics/renderdevice.h"
#include "coregraphics/memorytextureloader.h"
#include "coregraphics/texture.h"
#include "coregraphics/shaderserver.h"

#include "resources/managedtexture.h"
#include "resources/resourcemanager.h"

using namespace Math;
using namespace CoreGraphics;
using namespace Resources;
namespace AwesomiumUI
{
	__ImplementClass(AwesomiumUI::AwesomiumRenderer, 'AWRE', Core::RefCounted);

AwesomiumRenderer::AwesomiumRenderer()
{
	// get shader server
	Ptr<ShaderServer> shaderServer = ShaderServer::Instance();

	// set render device
	this->renderDevice = RenderDevice::Instance();

	// get shader and create instance
	this->shader = shaderServer->GetShader("shd:modernui");

	// get texture
	this->diffMap = this->shader->GetVariableByName("Texture");
	this->modelVar = this->shader->GetVariableByName("Model");

	this->whiteTexture = ResourceManager::Instance()->CreateManagedResource(Texture::RTTI, "tex:system/white.dds").downcast<ManagedTexture>();
}

AwesomiumRenderer::~AwesomiumRenderer()
{

}

void AwesomiumRenderer::Render(AwesomiumLayout* view)
{
	//Render View
	Ptr<RenderDevice> device = RenderDevice::Instance();
	Ptr<ShaderServer> shaderServer = ShaderServer::Instance();


	NebulaGeometry* geometry = view->GetGeometry();

	if (geometry)
	{
		try
		{
			Ptr<Texture> texture = view->GetSurface()->GetTexture();
			if (texture.isvalid())
			{
				this->diffMap->SetTexture(texture);
			}
		}
		catch (...) // TODO REMOVE UGLY TEMP FIX
		{
			this->diffMap->SetTexture(this->whiteTexture->GetTexture());
		}

		// apply shader
		shaderServer->SetActiveShader(this->shader);
		this->shader->Apply();

		// get dimensions
		//Rocket::Core::Vector2i dimensions = AwesomiumRenderer::Instance()->GetContext()->GetDimensions();

		matrix44 world = matrix44::translation(view->GetPosition());
		matrix44 scale = matrix44::scaling(float4(2.0f, -2.0f, 1, 1));
		//matrix44 scale = matrix44::scaling(float4(2.0f / (float)dimensions.x, -2.0f / (float)dimensions.y, 1, 1));
		matrix44 trans = matrix44::translation(float4(-1, 1, 0, 0));

		// combine matrices and set in shader
		world = matrix44::multiply(matrix44::multiply(world, scale), trans);
		this->shader->BeginUpdate();
		this->modelVar->SetMatrix(world);
		this->shader->EndUpdate();

		// commit shader
		this->shader->Commit();

		// setup render device and draw
		device->SetVertexLayout(geometry->vb->GetVertexLayout());
		device->SetIndexBuffer(geometry->ib);
		device->SetStreamSource(0, geometry->vb, 0);
		device->SetPrimitiveGroup(geometry->primGroup);
		device->Draw();
	}
}

}