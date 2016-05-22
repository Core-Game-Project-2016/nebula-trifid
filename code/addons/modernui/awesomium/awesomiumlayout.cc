#include "stdneb.h"
#include "awesomiumlayout.h"
#include "awesomiumserver.h"

#include "Awesomium/BitmapSurface.h"
#include "Awesomium/STLHelpers.h"

namespace AwesomiumUI
{
	__ImplementClass(AwesomiumUI::AwesomiumLayout, 'AWLA', Core::RefCounted);

AwesomiumLayout::AwesomiumLayout()
{
	
}

void AwesomiumLayout::Setup(Awesomium::WebView* view, UIType type)
{
	this->visible = true;
	this->view = view;
	//Keep CSS transparency
	this->view->SetTransparent(true);
	this->hasFocus = false;
	this->matrix = Math::matrix44::identity();
	this->methodHandler = new AwesomiumJSMethodHandler();
	this->methodHandler->Setup(this);
	this->view->set_js_method_handler(methodHandler);
	this->type = type;
}

AwesomiumLayout::~AwesomiumLayout()
{
	delete this->methodHandler;
	this->view->Destroy();
}

bool AwesomiumLayout::IsLoaded() const
{
	return !this->view->IsLoading();
}

void AwesomiumLayout::LoadURL(const IO::URI& URL) const
{
	Awesomium::WebURL url(Awesomium::WSLit((URL.Scheme() + ":" + URL.GetHostAndLocalPath()).Get()));

	n_assert(url.IsValid());

	this->view->LoadURL(url);
}

AwesomiumSurface* AwesomiumLayout::GetSurface() const
{
	return static_cast<AwesomiumSurface*>(this->view->surface());
}

bool AwesomiumLayout::HasFocus() const
{
	return this->hasFocus;
}

void AwesomiumLayout::SetFocus(bool focus)
{
	this->hasFocus = focus;

	if (focus)
		this->view->Focus();
	else
		this->view->Unfocus();
}

void AwesomiumLayout::Resize(SizeT width, SizeT height)
{
	this->view->Resize(width, height);
}

void AwesomiumLayout::InvokeJS(const char* function, const Awesomium::JSArray& args, const char* objectName)
{
	Awesomium::JSValue window = this->view->ExecuteJavascriptWithResult(Awesomium::WSLit(objectName), Awesomium::WSLit(""));

	n_assert(window.IsObject());
	window.ToObject().Invoke(Awesomium::WSLit(function), args);
}

void AwesomiumLayout::SetJSProperty(const char* propertyName, const Awesomium::JSValue& value, const char* objectName)
{
	Awesomium::JSValue window = this->view->ExecuteJavascriptWithResult(Awesomium::WSLit(objectName), Awesomium::WSLit(""));

	n_assert(window.IsObject());
	window.ToObject().SetProperty(Awesomium::WSLit(propertyName), value);
}

Awesomium::JSValue AwesomiumLayout::GetJSProperty(const char* objectName, const char* propertyName)
{
	Awesomium::JSValue window = this->view->ExecuteJavascriptWithResult(Awesomium::WSLit(objectName), Awesomium::WSLit(""));

	n_assert(window.IsObject());
	return window.ToObject().GetProperty(Awesomium::WSLit(propertyName));
}

void AwesomiumLayout::HandleInput(const Input::InputEvent& inputEvent)
{
	switch (inputEvent.GetType())
	{
	case Input::InputEvent::KeyDown:
	{
		Awesomium::WebKeyboardEvent kEvent;
		Input::Key::Code key = inputEvent.GetKey();
		kEvent.is_system_key = key == Input::Key::F10;
		kEvent.type = Awesomium::WebKeyboardEvent::Type::kTypeKeyDown;
		kEvent.virtual_key_code = Input::Key::ToAwesomium(key);
		this->view->InjectKeyboardEvent(kEvent);
		break;
	}
	case Input::InputEvent::KeyUp:
	{
		Awesomium::WebKeyboardEvent kEvent;
		Input::Key::Code key = inputEvent.GetKey();
		kEvent.is_system_key = key == Input::Key::F10;
		kEvent.type = Awesomium::WebKeyboardEvent::Type::kTypeKeyUp;
		kEvent.virtual_key_code = Input::Key::ToAwesomium(key);
		this->view->InjectKeyboardEvent(kEvent);
		break;
	}
	case Input::InputEvent::Character:
	{
		Awesomium::WebKeyboardEvent kEvent;
		kEvent.type = Awesomium::WebKeyboardEvent::Type::kTypeChar;
		kEvent.text[0] = inputEvent.GetChar();
		this->view->InjectKeyboardEvent(kEvent);
		break;
	}
	case Input::InputEvent::MouseButtonDown:
	{
		switch (inputEvent.GetMouseButton())
		{
		case Input::MouseButton::LeftButton:
			this->view->InjectMouseDown(Awesomium::MouseButton::kMouseButton_Left);
			break;
		case Input::MouseButton::RightButton:
			this->view->InjectMouseDown(Awesomium::MouseButton::kMouseButton_Right);
			break;
		case Input::MouseButton::MiddleButton:
			this->view->InjectMouseDown(Awesomium::MouseButton::kMouseButton_Middle);
			break;
		}
		break;
	}
	case Input::InputEvent::MouseButtonUp:
	{
		switch (inputEvent.GetMouseButton())
		{
		case Input::MouseButton::LeftButton:
			this->view->InjectMouseUp(Awesomium::MouseButton::kMouseButton_Left);
			break;
		case Input::MouseButton::RightButton:
			this->view->InjectMouseUp(Awesomium::MouseButton::kMouseButton_Right);
			break;
		case Input::MouseButton::MiddleButton:
			this->view->InjectMouseUp(Awesomium::MouseButton::kMouseButton_Middle);
			break;
		}
		break;
	}
	case Input::InputEvent::MouseMove:
		this->view->InjectMouseMove(static_cast<int>(inputEvent.GetAbsMousePos().x()), static_cast<int>(inputEvent.GetAbsMousePos().y()));
		break;
	case Input::InputEvent::MouseWheelForward:
		this->view->InjectMouseWheel(50, 0);
		break;
	case Input::InputEvent::MouseWheelBackward:
		this->view->InjectMouseWheel(-50, 0);
		break;
	}
}

void AwesomiumLayout::RegisterFunctionCallback(const Util::String& objectname, Util::String functionName, CallbackFunction function)
{
	Awesomium::JSObject object = this->objects.ValueAtIndex(this->objects.FindIndex(objectname));
	object.SetCustomMethod(Awesomium::WSLit(functionName.Get()), false);

	int index = this->callbackFunctions.FindIndex(object.remote_id());

	if (index == InvalidIndex)
	{
		this->callbackFunctions.Add(object.remote_id(), Util::Dictionary<Util::String, CallbackFunction>());
		index = this->callbackFunctions.FindIndex(object.remote_id());
	}
	this->callbackFunctions.ValueAtIndex(index).Add(functionName, function);
}

void AwesomiumLayout::RegisterFunctionReturnCallback(const Util::String& objectname, Util::String functionName, CallbackReturnFunction function)
{
	Awesomium::JSObject object = this->objects.ValueAtIndex(this->objects.FindIndex(objectname));
	object.SetCustomMethod(Awesomium::WSLit(functionName.Get()), true);
	
	int index = this->callbackReturnFunctions.FindIndex(object.remote_id());

	if (index == InvalidIndex)
	{
		this->callbackReturnFunctions.Add(object.remote_id(), Util::Dictionary<Util::String, CallbackReturnFunction>());
		index = this->callbackReturnFunctions.FindIndex(object.remote_id());
	}
	this->callbackReturnFunctions.ValueAtIndex(index).Add(functionName, function);
}

}
