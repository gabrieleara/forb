//
// Created by gabriele on 04/11/18.
//

#include <fstream>

#include <json.hpp>
#include <forb/remote_registry.hpp>

using json = nlohmann::json;

forb::remote_registry::remote_registry(std::string confname) {
    // TODO: wrap exception handling

    std::ifstream conf_file{confname};
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
}




