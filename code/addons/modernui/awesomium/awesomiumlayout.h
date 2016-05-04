// ------------------------------------------------------------------------------------------------------------------------
// 
// File:  awesomiumlayout.h
// 
// Author: Timmy Silesmo
// Creation Date: 10:55 PM - 03/05/2016
// 
// Contact: Timmy.Silesmo@gmail.com
// 
// ------------------------------------------------------------------------------------------------------------------------
#pragma once

#include "core/types.h"
#include "math/float4.h"

#include <Awesomium/WebCore.h>
#include <Awesomium/WebURL.h>
#include <Awesomium/BitmapSurface.h>

#include "core/rttimacros.h"
#include "core/refcounted.h"
#include <coregraphics/vertexbuffer.h>

namespace AwesomiumUI
{
	class AwesomiumServer;

	struct NebulaGeometry
	{
		Ptr<CoreGraphics::VertexBuffer> vb;
		Ptr<CoreGraphics::IndexBuffer> ib;
		Ptr<CoreGraphics::Texture> texture;
		CoreGraphics::PrimitiveGroup primGroup;
	};

	class AwesomiumLayout : public Core::RefCounted
	{
		__DeclareClass(AwesomiumLayout);

		friend class AwesomiumServer;
	public:
		AwesomiumLayout();
		void Setup(Awesomium::WebView* view, uint width, uint height);
		~AwesomiumLayout();

		/// returns true if the layout is visible
		bool IsVisible() const;
		/// shows layout
		void Show();
		/// hide layout
		void Hide();
		/// set size of layout
		void SetSize(const Math::float2& size);
		/// updates
		void Update();
		/// returns true if the layout is loaded
		bool IsLoaded() const;
		/// loads the specified url. use file://C:/ChickenNoodles for local paths
		void LoadURL(const Util::String& URL) const;
		/// returns true if the layout has focus
		bool HasFocus() const;
		/// set focus or disable focus on layout
		void SetFocus(bool focus);

		Awesomium::BitmapSurface* GetSurface() const;

		const Math::float4& GetPosition() const;
		NebulaGeometry* GetGeometry();

	private:
		struct NebulaVertex
		{
			float x, y;
			byte r, g, b, a;
			float u, v;
		};

		void GenerateTexture();
		void GenerateMesh();
		

		Awesomium::WebView* view;
		NebulaGeometry* geometry;
		Math::float4 position;
		uint width;
		uint height;
		Math::float2 dimentions;

		bool hasFocus;
		bool visible;
	};

	inline const Math::float4& AwesomiumLayout::GetPosition() const
	{
		return this->position;
	}

	inline NebulaGeometry* AwesomiumLayout::GetGeometry()
	{
		return this->geometry;
	}

	inline bool AwesomiumLayout::IsVisible() const
	{
		return this->visible;
	}

	inline void AwesomiumLayout::Show()
	{
		this->visible = true;
	}

	inline void AwesomiumLayout::Hide()
	{
		this->visible = false;
	}
}