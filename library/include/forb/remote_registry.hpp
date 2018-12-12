//
// Created by gabriele on 04/11/18.
//

#ifndef LIBFORB_REMOTE_REGISTRY_HPP
#define LIBFORB_REMOTE_REGISTRY_HPP

#include <map>

#include <forb/base_stub.hpp>

namespace forb {

    /// Emulates a complete registry of all active remote objects that use this framework.
    /// The list of all known objects is obtained by parsing a JSON file, whose name is given
    /// as input to the constructor.
    class remote_registry {
        /* ********************************************** CUSTOM TYPES ********************************************** */
    private:
        // Forward declaration, see implementation file.
        struct remote_obj_entry;

        /* ************************************************ ALIASES ************************************************* */

        /// The type of the set of all known remote objects.
        using obj_map_t = std::map<std::string, remote_obj_entry>;

        /* *********************************************** ATTRIBUTES *********************************************** */

        /// The complete set of all known remote objects.
        obj_map_t remote_objects;

        /* ********************************************** CONSTRUCTORS ********************************************** */
    public:
        /// Constructs a new registry.
        explicit remote_registry(std::string conf_file_name);

        /**************************************************************************************************************/
    public:
        /// Destructor.
        ~remote_registry() noexcept;

        /// This class does supports copy construction.
        remote_registry(const remote_registry &other);

        /// This class does supports copy assignment.
        remote_registry &operator=(const remote_registry &other);

        /// This class does supports move construction.
        remote_registry(remote_registry &&other) noexcept;

        /// This class does supports move assignment.
        remote_registry &operator=(remote_registry &&other) noexcept;

        /**************************************************************************************************************/
    public:
        /// Returns a reference to the object identified by the obj_name argument, a NIL pointer otherwise.
        remote_var get(const std::string &obj_name) const {
            return get(obj_name, false, 0);
        };

        /// Returns a reference to the object identified by the obj_name argument, a NIL pointer otherwise.
        /// The buffer_size argument can be used to force the size of the buffer that will be used in case
        /// a shared memory area will be allocated to exchange data between components.
        remote_var get(const std::string &obj_name, base_stub::buffer_size_t buffer_size) {
            return get(obj_name, false, buffer_size);
        };

        /// Returns a reference to the object identified by the obj_name argument, a NIL pointer otherwise.
        /// This method is the same as get, but forces the new handle to use of sockets for data
        /// exchange even when the two hosts are on the same machine.
        remote_var get_force_socket(const std::string &obj_name) const {
            return get(obj_name, true, 0);
        };

    private:
        /// Returns a reference to the object identified by the obj_name argument, a NIL pointer otherwise.
        /// The following two arguments cannot be respectively true and different than zero at the same time.
        /// For their meaning refer to the public overloads of this method.
        remote_var get(const std::string &obj_name, bool force_socket, base_stub::buffer_size_t buffer_size) const;
    };

} // namespace forb


#endif //LIBFORB_REMOTE_REGISTRY_HPP
