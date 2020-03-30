#pragma once

#include "SerializerOutput.h"

namespace ska {
	
	template <class Type>
	struct SerializerTypeTraits {
		static constexpr std::size_t BytesRequired = sizeof(Type);
		static constexpr const char* Name = "";
	};

	template <class Type, class ... Args>
	struct SerializerType {
		template <class T, class ... Args>
		friend struct SerializerType;
		
		static constexpr auto BytesRequired = SerializerTypeTraits<Type>::BytesRequired;
		static constexpr auto TypeName = SerializerTypeTraits<Type>::Name;
	public:
		SerializerType(SerializerOutput& output) :
			m_zone(output.acquireMemory<BytesRequired>(TypeName)) {}

		template <std::size_t Bytes>
		SerializerType(SerializerSafeZone<Bytes>& zone) :
			m_zone(zone.acquireMemory<BytesRequired>(TypeName)) {
		}

		void write(const Type& type, Args&& ... args) { SerializerTypeTraits<Type>::Write(m_zone, type, std::forward<Args>(args)...); }
		void read(Type& type, Args&& ... args) { SerializerTypeTraits<Type>::Read(m_zone, type, std::forward<Args>(args)...); }
		void reset() { m_zone.reset(); }

	protected:
		SerializerSafeZone<BytesRequired> m_zone;
	};
}
