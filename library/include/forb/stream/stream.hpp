#ifndef __forb_stream_hpp__
#define __forb_stream_hpp__

#include <sys/types.h>
#include <byteswap.h>
#include <limits>

#include <forb/declarations.hpp>

namespace forb {
    namespace streams {

        /// Returns the marshalled version of the given object.
        /// This implementation works only on primitive types.
        /// Custom types declared in FORB IDL files shall define a partial specialization of this template.
        template<typename T>
        T marshal(T v) {
            static_assert(
                    sizeof(T) == 1
                    || sizeof(T) == 2
                    || sizeof(T) == 4
                    || sizeof(T) == 8,
                    "Cannot marshal the type T");

            switch (sizeof(T)) {
                case 2:
                    return bswap_16(v);
                case 4:
                    return bswap_32(v);
                case 8:
                    return bswap_64(v);
                default:
                    // Should only happen for chars
                    return v;
            }
        }

        /// Returns the unmarshalled version of the given object.
        /// This implementation works only on primitive types.
        /// Custom types declared in FORB IDL files shall define a partial specialization of this template.
        template<typename T>
        T unmarshal(T v) {
            return marshal(v);
        }

        /// Returns the marshalled version of the given array.
        template<typename T>
        void marshal(T v[], size_t N) {
            for (size_t idx = 0; idx < N; ++idx) {
                v[idx] = marshal(v[idx]);
            }
        }

        /// Returns the unmarshalled version of the given array.
        template<typename T>
        void unmarshal(T v[], size_t N) {
            return marshal(v, N);
        }

        /// Virtual class that represents a stream.
        /// The actual implementations of this class (socket or shared_memory)
        /// may rely on a unidirectional data channel to perform communication.
        /// The classes that use this stream shall ensure that there won't be
        /// concurrent access to an end of the stream (either input or output),
        /// otherwise the behavior of the stream is undefined.
        /// Concurrent access to different ends of the stream is instead a
        /// good practice, also because some implementations may block a sending
        /// task waiting for a receiving task to free some space consuming data.
        class stream {
        public:
            /// An unsigned size type.
            using size_t = std::size_t;

            /// The type of the stream.
            /// The underlying value of the enumeration uses the most significant bit to
            /// check the socket type, so it can be bit-wisely ORed with the remotely
            /// called method ID.
            enum class type : call_id_t {
                SOCKET = 0,
                SHMEM  = 1 << (std::numeric_limits<call_id_t>::digits - 1)
            };

            /// This class is virtual, so it requires a virtual destructor.
            /// It has no fields, so no copy/move operations are needed.
            virtual ~stream() noexcept = default;

            /// Blocking send operation.
            /// It repeats the underlying call until all data are received or an error occurs.
            virtual void send(const void *buffer, size_t sise) = 0;

            /// Blocking receive operation.
            /// It repeats the underlying call until all data are received or an error occurs.
            virtual void recv(void *buffer, size_t sise) = 0;

            /// Closes the stream.
            virtual void close() = 0;

            /// Returns the type of the stream, either type::SOCKET or type::SHMEM.
            virtual type get_type() const = 0;

            /// Returns true if the execution platform requires marshalling before sending
            /// data over the stream, false otherwise.
            virtual bool require_marshal() const { return false; };
        };

    } // namespace streams

} // namespace forb

#endif // #ifndef __forb_stream_hpp__
