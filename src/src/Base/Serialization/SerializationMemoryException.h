#pragma once
#include <string>
#include <stdexcept>
#include "SerializerErrorData.h"

namespace ska {
	struct SerializationMemoryException : 
		public std::runtime_error {
		SerializationMemoryException(std::vector<SerializerErrorData> data) : std::runtime_error("Serialization error"), data(std::move(data)) {}
		std::vector<SerializerErrorData> data;
	};
}
