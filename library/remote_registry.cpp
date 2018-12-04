//
// Created by gabriele on 04/11/18.
//

#include <fstream>

#include <json.hpp>
#include <forb/remote_registry.hpp>
#include <forb/exception.hpp>

using json = nlohmann::json;

/* ****************************************** PRIVATE TYPE IMPLEMENTATION ******************************************* */

/// The type that will store all information about a remote object
/// that will be parsed from the JSON file.
struct forb::remote_registry::remote_obj_entry {
    /// The namespace in which the class is defined
    std::string _namespace = "";

    /// The class name
    std::string _class = "";

    /// The IP address of the remote host
    std::string _ip = "";

    /// The port number of the remote host
    int _port = 0;
};

/* ********************************************** CLASS IMPLEMENTATION ********************************************** */

// For documentation see corresponding header file

// Constructor and destructors are defined in implementation file because otherwise remote_registry would have an
// attribute with incomplete type.

forb::remote_registry::~remote_registry() noexcept = default;

forb::remote_registry::remote_registry(const remote_registry &other) = default;

forb::remote_registry &forb::remote_registry::operator=(const remote_registry &other) = default;

forb::remote_registry::remote_registry(remote_registry &&other) noexcept = default;

forb::remote_registry &forb::remote_registry::operator=(forb::remote_registry &&other) noexcept = default;

forb::remote_registry::remote_registry(std::string conf_file_name) {
    try {
        std::ifstream conf_file{conf_file_name};
        json          conf = json::parse(conf_file);

        json::array_t objects = conf["objects"];

        remote_obj_entry entry;
        std::string      name;

        for (auto &obj : objects) {
            name = obj["name"];
            entry._namespace = obj["namespace"];
            entry._class     = obj["class"];
            entry._ip        = obj["ip"];
            entry._port      = obj["port"];

            remote_objects.insert(make_pair(name, entry));
        }
    } catch (std::exception &ex) {
        throw forb::exception{
            "Could not initialize remote registry because an exception arised: "
            + std::string(ex.what())
        };
    }

}

forb::remote_var forb::remote_registry::get(
        const std::string &obj_name,
        bool force_socket,
        base_stub::buffer_size_t buffer_size) const {
    remote_obj_entry entry;

    try {
        entry = remote_objects.at(obj_name);
    } catch (std::out_of_range &ex) {
        return nullptr;
    }

    std::string classname = entry._class;
    if (entry._namespace.length() > 0) {
        classname = entry._namespace + "::" + classname;
    }

    remote_var var = base_stub::_create(classname);

    if (var != nullptr) {
        if (force_socket) {
            var->force_socket();
        } else if (buffer_size > 0) {
            assert (!force_socket);

            var->set_buffer_size(buffer_size);
        }

        var->set_skeleton(entry._ip, entry._port);
    }

    return var;
}




