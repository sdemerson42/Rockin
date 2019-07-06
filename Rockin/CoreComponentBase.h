#pragma once

#include "sde.h"

namespace Core
{
	class CoreEntity;
	
	/*
	CoreComponentBase - Base class for all components.
	*/

	class CoreComponentBase : public sde::ComponentBaseNoParent
	{
	public:
		CoreComponentBase(CoreEntity *parent) :
			m_parent{ parent }
		{}
		virtual ~CoreComponentBase()
		{}
		CoreEntity *parent()
		{
			return m_parent;
		}
	private:
		CoreEntity *m_parent;
	};
}
