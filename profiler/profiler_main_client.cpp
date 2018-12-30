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

/* ******************************************* ALIASES FOR LONG STL NAMES ******************************************* */

using hires_clock = std::chrono::high_resolution_clock;
using time_point = hires_clock::time_point;
using duration = std::chrono::duration<long, hires_clock::period>;
using time_unit = duration::rep;

using microseconds = std::chrono::microseconds;
using milliseconds = std::chrono::milliseconds;
using seconds = std::chrono::seconds;
using minutes = std::chrono::minutes;
using hours = std::chrono::hours;

/* ***************************************************** MACROS ***************************************************** */

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

/// Exponentiation by square, source: Wikipedia.
template<typename B, typename E>
constexpr B pow(B base, E exponent) {
    return (exponent == 0)
           ? 1 : (exponent == 1)
                 ? base : (exponent & 0x1)
                          ? base * pow(base * base, (exponent - 1) / 2)
                          : pow(base * base, exponent / 2);
}


/* ************************************************* HASH FUNCTION ************************************************** */

/// Calculates the checksum of the given data.
/// This function is the same that will be executed on the remote host and thus it will be used
/// to check the correctness of the transmission and to simulate a constant workload.
int32_t hash(const int32_t *data, size_t length) {
    int32_t res = data[0];

    for (size_t i = 1; i < length; ++i)
        res ^= data[i];

    return res;
}

/* *************************************************** CONSTANTS **************************************************** */

#define bytes_to_words(n) ((n)/4)
#define words_to_bytes(n) ((n)*4)

constexpr size_t KB = 1024L;
constexpr size_t MB = KB * 1024L;

constexpr size_t min_size    = bytes_to_words(4 * MB);
constexpr size_t multiplier  = 2;
constexpr size_t repetitions = 16;

// Each test will transfer exactly one of the data sizes present in this array, each expressed in words
constexpr size_t sizes[8] = {
        min_size,
        min_size * multiplier,
        min_size * pow(multiplier, 2),
        min_size * pow(multiplier, 3),
        min_size * pow(multiplier, 4),
        min_size * pow(multiplier, 5),
        min_size * pow(multiplier, 6),
        min_size * pow(multiplier, 7),
};

// The memory area must be at least as large as the biggest argument
constexpr size_t arg_length = sizes[7];

// The data that will be transferred (the copy is necessary because input arrays are overwritten if using sockets)
int32_t *arg      = new int32_t[arg_length];
int32_t *arg_copy = new int32_t[arg_length];

// The expected checksum of the transferred data
int32_t expected_value[8];

/* *************************************************** FUNCTIONS **************************************************** */

/// Transfers the amount of data corresponding to sizes[i] and returns the duration of the remote call.
inline duration perform_rpc(profiler_var &var, size_t i) {
    time_point start_time, end_time;
    duration   time_elapsed;

    int32_t return_value = 0;

    // I need to copy every time arg because otherwise I would not be able to check the returned
    // value, since every time arg_copy may be overwritten in the case of marshalling
    memcpy(arg_copy, arg, words_to_bytes(sizes[i]));

    // Start measuring time
    start_time = hires_clock::now();

    switch (i) {
        case 0:
            return_value = var->method0(arg_copy);
            break;
        case 1:
            return_value = var->method1(arg_copy);
            break;
        case 2:
            return_value = var->method2(arg_copy);
            break;
        case 3:
            return_value = var->method3(arg_copy);
            break;
        case 4:
            return_value = var->method4(arg_copy);
            break;
        case 5:
            return_value = var->method5(arg_copy);
            break;
        case 6:
            return_value = var->method6(arg_copy);
            break;
        case 7:
            return_value = var->method7(arg_copy);
            break;
        default:
            assert(false);
            std::cerr << "Wrong parameter i: " << i << std::endl;
            exit(EXIT_FAILURE);
    }

    // Stop measuring time
    end_time = hires_clock::now();

    // Check correctness
    if (return_value != expected_value[i]) {
        std::cerr << "Returned value is wrong!" << std::endl;
        exit(EXIT_FAILURE);
    }

    time_elapsed = end_time - start_time;

    return time_elapsed;
}

