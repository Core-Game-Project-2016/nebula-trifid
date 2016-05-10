// ------------------------------------------------------------------------------------------------------------------------
// 
// File:  awesomiumjshandler.h
// 
// Author: Timmy Silesmo
// Creation Date: 02:36 PM - 09/05/2016
// 
// Contact: Timmy.Silesmo@gmail.com
// 
// ------------------------------------------------------------------------------------------------------------------------
#pragma once

#include <Awesomium/WebCore.h>

#include "core/types.h"
#include "core/rttimacros.h"
#include "core/refcounted.h"
#include "Awesomium/JSObject.h"
#include "util/dictionary.h"

namespace AwesomiumUI
{
	class AwesomiumServer;
	class AwesomiumLayout;

	class AwesomiumJSMethodHandler : public Awesomium::JSMethodHandler
	{
	public:
		AwesomiumJSMethodHandler();
		~AwesomiumJSMethodHandler();
		void Setup(const Ptr<AwesomiumUI::AwesomiumLayout>& layout);

		typedef void(*CallbackFunction)(Awesomium::WebView* caller, const Awesomium::JSArray& args);
		typedef Awesomium::JSValue(*CallbackReturnFunction)(Awesomium::WebView* caller, const Awesomium::JSArray& args);

		virtual void OnMethodCall(Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args) override;
		virtual Awesomium::JSValue OnMethodCallWithReturnValue(Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args) override;

	private:
		Ptr<AwesomiumUI::AwesomiumServer> server;
		Ptr<AwesomiumUI::AwesomiumLayout> layout;
	};
}