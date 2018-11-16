#ifndef __forb_stream_hpp__
#define __forb_stream_hpp__

#include <type_traits> // TODO: only for weird stuff

#include <sys/types.h>
#include <cstring>
#include <byteswap.h>
#include <limits>

#include "../declarations.hpp"

namespace forb {
    namespace streams {

        // TODO: custom types declared in FORB IDL Files shall define a partial specialization of this template
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

        template<typename T>
        T unmarshal(T v) {
            return marshal(v);
        }

        // TODO: this template should be tested
        template<typename T>
        void marshal(T v[], size_t N) {
            for (size_t idx = 0; idx < N; ++idx) {
                v[idx] = marshal(v[idx]);
            }
        }

        template<typename T>
        void unmarshal(T v[], size_t N) {
            return marshal(v, N);
        }

        class stream {
        public:
            using size_t = std::size_t;

            enum class type : call_id_t {
                SOCKET = 0,
                SHMEM = 1 << (std::numeric_limits<call_id_t>::digits - 1)
            };

            virtual ~stream() noexcept {};

            virtual bool require_marshal() { return false; };

            virtual void send(const void *buffer, size_t sise) = 0;

            virtual void recv(void *buffer, size_t sise) = 0;

            virtual void close() = 0;

            virtual type get_type() = 0;
        };

    } // namespace streams

} // namespace forb

#endif // #ifndef __forb_stream_hpp__
