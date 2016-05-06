#pragma once
//------------------------------------------------------------------------------
/**
    @class UI::RTPlugin
    
    (C) 2015-2016 Individual contributors, see AUTHORS file
*/
#include "core/refcounted.h"
#include "timing/time.h"
#include "frame/framebatch.h"
#include "rendermodules/rt/rtplugin.h"
#include "awesomium/awesomiumserver.h"
#include "awesomium/awesomiumrenderer.h"
#include "modernuiinputhandler.h"

//------------------------------------------------------------------------------
namespace UI
{
class ModernUiRTPlugin : public RenderModules::RTPlugin
{
	__DeclareClass(ModernUiRTPlugin);
public:
    /// constructor
	ModernUiRTPlugin();
    /// destructor
	virtual ~ModernUiRTPlugin();

	/// called when plugin is registered on the render-thread side
	virtual void OnRegister();
	/// called when plugin is unregistered on the render-thread side
	virtual void OnUnregister();

    /// called when rendering a frame batch
    virtual void OnRenderFrameBatch(const Ptr<Frame::FrameBatch>& frameBatch);

	/// called if the window size has changed
	virtual void OnWindowResized(SizeT width, SizeT height);

private:
	Ptr<AwesomiumUI::AwesomiumRenderer> uirenderer;
	Ptr<AwesomiumUI::AwesomiumServer> uiServer;
	Ptr<UI::ModernUiInputHandler> uiInputHandler;
};

} // namespace UI
//------------------------------------------------------------------------------
