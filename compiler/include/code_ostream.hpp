//
// Created by gabriele on 14/11/18.
//

#ifndef FORBCC_CODE_OSSTREAM_H
#define FORBCC_CODE_OSSTREAM_H

#include <ostream>
#include <sstream>

namespace forbcc {

    /// This output stream supports indentation of code lines, which are formatted at each std::endl.
    class code_ostream : public std::ostream {

        /// This string buffer inserts the requested indentation whenever std::endl is reached.
        class code_stringbuf : public std::stringbuf {
            std::ostream &output;
            int          indentation = 0;

        public:
            /// Constructs a stringbuf for the given output stream
            explicit code_stringbuf(std::ostream &output) : output(output) {};

            ~code_stringbuf() override {
                if (pbase() != pptr()) {
                    putOutput();
                }
            };

            // When we sync the stream with the output.
            // 1) Output as many indentation spaces as required, then the buffer
            // 2) Reset the buffer
            // 3) flush the actual output stream we are using.
            int sync() override {
                putOutput();
                return 0;
            };

            /// Increments indentation level
            void increment_indentation() { ++indentation; };

            /// Decrements indentation level
            void decrement_indentation() { --indentation; };

        private:
            /// Writes buffered data to output stream
            void putOutput();

        };

        /// The buffer used
        code_stringbuf buffer;

    public:

        /// Creates a new code_ostream starting from the given one
        explicit code_ostream(std::ostream &output) : std::ostream(&buffer), buffer(output) {};

        /// Proxy to code_stringbuf::increment_indentation
        void increment_indentation() { buffer.increment_indentation(); };

        /// Proxy to code_stringbuf::decrement_indentation
        void decrement_indentation() { buffer.decrement_indentation(); };
    };

}


#endif //FORB_CODE_OSSTREAM_H
