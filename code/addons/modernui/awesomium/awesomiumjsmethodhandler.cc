#include "stdneb.h"
#include "awesomiumjsmethodhandler.h"
#include "Awesomium\STLHelpers.h"
#include "awesomiumserver.h"
#include "util/dictionary.h"

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

	void AwesomiumJSMethodHandler::Setup(const Ptr<AwesomiumUI::AwesomiumLayout>& layout)
	{
		this->layout = layout;
		this->server = AwesomiumUI::AwesomiumServer::Instance();
	}

	void AwesomiumJSMethodHandler::OnMethodCall(Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args)
	{
		for (SizeT i = 0; i < this->layout->objects.Size(); i++)
		{
			Awesomium::JSObject& object = this->layout->objects.ValueAtIndex(i);
			if (object.remote_id() == remote_object_id)
			{
				int index = this->layout->callbackFunctions.FindIndex(object.remote_id());

				if (index == InvalidIndex)
					return;

				for (SizeT j = 0; j < this->layout->callbackFunctions.ValueAtIndex(index).Size(); j++)
				{
					if (Awesomium::WSLit(this->layout->callbackFunctions.ValueAtIndex(index).KeyAtIndex(j).Get()) == method_name)
					{
						(*this->layout->callbackFunctions.ValueAtIndex(index).ValueAtIndex(j))(caller, args);
						return;
					}
				}
				return;
			}
		}
	}

	Awesomium::JSValue AwesomiumJSMethodHandler::OnMethodCallWithReturnValue(Awesomium::WebView* caller, unsigned int remote_object_id, const Awesomium::WebString& method_name, const Awesomium::JSArray& args)
	{
		for (SizeT i = 0; i < this->layout->objects.Size(); i++)
		{
			Awesomium::JSObject& object = this->layout->objects.ValueAtIndex(i);
			if (object.remote_id() == remote_object_id)
			{
				int index = this->layout->callbackReturnFunctions.FindIndex(object.remote_id());

				if (index == InvalidIndex)
					break;

				for (SizeT j = 0; j < this->layout->callbackReturnFunctions.ValueAtIndex(index).Size(); j++)
				{
					if (Awesomium::WSLit(this->layout->callbackReturnFunctions.ValueAtIndex(index).KeyAtIndex(j).Get()) == method_name)
					{
						return (*this->layout->callbackReturnFunctions.ValueAtIndex(index).ValueAtIndex(j))(caller, args);
					}
				}
			}
		}


		return Awesomium::JSValue::Undefined();
	}

}