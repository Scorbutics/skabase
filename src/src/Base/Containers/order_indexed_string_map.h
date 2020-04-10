#pragma once
#include "insertion_indexed_map.h"
#include "Base/Meta/IsSmartPtr.h"
namespace ska {
	template <class T>
	class order_indexed_string_map : public insertion_indexed_map<std::string, std::conditional_t<is_smart_ptr<T>::value, T, std::unique_ptr<T>>> {
	public:
		auto& emplaceNamed(T element, bool force = false) {
			auto key = element.name();
			emplace(key, std::move(element), force);
			return at(key);
		}
	};
}