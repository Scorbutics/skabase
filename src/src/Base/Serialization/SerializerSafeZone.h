#pragma once

#include "SerializerOutputData.h"
#include "SerializerValidator.h"

namespace ska {

	namespace detail {
		class SerializerSafeZone {
		public:
			SerializerSafeZone(SerializerSafeZone* parent, SerializerValidator& pusher, std::string name, SerializerOutputData& data, std::size_t bytesRequired) :
				m_parent(parent),
				m_data(data),
				m_name(std::move(name)),
				m_pusher(pusher),
				m_bytesAllocated(0),
				m_bytesAllocatedMax(bytesRequired) {
			}

			SerializerSafeZone(SerializerSafeZone&&) noexcept = default;
			SerializerSafeZone& operator=(SerializerSafeZone&&) = default;

			virtual ~SerializerSafeZone() = default;

			template <class T>
			void write(const T& data) {
				if constexpr (std::is_same_v<std::remove_const_t<std::remove_reference_t<T>>, std::string>) {
					incBytes(sizeof(int64_t));
					m_data.natives().emplace(data, m_data.natives().size());
					auto refIndex = m_data.natives().at(data);
					m_data.buffer().write(reinterpret_cast<const char*>(&refIndex), sizeof(int64_t));
				} else {
					incBytes(sizeof(T));
					m_data.buffer().write(reinterpret_cast<const char*>(&data), sizeof(T));
				}
			}

		private:
			SerializerSafeZone* m_parent;
			std::size_t m_bytesWritten = 0;
			std::size_t m_bytesAllocated = 0;
			std::size_t m_bytesAllocatedMax = 0;

			void incBytes(std::size_t bytes) {
				if (m_parent != nullptr) {
					m_parent->incBytes(bytes);
				}
				m_bytesWritten += bytes;

				const auto requiredAllocation = static_cast<long>(m_bytesWritten - m_bytesAllocated);
				if (requiredAllocation > 0) {
					allocate(requiredAllocation);
				}
			}

		protected:
			std::size_t bytesWritten() const { return m_bytesWritten; }
			template <class T>
			long allocate(T bytes) { m_bytesAllocated += static_cast<long>(bytes); return static_cast<long>(m_bytesAllocatedMax - m_bytesAllocated); }

			SerializerOutputData m_data;
			std::string m_name;
			SerializerValidator& m_pusher;
			
		};
	}

	template <std::size_t BytesWrittenRequired>
	class SerializerSafeZone : 
		public detail::SerializerSafeZone {
	public:
		SerializerSafeZone(detail::SerializerSafeZone* parent, SerializerValidator& pusher, std::string name, SerializerOutputData& data) :
			detail::SerializerSafeZone(parent, pusher, std::move(name), data, BytesWrittenRequired) {
		}

		SerializerSafeZone(SerializerSafeZone&&) noexcept = default;
		SerializerSafeZone& operator=(SerializerSafeZone&&) = default;

		virtual ~SerializerSafeZone() {
			if (bytesWritten() != BytesWrittenRequired) {
				m_pusher.onError({0, BytesWrittenRequired, bytesWritten(), m_name, "bad serializer sync detected"});
			}
		}

		template <std::size_t Bytes>
		SerializerSafeZone<Bytes> acquireMemory(std::string zoneName) {
			static_assert(Bytes <= BytesWrittenRequired, "Unable to extract so many bytes from the existing safe zone");
			const long remainingBytes = allocate(Bytes);
			if (remainingBytes < 0) {
				m_pusher.onError({ Bytes, BytesWrittenRequired, BytesWrittenRequired - (remainingBytes + Bytes), m_name + " | " + zoneName, "not enough bytes were remaining into parent zone to allocate the child"});
			}
			return SerializerSafeZone<Bytes> { this, m_pusher, m_name + " | " + zoneName, m_data };
		}
	};

}
