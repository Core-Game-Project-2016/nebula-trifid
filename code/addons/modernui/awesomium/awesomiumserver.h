// ------------------------------------------------------------------------------------------------------------------------
// 
// File:  awesomiumserver.h
// 
// Author: Timmy Silesmo
// Creation Date: 4:00 PM - 31/01/2016
// 
// Contact: Timmy.Silesmo@gmail.com
// 
// ------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "core/types.h"
#include "util/dictionary.h"
#include "awesomiumlayout.h"
#include "Awesomium/WebCore.h"

#include "core/rttimacros.h"
#include "core/singleton.h"
#include "core/refcounted.h"
#include "ui/base/uiserverbase.h"
#include "awesomiumsurfacefactory.h"

namespace AwesomiumUI
{
	class AwesomiumServer : public Core::RefCounted
	{
		__DeclareClass(AwesomiumServer);
		__DeclareSingleton(AwesomiumServer);
	public:
		AwesomiumServer();
		~AwesomiumServer();

		AwesomiumLayout* CreateView(const Util::String& name, uint width, uint height);

		bool IsValid() const;
		/// updates context
		void Update() const;
		/// renders context
		void Render(const Ptr<Frame::FrameBatch>& frameBatch);

		/// resize context
		void Resize(SizeT width, SizeT height);

		Ptr<AwesomiumLayout>& GetView(const Util::String& name);
		const Util::Dictionary<Util::String, Ptr<AwesomiumLayout>>& GetViews() const;

		bool HandleInput(const Input::InputEvent& inputEvent);
	private:
		Awesomium::WebCore* webCore;
		AwesomiumSurfaceFactory* factory;
		Util::Dictionary<Util::String, Ptr<AwesomiumLayout>> views;
	};

	inline Ptr<AwesomiumLayout>& AwesomiumServer::GetView(const Util::String& name)
	{
		return this->views[name];
	}

	inline const Util::Dictionary<Util::String, Ptr<AwesomiumLayout>>& AwesomiumServer::GetViews() const
	{
		return this->views;
	}
}

