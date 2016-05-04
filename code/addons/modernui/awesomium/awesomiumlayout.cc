#include "stdneb.h"
#include "awesomiumlayout.h"
#include "awesomiumserver.h"
#include <coregraphics/memorytextureloader.h>
#include "resources/resourcemanager.h"
#include <coregraphics/memoryvertexbufferloader.h>
#include <coregraphics/memoryindexbufferloader.h>
#include <coregraphics/vertexcomponent.h>

#include "Awesomium/BitmapSurface.h"
#include "Awesomium/STLHelpers.h"

namespace AwesomiumUI
{
	__ImplementClass(AwesomiumUI::AwesomiumLayout, 'AWLA', Core::RefCounted);

AwesomiumLayout::AwesomiumLayout()
{
	
}

void AwesomiumLayout::GenerateTexture()
{
	Awesomium::BitmapSurface* surface = this->GetSurface();

	if (surface)
	{
		this->geometry->texture = CoreGraphics::Texture::Create();
		Ptr<CoreGraphics::MemoryTextureLoader> loader = CoreGraphics::MemoryTextureLoader::Create();

		int width = surface->width();
		int height = surface->height();
		int channels = 4;

		unsigned char* data = new unsigned char[width * height * channels];
		surface->CopyTo(data, width * 4, channels, true, false);

		loader->SetImageBuffer(data, width, height, CoreGraphics::PixelFormat::SRGBA8);
		this->geometry->texture->SetLoader(loader.upcast<Resources::ResourceLoader>());
		this->geometry->texture->SetAsyncEnabled(false);
		this->geometry->texture->Load();
		n_assert(this->geometry->texture->IsLoaded());
		this->geometry->texture->SetResourceId("AwesomiumUI");
		Resources::ResourceManager::Instance()->RegisterUnmanagedResource(this->geometry->texture.upcast<Resources::Resource>());
		this->geometry->texture->SetLoader(0);

		delete[] data;
	}
}

void AwesomiumLayout::GenerateMesh()
{
	this->geometry = static_cast<NebulaGeometry*>(Alloc(Memory::DefaultHeap, sizeof(NebulaGeometry)));
	Memory::Clear(this->geometry, sizeof(NebulaGeometry));

	// set texture
	//this->GenerateTexture();

	// create vertex buffer
	this->geometry->vb = CoreGraphics::VertexBuffer::Create();

	Util::Array<CoreGraphics::VertexComponent> vertexComponents;
	vertexComponents.Append(CoreGraphics::VertexComponent(static_cast<CoreGraphics::VertexComponent::SemanticName>(0), 0, CoreGraphics::VertexComponent::Float2));     // position
	vertexComponents.Append(CoreGraphics::VertexComponent(static_cast<CoreGraphics::VertexComponent::SemanticName>(1), 0, CoreGraphics::VertexComponent::UByte4N));    // color
	vertexComponents.Append(CoreGraphics::VertexComponent(static_cast<CoreGraphics::VertexComponent::SemanticName>(2), 0, CoreGraphics::VertexComponent::Float2));     // UV

	int indices[] = 
	{ 
		0, 1, 2,		
		2, 3, 0 
	};

	NebulaVertex vertices[4] =
	{
		{  0, 0,	255, 255, 255, 255,		0, 0 },
		{  0, 1,	255, 255, 255, 255,		0, 1 },
		{  1, 1,	255, 255, 255, 255,		1, 1 },
		{  1, 0,	255, 255, 255, 255,		1, 0 }
	};

	int numVertices = 4;
	int numIndices = 6;

	// create loader for vertex buffer
	Ptr<CoreGraphics::MemoryVertexBufferLoader> vbLoader = CoreGraphics::MemoryVertexBufferLoader::Create();
	vbLoader->Setup(vertexComponents,
		numVertices,
		vertices,
		numVertices * sizeof(float) * 4 + numVertices * sizeof(unsigned char) * 4,
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

void
	AwesomiumLayout::Setup(Awesomium::WebView* view, uint width, uint height)
{
	this->visible = true;
	this->view = view;
	this->width = width;
	this->height = height;
	this->hasFocus = false;
	this->position = Math::float4(0.0f, 0.0f, 0.0f, 1.0f);
	this->GenerateMesh();
}

AwesomiumLayout::~AwesomiumLayout()
{
	this->view->Destroy();
}

void AwesomiumLayout::Update()
{
	Awesomium::BitmapSurface* surface = this->GetSurface();
	static bool hack = false;
	if (surface && !hack)
	{
		GenerateTexture();
		hack = true;
	}
	else if (hack)
	{
		Awesomium::BitmapSurface* surface = this->GetSurface();

		if (surface)
		{
			Ptr<CoreGraphics::MemoryTextureLoader> loader = CoreGraphics::MemoryTextureLoader::Create();

			int width = surface->width();
			int height = surface->height();
			int channels = 4;

			unsigned char* data = new unsigned char[width * height * channels];
			surface->CopyTo(data, width * 4, channels, true, false);

			this->geometry->texture->Update(data, width * height * channels, width, height, 0, 0, 0);
			delete[] data;
		}
	}
	if(this->hasFocus)
	{
		//Inject Moude/Keyboard Events

		//this->view->InjectKeyboardEvent();

		//this->view->InjectMouseDown();
		//this->view->InjectMouseUp();
		//this->view->InjectMouseMove();
		//this->view->InjectMouseWheel();
	}
}

bool AwesomiumLayout::IsLoaded() const
{
	return !this->view->IsLoading();
}

void AwesomiumLayout::LoadURL(const Util::String& URL) const
{
	Awesomium::WebURL url(Awesomium::WSLit(URL.Get()));

	if (url.IsValid())
	{
		this->view->LoadURL(url);
	}
}

Awesomium::BitmapSurface* AwesomiumLayout::GetSurface() const
{
	return static_cast<Awesomium::BitmapSurface*>(this->view->surface());
}

bool AwesomiumLayout::HasFocus() const
{
	return this->hasFocus;
}

void AwesomiumLayout::SetFocus(bool focus)
{
	this->hasFocus = focus;

	if (focus)
		this->view->Focus();
	else
		this->view->Unfocus();
}

void AwesomiumLayout::SetSize(const Math::float2& size)
{
	this->dimentions = size;
}

}
