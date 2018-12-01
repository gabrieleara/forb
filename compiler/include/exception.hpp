//
// Created by gabriele on 01/12/18.
//

#ifndef FORB_EXCEPTION_H
#define FORB_EXCEPTION_H

#include <string>
#include <exception>

namespace forbcc {

    /// Custom exception type used in the FORB IDL compiler.
    class exception : public std::exception {
        /// The description of the exception.
        std::string _what;

    public:
        /// Constructs a new exception; the first argument represents the action that was performed when the exception
        /// was thrown, while the second one is a more detailed description.
        exception(const std::string &when, const std::string &cause)
                : std::exception() {
            _what = "Error during " + when + ": " + cause + ".";
        };

        /// Virtual destructor
        ~exception() override = default;

        /// This class supports moving
        exception(exception &&) = default;

        /// This class supports moving
        exception &operator=(exception &&) = default;

        /// This class supports copying
        exception(const exception &) = default;

        /// This class supports copying
        exception &operator=(const exception &) = default;

        /// Returns the cause of the exception
        const char *what() const noexcept override {
            return _what.c_str();
        }

    };
}


#endif //FORB_EXCEPTION_H
