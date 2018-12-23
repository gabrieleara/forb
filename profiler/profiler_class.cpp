#include "profiler_class.hpp"

using ssocket = forb::streams::socket;
using shared_memory = forb::streams::shared_memory;
using stream = forb::streams::stream;

/// Initializing static attributes for forb_profiler::profiler class factory.
forb_profiler::profiler             forb_profiler::profiler::_factory{};
forb_profiler::profiler::init_class forb_profiler::profiler::_init{};

/// Match method, needed by the factory.
bool forb_profiler::profiler::_match(const std::string &type) const {
    return type == "forb_profiler::profiler";
}

/// Create method, used by the factory.
forb::remote_var forb_profiler::profiler::_create_empty() const {
    return std::make_unique<forb_profiler::profiler>();
}

/// Narrows a generic forb::remote_var reference to a forb_profiler::profiler_ptr if possible.
/// Returns nullptr if not possible.
forb_profiler::profiler_ptr forb_profiler::profiler::_narrow(forb::remote_var &&reference) {
    profiler *ptr = dynamic_cast<profiler *>(reference.get());
    
    if (ptr == nullptr) {
        return forb_profiler::profiler_ptr{nullptr};
    } else {
        // The reference is moved to the new one, after correct casting
        return std::dynamic_pointer_cast<profiler>(
                forb::remote_ptr{std::move(reference)});
    }
}

/// Narrows a generic forb::remote_ptr reference to a forb_profiler::profiler_ptr if possible.
/// Returns nullptr if not possible.
forb_profiler::profiler_ptr forb_profiler::profiler::_narrow(forb::remote_ptr &reference) {
    profiler *ptr = dynamic_cast<profiler *>(reference.get());
    
    if (ptr == nullptr) {
        return forb_profiler::profiler_ptr{nullptr};
    } else {
        // The reference is copied to the new one, after correct casting
        return std::dynamic_pointer_cast<profiler>(reference);
    }
}

/// Assigns a generic forb::remote_var reference to a forb_profiler::profiler_var if possible.
/// Returns nullptr if not possible.
forb_profiler::profiler_var forb_profiler::profiler::_assign(forb::remote_var &&reference) {
    profiler *ptr = dynamic_cast<profiler *>(reference.get());
    
    if (ptr == nullptr) {
        return profiler_var{nullptr};
    } else {
        // The original reference must be manually released first
        reference.release();
        
        return profiler_var{ptr};
    }
}

int32_t forb_profiler::profiler::method0(int32_t arg[1048576]) {
    // Calls cannot be executed concurrently, since the connection is recycled for each call.
    std::lock_guard<std::mutex> lock{base_stub::_mutex};
    
    this->init_call(forb::call_id_t_cast(profiler_method_codes::_Fmethod0E30_Forb_Array_int32_t_D_1048576_));
    
    // Objects serialization
    if (datastream->require_marshal()) {
        // Data requires marshalling before being sent
        forb::streams::marshal(arg, 1048576);
    }
    
    // Now write stuff to the stream
    datastream->send(arg, sizeof(int32_t) * 1048576);
    
    // Wait for the response
    this->wait_return();
    
    // Finally read returned value
    int32_t res_value;
    
    datastream->recv(&res_value, sizeof(res_value));
    
    if (datastream->require_marshal()) {
        res_value = forb::streams::unmarshal(res_value);
    }
    
    return res_value;
}

int32_t forb_profiler::profiler::method1(int32_t arg[2097152]) {
    // Calls cannot be executed concurrently, since the connection is recycled for each call.
    std::lock_guard<std::mutex> lock{base_stub::_mutex};
    
    this->init_call(forb::call_id_t_cast(profiler_method_codes::_Fmethod1E30_Forb_Array_int32_t_D_2097152_));
    
    // Objects serialization
    if (datastream->require_marshal()) {
        // Data requires marshalling before being sent
        forb::streams::marshal(arg, 2097152);
    }
    
    // Now write stuff to the stream
    datastream->send(arg, sizeof(int32_t) * 2097152);
    
    // Wait for the response
    this->wait_return();
    
    // Finally read returned value
    int32_t res_value;
    
    datastream->recv(&res_value, sizeof(res_value));
    
    if (datastream->require_marshal()) {
        res_value = forb::streams::unmarshal(res_value);
    }
    
    return res_value;
}

