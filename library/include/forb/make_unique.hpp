//
// Created by gabriele on 04/12/18.
//

#ifndef FORB_MAKE_UNIQUE_H
#define FORB_MAKE_UNIQUE_H

// The std::make_unique template joined the C++ Standard Library only in C++14.
// Since this project wants to support C++11, here is an implementation of std::make_unique as
// published on ISO C++14 draft by Stephan T. Lavavej in date 2013-04-18 and accepted into the
// draft C++14 on 2013-04-20 at the Bristol UK ISO C++ meeting.
// The author thanks Scott Meyer for his directions within the book "Effective Modern C++: 42 Specific Ways to
// Improve Your Use of C++11 and C++14", published by O'Reilly Media Inc.

// See std::make_unique from C++14 Standard Library documentation.


#if(__cplusplus < 201402L)

#ifndef DOXYGEN_SHOULD_SKIP_THIS

namespace std {
    template<class T>
    struct _Unique_if {
        typedef unique_ptr<T> _Single_object;
    };

    template<class T>
    struct _Unique_if<T[]> {
        typedef unique_ptr<T[]> _Unknown_bound;
    };

    template<class T, size_t N>
    struct _Unique_if<T[N]> {
        typedef void _Known_bound;
    };

    template<class T, class... Args>
    typename _Unique_if<T>::_Single_object
    make_unique(Args &&... args) {
        return unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template<class T>
    typename _Unique_if<T>::_Unknown_bound
    make_unique(size_t n) {
        typedef typename remove_extent<T>::type U;
        return unique_ptr<T>(new U[n]());
    }

    template<class T, class... Args>
    typename _Unique_if<T>::_Known_bound
    make_unique(Args &&...) = delete;

} // namespace std

#endif // DOXYGEN_SHOULD_SKIP_THIS


#endif // __cplusplus < 201402L


#endif //FORB_MAKE_UNIQUE_H
