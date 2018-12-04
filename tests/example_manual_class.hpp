//
// Created by gabriele on 02/11/18.
//

#ifndef LIBFORB_RPC_CLASS_EXAMPLE_HPP
#define LIBFORB_RPC_CLASS_EXAMPLE_HPP

#include <string>

#include <forb/forb.hpp> // "forb.hpp"

namespace example {

    class rpc_class;

    using rpc_class_ptr = std::shared_ptr<rpc_class>;
    using rpc_class_var = std::unique_ptr<rpc_class>;

    class rpc_class : public virtual forb::base_stub {
    public:
        // Importing constructor from superclass
        using forb::base_stub::base_stub;

    public:
        // Methods as listed in the specification file
        int sum_ints(int a, int b);

    protected:
        // Virtual methods needed for the factory
        bool _match(const std::string &type) const override;

        forb::remote_var _create_empty() const override;

    public:
        // Method needed to downcast pointers to this class
        static rpc_class_ptr _narrow(forb::remote_var &&reference);

        static rpc_class_ptr _narrow(forb::remote_ptr &reference);

        static rpc_class_var _assign(forb::remote_var &&reference);

    private:
        // Static instance that will act as factory
        static rpc_class _factory;

        // Inner class used to push the factory in the polymorphic one
        class init_class {
        public:
            init_class() {
                forb::base_stub::_protos().push_back(&_factory);
            }
        };

        // Attribute used to execute code at program initialization
        static init_class _init;

    };

    class rpc_class_skeleton : public virtual forb::base_skeleton {

    public:
        // Importing constructors from base class
        using base_skeleton::base_skeleton;

        virtual int sum_ints(int a, int b) = 0;

    protected:
        void execute_call(forb::call_id_t code,
                          forb::streams::stream *callstream,
                          forb::streams::stream *datastream) final override;

    };

    class rpc_class_impl : public rpc_class_skeleton {
    public:
        // Importing constructors
        using rpc_class_skeleton::rpc_class_skeleton;

        int sum_ints(int a, int b) override;
    };


} // forb::example


#endif //LIBFORB_RPC_CLASS_EXAMPLE_STUB_HPP
