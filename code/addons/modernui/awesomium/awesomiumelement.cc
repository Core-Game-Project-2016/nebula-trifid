//------------------------------------------------------------------------------
//  rocketelement.cc
//  (C) 2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "awesomiumelement.h"
#include "awesomiumlayout.h"
#include "ui/uilayout.h"


namespace AwesomiumUI
{
	__ImplementClass(AwesomiumUI::AwesomiumElement, 'AWEM', Base::UiElementBase);

//------------------------------------------------------------------------------
/**
*/
AwesomiumElement::AwesomiumElement()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
AwesomiumElement::~AwesomiumElement()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::Setup(const Ptr<UI::UiLayout>& layout)
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::Show()
{
       
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::Hide()
{
      
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::SetText(const Util::String& text)
{
	
}

//------------------------------------------------------------------------------
/**
*/
Util::String
AwesomiumElement::GetText() const
{
	return "";
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::AppendText(const Util::String& text)
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::AddListItem(const Util::String& item, void* userData)
{
	
}

//------------------------------------------------------------------------------
/**
*/
void* 
AwesomiumElement::GetListItem(const Util::String& item)
{
	
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::RemoveListItem(const Util::String& item)
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::ClearList()
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::SetInputValue(const Util::String& value)
{
   
}

//------------------------------------------------------------------------------
/**
*/
Util::String
AwesomiumElement::GetInputValue()
{
	return "";
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::SetInputText(const Util::String& text)
{
   
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
AwesomiumElement::GetInputText()
{
	return "";
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::SetChecked(bool b)
{
	
}

//------------------------------------------------------------------------------
/**
*/
bool 
AwesomiumElement::IsChecked()
{
	
	return false;
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::SetScrollSizes(const Math::float2& sizes)
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::SetScrollPositions(const Math::float2& positions)
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::SetImage(const Util::String& image)
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::SetProgress(float progress)
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::SetEnabled(bool b)
{
	  
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::SetSelected(bool b)
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::AddComboBoxItem(const Util::String& item, const Util::String & key)
{
	
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
AwesomiumElement::GetComboBoxItem(unsigned index)
{
    return "";
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::RemoveComboBoxItem(unsigned index)
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::ClearComboBox()
{
	
}

//------------------------------------------------------------------------------
/**
*/
const unsigned
AwesomiumElement::GetSelectedComboBoxItem()const
{
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
void
AwesomiumElement::SetCurrentComboBoxItem(int index)const
{
	
}

//------------------------------------------------------------------------------
/**
*/
const unsigned
AwesomiumElement::GetNumOptionsInComboBox()const
{
	return 0;
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::SetFocus(bool b)
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::SetProperty(const Util::String& p, const Util::String& value)
{
	
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
AwesomiumElement::GetProperty(const Util::String& p) const
{
	return "";
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::SetAttribute(const Util::String& attr, const Util::String& value)
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::RemoveAttribute(const Util::String& attr)
{
	
}

//------------------------------------------------------------------------------
/**
*/
Util::String 
AwesomiumElement::GetAttribute(const Util::String& attr)
{
	return "";
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::AddClass(const Util::String& c)
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::RemoveClass(const Util::String& c)
{
	
}

//------------------------------------------------------------------------------
/**
*/
void 
AwesomiumElement::SetClasses(const Util::String& c)
{
   
}

//------------------------------------------------------------------------------
/**
*/
const Util::String 
AwesomiumElement::GetClasses() const
{
	return "";
}

//------------------------------------------------------------------------------
/**
*/
void
AwesomiumElement::RemoveElement(const Ptr<UI::UiElement> & elem)
{

}
} // namespace Awesomium