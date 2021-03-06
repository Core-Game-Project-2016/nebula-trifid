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

AwesomiumLayout* AwesomiumServer::CreateView(const Util::String& name, uint width, uint height, UIType type)
{
	//if (this->views.Contains(name))
	//	return this->views[name];
	//else if(this->holograms.Contains(name))
	//	return this->holograms[name];

	Ptr<AwesomiumLayout> view = AwesomiumLayout::Create();
	view->Setup(this->webCore->CreateWebView(width, height), type);
	if (type == UIType::UI)
		this->views.Add(name, view);
	else if (type == UIType::Hologram)
		this->holograms.Add(name, view);

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

	if (CoreGraphics::FrameBatchType::UI == frameBatch->GetType())
	{
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

	if (CoreGraphics::FrameBatchType::Shapes == frameBatch->GetType())
	{
		this->Update();

		for (int i = 0; i < this->holograms.Size(); i++)
		{
			const Ptr<AwesomiumUI::AwesomiumLayout>& view = this->holograms.ValueAtIndex(i);
			if (view->IsVisible() && !view->IsInstanced())
			{
				this->renderer->Render(view);
			}
		}
	}

	if (CoreGraphics::FrameBatchType::Shapes == frameBatch->GetType())
	{
		this->Update();

		for (int i = 0; i < this->instances.Size(); i++)
		{
			const Ptr<AwesomiumUI::AwesomiumLayout>& view = this->instances.KeyAtIndex(i);
			const Util::Array<Math::matrix44>& matrices = this->instances.ValueAtIndex(i);

			if (view->IsVisible() && view->IsInstanced())
			{
				if (matrices.Size() > 0)
					this->renderer->InstancedRender(view, matrices);
			}
			this->instances.ValueAtIndex(i).Clear();
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
