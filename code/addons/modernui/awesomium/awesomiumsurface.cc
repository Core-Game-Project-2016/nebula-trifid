#include "stdneb.h"
#include "awesomiumsurface.h"
#include "coregraphics\memorytextureloader.h"
#include "resources\resourcemanager.h"

namespace AwesomiumUI
{
	AwesomiumSurface::AwesomiumSurface(int width, int height)
	{
		this->scissor = Math::rectangle<int>(0, 0, width, height);

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
		this->texture = 0;
	}

	void AwesomiumSurface::Paint(unsigned char* src_buffer, int src_row_span, const Awesomium::Rect& src_rect, const Awesomium::Rect& dest_rect)
	{
		//Copy Selected Rectangle width start from source origin to start of destination origin
		static int numChannels = 4;

		//Prevent displaying corrupt textures
		if ((src_row_span / src_rect.width) == 4)
		{
			//Copy Rectangle
			char* buffer = n_new_array(char, dest_rect.width * dest_rect.height * 4);
			for (int i = 0; i < src_rect.height; i++)
			{
				memcpy(	&buffer[i * src_rect.width * numChannels],
						&src_buffer[(((src_rect.y + i) * src_rect.width) + src_rect.x) * numChannels],
						src_rect.width * numChannels);
			}

			this->texture->Update(buffer, dest_rect.width * dest_rect.height, dest_rect.width, dest_rect.height, dest_rect.x, dest_rect.y, 0);
			n_delete_array(buffer);

			//this->texture->Update(&src_buffer[src_rect.y * src_rect.width + src_rect.x], dest_rect.width * dest_rect.height, dest_rect.width, dest_rect.height, dest_rect.x, dest_rect.y, 0);
			//this->texture->Update(src_buffer, dest_rect.width * dest_rect.height, dest_rect.width, dest_rect.height, dest_rect.x, dest_rect.y, 0);

			this->scissor.set(dest_rect.x, dest_rect.y, dest_rect.x + dest_rect.width, dest_rect.y + dest_rect.height);
		}
	}

	void AwesomiumSurface::Scroll(int dx, int dy, const Awesomium::Rect& clip_rect)
	{
		//Offset Pixels in texture by dx and dy with the origin of clip_rect.xy
		this->scissor.set(dx, dy, dx + clip_rect.x + clip_rect.width, dy + clip_rect.y + clip_rect.height);
	}

}

