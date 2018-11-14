//
// Created by gabriele on 04/11/18.
//

#ifndef LIBFORB_REMOTE_REGISTRY_HPP
#define LIBFORB_REMOTE_REGISTRY_HPP

#include <map>

#include "base_stub.hpp"

namespace forb {

    class remote_registry {
        struct remote_obj_entry {
            std::string _namespace;
            std::string _class;
            std::string _ip;
            int         _port;
        };

        using obj_map_t = std::map<std::string, remote_obj_entry>;
        obj_map_t remote_objects{};

    public:
        explicit remote_registry(std::string confname);

        remote_var get(const std::string &obj_name) const {
            remote_obj_entry entry;

            try {
                entry = remote_objects.at(obj_name);
            } catch (std::out_of_range ex) {
                return nullptr;
            }

            std::string classname = entry._class;
            if (entry._namespace.length() > 0) {
                classname = entry._namespace + "::" + classname;
            }

            remote_var var = remote_var{base_stub::_create(classname)};

            if (var != nullptr) {
                var->set_skeleton(entry._ip, entry._port);
            }

            return var;
        };

    };

} // namespace forb


#endif //LIBFORB_REMOTE_REGISTRY_HPP
