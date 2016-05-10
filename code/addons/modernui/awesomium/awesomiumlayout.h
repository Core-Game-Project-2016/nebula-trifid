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

#include "core/rttimacros.h"
#include "core/refcounted.h"
#include <coregraphics/vertexbuffer.h>
#include "awesomiumsurface.h"
#include "input/inputevent.h"
#include "awesomiumjsmethodhandler.h"
#include "Awesomium\STLHelpers.h"

namespace AwesomiumUI
{
	class AwesomiumServer;

	struct NebulaGeometry
	{
		Ptr<CoreGraphics::VertexBuffer> vb;
		Ptr<CoreGraphics::IndexBuffer> ib;
		CoreGraphics::PrimitiveGroup primGroup;
	};

	class AwesomiumLayout : public Core::RefCounted
	{
		__DeclareClass(AwesomiumLayout);

		friend class AwesomiumServer;
		friend class AwesomiumJSMethodHandler;
	public:
		typedef void(*CallbackFunction)(Awesomium::WebView* caller, const Awesomium::JSArray& args);
		typedef Awesomium::JSValue(*CallbackReturnFunction)(Awesomium::WebView* caller, const Awesomium::JSArray& args);


		AwesomiumLayout();
		void Setup(Awesomium::WebView* view);
		~AwesomiumLayout();

		/// returns true if the layout is visible
		bool IsVisible() const;
		/// shows layout
		void Show();
		/// hide layout
		void Hide();
		/// set size of layout
		void Resize(SizeT width, SizeT height);
		/// returns true if the layout is loaded
		bool IsLoaded() const;
		/// loads the specified url. use file://C:/ChickenNoodles for local paths
		void LoadURL(const Util::String& URL) const;
		/// returns true if the layout has focus
		bool HasFocus() const;
		/// set focus or disable focus on layout
		void SetFocus(bool focus);

		void HandleInput(const Input::InputEvent& inputEvent);

		void CreateGlobalJSObject(const Util::String& name);

		void RegisterFunctionCallback(const Util::String& objectname, Util::String functionName, CallbackFunction function);
		void RegisterFunctionReturnCallback(const Util::String& objectname, Util::String functionName, CallbackReturnFunction function);

		AwesomiumSurface* GetSurface() const;

		const Math::float4& GetPosition() const;
		NebulaGeometry* GetGeometry();

		AwesomiumJSMethodHandler* GetMethodHandler()
		{
			return this->methodHandler;
		}

	private:
		struct NebulaVertex
		{
			float x, y;
			float u, v;
		};

		void GenerateTexture();
		void GenerateMesh();
		
		Util::Dictionary<Util::String, Awesomium::JSObject> objects;

		Util::Dictionary<int, Util::Dictionary<Util::String, CallbackFunction>> callbackFunctions;
		Util::Dictionary<int, Util::Dictionary<Util::String, CallbackReturnFunction>> callbackReturnFunctions;

		Awesomium::WebView* view;
		AwesomiumJSMethodHandler* methodHandler;

		NebulaGeometry* geometry;
		Math::float4 position;

		bool hasFocus;
		bool visible;
	};


	inline void AwesomiumLayout::CreateGlobalJSObject(const Util::String& name)
	{
		Awesomium::JSObject object = this->view->CreateGlobalJavascriptObject(Awesomium::WSLit(name.Get())).ToObject();
		this->objects.Add(name, object);
	}

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