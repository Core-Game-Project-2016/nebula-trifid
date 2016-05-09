#include "stdneb.h"
#include "awesomiumserver.h"
#include "awesomiumlayout.h"

namespace AwesomiumUI
{
__ImplementClass(AwesomiumUI::AwesomiumServer, 'AWSV', Core::RefCounted);
__ImplementSingleton(AwesomiumUI::AwesomiumServer);

AwesomiumServer::AwesomiumServer()
{
	__ConstructSingleton;
	this->webCore = Awesomium::WebCore::Initialize(Awesomium::WebConfig());
	this->factory = new AwesomiumSurfaceFactory();
	this->webCore->set_surface_factory(this->factory);
}

AwesomiumServer::~AwesomiumServer()
{
	for (int i = 0; i < this->views.Size(); i++)
	{
		delete this->views.ValueAtIndex(i);
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
	return view;
}

void AwesomiumServer::Render(const Ptr<Frame::FrameBatch>& frameBatch)
{

}

void AwesomiumServer::Resize(SizeT width, SizeT height)
{

}

void AwesomiumServer::Update() const
{
	this->webCore->Update();
}

bool AwesomiumServer::HandleInput(const Input::InputEvent& inputEvent)
{
	for (SizeT i = 0; i < this->views.Size(); i++)
	{
		if (this->views.ValueAtIndex(i)->HasFocus())
		{
			this->views.ValueAtIndex(i)->HandleInput(inputEvent);
			return true;
		}
	}
	return false;
}

}
