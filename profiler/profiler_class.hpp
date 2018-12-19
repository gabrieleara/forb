#ifndef PROFILER_CLASS_HPP
#define PROFILER_CLASS_HPP

#include <forb/forb.hpp>

namespace forb_profiler {
    struct roba {
        int8_t a[131072];
    };
    
    
    class profiler;
    
    using profiler_ptr = std::shared_ptr<profiler>;
    using profiler_var = std::unique_ptr<profiler>;
    
    class profiler : public virtual forb::base_stub {
    public:
        // Importing constructor from superclass
        using forb::base_stub::base_stub;
        
    public:
        // Methods as listed in the specification file
        int32_t method(int32_t arg[1048576]);
        forb_profiler::roba method2(int32_t arg[1048576]);
        
    protected:
        // Virtual methods needed for the factory
        bool _match(const std::string &type) const override;
        
        forb::remote_var _create_empty() const override;
        
    public:
        static profiler_ptr _narrow(forb::remote_var &&reference);
        
        static profiler_ptr _narrow(forb::remote_ptr &reference);
        
        static profiler_var _assign(forb::remote_var &&reference);
        
    private:
        // Static instance that will act as factory
        static profiler _factory;
        
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
    
    class profiler_skeleton : public virtual forb::base_skeleton {
    
    public:
        // Importing constructors from base class
        using base_skeleton::base_skeleton;
        
        // Methods as listed in the specification file
        virtual int32_t method(int32_t arg[1048576]) = 0;
        virtual forb_profiler::roba method2(int32_t arg[1048576]) = 0;
        
    protected:
        void execute_call(forb::call_id_t code,
                          forb::streams::stream *callstream,
                          forb::streams::stream *datastream) final;
        
    };
    
    /// Enumerator used to distinguish the requested call.
    enum class profiler_method_codes : forb::call_id_t {
        _FmethodE30_Forb_Array_int32_t_D_1048576_,
        _Fmethod2E30_Forb_Array_int32_t_D_1048576_,
    };
    
    
} // namespace forb_profiler

#endif // PROFILER_CLASS_HPP
