#include "stdneb.h"
#include "memory/memory.h"
#include "awesomiumeventlistener.h"
#include "util/array.h"
#include "awesomiumserver.h"
#include "ui/uievent.h"
#include "ui/uifeatureunit.h"
#include "faudio/audiodevice.h"
#include "faudio/eventid.h"

using namespace Util;
namespace AwesomiumUI
{
	__ImplementClass(AwesomiumUI::AwesomiumEventListener, 'AWEL', Core::RefCounted);

//------------------------------------------------------------------------------
/**
*/
AwesomiumEventListener::AwesomiumEventListener()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AwesomiumEventListener::~AwesomiumEventListener()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void AwesomiumEventListener::HandleEvent(const UI::UiEvent& e)
{

}

}


