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

constexpr long KB = 1024L;
constexpr long MB = KB * 1024L;
constexpr long GB = MB * 1024L;

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

constexpr long arg_length = 4 * MB / sizeof(int32_t);

int32_t arg[arg_length];

duration transfer_data(profiler_var &var, long size) {
    time_point start_time, end_time;
    duration   time_elapsed;

    start_time = hires_clock::now();

    // Transfer data
    for (; size > 0; size -= sizeof(arg)) {
        var->method(arg);
    }

    end_time = hires_clock::now();

    time_elapsed = end_time - start_time;

    return time_elapsed;
}

// Online mean and variance algorithm, Knuth

bool first_ever = true;

void test_size(profiler_var &var, unsigned long size, std::vector<duration> &samples) {
    if (first_ever) {
        transfer_data(var, size);
        first_ever = false;
    }

    for (auto &iterator : samples) {
        iterator = transfer_data(var, size);
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
                   unsigned long min_size,
                   unsigned long max_size,
                   unsigned long multiplier,
                   unsigned long repetitions,
                   std::string filename) {
    std::vector<duration>  samples = std::vector<duration>(repetitions);
    std::vector<time_unit> values  = std::vector<time_unit>(repetitions);

    std::ofstream out(filename);

    out << "#MB\tmicroseconds" << std::endl << std::endl;

    // statistics<duration> stats;

    for (unsigned long size = min_size; size <= max_size; size *= multiplier) {
        std::cout << size / 1024 / 1024 << "MB\t";
        std::cout.flush();

        first_ever = true;

        test_size(var, size, samples);

        std::cout << std::endl;

        for (unsigned int i = 0; i < repetitions; ++i) {
            values[i] = count_micros(samples[i]);
        }

        print_vector(out, size, values);

        /*
        stats = statistics<duration>::calculate_stats(samples);

        std::cout << size
                  << "," << count_micros(stats.minimum)
                  << "," << count_micros(stats.first_quartile)
                  << "," << count_micros(stats.median)
                  << "," << count_micros(stats.third_quartile)
                  << "," << count_micros(stats.maximum)
                  << std::endl;
        */
    }


}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Please, give as argument the name of the file to be opened for the Remote Registry." << std::endl;
        return EXIT_FAILURE;
    }

    srand(42);

    for (long  i = 0; i < arg_length; ++i) {
        arg[i] = rand();
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

    /*
    constexpr long max_shmem_size = 4 * GB;
    constexpr long min_size       = 4 * MB;
    constexpr long max_size       = 4 * GB;
    constexpr long multiplier     = 8;
    constexpr long repetitions    = 16;
     */

    // TODO: remove this, only for debug purposes
    constexpr long max_shmem_size = 4 * MB;
    constexpr long min_size       = 4 * MB;
    constexpr long max_size       = 4 * MB;
    constexpr long multiplier     = 8;
    constexpr long repetitions    = 1;

    first_ever = true;
    std::cout << "Testing WITHOUT shared memory optimization..." << std::endl;

    profiler = profiler::_assign(registry.get_force_socket("remote_profiler_single"));
    test_variable(profiler, min_size, max_size, multiplier, repetitions, filename);

    for (long shmem_size = max_shmem_size; shmem_size > min_size; shmem_size /= (multiplier)) {
        filename   = "results/shmem_" + std::to_string(shmem_size / 1024 / 1024) + "MB.dat";
        std::cout << "Testing WITH shared memory optimization with using " << shmem_size / 1024 / 1024
                  << "MB of shared memory..." << std::endl;

        profiler = profiler::_assign(registry.get("remote_profiler_single", shmem_size));
        test_variable(profiler, min_size, max_size, multiplier, repetitions, filename);
    }

    return 0;

}
