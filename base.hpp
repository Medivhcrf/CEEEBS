
// TRAITS ROAD BEGIN
typedef  int __int64; //FAKE
typedef __int64 fpos_t;


//ALLOCATOR
#define _NODISCARD [[nodiscard]]

typedef unsigned int     size_t;
typedef int              ptrdiff_t;
#define _INLINE_VAR
#define _CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS



#if !defined(_HAS_CXX17) && !defined(_HAS_CXX20)
    #if defined(_MSVC_LANG)
        #define _STL_LANG _MSVC_LANG
    #elif defined(__cplusplus) // ^^^ use _MSVC_LANG / use __cplusplus vvv
        #define _STL_LANG __cplusplus
    #else  // ^^^ use __cplusplus / no C++ support vvv
        #define _STL_LANG 0L
    #endif // ^^^ no C++ support ^^^

    #if _STL_LANG > 201703L
        #define _HAS_CXX17 1
        #define _HAS_CXX20 1
    #elif _STL_LANG > 201402L
        #define _HAS_CXX17 1
        #define _HAS_CXX20 0
    #else // _STL_LANG <= 201402L
        #define _HAS_CXX17 0
        #define _HAS_CXX20 0
    #endif // Use the value of _STL_LANG to define _HAS_CXX17 and _HAS_CXX20

    #undef _STL_LANG
#endif // !defined(_HAS_CXX17) && !defined(_HAS_CXX20)

#ifndef _HAS_FEATURES_REMOVED_IN_CXX20
#define _HAS_FEATURES_REMOVED_IN_CXX20 (!_HAS_CXX20)
#endif // _HAS_FEATURES_REMOVED_IN_CXX20

#ifndef _HAS_DEPRECATED_ALLOCATOR_MEMBERS
#define _HAS_DEPRECATED_ALLOCATOR_MEMBERS (_HAS_FEATURES_REMOVED_IN_CXX20)
#endif // _HAS_DEPRECATED_ALLOCATOR_MEMBERS