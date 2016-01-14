//------------------------------------------------------------------------------
//  modelnodeframe.cc
//  (C) 2012-2015 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "stdneb.h"
#include "modelnodeframe.h"

namespace Widgets
{
//------------------------------------------------------------------------------
/**
*/
ModelNodeFrame::ModelNodeFrame()
{
	this->setLayout(&this->layout);
}

//------------------------------------------------------------------------------
/**
*/
ModelNodeFrame::~ModelNodeFrame()
{
	// empty
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeFrame::AddModelNode(const Util::String& type, const Util::String& name, const Util::String& path, const Util::String& res)
{
	Ptr<ModelNodeHandler> handler = ModelNodeHandler::Create();
	handler->SetModelHandler(this->modelHandler);
	Ui::ModelNodeInfoWidget* ui = new Ui::ModelNodeInfoWidget;
	QFrame* frame = new QFrame;
	ui->setupUi(frame);
	handler->SetUI(ui);
	handler->SetType(type);
	handler->SetName(name);
	handler->SetPath(path);
	handler->Setup(res);
	this->layout.addWidget(frame);
	this->itemHandlers.Append(handler);
	this->itemFrames.Append(frame);
}

//------------------------------------------------------------------------------
/**
*/
void 
ModelNodeFrame::Discard()
{
	this->modelHandler = 0;
	IndexT i;
	for (i = 0; i < this->itemHandlers.Size(); i++)
	{
		this->itemHandlers[i]->Discard();
		this->layout.removeWidget(this->itemFrames[i]);
		delete this->itemFrames[i];
	}
	this->itemHandlers.Clear();
	this->itemFrames.Clear();
}

//------------------------------------------------------------------------------
/**
*/
void
ModelNodeFrame::Refresh()
{
	IndexT i;
	for (i = 0; i < this->itemHandlers.Size(); i++)
	{
		this->itemHandlers[i]->Refresh();
	}
}

} // namespace Widgets