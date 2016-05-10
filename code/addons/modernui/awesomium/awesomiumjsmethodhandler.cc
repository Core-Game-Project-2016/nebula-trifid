#include "stdneb.h"
#include "awesomiumjsmethodhandler.h"
#include "Awesomium\STLHelpers.h"

namespace AwesomiumUI
{
	AwesomiumJSMethodHandler::AwesomiumJSMethodHandler()
	{
		// Empty
	}

	AwesomiumJSMethodHandler::~AwesomiumJSMethodHandler()
	{
		// Empty
	}

	void AwesomiumJSMethodHandler::Setup(const char* name, Awesomium::WebView* view)
	{
		this->object = view->CreateGlobalJavascriptObject(Awesomium::WSLit(name)).ToObject();
	}

	void AwesomiumJSMethodHandler::OnMethodCall(Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args)
	{
		for (SizeT i = 0; i < this->callbackFunctions.Size(); i++)
		{
			if (method_name == Awesomium::WSLit(this->callbackFunctions.KeyAtIndex(i).Get()))
			{
				(*this->callbackFunctions.ValueAtIndex(i))(caller, args);
			}
		}
	}

	Awesomium::JSValue AwesomiumJSMethodHandler::OnMethodCallWithReturnValue(Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args)
	{
		for (SizeT i = 0; i < this->callbackReturnFunctions.Size(); i++)
		{
			if (method_name == Awesomium::WSLit(this->callbackFunctions.KeyAtIndex(i).Get()))
			{
				return (*this->callbackReturnFunctions.ValueAtIndex(i))(caller, args);
			}
		}

		return Awesomium::JSValue::Undefined();
	}

	void AwesomiumJSMethodHandler::RegisterFunctionCallback(Util::String functionName, CallbackFunction function)
	{
		this->object.SetCustomMethod(Awesomium::WSLit(functionName.Get()), false);
		this->callbackFunctions.Add(functionName, function);
	}

	void AwesomiumJSMethodHandler::RegisterFunctionReturnCallback(Util::String functionName, CallbackReturnFunction function)
	{
		this->object.SetCustomMethod(Awesomium::WSLit(functionName.Get()), true);
		this->callbackReturnFunctions.Add(functionName, function);
	}

}