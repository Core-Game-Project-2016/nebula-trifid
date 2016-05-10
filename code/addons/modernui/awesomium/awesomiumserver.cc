#include "stdneb.h"
#include "awesomiumserver.h"

namespace AwesomiumUI
{
__ImplementClass(AwesomiumUI::AwesomiumServer, 'AWSV', Core::RefCounted);
__ImplementSingleton(AwesomiumUI::AwesomiumServer);

AwesomiumServer::AwesomiumServer()
{
	__ConstructSingleton;
	this->renderer = AwesomiumUI::AwesomiumRenderer::Create();
	this->webCore = Awesomium::WebCore::Initialize(Awesomium::WebConfig());
	this->factory = new AwesomiumSurfaceFactory();
	this->webCore->set_surface_factory(this->factory);
}

AwesomiumServer::~AwesomiumServer()
{
	for (int i = 0; i < this->views.Size(); i++)
	{
		this->views.ValueAtIndex(i) = 0;
	}

	Awesomium::WebCore::Shutdown();
	delete this->factory;
	__DestructSingleton;
}

AwesomiumLayout* AwesomiumServer::CreateView(const Util::String& name, uint width, uint height)
{
	Ptr<AwesomiumLayout> view = AwesomiumLayout::Create();
	view->Setup(this->webCore->CreateWebView(width, height));
	this->views.Add(name, view);

	view->CreateGlobalJSObject("src");
	for (SizeT i = 0; i < this->globalCallbackFunctions.Size(); i++)
	{
		view->RegisterFunctionCallback("src", this->globalCallbackFunctions.KeyAtIndex(i), this->globalCallbackFunctions.ValueAtIndex(i));
	}
	for (SizeT i = 0; i < this->globalCallbackReturnFunctions.Size(); i++)
	{
		view->RegisterFunctionReturnCallback("src", this->globalCallbackReturnFunctions.KeyAtIndex(i), this->globalCallbackReturnFunctions.ValueAtIndex(i));
	}

	return view;
}

void AwesomiumServer::Render(const Ptr<Frame::FrameBatch>& frameBatch)
{
	n_assert(this->renderer.isvalid());

	this->Update();

	for (int i = 0; i < this->views.Size(); i++)
	{
		const Ptr<AwesomiumUI::AwesomiumLayout>& view = this->views.ValueAtIndex(i);
		if (view->IsVisible())
		{
			this->renderer->Render(view);
		}
	}
}

void AwesomiumServer::Resize(SizeT width, SizeT height)
{
	for (SizeT i = 0; i < this->views.Size(); i++)
	{
		this->views.ValueAtIndex(i)->Resize(width, height);
	}
}

void AwesomiumServer::Update() const
{
	this->webCore->Update();
}

bool AwesomiumServer::HandleInput(const Input::InputEvent& inputEvent)
{
	for (SizeT i = 0; i < this->views.Size(); i++)
	{
		const Ptr<AwesomiumUI::AwesomiumLayout>& view = this->views.ValueAtIndex(i);
		if (view->HasFocus())
		{
			view->HandleInput(inputEvent);
			return true;
		}
	}
	return false;
}

void AwesomiumServer::RegisterGloabalFunctionCallback(Util::String functionName, CallbackFunction function)
{
	this->globalCallbackFunctions.Add(functionName, function);
}

void AwesomiumServer::RegisterGloabalFunctionReturnCallback(Util::String functionName, CallbackReturnFunction function)
{
	this->globalCallbackReturnFunctions.Add(functionName, function);
}

}
