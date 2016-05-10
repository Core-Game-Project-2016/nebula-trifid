#include "stdneb.h"
#include "awesomiumlayout.h"
#include "awesomiumserver.h"
#include <coregraphics/memorytextureloader.h>
#include "resources/resourcemanager.h"
#include <coregraphics/memoryvertexbufferloader.h>
#include <coregraphics/memoryindexbufferloader.h>
#include <coregraphics/vertexcomponent.h>

#include "Awesomium/BitmapSurface.h"
#include "Awesomium/STLHelpers.h"

namespace AwesomiumUI
{
	__ImplementClass(AwesomiumUI::AwesomiumLayout, 'AWLA', Core::RefCounted);

AwesomiumLayout::AwesomiumLayout()
{
	
}

void AwesomiumLayout::GenerateMesh()
{
	this->geometry = static_cast<NebulaGeometry*>(Alloc(Memory::DefaultHeap, sizeof(NebulaGeometry)));
	Memory::Clear(this->geometry, sizeof(NebulaGeometry));

	// create vertex buffer
	this->geometry->vb = CoreGraphics::VertexBuffer::Create();

	Util::Array<CoreGraphics::VertexComponent> vertexComponents;
	vertexComponents.Append(CoreGraphics::VertexComponent(static_cast<CoreGraphics::VertexComponent::SemanticName>(0), 0, CoreGraphics::VertexComponent::Float2));     // position
	vertexComponents.Append(CoreGraphics::VertexComponent(static_cast<CoreGraphics::VertexComponent::SemanticName>(2), 0, CoreGraphics::VertexComponent::Float2));     // UV

	int indices[] = 
	{ 
		0, 1, 2,		
		2, 3, 0 
	};

	NebulaVertex vertices[4] =
	{
		{  0, 0,	0, 0 },
		{  0, 1,	0, 1 },
		{  1, 1,	1, 1 },
		{  1, 0,	1, 0 }
	};

	int numVertices = 4;
	int numIndices = 6;

	// create loader for vertex buffer
	Ptr<CoreGraphics::MemoryVertexBufferLoader> vbLoader = CoreGraphics::MemoryVertexBufferLoader::Create();
	vbLoader->Setup(vertexComponents,
		numVertices,
		vertices,
		numVertices * sizeof(float) * 4,
		Base::ResourceBase::UsageImmutable,
		Base::ResourceBase::AccessNone);
	this->geometry->vb->SetLoader(vbLoader.upcast<Resources::ResourceLoader>());
	this->geometry->vb->SetAsyncEnabled(false);
	this->geometry->vb->Load();
	n_assert(this->geometry->vb->IsLoaded());
	this->geometry->vb->SetLoader(0);

	// create index buffer
	this->geometry->ib = CoreGraphics::IndexBuffer::Create();

	// create loader for index buffer
	Ptr<CoreGraphics::MemoryIndexBufferLoader> ibLoader = CoreGraphics::MemoryIndexBufferLoader::Create();
	ibLoader->Setup(CoreGraphics::IndexType::Index32,
		numIndices,
		indices,
		numIndices * sizeof(int),
		Base::ResourceBase::UsageImmutable,
		Base::ResourceBase::AccessNone);
	this->geometry->ib->SetLoader(ibLoader.upcast<Resources::ResourceLoader>());
	this->geometry->ib->SetAsyncEnabled(false);
	this->geometry->ib->Load();
	n_assert(this->geometry->ib->IsLoaded());
	this->geometry->ib->SetLoader(0);

	this->geometry->primGroup.SetBaseIndex(0);
	this->geometry->primGroup.SetNumVertices(numVertices);
	this->geometry->primGroup.SetBaseVertex(0);
	this->geometry->primGroup.SetNumIndices(numIndices);
	this->geometry->primGroup.SetPrimitiveTopology(CoreGraphics::PrimitiveTopology::TriangleList);
}

void AwesomiumLayout::Setup(Awesomium::WebView* view)
{
	this->visible = true;
	this->view = view;
	//Keep CSS transparency
	this->view->SetTransparent(true);
	this->hasFocus = false;
	this->position = Math::float4(0.0f, 0.0f, 0.0f, 1.0f);
	this->GenerateMesh();
	this->methodHandler = new AwesomiumJSMethodHandler();
	this->methodHandler->Setup(this);
	this->view->set_js_method_handler(methodHandler);
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

void AwesomiumLayout::LoadURL(const Util::String& URL) const
{
	Awesomium::WebURL url(Awesomium::WSLit(URL.Get()));

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
	//TODO: Implement
}

void AwesomiumLayout::HandleInput(const Input::InputEvent& inputEvent)
{
	switch (inputEvent.GetType())
	{
	case Input::InputEvent::KeyDown:
	{
		Awesomium::WebKeyboardEvent kEvent;
		kEvent.type = Awesomium::WebKeyboardEvent::Type::kTypeKeyDown;
		kEvent.native_key_code = inputEvent.GetKey(); //TODO Convert to Awesomium Keycode
		this->view->InjectKeyboardEvent(kEvent);
		break;
	}
	case Input::InputEvent::KeyUp:
	{
		Awesomium::WebKeyboardEvent kEvent;
		kEvent.type = Awesomium::WebKeyboardEvent::Type::kTypeKeyUp;
		kEvent.native_key_code = inputEvent.GetKey(); //TODO Convert to Awesomium Keycode
		this->view->InjectKeyboardEvent(kEvent);
		break;
	}
	case Input::InputEvent::Character:
	{
		Awesomium::WebKeyboardEvent kEvent;
		kEvent.type = Awesomium::WebKeyboardEvent::Type::kTypeChar;
		kEvent.native_key_code = inputEvent.GetKey();
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
