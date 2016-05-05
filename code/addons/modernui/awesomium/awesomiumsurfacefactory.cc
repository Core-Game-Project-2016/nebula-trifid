#include "stdneb.h"
#include "awesomiumsurfacefactory.h"
#include "awesomiumsurface.h"

#include "Awesomium/WebCore.h"
#include "Awesomium/WebView.h"

namespace AwesomiumUI
{

	Awesomium::Surface* AwesomiumSurfaceFactory::CreateSurface(Awesomium::WebView* view, int width, int height)
	{
		return new AwesomiumUI::AwesomiumSurface(width, height);
	}

	void AwesomiumSurfaceFactory::DestroySurface(Awesomium::Surface* surface)
	{
		delete surface;
	}

}