/// Test a profiler_var object, which means a communication channel, using various data sizes.
inline void test_variable(profiler_var &var, std::string filename) {
    std::vector<time_unit> values = std::vector<time_unit>(repetitions);

    std::ofstream out(filename);

    // Column hedings
    out << "#MB\tmicroseconds" << std::endl << std::endl;

    // Test with all data sizes
    for (size_t i = 0; i < 8; ++i) {
        size_t size_in_megabytes = words_to_bytes(sizes[i]) / 1024 / 1024;

        // Print to stdout the progression
        std::cout << size_in_megabytes << "MB\t";
        std::cout.flush();

        // First transfer of each size is discarded because we want to measure the system at "steady state" and
        // statistically first measure is an outlier with respect to other measures.
        perform_rpc(var, i);

        // Perform a RPC for a certain number of repetitions
        for (size_t j = 0; j < repetitions; ++j) {
            // Perform RPC and measure time
            values[j] = count_micros(perform_rpc(var, i));

            // Print to stdout the progression
            std::cout << ".";
            std::cout.flush();
        }

        // Printing data to file
        for (auto it : values) {
            out << size_in_megabytes << "\t" << it << std::endl;
        }
        out << std::endl << std::endl;

        // Go to next line on stdout
        std::cout << std::endl;
    }

}

/// Check command-line arguments
inline void check_arguments(int argc, char *[]) {
    if (argc < 2) {
        std::cerr << "Please, give as argument the name of the file to be opened for the Remote Registry." << std::endl;
        exit(EXIT_FAILURE);
    }
}

/// Creates a directory called results if it doesn't exist already
inline void create_directory() {
    if (mkdir("results", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1) {
        if (errno == EEXIST) {
            // Everything is fine, folder alredy exists
        } else {
            // Another error occurred!
            std::cerr << "Cannot create `results` folder, error: " << strerror(errno) << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}


/// Generates the data that will be used as RPC argument.
inline void generate_data() {
    // Generated data is random, but can be controlled by setting this to a different value
    srand(42);

    std::cout << "Preparing data ";
    std::cout.flush();

    size_t i;
    size_t j;

    // Generating random data, double loop is used to print progression on stdout
    for (i = 0; i < arg_length; i += j) {
        for (j = 0; j < bytes_to_words(128 * MB); ++j) {
            arg[i + j] = rand();
        }

        // Print progression to stdout
        std::cout << '.';
        std::cout.flush();
    }
    std::cout << "\r\n" << std::endl;

    // Calculate the checksums for each data size
    for (i = 0; i < 8; ++i) {
        expected_value[i] = hash(arg, sizes[i]);
    }
}


int main(int argc, char *argv[]) {
    check_arguments(argc, argv);

    // Variables
    forb::remote_registry registry{argv[1]};
    profiler_var          profiler;
    std::string           filename;

    create_directory();

    generate_data();

    // TODO: do something with this
    // std::cout << std::chrono::high_resolution_clock::period::den << std::endl;

    // Test data transfer over socket channel
    std::cout << "Testing WITHOUT shared memory optimization..." << std::endl;

    filename = "results/socket.dat";
    profiler = profiler::_assign(registry.get_force_socket("remote_profiler_single"));
    test_variable(profiler, filename);

    // For each size, create a shared memory channel of that size and test the rpc on it
    for (int i = 8 - 1; i >= 0; --i) {
        size_t shmem_size        = words_to_bytes(sizes[i]);
        size_t size_in_megabytes = shmem_size / 1024 / 1024;

        std::cout << "Testing WITH shared memory optimization using " << size_in_megabytes
                  << "MB of shared memory..." << std::endl;

        filename = "results/shmem_" + std::to_string(size_in_megabytes) + "MB.dat";
        profiler = profiler::_assign(registry.get("remote_profiler_single", shmem_size));
        test_variable(profiler, filename);
    }

    return 0;

}
