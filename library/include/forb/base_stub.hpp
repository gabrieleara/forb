//
// Created by gabriele on 04/11/18.
//

#ifndef LIBFORB_BASE_STUB_HPP
#define LIBFORB_BASE_STUB_HPP

#include <vector>
#include <string>
#include <memory>
#include <mutex>

#include <forb/stream/shared_memory.hpp>
#include <forb/stream/socket.hpp>
#include <forb/stream/stream.hpp>

namespace forb {
    // Forward declaration of registry class
    class remote_registry;

    /* ************************************************** ALIASES *************************************************** */

    // Forward declaration
    class base_stub;

    /// Shared pointer to a base_class object (or a derived class).
    using remote_ptr = std::shared_ptr<base_stub>;

    /// Unique pointer to a base_class object (or a derived class).
    using remote_var = std::unique_ptr<base_stub>;

    /* *************************************************** CLASS **************************************************** */

    /// The class from which any automatically generated stub will inherit.
    class base_stub {
        /// The forb::remote_registry class will be able to call private methods of this class,
        /// to instantiate correctly new instances.
        friend class remote_registry;

        /* ************************************* ALIASES AND STATIC ATTRIBUTES ************************************** */
    public:
        /// The type of the buffer size.
        using buffer_size_t = forb::streams::shared_memory::size_t;

    private:
        using ssocket = forb::streams::socket;
        using shared_memory = forb::streams::shared_memory;
        using stream = forb::streams::stream;

        /// The default size of the buffer within the shared memory area (if used).
        constexpr static buffer_size_t default_buffer_size = 1024;

        /* *********************************************** ATTRIBUTES *********************************************** */
    protected:
        /// The mutex that avoids concurrent call requests using the same stub handle.
        std::mutex _mutex;

        /// The pointer to the stream that will be used to make call requests.
        /// In this implementation this is always pointing to _socket attribute.
        stream *rpcstream = nullptr;

        /// The pointer to the stream that will be used to exchange data.
        /// This may point either to _socket or to _shmem attributes.
        stream *datastream = nullptr;

    private:
        /// The socket used to make call request (and maybe exchange data).
        ssocket _socket;

        /// The (optional) shared memory used to exchange data.
        shared_memory _shmem;

        /// If true it will force the usage of sockets even when shared memories could be used.
        bool _force_socket = false;

        /// The size of the buffer within the shared memory
        /// (if shared memory is adopted as data exchange stream).
        buffer_size_t _buffer_size = default_buffer_size;

        /* ********************************************** CONSTRUCTORS ********************************************** */
    protected:
        /// Default constructor, creates an empty stub handle.
        /// The optional argument is the size of the buffer within the shared memory area (if shared memory is used).
        explicit base_stub() = default;

        /**************************************************************************************************************/
    public:
        /// Virtual destructor.
        virtual ~base_stub() noexcept = default;

        /// This class does not support copy construction.
        base_stub(const base_stub &other) = delete;

        /// This class does not support copy assignment.
        base_stub &operator=(const base_stub &other) = delete;

        /// This class does not support move construction.
        base_stub(base_stub &&other) = delete;

        /// This class does not support move assignment.
        base_stub &operator=(base_stub &&other) = delete;

        /**************************************************************************************************************/
    public:
        /// The size of the buffer within the shared memory
        /// (if shared memory is adopted as data exchange stream).
        shared_memory::size_t buffer_size() const {
            return _buffer_size;
        };


    protected:
        /// Initializes a new call request.
        void init_call(call_id_t code);

        /// Waits the termination of the call request.
        /// Actually this method waits the remote host to read all incoming parameters, not to actually perform the
        /// call. This way, signal-like paradigm calls are supported efficiently.
        // TODO: check wait return
        void wait_return();

    private:
        /// Forces the use of sockets to exchange data even on a local machine.
        /// It shall be used only by forb::remote_registry class before setting the skeleton.
        void force_socket() {
            _force_socket = true;
        };

        /// Sets the dimension of the buffer within the shared memory.
        /// It shall be used only by forb::remote_registry class before setting the skeleton.
        void set_buffer_size(buffer_size_t buffer_size) {
            _buffer_size = buffer_size;
        };

        /// Sets the address/port pair as the associated skeleton for the current stub handle.
        /// It shall be used only by forb::remote_registry class.
        void set_skeleton(std::string address, int port);

        /* ******************************************** FACTORY METHODS ********************************************* */
    protected:
        /// Returns true of the type string equals the fully qualified name of the given stub class.
        virtual bool _match(const std::string &type) const = 0;

        /// Creates a new empty stub object of the current type.
        // TODO: move to unique_ptr for better exception safety?
        virtual remote_var _create_empty() const = 0;

        /// Returns a reference to the vector of prototypes of all known stub implementations.
        static std::vector<base_stub *> &_protos();

    private:
        /// Creates a new empty stub object of the given type.
        /// It shall be used only by forb::remote_registry class.
        // TODO: move to unique_ptr for better exception safety?
        static remote_var _create(const std::string &type);
    };

    /* ************************************************* TEMPLATES ************************************************** */

    /// Returns true if the given pointer to a base_stub (or derived class) is null.
    template<typename T>
    bool is_nil(const std::shared_ptr<T> &ptr) {
        static_assert(std::is_base_of<base_stub, T>::value,
                      "The given type is not derived from forb::base_stub!");
        return ptr == nullptr;
    }

    /// Returns true if the given pointer to a base_stub (or derived class) is null.
    template<typename T>
    bool is_nil(const std::unique_ptr<T> &ptr) {
        static_assert(std::is_base_of<base_stub, T>::value,
                      "The given type is not derived from forb::base_stub!");
        return ptr == nullptr;
    }


} // namespace forb


#endif //LIBFORB_BASE_STUB_HPP
