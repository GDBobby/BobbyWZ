#pragma once

#include <list>
#include "WzImageProperty.h"

namespace MapleLib {
	namespace WzLib {
		class IPropertyContainer {
		public:
			virtual void AddProperty(WzImageProperty prop) = 0;
			virtual void AddProperties(std::list<WzImageProperty> props) = 0;
			virtual void RemoveProperty(WzImageProperty prop) = 0;
			virtual void ClearProperties() = 0;
			virtual const std::list<WzImageProperty>& GetWzProperties() const = 0;
			virtual WzImageProperty* GetPropertyByName(const std::wstring& name) const = 0;
			virtual void SetPropertyByName(const std::wstring& name, WzImageProperty* prop) = 0;

			virtual ~IPropertyContainer() = default;
		};
	}
}