int32_t forb_profiler::profiler::method2(int32_t arg[4194304]) {
    // Calls cannot be executed concurrently, since the connection is recycled for each call.
    std::lock_guard<std::mutex> lock{base_stub::_mutex};
    
    this->init_call(forb::call_id_t_cast(profiler_method_codes::_Fmethod2E30_Forb_Array_int32_t_D_4194304_));
    
    // Objects serialization
    if (datastream->require_marshal()) {
        // Data requires marshalling before being sent
        forb::streams::marshal(arg, 4194304);
    }
    
    // Now write stuff to the stream
    datastream->send(arg, sizeof(int32_t) * 4194304);
    
    // Wait for the response
    this->wait_return();
    
    // Finally read returned value
    int32_t res_value;
    
    datastream->recv(&res_value, sizeof(res_value));
    
    if (datastream->require_marshal()) {
        res_value = forb::streams::unmarshal(res_value);
    }
    
    return res_value;
}

int32_t forb_profiler::profiler::method3(int32_t arg[8388608]) {
    // Calls cannot be executed concurrently, since the connection is recycled for each call.
    std::lock_guard<std::mutex> lock{base_stub::_mutex};
    
    this->init_call(forb::call_id_t_cast(profiler_method_codes::_Fmethod3E30_Forb_Array_int32_t_D_8388608_));
    
    // Objects serialization
    if (datastream->require_marshal()) {
        // Data requires marshalling before being sent
        forb::streams::marshal(arg, 8388608);
    }
    
    // Now write stuff to the stream
    datastream->send(arg, sizeof(int32_t) * 8388608);
    
    // Wait for the response
    this->wait_return();
    
    // Finally read returned value
    int32_t res_value;
    
    datastream->recv(&res_value, sizeof(res_value));
    
    if (datastream->require_marshal()) {
        res_value = forb::streams::unmarshal(res_value);
    }
    
    return res_value;
}

int32_t forb_profiler::profiler::method4(int32_t arg[16777216]) {
    // Calls cannot be executed concurrently, since the connection is recycled for each call.
    std::lock_guard<std::mutex> lock{base_stub::_mutex};
    
    this->init_call(forb::call_id_t_cast(profiler_method_codes::_Fmethod4E31_Forb_Array_int32_t_D_16777216_));
    
    // Objects serialization
    if (datastream->require_marshal()) {
        // Data requires marshalling before being sent
        forb::streams::marshal(arg, 16777216);
    }
    
    // Now write stuff to the stream
    datastream->send(arg, sizeof(int32_t) * 16777216);
    
    // Wait for the response
    this->wait_return();
    
    // Finally read returned value
    int32_t res_value;
    
    datastream->recv(&res_value, sizeof(res_value));
    
    if (datastream->require_marshal()) {
        res_value = forb::streams::unmarshal(res_value);
    }
    
    return res_value;
}

int32_t forb_profiler::profiler::method5(int32_t arg[33554432]) {
    // Calls cannot be executed concurrently, since the connection is recycled for each call.
    std::lock_guard<std::mutex> lock{base_stub::_mutex};
    
    this->init_call(forb::call_id_t_cast(profiler_method_codes::_Fmethod5E31_Forb_Array_int32_t_D_33554432_));
    
    // Objects serialization
    if (datastream->require_marshal()) {
        // Data requires marshalling before being sent
        forb::streams::marshal(arg, 33554432);
    }
    
    // Now write stuff to the stream
    datastream->send(arg, sizeof(int32_t) * 33554432);
    
    // Wait for the response
    this->wait_return();
    
    // Finally read returned value
    int32_t res_value;
    
    datastream->recv(&res_value, sizeof(res_value));
    
    if (datastream->require_marshal()) {
        res_value = forb::streams::unmarshal(res_value);
    }
    
    return res_value;
}

int32_t forb_profiler::profiler::method6(int32_t arg[67108864]) {
    // Calls cannot be executed concurrently, since the connection is recycled for each call.
    std::lock_guard<std::mutex> lock{base_stub::_mutex};
    
    this->init_call(forb::call_id_t_cast(profiler_method_codes::_Fmethod6E31_Forb_Array_int32_t_D_67108864_));
    
    // Objects serialization
    if (datastream->require_marshal()) {
        // Data requires marshalling before being sent
        forb::streams::marshal(arg, 67108864);
    }
    
    // Now write stuff to the stream
    datastream->send(arg, sizeof(int32_t) * 67108864);
    
    // Wait for the response
    this->wait_return();
    
    // Finally read returned value
    int32_t res_value;
    
    datastream->recv(&res_value, sizeof(res_value));
    
    if (datastream->require_marshal()) {
        res_value = forb::streams::unmarshal(res_value);
    }
    
    return res_value;
}

