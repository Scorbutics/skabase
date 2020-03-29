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
				m_bytesAllocatedMax(bytesRequired) {
			}

			SerializerSafeZone(SerializerSafeZone&&) noexcept = default;
			SerializerSafeZone& operator=(SerializerSafeZone&&) = default;

			virtual ~SerializerSafeZone() {
				validate(true);
			}

			void validate(bool exact = false) {
				if (m_valid) {
					return;
				}

				if (exact && m_bytesWritten != m_bytesAllocatedMax || m_bytesWritten > m_bytesAllocatedMax) {
					m_valid = true;
					m_pusher.onError({ 0, m_bytesAllocatedMax, m_bytesWritten, m_name, "bad serializer sync detected" });
				}
			}

			template <class T>
			void write(const T& data) {
				if constexpr (std::is_same_v<std::remove_const_t<std::remove_reference_t<T>>, std::string>) {
					incBytes(sizeof(int64_t));
					m_data.natives().emplace(data, data);
					auto refIndex = m_data.natives().id(data);
					m_data.buffer().write(reinterpret_cast<const char*>(&refIndex), sizeof(int64_t));
				} else {
					incBytes(sizeof(T));
					m_data.buffer().write(reinterpret_cast<const char*>(&data), sizeof(T));
				}
			}

			template <class T>
			T read() {
				if constexpr (std::is_same_v<std::remove_const_t<std::remove_reference_t<T>>, std::string>) {
					incBytes(sizeof(int64_t));
					auto size = int64_t { 0 };
					m_data.buffer().read(reinterpret_cast<char*>(&size), sizeof(int64_t));
					if (size >= 0 && static_cast<std::size_t>(size) < m_data.natives().size()) {
						return m_data.natives()[size];
					}
					return "";
				} else {
					incBytes(sizeof(T));
					T value;
					m_data.buffer().read(reinterpret_cast<char*>(&value), sizeof(T));
					return value;
				}
			}

			void reset() {
				m_bytesAllocated = 0;
				decBytes(m_bytesWritten);
			}

		private:
			bool m_valid = false;
			SerializerSafeZone* m_parent;
			std::size_t m_bytesWritten = 0;
			std::size_t m_bytesAllocated = 0;
			std::size_t m_bytesAllocatedMax = 0;

			void decBytes(std::size_t bytes) {
				if (m_parent != nullptr) {
					m_parent->decBytes(bytes);
				}
				m_bytesWritten -= bytes;
			}

			void incBytes(std::size_t bytes) {
				if (m_parent != nullptr) {
					m_parent->incBytes(bytes);
				}
				m_bytesWritten += bytes;

				const auto requiredAllocation = static_cast<long>(m_bytesWritten - m_bytesAllocated);
				if (requiredAllocation > 0) {
					allocate(requiredAllocation);
				}
				validate();
			}

		protected:
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
