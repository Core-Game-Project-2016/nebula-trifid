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
	this->uiShader = shaderServer->GetShader("shd:modernui");
	this->hologramShader = shaderServer->GetShader("shd:hologram");

	this->uiDiffMap = this->uiShader->GetVariableByName("Texture");
	this->uiModelVar = this->uiShader->GetVariableByName("Model");

	this->hologramDiffMap = this->hologramShader->GetVariableByName("Texture");
	this->hologramModelVar = this->hologramShader->GetVariableByName("Model");
	this->hologramViewProjVar = this->hologramShader->GetVariableByName("ViewProjection");
	this->GenerateMesh();
}

AwesomiumRenderer::~AwesomiumRenderer()
{

}

void AwesomiumRenderer::GenerateMesh()
{
	this->geometry = static_cast<NebulaGeometry*>(Alloc(Memory::DefaultHeap, sizeof(NebulaGeometry)));
	Memory::Clear(this->geometry, sizeof(NebulaGeometry));

	// create vertex buffer
	this->geometry->vb = CoreGraphics::VertexBuffer::Create();

	Util::Array<CoreGraphics::VertexComponent> vertexComponents;
	vertexComponents.Append(CoreGraphics::VertexComponent(static_cast<CoreGraphics::VertexComponent::SemanticName>(0), 0, CoreGraphics::VertexComponent::Float2));     // position
	vertexComponents.Append(CoreGraphics::VertexComponent(static_cast<CoreGraphics::VertexComponent::SemanticName>(2), 0, CoreGraphics::VertexComponent::Float2));     // UV

	int indices[] =
	{
		0, 1, 2,
		2, 3, 0
	};

	NebulaVertex vertices[4] =
	{
		{ 0, 0, 0, 0 },
		{ 0, 1, 0, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 0, 1, 0 }
	};

	int numVertices = 4;
	int numIndices = 6;

	// create loader for vertex buffer
	Ptr<CoreGraphics::MemoryVertexBufferLoader> vbLoader = CoreGraphics::MemoryVertexBufferLoader::Create();
	vbLoader->Setup(vertexComponents,
		numVertices,
		vertices,
		numVertices * sizeof(float) * 4,
		Base::ResourceBase::UsageImmutable,
		Base::ResourceBase::AccessNone);
	this->geometry->vb->SetLoader(vbLoader.upcast<Resources::ResourceLoader>());
	this->geometry->vb->SetAsyncEnabled(false);
	this->geometry->vb->Load();
	n_assert(this->geometry->vb->IsLoaded());
	this->geometry->vb->SetLoader(0);

	// create index buffer
	this->geometry->ib = CoreGraphics::IndexBuffer::Create();

	// create loader for index buffer
	Ptr<CoreGraphics::MemoryIndexBufferLoader> ibLoader = CoreGraphics::MemoryIndexBufferLoader::Create();
	ibLoader->Setup(CoreGraphics::IndexType::Index32,
		numIndices,
		indices,
		numIndices * sizeof(int),
		Base::ResourceBase::UsageImmutable,
		Base::ResourceBase::AccessNone);
	this->geometry->ib->SetLoader(ibLoader.upcast<Resources::ResourceLoader>());
	this->geometry->ib->SetAsyncEnabled(false);
	this->geometry->ib->Load();
	n_assert(this->geometry->ib->IsLoaded());
	this->geometry->ib->SetLoader(0);

	this->geometry->primGroup.SetBaseIndex(0);
	this->geometry->primGroup.SetNumVertices(numVertices);
	this->geometry->primGroup.SetBaseVertex(0);
	this->geometry->primGroup.SetNumIndices(numIndices);
	this->geometry->primGroup.SetPrimitiveTopology(CoreGraphics::PrimitiveTopology::TriangleList);
}

void AwesomiumRenderer::Render(AwesomiumLayout* view)
{
	//Render View
	Ptr<RenderDevice> device = RenderDevice::Instance();
	Ptr<ShaderServer> shaderServer = ShaderServer::Instance();

	switch (view->GetType())
	{
		case UIType::Hologram:
		{
			AwesomiumSurface* surface = view->GetSurface();
			if (surface)
			{
				this->hologramDiffMap->SetTexture(surface->GetTexture());
			}

			// apply shader
			shaderServer->SetActiveShader(this->hologramShader);
			this->hologramShader->Apply();

			const matrix44& viewProjection = Graphics::GraphicsServer::Instance()->GetCurrentView()->GetCameraEntity()->GetCameraSettings().GetViewProjTransform();

			this->hologramShader->BeginUpdate();
			this->hologramViewProjVar->SetMatrix(viewProjection);
			this->hologramModelVar->SetMatrix(view->GetMatrix());
			this->hologramShader->EndUpdate();

			// commit shader
			this->hologramShader->Commit();
			break;
		}
		case UIType::UI:
		{
			AwesomiumSurface* surface = view->GetSurface();
			if (surface)
			{
				this->uiDiffMap->SetTexture(surface->GetTexture());
			}

			// apply shader
			shaderServer->SetActiveShader(this->uiShader);
			this->uiShader->Apply();

			matrix44 world = matrix44::translation(view->GetPosition());
			matrix44 trans = matrix44::translation(float4(-1.0f, 1.0f, 0.0f, 0.0f));
			matrix44 scale = matrix44::scaling(float4(2.0f, -2.0f, 1, 1));

			world = matrix44::multiply(matrix44::multiply(world, scale), trans);

			this->uiShader->BeginUpdate();
			this->uiModelVar->SetMatrix(world);
			this->uiShader->EndUpdate();

			// commit shader
			this->uiShader->Commit();
			break;
		}
		default:
			break;
	}

	// setup render device and draw
	device->SetVertexLayout(this->geometry->vb->GetVertexLayout());
	device->SetIndexBuffer(this->geometry->ib);
	device->SetStreamSource(0, this->geometry->vb, 0);
	device->SetPrimitiveGroup(this->geometry->primGroup);
	device->Draw();
}

}