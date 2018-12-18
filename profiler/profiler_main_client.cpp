//
// Created by gabriele on 04/12/18.
//

#include <iostream>
#include <fstream>
#include <functional> // Is this needed?

#include <sys/stat.h> // mkdir

#include <tuple>
#include <vector>
#include <algorithm>
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

/*
typedef int (profiler_class::*method_ptr_t)(int8_t *);

// 1    KB = 1024       B
// 8    KB = 8192       B
// 32   KB = 32768      B
// 128  KB = 131072     B
// 512  KB = 524288     B
// 1    MB = 1048576    B
// 4    MB = 4194304    B
int8_t arg00[1 * KB];
int8_t arg01[8 * KB];
int8_t arg02[32 * KB];
int8_t arg03[128 * KB];
int8_t arg04[512 * KB];
int8_t arg05[1 * MB];
int8_t arg06[4 * MB];


duration profile_method(profiler_class_var &var, method_ptr_t method, int8_t *arg) {
    time_point start_time, end_time;
    duration   time_elapsed;

    start_time = hires_clock::now();

    // Call by method pointer
    ((*var).*method)(arg);

    end_time = hires_clock::now();

    time_elapsed = end_time - start_time;

    return time_elapsed;
}
*/
/*
diff_time_t average_profile_method(profiler_class_var &var, method_ptr_t method, int8_t *arg, int num_times, int index) {

    std::cout << " - method " << index + 1 << std::endl;

    diff_time_t sum = 0;

    for (int i = 0; i < num_times; ++i) {
        profile_method()
    }

}
 */

// TODO: macro
/*
void profile_var(profiler_class_var &var, duration *time_elapsed) {
    time_elapsed[0] = profile_method(var, &profiler_class::method00, arg00);
    time_elapsed[1] = profile_method(var, &profiler_class::method01, arg01);
    time_elapsed[2] = profile_method(var, &profiler_class::method02, arg02);
    time_elapsed[3] = profile_method(var, &profiler_class::method03, arg03);
    time_elapsed[4] = profile_method(var, &profiler_class::method04, arg04);
    time_elapsed[5] = profile_method(var, &profiler_class::method05, arg05);
    time_elapsed[6] = profile_method(var, &profiler_class::method06, arg06);

    std::cout << "Done! Printing results, one per line:" << std::endl;

    for (int i = 0; i < 7; ++i) {
        std::cout << duration_cast<std::chrono::seconds>(time_elapsed[i]).count() << '\t'
                  << std::chrono::duration_cast<std::chrono::milliseconds>(time_elapsed[i]).count() << '\t'
                  << std::chrono::duration_cast<std::chrono::microseconds>(time_elapsed[i]).count() << '\t'
                  << std::endl;
    }

    std::cout << std::endl;
}

*/


int32_t arg[4 * MB / sizeof(int32_t)];

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

void profile_average(
        profiler_var &var, long size, int how_many_times, time_unit &mean, time_unit &variance) {
    long      N = 0;
    time_unit m_square;
    time_unit delta;
    time_unit x;
    duration  t;

    mean = m_square = 0;

    // First run
    // t1 = transfer_data(var, size);

    // // After the first run

    std::cout << "START:\t";

    for (int i = 0; i < how_many_times; ++i) {
        ++N;

        t = transfer_data(var, size);

        x = duration_count<microseconds>(t);

        delta = x - mean;
        mean += delta / N;
        m_square += delta * (x - mean);

        std::cout << x << "\t";

        // sum += (transfer_data(var, size) - t1) / how_many_times;
    }

    variance = m_square / (N - 1);

    std::cout << "| Average = " << mean << "\t| Variance = " << variance << std::endl;

    // return t1 + sum;
}

