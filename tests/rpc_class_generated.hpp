//
// Created by gabriele on 16/11/18.
//

#ifndef FORB_RPC_CLASS_GENERATED_HPP
#define FORB_RPC_CLASS_GENERATED_HPP

#include <forb/forb.hpp>

namespace example {
    struct structure {
        long a1;
        short a2;
        char a3[10];
    };

    class rpc_class;

    using rpc_class_ptr = std::shared_ptr<rpc_class>;
    using rpc_class_var = std::unique_ptr<rpc_class>;

    class rpc_class : public virtual forb::base_stub {
    private:
        // Default constructor, no other constructor needed
        rpc_class() = default;

    public:
        // Methods as listed in the specification file
        int sum(int a, int b, example::structure &c);

    protected:
        // Virtual methods needed for the factory
        bool _match(const std::string &type) const override;

        forb::base_stub *_create_empty() const override;

    public:
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

        // Methods as listed in the specification file
        virtual int sum(int a, int b, example::structure &c) = 0;

    protected:
        void execute_call(forb::call_id_t code,
                          forb::streams::stream *callstream,
                          forb::streams::stream *datastream) override;

    };
} // namespace example



#endif //FORB_RPC_CLASS_GENERATED_HPP