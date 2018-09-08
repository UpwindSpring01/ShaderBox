#pragma once

#include "BaseComponent.h"

namespace SpringWindEngine
{
	template<class T>
	class Component : public BaseComponent
	{
	public:
		Component() {};
		virtual ~Component() {};
	private:
		Component(const Component& t) = delete;
		Component& operator=(const Component& t) = delete;
	};
}