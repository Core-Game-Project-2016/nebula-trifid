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

	enum class UIType
	{
		UI,
		Hologram
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
		void Setup(Awesomium::WebView* view, UIType type);
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
		/// loads the specified url.
		void LoadURL(const IO::URI& URL) const;
		/// returns true if the layout has focus
		bool HasFocus() const;
		/// set focus or disable focus on layout
		void SetFocus(bool focus);

		/// default objects/functions for JS are stored in "window"
		void InvokeJS(const char* function, const Awesomium::JSArray& args, const char* objectName = "window");
		/// set js variable with specified name
		void SetJSProperty(const char* propertyName, const Awesomium::JSValue& value, const char* objectName = "window");
		/// get js variable with specified name
		Awesomium::JSValue GetJSProperty(const char* objectName, const char* propertyName);

		void HandleInput(const Input::InputEvent& inputEvent);

		void CreateGlobalJSObject(const Util::String& name);

		void RegisterFunctionCallback(const Util::String& objectname, Util::String functionName, CallbackFunction function);
		void RegisterFunctionReturnCallback(const Util::String& objectname, Util::String functionName, CallbackReturnFunction function);

		AwesomiumSurface* GetSurface() const;

		void SetTransform(const Math::matrix44& matrix);
		const Math::matrix44& GetMatrix() const;

		void SetPosition(const Math::float4& position);
		const Math::float4& GetPosition() const;

		UIType GetType() const;

	private:
		UIType type;

		Util::Dictionary<Util::String, Awesomium::JSObject> objects;

		Util::Dictionary<int, Util::Dictionary<Util::String, CallbackFunction>> callbackFunctions;
		Util::Dictionary<int, Util::Dictionary<Util::String, CallbackReturnFunction>> callbackReturnFunctions;

		Awesomium::WebView* view;
		AwesomiumJSMethodHandler* methodHandler;

		Math::matrix44 matrix;

		bool hasFocus;
		bool visible;
	};


	inline UIType AwesomiumLayout::GetType() const
	{
		return this->type;
	}

	inline void AwesomiumLayout::CreateGlobalJSObject(const Util::String& name)
	{
		Awesomium::JSObject object = this->view->CreateGlobalJavascriptObject(Awesomium::WSLit(name.Get())).ToObject();
		this->objects.Add(name, object);
	}

	inline void AwesomiumLayout::SetTransform(const Math::matrix44& matrix)
	{
		this->matrix = matrix;
	}

	inline const Math::matrix44& AwesomiumLayout::GetMatrix() const
	{
		return this->matrix;
	}

	inline void AwesomiumLayout::SetPosition(const Math::float4& position)
	{
		this->matrix.set_position(position);
	}

	inline const Math::float4& AwesomiumLayout::GetPosition() const
	{
		return this->matrix.get_position();
	}

	inline bool AwesomiumLayout::IsVisible() const
	{
		return this->visible;
	}

	inline void AwesomiumLayout::Show()
	{
		this->view->ResumeRendering();
		this->visible = true;
	}

	inline void AwesomiumLayout::Hide()
	{
		this->view->PauseRendering();
		this->visible = false;
	}
}