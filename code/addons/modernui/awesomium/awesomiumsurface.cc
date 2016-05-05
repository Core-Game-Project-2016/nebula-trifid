#include "stdneb.h"
#include "awesomiumsurface.h"
#include "coregraphics\memorytextureloader.h"
#include "resources\resourcemanager.h"

namespace AwesomiumUI
{
	AwesomiumSurface::AwesomiumSurface(int width, int height)
	{
		static int channels = 4;

		this->texture = CoreGraphics::Texture::Create();
		Ptr<CoreGraphics::MemoryTextureLoader> loader = CoreGraphics::MemoryTextureLoader::Create();

		loader->SetImageBuffer(nullptr, width, height, CoreGraphics::PixelFormat::SRGBA8);
		this->texture->SetLoader(loader.upcast<Resources::ResourceLoader>());
		this->texture->SetAsyncEnabled(false);
		this->texture->Load();
		n_assert(this->texture->IsLoaded());
		this->texture->SetLoader(0);
	}

	AwesomiumSurface::~AwesomiumSurface()
	{
		this->texture = 0;
	}

	void AwesomiumSurface::Paint(unsigned char* src_buffer, int src_row_span, const Awesomium::Rect& src_rect, const Awesomium::Rect& dest_rect)
	{
		//Copy Selected Rectangle width start from source origin to start of destination origin
		this->texture->Update(&src_buffer[src_rect.y * src_row_span + src_rect.x], src_row_span * src_rect.height, dest_rect.width, dest_rect.height, dest_rect.x, dest_rect.y, 0);
	}

	void AwesomiumSurface::Scroll(int dx, int dy, const Awesomium::Rect& clip_rect)
	{
		//TODO Implement.
		//Offset Pixels in texture by dx and dy with the origin of clip_rect.xy
	}

}