/*
void profile_multiple(profiler_class_single_var &var) {

    for (long size = 4 * KB; size <= 16 * GB; size *= 4) {
        std::cout << "Transferred " << double(size) / double(1024 * 1024) << " MB in: ";
        time_unit mean, variance;

        profile_average(var, size, 4, mean, variance);

        duration time = duration(mean);

        std::cout << duration_count<seconds>(time) << '\t'
                  << duration_count<milliseconds>(time) << '\t'
                  << duration_count<microseconds>(time) << '\t'
                  << std::endl;
    }
}

template<typename T>
struct statistics {
public:
    T minimum        = T(0);
    T first_quartile = T(0);
    T median         = T(0);
    T third_quartile = T(0);
    T maximum        = T(0);

private:
    using const_it = typename std::vector<T>::const_iterator;

    static std::tuple<const_it, const_it, T> calculate_median(const_it begin, const_it end) {
        // using size_t = typename std::vector<T>::size_type;
        auto size = end - begin + 1;

        // Too small, only one element
        if (size < 2) {
            T median = *begin;
            return std::make_tuple(begin, begin, median);
        }

        bool odd     = (size % 2) == 1;
        auto half    = size / 2;
        auto half_it = begin + half - 1;

        if (odd) {
            T median = *(half_it + 1);

            return std::make_tuple(half_it, half_it + 2, median);
        } else {
            T median_1 = *half_it;
            T median_2 = *(half_it + 1);
            T median   = (median_1 + median_2) / 2;

            return std::make_tuple(half_it, half_it + 1, median);
        }
    };

public:
    static statistics<T> calculate_stats(std::vector<T> &samples) {
        statistics<T> stats;

        if (samples.empty())
            return stats;

        // Optimization for one only element
        if (samples.size() == 1) {
            stats.minimum
                    = stats.first_quartile
                    = stats.median
                    = stats.third_quartile
                    = stats.maximum
                    = samples.front();

            return stats;
        }

        std::sort(samples.begin(), samples.end());

        stats.minimum = samples.front();
        stats.maximum = samples.back();

        // Optimization for only two elements
        if (samples.size() == 2) {
            stats.median         = (stats.minimum + stats.maximum) / 2;
            stats.first_quartile = stats.minimum;
            stats.third_quartile = stats.maximum;

            return stats;
        }

        // Optimization for only three elements
        if (samples.size() == 3) {
            stats.median         = samples[2];
            stats.first_quartile = stats.minimum;
            stats.third_quartile = stats.maximum;

            return stats;
        }

        // For more than three elements, use the algorithm

        auto result = calculate_median(samples.cbegin(), samples.cend() - 1);

        stats.median = std::get<2>(result);

        // The upper bound of the first half
        auto upper_second_quartile = std::get<0>(result);

        // The lower bound of the second half
        auto lower_third_quartile = std::get<1>(result);

        // Repeat for first quartile
        result = calculate_median(samples.cbegin(), upper_second_quartile);
        stats.first_quartile = std::get<2>(result);

        // Repeat for third quartile
        result = calculate_median(lower_third_quartile, samples.cend() - 1);
        stats.third_quartile = std::get<2>(result);

        return stats;
    };
};
*/
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

    // TODO: do something with this
    // std::cout << std::chrono::high_resolution_clock::period::den << std::endl;

    forb::remote_registry registry{argv[1]};

    /*
    profiler_class_var profiler;

    profiler = profiler_class::_assign(registry.get_force_socket("remote_profiler"));

    if (forb::is_nil(profiler)) {
        std::cerr << "Unable to find requested object \"" << "remote_profiler" << "\"." << std::endl;
        return EXIT_FAILURE;
    }

    diff_time_t time_elapsed[6];

    std::cout << "Profiling the class without the shared memory optimization:" << std::endl;
    profile_var(profiler, time_elapsed);

    profiler = profiler_class::_assign(registry.get("remote_profiler"));

    std::cout << "Profiling the class with the shared memory optimization:" << std::endl;
    profile_var(profiler, time_elapsed);

    */

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
    std::string  filename = "out/socket.dat";

    constexpr long max_shmem_size = 4 * GB;
    constexpr long min_size       = 4 * MB;
    constexpr long max_size       = 4 * GB;
    constexpr long multiplier     = 8;
    constexpr long repetitions    = 8;


    first_ever = true;
    std::cout << "Testing WITHOUT shared memory optimization..." << std::endl;

    profiler = profiler::_assign(registry.get_force_socket("remote_profiler_single"));
    test_variable(profiler, min_size, max_size, multiplier, repetitions, filename);

    for (long shmem_size = max_shmem_size; shmem_size > min_size; shmem_size /= (multiplier * 2)) {
        first_ever = true;
        filename   = "out/shmem_" + std::to_string(shmem_size / 1024 / 1024) + "MB.dat";
        std::cout << "Testing WITH shared memory optimization with using " << shmem_size / 1024 / 1024
                  << "MB of shared memory..." << std::endl;

        profiler = profiler::_assign(registry.get("remote_profiler_single", shmem_size));
        test_variable(profiler, min_size, max_size, multiplier, repetitions, filename);
    }

    return 0;

}
