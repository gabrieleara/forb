//
// Created by gabriele on 04/12/18.
//

#include <iostream>
#include <fstream>

#include <sys/stat.h> // mkdir

#include <vector>
#include <chrono>

#include "profiler_class.hpp"

using namespace forb_profiler;

using hires_clock = std::chrono::high_resolution_clock;
using time_point = hires_clock::time_point;
using duration = std::chrono::duration<long, hires_clock::period>;
using time_unit = duration::rep;

using microseconds = std::chrono::microseconds;
using milliseconds = std::chrono::milliseconds;
using seconds = std::chrono::seconds;
using minutes = std::chrono::minutes;
using hours = std::chrono::hours;

constexpr size_t KB = 1024L;
constexpr size_t MB = KB * 1024L;
constexpr size_t GB = MB * 1024L;

template<typename T_time>
inline T_time duration_cast(duration d) {
    return std::chrono::duration_cast<T_time>(d);
}

template<typename T_time>
inline time_unit duration_count(duration d) {
    return duration_cast<T_time>(d).count();
}

inline time_unit count_micros(duration d) {
    return duration_count<microseconds>(d);
}

inline time_unit count_seconds(duration d) {
    return duration_count<seconds>(d);
}

inline time_unit count_minutes(duration d) {
    return duration_count<minutes>(d);
}

inline time_unit count_hours(duration d) {
    return duration_count<hours>(d);
}

int32_t hash(const int32_t *data, size_t length) {
    int32_t res = data[0];

    for (size_t i = 1; i < length; ++i)
        res ^= data[i];

    return res;
}

#define to_words(n) ((n)/4)
#define to_bytes(n) ((n)*4)

constexpr size_t arg_length  = to_words(256 * MB);
constexpr size_t max_size    = to_words(256 * MB);
constexpr size_t min_size    = to_words(4 * MB);
constexpr size_t multiplier  = 4;
constexpr size_t repetitions = 16;

constexpr size_t sizes[] = {
        min_size,
        min_size * multiplier,
        min_size * multiplier * multiplier,
        min_size * multiplier * multiplier * multiplier,
        // min_size * multiplier * multiplier * multiplier * multiplier,
        // min_size * multiplier * multiplier * multiplier * multiplier * multiplier,
};

int32_t *arg      = new int32_t[arg_length];
int32_t *arg_copy = new int32_t[arg_length];
// int32_t expected_value[6];
int32_t expected_value[4];

duration transfer_data(profiler_var &var, size_t size) {
    time_point start_time, end_time;
    duration   time_elapsed;

    int32_t return_value = 0;
    size_t  index        = 0;

    // I need to copy every time arg because otherwise I would not be able to check the returned
    // value, since every time arg_copy may be overwritten in the case of marshalling
    memcpy(arg_copy, arg, to_bytes(size));

    start_time = hires_clock::now();

    switch (size) {
        case sizes[0]:
            return_value = var->method0(arg_copy);
            index        = 0;
            break;
        case sizes[1]:
            return_value = var->method1(arg_copy);
            index        = 1;
            break;
        case sizes[2]:
            return_value = var->method2(arg_copy);
            index        = 2;
            break;
        case sizes[3]:
            return_value = var->method3(arg_copy);
            index        = 3;
            break;
            /*
        case sizes[4]:
            return_value = var->method4(arg_copy);
            index        = 4;
            break;
        case sizes[5]:
            return_value = var->method5(arg_copy);
            index        = 5;
            break;
             */
        default:
            assert(false);
            std::cerr << "Wrong size parameter: " << size << std::endl;
            exit(EXIT_FAILURE);
    }

    // Transfer data
    if (return_value != expected_value[index]) {
        std::cerr << "Returned value is wrong!" << std::endl;
        exit(EXIT_FAILURE);
    }

    end_time = hires_clock::now();

    time_elapsed = end_time - start_time;

    return time_elapsed;
}

// Online mean and variance algorithm, Knuth

void test_size(profiler_var &var, size_t size, std::vector<duration> &samples) {
    // First transfer is discarded because we want to measure the system at "steady state"
    if (size == sizes[0]) {
        transfer_data(var, size);
    }

    for (auto &sample : samples) {
        sample = transfer_data(var, size);
        std::cout << ".";
        std::cout.flush();
    }
}

template<typename T>
inline void print_vector(std::ostream &out, unsigned long size, const std::vector<T> &v) {
    for (auto it : v) {
        out << size / 1024 / 1024 << "\t" << it << std::endl;
    }

    out << std::endl << std::endl;
}

void test_variable(profiler_var &var,
                   std::string filename) {
    std::vector<duration>  samples = std::vector<duration>(repetitions);
    std::vector<time_unit> values  = std::vector<time_unit>(repetitions);

    std::ofstream out(filename);

    out << "#MB\tmicroseconds" << std::endl << std::endl;

    int i = 0;

    for (size_t size = min_size; size <= max_size; size *= multiplier) {
        std::cout << to_bytes(size) / 1024 / 1024 << "MB\t";
        std::cout.flush();

        test_size(var, to_bytes(size), samples);
        ++i;

        std::cout << std::endl;

        for (size_t i = 0; i < repetitions; ++i) {
            values[i] = count_micros(samples[i]);
        }

        print_vector(out, size, values);
    }


}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Please, give as argument the name of the file to be opened for the Remote Registry." << std::endl;
        return EXIT_FAILURE;
    }

    srand(42);

    std::cout << "Preparing data ";
    std::cout.flush();

    size_t i;
    size_t j;

    for (i = 0; i < arg_length; i += j) {
        for (j = 0; j < to_words(128 * MB); ++j) {
            arg[i + j] = rand();
        }
        std::cout << '.';
        std::cout.flush();
    }

    std::cout << "\r\n" << std::endl;

    i = 0;
    for (size_t size      = min_size; size <= max_size; size *= multiplier) {
        expected_value[i] = hash(arg, size);
        ++i;
    }

    // TODO: do something with this
    // std::cout << std::chrono::high_resolution_clock::period::den << std::endl;

    forb::remote_registry registry{argv[1]};

    if (mkdir("results", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
        if (errno == EEXIST) {
            // Everything is fine, folder alredy exists
        } else {
            // Another error occurred!
            std::cerr << "Cannot create `results` folder, error: " << strerror(errno) << std::endl;
            return EXIT_FAILURE;
        }
    }

    profiler_var profiler;
    std::string  filename = "results/socket.dat";

    std::cout << "Testing WITHOUT shared memory optimization..." << std::endl;

    profiler = profiler::_assign(registry.get_force_socket("remote_profiler_single"));
    test_variable(profiler, filename);

    for (size_t shmem_size = max_size; shmem_size >= min_size; shmem_size /= multiplier) {
        filename = "results/shmem_" + std::to_string(to_bytes(shmem_size) / 1024 / 1024) + "MB.dat";
        std::cout << "Testing WITH shared memory optimization using " << to_bytes(shmem_size) / 1024 / 1024
                  << "MB of shared memory..." << std::endl;

        profiler = profiler::_assign(registry.get("remote_profiler_single", shmem_size));
        test_variable(profiler, filename);
    }

    return 0;

}
