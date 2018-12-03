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

        /* ********************************************** INNER CLASS *********************************************** */

        /// This string buffer inserts the requested indentation whenever std::endl is reached.
        class code_stringbuf : public std::stringbuf {

            /* ********************************************* ATTRIBUTES ********************************************* */
            /// Reference to the output stream.
            std::ostream &output;

            /// Current indentation level; each indentation level corresponds to 4 spaces.
            int indentation = 0;

            /* ******************************************** CONSTRUCTORS ******************************************** */
        public:
            /// Constructs a stringbuf for the given output stream.
            explicit code_stringbuf(std::ostream &output) : output(output) {};

            /**********************************************************************************************************/

            /// This class supports move construction
            code_stringbuf(code_stringbuf &&) = default;

            /// This class does not support move-assignment
            code_stringbuf &operator=(code_stringbuf &&) = delete;

            /// This class does not support copy construction
            code_stringbuf(const code_stringbuf &) = delete;

            /// This class does not support copy-assignment
            code_stringbuf &operator=(const code_stringbuf &) = delete;

            /**********************************************************************************************************/

            /// Flushes the buffer on destroy.
            ~code_stringbuf() override {
                if (pbase() != pptr()) {
                    putOutput();
                }
            };

            /// Called when we sync the buffer with the output stream.
            /// Actions are:
            /// 1. output as many indentation spaces as required, then the buffer;
            /// 2. reset the buffer;
            /// 3. flush the actual output stream we are using.
            int sync() override {
                putOutput();
                return 0;
            };

            /// Increments indentation level.
            void increment_indentation() { ++indentation; };

            /// Decrements indentation level.
            void decrement_indentation() { --indentation; };

        private:
            /// Writes buffered data to output stream, flushing it.
            void putOutput();

        };

        /* *********************************************** ATTRIBUTES *********************************************** */
    private:
        /// The buffer used
        code_stringbuf buffer;

    public:
        /* ********************************************** CONSTRUCTORS ********************************************** */

        /// Creates a new code_ostream starting from the given one.
        explicit code_ostream(std::ostream &output) : std::ostream(&buffer), buffer(output) {};

        /**************************************************************************************************************/

        /// This class is virtual, so it requires a virtual destructor
        ~code_ostream() override = default;

        /// This class does not support move construction
        code_ostream(code_ostream &&) = delete;

        /// This class does not support move-assignment
        code_ostream &operator=(code_ostream &&) = delete;

        /// This class does not support copy construction
        code_ostream(const code_ostream &) = delete;

        /// This class does not support copy-assignment
        code_ostream &operator=(const code_ostream &) = delete;

        /**************************************************************************************************************/

        /// Proxy to code_ostream::code_stringbuf::increment_indentation.
        void increment_indentation() { buffer.increment_indentation(); };

        /// Proxy to code_ostream::code_stringbuf::decrement_indentation.
        void decrement_indentation() { buffer.decrement_indentation(); };
    };

}


#endif //FORBCC_CODE_OSSTREAM_H
