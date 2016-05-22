// ------------------------------------------------------------------------------------------------------------------------
// 
// File:  awesomiumserver.h
// 
// Author: Timmy Silesmo
// Creation Date: 4:00 PM - 31/01/2016
// 
// Contact: Timmy.Silesmo@gmail.com
// 
// ------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "util/dictionary.h"

#include "core/types.h"
#include "core/rttimacros.h"
#include "core/singleton.h"
#include "core/refcounted.h"

#include "ui/base/uiserverbase.h"

#include "Awesomium/WebCore.h"

#include "awesomiumsurfacefactory.h"
#include "awesomiumrenderer.h"
#include "awesomiumlayout.h"

#define __RegisterGlobalFunction(name, func) \
	AwesomiumUI::AwesomiumServer::Instance()->RegisterGloabalFunctionCallback(name, func)

#define __RegisterGlobalReturnFunction(name, func) \
	AwesomiumUI::AwesomiumServer::Instance()->RegisterGloabalFunctionReturnCallback(name, func)

namespace AwesomiumUI
{
	class AwesomiumServer : public Core::RefCounted
	{
		__DeclareClass(AwesomiumServer);
		__DeclareSingleton(AwesomiumServer);
		friend class AwesomiumUI::AwesomiumJSMethodHandler;
	public:
		typedef void(*CallbackFunction)(Awesomium::WebView* caller, const Awesomium::JSArray& args);
		typedef Awesomium::JSValue(*CallbackReturnFunction)(Awesomium::WebView* caller, const Awesomium::JSArray& args);

		AwesomiumServer();
		~AwesomiumServer();

		AwesomiumLayout* CreateView(const Util::String& name, uint width = 800, uint height = 600, UIType type = UIType::UI);
		
		void Update() const;
		
		void Render(const Ptr<Frame::FrameBatch>& frameBatch);

		void Resize(SizeT width, SizeT height);

		Ptr<AwesomiumLayout>& GetView(const Util::String& name);

		bool HandleInput(const Input::InputEvent& inputEvent);

		void RegisterGloabalFunctionCallback(Util::String functionName, CallbackFunction function);
		void RegisterGloabalFunctionReturnCallback(Util::String functionName, CallbackReturnFunction function);
	private:
		Awesomium::WebCore* webCore;
		AwesomiumSurfaceFactory* factory;

		Ptr<AwesomiumUI::AwesomiumRenderer> renderer;

		Util::Dictionary<Util::String, Ptr<AwesomiumLayout>> views;
		Util::Dictionary<Util::String, Ptr<AwesomiumLayout>> holograms;

		Util::Dictionary<Util::String, CallbackFunction> globalCallbackFunctions;
		Util::Dictionary<Util::String, CallbackReturnFunction> globalCallbackReturnFunctions;
	};

	inline Ptr<AwesomiumLayout>& AwesomiumServer::GetView(const Util::String& name)
	{
		if (this->views.Contains(name))
			return this->views[name];
		else
			return this->holograms[name];
	}
}

