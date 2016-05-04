#pragma once

#include "util/string.h"
#include "util/stringatom.h"
#include <core/refcounted.h>

namespace UI{
	class UiEvent;
}

namespace AwesomiumUI
{

class AwesomiumEventListener : public Core::RefCounted
{
	__DeclareClass(AwesomiumEventListener);
public:
	/// constructor
	AwesomiumEventListener();
	/// destructor
	virtual ~AwesomiumEventListener();
	
	/// Process the incoming Event
	virtual void HandleEvent(const UI::UiEvent& e);
};

}
