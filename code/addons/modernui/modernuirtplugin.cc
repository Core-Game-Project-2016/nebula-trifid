//------------------------------------------------------------------------------
//  uirtplugin.cc
//  (C) 2015-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "modernuirtplugin.h"
#include "frame/framebatch.h"
#include "core/rttimacros.h"


namespace UI
{
__ImplementClass(UI::ModernUiRTPlugin, 'MORT', RenderModules::RTPlugin);

using namespace Graphics;

//------------------------------------------------------------------------------
/**
*/
ModernUiRTPlugin::ModernUiRTPlugin()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ModernUiRTPlugin::~ModernUiRTPlugin()
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
void
ModernUiRTPlugin::OnRegister()
{
	this->uiServer = AwesomiumUI::AwesomiumServer::Create();
	this->uirenderer = AwesomiumUI::AwesomiumRenderer::Create();
	this->uiInputHandler = UI::ModernUiInputHandler::Create();
	//Input::InputServer::Instance()->AttachInputHandler(Input::InputPriority::Gui, this->uiInputHandler.cast<Input::InputHandler>());
}

//------------------------------------------------------------------------------
/**
*/
void
ModernUiRTPlugin::OnUnregister()
{
	this->uiServer = 0;
}

//------------------------------------------------------------------------------
/**
*/
void
ModernUiRTPlugin::OnRenderFrameBatch(const Ptr<Frame::FrameBatch>& frameBatch)
{
	n_assert(this->uiServer.isvalid());
	if (CoreGraphics::FrameBatchType::UI == frameBatch->GetType())
	{				
		this->uiServer->Render(frameBatch);	
		this->uiServer->Update();

		Util::Array<Ptr<AwesomiumUI::AwesomiumLayout>> views = this->uiServer->GetViews();

		for (int i = 0; i < views.Size(); i++)
		{
			if (views[i]->IsLoaded())
			{
				views[i]->Update();
				this->uirenderer->Render(views[i]);
			}
		}
	}
}

//------------------------------------------------------------------------------
/**
*/
void 
ModernUiRTPlugin::OnWindowResized(SizeT width, SizeT height)
{
	n_assert(this->uiServer.isvalid());
	this->uiServer->Resize(width, height);	
}
} // namespace RenderModules