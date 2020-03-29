#pragma once

#include "SerializerOutput.h"

namespace ska {
	
	template <class Type>
	struct SerializerTypeTraits {
		static constexpr std::size_t BytesRequired = sizeof(Type);
		static constexpr const char* Name = "";
	};

	template <class Type>
	struct SerializerType {
		template <class T>
		friend struct SerializerType;
		
		static constexpr auto BytesRequired = SerializerTypeTraits<Type>::BytesRequired;
		static constexpr auto TypeName = SerializerTypeTraits<Type>::Name;
	public:
		SerializerType(SerializerOutput& output) :
			m_output(output),
			m_zone(output.acquireMemory<BytesRequired>(TypeName)) {}


		void write(const Type& type) { SerializerTypeTraits<Type>::Write(m_zone, type); reset(); }
		Type read() { return SerializerTypeTraits<Type>::Read(m_zone); reset(); }

	protected:
		SerializerOutput& m_output;
		SerializerSafeZone<BytesRequired> m_zone;

	private:
		void reset() { m_zone.reset(); }
	};
}
