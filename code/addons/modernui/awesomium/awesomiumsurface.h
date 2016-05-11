// ------------------------------------------------------------------------------------------------------------------------
// 
// File:  AwesomiumSurface.h
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
#include "coregraphics/texture.h"
#include "Awesomium\BitmapSurface.h"

namespace AwesomiumUI
{
class AwesomiumSurface : public Awesomium::BitmapSurface
{
public:
	AwesomiumSurface(int width, int height);
	~AwesomiumSurface();

	Ptr<CoreGraphics::Texture>& GetTexture();

	virtual void Paint(unsigned char* src_buffer, int src_row_span, const Awesomium::Rect& src_rect, const Awesomium::Rect& dest_rect) override;

	virtual void Scroll(int dx, int dy, const Awesomium::Rect& clip_rect) override;

private:
	Ptr<CoreGraphics::Texture> texture;
};

inline Ptr<CoreGraphics::Texture>& AwesomiumSurface::GetTexture()
{
	return this->texture;
}

}