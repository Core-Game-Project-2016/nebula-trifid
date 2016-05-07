// ------------------------------------------------------------------------------------------------------------------------
// 
// File:  AwesomiumSurfaceFactory.h
// 
// Author: Timmy Silesmo
// Creation Date: 9:38 PM - 03/05/2016
// 
// Contact: Timmy.Silesmo@gmail.com
// 
// Copyright (C) 2016
// All rights reserved.
// 
// ------------------------------------------------------------------------------------------------------------------------
#pragma once

#include "Awesomium/Surface.h"

namespace AwesomiumUI
{
class AwesomiumSurfaceFactory : public Awesomium::SurfaceFactory
{
public:
	virtual Awesomium::Surface* CreateSurface(Awesomium::WebView* view, int width, int height) override;


	virtual void DestroySurface(Awesomium::Surface* surface) override;

private:
};
}
