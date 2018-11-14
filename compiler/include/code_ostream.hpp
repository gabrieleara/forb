//
// Created by gabriele on 14/11/18.
//

#ifndef FORBCC_CODE_OSSTREAM_H
#define FORBCC_CODE_OSSTREAM_H

#include <ostream>
#include <sstream>

namespace forbcc {

    /**
     * @brief   The following string buffer adds correct indentation before the
     *          line when endl is reached.
     */
    class code_ostream : public std::ostream {

        class code_stringbuf : public std::stringbuf {
            std::ostream &output;
            int indentation = 0;

        public:
            code_stringbuf(std::ostream &output) : output(output) {};

            ~code_stringbuf() {
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

            void increment_indentation() { ++indentation; };

            void decrement_indentation() { --indentation; };

        private:
            void putOutput();

        };

        code_stringbuf buffer;

    public:
        code_ostream(std::ostream &output) : std::ostream(&buffer), buffer(output) {};

        void increment_indentation() { buffer.increment_indentation(); };

        void decrement_indentation() { buffer.decrement_indentation(); };
    };

}


#endif //FORB_CODE_OSSTREAM_H
