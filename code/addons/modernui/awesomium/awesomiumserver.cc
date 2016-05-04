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
}

AwesomiumServer::~AwesomiumServer()
{
	for (int i = 0; i < this->views.Size(); i++)
	{
		delete this->views[i];
	}

	Awesomium::WebCore::Shutdown();
	__DestructSingleton;
}

AwesomiumLayout* AwesomiumServer::CreateView(uint width, uint height)
{
	Ptr<AwesomiumLayout> view = AwesomiumLayout::Create();
	view->Setup(this->webCore->CreateWebView(width, height), width, height);
	this->views.Append(view);
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

}
