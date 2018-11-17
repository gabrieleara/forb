//
// Created by gabriele on 16/11/18.
//

#ifndef FORB_UNIQUES_LIST_H
#define FORB_UNIQUES_LIST_H

#include <string>
#include <map>
#include <vector>

namespace forbcc {

    /// A template that can be used to declare a list of unique ordered items.
    template<typename T>
    class ordered_unique_list {
        using set_t = std::map<std::string, int>;
        using list_t = std::vector<T>;

        // TODO: do I need indexing now?
        /// The set is used to store the keys of each element, each paired with the index of the stored element within
        /// the list.
        set_t _set;

        /// The list contains the insertered elements in inserted order.
        list_t _list;

    public:
        /// Inserts a new value within the list if the corrisponding key is not present.
        /// Returns true on successful insertion.
        virtual bool insert(std::string key, const T &value) {
            auto mypair = make_pair(key, _list.size());
            if (_set.insert(mypair).second) {
                // Successful insertion
                _list.push_back(value);
                return true;
            }

            return false;
        };

        /// Returns a const reference to the list of the elements.
        const list_t &list() const {
            return _list;
        };

        /// Returns true if the given key is present within the set of keys.
        bool is_contained(std::string key) const {
            return _set.find(key) != _set.end();
        };

        /// Returns a reference to an element from its key.
        /// The key must be present, an exception may be thrown if that condition is not satisfied.
        T &operator[](std::string key) {
            return _list[_set.at(key)];
        };
    };

} // namespace forbcc

#endif //FORB_UNIQUES_LIST_H