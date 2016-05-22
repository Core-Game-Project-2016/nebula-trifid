#include "stdneb.h"
#include "awesomiumsurface.h"
#include "coregraphics\memorytextureloader.h"
#include "resources\resourcemanager.h"

namespace AwesomiumUI
{
	AwesomiumSurface::AwesomiumSurface(int width, int height) : Awesomium::BitmapSurface(width, height)
	{
		this->texture = CoreGraphics::Texture::Create();
		Ptr<CoreGraphics::MemoryTextureLoader> loader = CoreGraphics::MemoryTextureLoader::Create();

		loader->SetImageBuffer(nullptr, width, height, CoreGraphics::PixelFormat::SRGBA8);
		this->texture->SetLoader(loader.upcast<Resources::ResourceLoader>());
		this->texture->SetAsyncEnabled(false);
		this->texture->Load();
		n_assert(this->texture->IsLoaded());
		//this->texture->SetResourceId("AwesomiumUI");
		//Resources::ResourceManager::Instance()->RegisterUnmanagedResource(this->texture.upcast<Resources::Resource>());
		this->texture->SetLoader(0);
	}

	AwesomiumSurface::~AwesomiumSurface()
	{
		this->texture->Unload();
		Awesomium::BitmapSurface::~BitmapSurface();
	}

	void AwesomiumSurface::Paint(unsigned char* src_buffer, int src_row_span, const Awesomium::Rect& src_rect, const Awesomium::Rect& dest_rect)
	{
		Awesomium::BitmapSurface::Paint(src_buffer, src_row_span, src_rect, dest_rect);
		if (Awesomium::BitmapSurface::is_dirty())
			this->texture->Update(const_cast<unsigned char*>(Awesomium::BitmapSurface::buffer()), Awesomium::BitmapSurface::width() * Awesomium::BitmapSurface::height(), Awesomium::BitmapSurface::width(), Awesomium::BitmapSurface::height(), 0, 0, 0);
	}

	void AwesomiumSurface::Scroll(int dx, int dy, const Awesomium::Rect& clip_rect)
	{
		Awesomium::BitmapSurface::Scroll(dx, dy, clip_rect);
	}

}

