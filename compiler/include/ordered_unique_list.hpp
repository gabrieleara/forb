//
// Created by gabriele on 16/11/18.
//

#ifndef FORB_UNIQUES_LIST_H
#define FORB_UNIQUES_LIST_H

#include <string>
#include <set>
#include <vector>

template<typename T>
class ordered_unique_list {
protected:
    using set_t = std::set<std::string>;
    set_t _set;

    using list_t = std::vector<T>;
    list_t _list;
public:
    /// Returns true on successful insertion
    virtual bool insert(std::string key, const T &value) {
        if (_set.insert(key).second) {
            // Successful insertion
            _list.push_back(value);
            return true;
        }

        return false;
    };

    /// Returns a const reference to the list elements
    const list_t &list() const {
        return _list;
    };

    bool is_contained(std::string key) const {
        return _set.find(key) != _set.end();
    };
};


#endif //FORB_UNIQUES_LIST_H
