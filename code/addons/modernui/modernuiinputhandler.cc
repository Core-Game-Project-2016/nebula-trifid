//------------------------------------------------------------------------------
//  uiinputhandler.cc
//  (C) 2007 Radon Labs GmbH
//  (C) 2013-2016 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "modernuiinputhandler.h"
#include "input/inputserver.h"
#include "graphics/graphicsinterface.h"
#include "awesomium/awesomiumserver.h"


namespace UI
{
__ImplementClass(UI::ModernUiInputHandler, 'MINP', Input::InputHandler);
using namespace Core;
using namespace Util;
//------------------------------------------------------------------------------
/**
*/
ModernUiInputHandler::ModernUiInputHandler() : handleInput(true)
{
    // empty
}

//------------------------------------------------------------------------------
/**
*/
ModernUiInputHandler::~ModernUiInputHandler()
{
   // empty
}

//------------------------------------------------------------------------------
/**
    Begin capturing input to this input handler. This method must be
    overriden in a subclass, the derived method must call 
    ObtainMouseCapture(), ObtainKeyboardCapture(), or both, depending
    on what type input events you want to capture. An input handler
    which captures input gets all input events of the given type exclusively.
*/
void
ModernUiInputHandler::BeginCapture()
{
	Input::InputServer::Instance()->ObtainMouseCapture(this);
	Input::InputServer::Instance()->ObtainKeyboardCapture(this);
}

//------------------------------------------------------------------------------
/**
    End capturing input to this input handler. Override this method
    in a subclass and release the captures obtained in BeginCapture().
*/
void
ModernUiInputHandler::EndCapture()
{
	Input::InputServer::Instance()->ReleaseMouseCapture(this);
	Input::InputServer::Instance()->ReleaseKeyboardCapture(this);
}

//------------------------------------------------------------------------------
/**
*/
void
ModernUiInputHandler::OnBeginFrame()
{
	//inputEvents.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
ModernUiInputHandler::OnEndFrame()
{
    //UIServer::Instance()->ProcessInputEvents(this->inputEvents);
}

//------------------------------------------------------------------------------
/**
*/
bool
ModernUiInputHandler::OnEvent(const Input::InputEvent& inputEvent)
{
	if (handleInput == false)
	{
		return false;
	}
	AwesomiumUI::AwesomiumServer* uiServer = AwesomiumUI::AwesomiumServer::Instance();
    switch (inputEvent.GetType())
    {
#ifndef _DEBUG
        case Input::InputEvent::AppObtainFocus:
        case Input::InputEvent::AppLoseFocus:
#endif
        case Input::InputEvent::Reset:
            this->OnReset();
            break;

		default:
            return uiServer->HandleInput(inputEvent);
			break;
    }        
    return false;
}

//------------------------------------------------------------------------------
/**
    OnReset is called when the app loses or gains focus (amongst other
    occasions). The input handler should reset its internal state
    to prevent keys from sticking down, etc...
*/
void
ModernUiInputHandler::OnReset()
{
    // empty
}

void ModernUiInputHandler::SetHandleInput(bool handleInput)
{
	this->handleInput = handleInput;
}

bool ModernUiInputHandler::GetHandleInput()
{
	return handleInput;
}

} // namespace Input