int32_t forb_profiler::profiler::method7(int32_t arg[134217728]) {
    // Calls cannot be executed concurrently, since the connection is recycled for each call.
    std::lock_guard<std::mutex> lock{base_stub::_mutex};
    
    this->init_call(forb::call_id_t_cast(profiler_method_codes::_Fmethod7E32_Forb_Array_int32_t_D_134217728_));
    
    // Objects serialization
    if (datastream->require_marshal()) {
        // Data requires marshalling before being sent
        forb::streams::marshal(arg, 134217728);
    }
    
    // Now write stuff to the stream
    datastream->send(arg, sizeof(int32_t) * 134217728);
    
    // Wait for the response
    this->wait_return();
    
    // Finally read returned value
    int32_t res_value;
    
    datastream->recv(&res_value, sizeof(res_value));
    
    if (datastream->require_marshal()) {
        res_value = forb::streams::unmarshal(res_value);
    }
    
    return res_value;
}


/// This method dispatches the call to the right virtual method, which must be redefined in a subclass.
void forb_profiler::profiler_skeleton::execute_call(forb::call_id_t code,
                                                    forb::streams::stream *callstream,
                                                    forb::streams::stream *datastream) {
    // callstream is the stream used by the RPC protocol and
    // datastream is the one used to exchange actual data
    // They might be the same stream
    switch ((profiler_method_codes) code) {
        case profiler_method_codes::_Fmethod0E30_Forb_Array_int32_t_D_1048576_: {
            std::unique_ptr<int32_t[]> arg = std::make_unique<int32_t[]>(1048576);
            
            datastream->recv(arg.get(), sizeof(int32_t) * 1048576);
            
            if (datastream->require_marshal()) {
                forb::streams::unmarshal(arg.get(), 1048576);
            }
            
            // Send over callstream an ACK
            forb::res_code_t  res_code = 1;
            if (callstream->require_marshal()) {
                res_code = forb::streams::marshal(res_code);
            }
            
            callstream->send(&res_code, sizeof(res_code));
            
            // Perform virtual call
            int32_t res_value;
            res_value = method0(arg.get());
            
            if (datastream->require_marshal()) {
                res_value = forb::streams::marshal(res_value);
            }
            datastream->send(&res_value, sizeof(res_value));
        }
            break;
            
        case profiler_method_codes::_Fmethod1E30_Forb_Array_int32_t_D_2097152_: {
            std::unique_ptr<int32_t[]> arg = std::make_unique<int32_t[]>(2097152);
            
            datastream->recv(arg.get(), sizeof(int32_t) * 2097152);
            
            if (datastream->require_marshal()) {
                forb::streams::unmarshal(arg.get(), 2097152);
            }
            
            // Send over callstream an ACK
            forb::res_code_t  res_code = 1;
            if (callstream->require_marshal()) {
                res_code = forb::streams::marshal(res_code);
            }
            
            callstream->send(&res_code, sizeof(res_code));
            
            // Perform virtual call
            int32_t res_value;
            res_value = method1(arg.get());
            
            if (datastream->require_marshal()) {
                res_value = forb::streams::marshal(res_value);
            }
            datastream->send(&res_value, sizeof(res_value));
        }
            break;
            
        case profiler_method_codes::_Fmethod2E30_Forb_Array_int32_t_D_4194304_: {
            std::unique_ptr<int32_t[]> arg = std::make_unique<int32_t[]>(4194304);
            
            datastream->recv(arg.get(), sizeof(int32_t) * 4194304);
            
            if (datastream->require_marshal()) {
                forb::streams::unmarshal(arg.get(), 4194304);
            }
            
            // Send over callstream an ACK
            forb::res_code_t  res_code = 1;
            if (callstream->require_marshal()) {
                res_code = forb::streams::marshal(res_code);
            }
            
            callstream->send(&res_code, sizeof(res_code));
            
            // Perform virtual call
            int32_t res_value;
            res_value = method2(arg.get());
            
            if (datastream->require_marshal()) {
                res_value = forb::streams::marshal(res_value);
            }
            datastream->send(&res_value, sizeof(res_value));
        }
            break;
            
        case profiler_method_codes::_Fmethod3E30_Forb_Array_int32_t_D_8388608_: {
            std::unique_ptr<int32_t[]> arg = std::make_unique<int32_t[]>(8388608);
            
            datastream->recv(arg.get(), sizeof(int32_t) * 8388608);
            
            if (datastream->require_marshal()) {
                forb::streams::unmarshal(arg.get(), 8388608);
            }
            
            // Send over callstream an ACK
            forb::res_code_t  res_code = 1;
            if (callstream->require_marshal()) {
                res_code = forb::streams::marshal(res_code);
            }
            
            callstream->send(&res_code, sizeof(res_code));
            
            // Perform virtual call
            int32_t res_value;
            res_value = method3(arg.get());
            
            if (datastream->require_marshal()) {
                res_value = forb::streams::marshal(res_value);
            }
            datastream->send(&res_value, sizeof(res_value));
        }
            break;
            
        case profiler_method_codes::_Fmethod4E31_Forb_Array_int32_t_D_16777216_: {
            std::unique_ptr<int32_t[]> arg = std::make_unique<int32_t[]>(16777216);
            
            datastream->recv(arg.get(), sizeof(int32_t) * 16777216);
            
            if (datastream->require_marshal()) {
                forb::streams::unmarshal(arg.get(), 16777216);
            }
            
            // Send over callstream an ACK
            forb::res_code_t  res_code = 1;
            if (callstream->require_marshal()) {
                res_code = forb::streams::marshal(res_code);
            }
            
            callstream->send(&res_code, sizeof(res_code));
            
            // Perform virtual call
            int32_t res_value;
            res_value = method4(arg.get());
            
            if (datastream->require_marshal()) {
                res_value = forb::streams::marshal(res_value);
            }
            datastream->send(&res_value, sizeof(res_value));
        }
            break;
            
        case profiler_method_codes::_Fmethod5E31_Forb_Array_int32_t_D_33554432_: {
            std::unique_ptr<int32_t[]> arg = std::make_unique<int32_t[]>(33554432);
            
            datastream->recv(arg.get(), sizeof(int32_t) * 33554432);
            
            if (datastream->require_marshal()) {
                forb::streams::unmarshal(arg.get(), 33554432);
            }
            
            // Send over callstream an ACK
            forb::res_code_t  res_code = 1;
            if (callstream->require_marshal()) {
                res_code = forb::streams::marshal(res_code);
            }
            
            callstream->send(&res_code, sizeof(res_code));
            
            // Perform virtual call
            int32_t res_value;
            res_value = method5(arg.get());
            
            if (datastream->require_marshal()) {
                res_value = forb::streams::marshal(res_value);
            }
            datastream->send(&res_value, sizeof(res_value));
        }
            break;
            
        case profiler_method_codes::_Fmethod6E31_Forb_Array_int32_t_D_67108864_: {
            std::unique_ptr<int32_t[]> arg = std::make_unique<int32_t[]>(67108864);
            
            datastream->recv(arg.get(), sizeof(int32_t) * 67108864);
            
            if (datastream->require_marshal()) {
                forb::streams::unmarshal(arg.get(), 67108864);
            }
            
            // Send over callstream an ACK
            forb::res_code_t  res_code = 1;
            if (callstream->require_marshal()) {
                res_code = forb::streams::marshal(res_code);
            }
            
            callstream->send(&res_code, sizeof(res_code));
            
            // Perform virtual call
            int32_t res_value;
            res_value = method6(arg.get());
            
            if (datastream->require_marshal()) {
                res_value = forb::streams::marshal(res_value);
            }
            datastream->send(&res_value, sizeof(res_value));
        }
            break;
            
        case profiler_method_codes::_Fmethod7E32_Forb_Array_int32_t_D_134217728_: {
            std::unique_ptr<int32_t[]> arg = std::make_unique<int32_t[]>(134217728);
            
            datastream->recv(arg.get(), sizeof(int32_t) * 134217728);
            
            if (datastream->require_marshal()) {
                forb::streams::unmarshal(arg.get(), 134217728);
            }
            
            // Send over callstream an ACK
            forb::res_code_t  res_code = 1;
            if (callstream->require_marshal()) {
                res_code = forb::streams::marshal(res_code);
            }
            
            callstream->send(&res_code, sizeof(res_code));
            
            // Perform virtual call
            int32_t res_value;
            res_value = method7(arg.get());
            
            if (datastream->require_marshal()) {
                res_value = forb::streams::marshal(res_value);
            }
            datastream->send(&res_value, sizeof(res_value));
        }
            break;
            
    }
}

