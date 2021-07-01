#include "base.hpp"
#define _HY_BEGIN namespace hy  {
#define _HY_END   }


_HY_BEGIN

// STREAM POSITIONING TYPES (from <streambuf>)
using streamoff  = long long;
using streamsize = long long;
// CLASS TEMPLATE fpos (from <streambuf>)
template <class _Statetype>
class fpos { // store arbitrary file position
private:
    streamoff _Myoff; // stream offset
    fpos_t _Fpos; // TRANSITION, ABI. C file position, not currently used
    _Statetype _Mystate; // current conversion state
public:
    /* implicit */ fpos(streamoff _Off = 0) : _Myoff(_Off), _Fpos(0), _Mystate() {}

    fpos(_Statetype _State, fpos_t _Fileposition) : _Myoff(_Fileposition), _Fpos(0), _Mystate(_State) {}

    _NODISCARD _Statetype state() const {
        return _Mystate;
    }

    void state(_Statetype _State) {
        _Mystate = _State;
    }

    operator streamoff() const {
        // TRANSITION, ABI: We currently always set _Fpos to 0 but older .objs containing old
        // basic_filebuf would set _Fpos.
        return _Myoff + _Fpos;
    }

#ifndef _REMOVE_FPOS_SEEKPOS
    _DEPRECATE_FPOS_SEEKPOS fpos_t seekpos() const noexcept {
        return {};
    }
#endif // _REMOVE_FPOS_SEEKPOS

    _NODISCARD streamoff operator-(const fpos& _Right) const {
        return static_cast<streamoff>(*this) - static_cast<streamoff>(_Right);
    }

    fpos& operator+=(streamoff _Off) { // add offset
        _Myoff += _Off;
        return *this;
    }

    fpos& operator-=(streamoff _Off) { // subtract offset
        _Myoff -= _Off;
        return *this;
    }

    _NODISCARD fpos operator+(streamoff _Off) const {
        fpos _Tmp = *this;
        _Tmp += _Off;
        return _Tmp;
    }

    _NODISCARD fpos operator-(streamoff _Off) const {
        fpos _Tmp = *this;
        _Tmp -= _Off;
        return _Tmp;
    }

    _NODISCARD bool operator==(const fpos& _Right) const {
        return static_cast<streamoff>(*this) == static_cast<streamoff>(_Right);
    }

    template <class _Int, enable_if_t<is_integral_v<_Int>, int> = 0>
    _NODISCARD friend bool operator==(const fpos& _Left, const _Int _Right) {
        return static_cast<streamoff>(_Left) == _Right;
    }

    template <class _Int, enable_if_t<is_integral_v<_Int>, int> = 0>
    _NODISCARD friend bool operator==(const _Int _Left, const fpos& _Right) {
        return _Left == static_cast<streamoff>(_Right);
    }

    _NODISCARD bool operator!=(const fpos& _Right) const {
        return static_cast<streamoff>(*this) != static_cast<streamoff>(_Right);
    }

    template <class _Int, enable_if_t<is_integral_v<_Int>, int> = 0>
    _NODISCARD friend bool operator!=(const fpos& _Left, const _Int _Right) {
        return static_cast<streamoff>(_Left) != _Right;
    }

    template <class _Int, enable_if_t<is_integral_v<_Int>, int> = 0>
    _NODISCARD friend bool operator!=(const _Int _Left, const fpos& _Right) {
        return _Left != static_cast<streamoff>(_Right);
    }
};


typedef struct _Mbstatet
{ // state of a multibyte translation
    unsigned long _Wchar;
    unsigned short _Byte, _State;
} _Mbstatet;
typedef _Mbstatet mbstate_t;

using streampos  = fpos<_Mbstatet>;
using streamoff  = long long;


// STRUCT TEMPLATE _Char_traits (FROM <string>)
template <class _Elem, class _Int_type>
struct _Char_traits { // properties of a string or stream element
    using char_type  = _Elem;
    using int_type   = _Int_type;
    using pos_type   = streampos;
    using off_type   = streamoff;
    using state_type = _Mbstatet;
#if _HAS_CXX20
    using comparison_category = strong_ordering;
#endif // _HAS_CXX20

    // For copy/move, we can uniformly call memcpy/memmove (or their builtin versions) for all element types.

    static _CONSTEXPR20 _Elem* copy(_Out_writes_all_(_Count) _Elem* const _First1,
        _In_reads_(_Count) const _Elem* const _First2, const size_t _Count) noexcept /* strengthened */ {
        // copy [_First2, _First2 + _Count) to [_First1, ...)
#if _HAS_MEMCPY_MEMMOVE_INTRINSICS
        __builtin_memcpy(_First1, _First2, _Count * sizeof(_Elem));
#else // ^^^ _HAS_MEMCPY_MEMMOVE_INTRINSICS ^^^ / vvv !_HAS_MEMCPY_MEMMOVE_INTRINSICS vvv
#ifdef __cpp_lib_is_constant_evaluated
        if (_STD is_constant_evaluated()) {
            // pre: [_First1, _First1 + _Count) and [_First2, _First2 + _Count) do not overlap; see LWG-3085
            for (size_t _Idx = 0; _Idx < _Count; ++_Idx) {
                _First1[_Idx] = _First2[_Idx];
            }

            return _First1;
        }
#endif // __cpp_lib_is_constant_evaluated

        _CSTD memcpy(_First1, _First2, _Count * sizeof(_Elem));
#endif // ^^^ !_HAS_MEMCPY_MEMMOVE_INTRINSICS ^^^

        return _First1;
    }

    _Pre_satisfies_(_Dest_size >= _Count) static _CONSTEXPR20 _Elem* _Copy_s(_Out_writes_all_(_Dest_size)
                                                                                 _Elem* const _First1,
        const size_t _Dest_size, _In_reads_(_Count) const _Elem* const _First2, const size_t _Count) noexcept {
        // copy [_First2, _First2 + _Count) to [_First1, _First1 + _Dest_size)
        _STL_VERIFY(_Count <= _Dest_size, "invalid argument");
        return copy(_First1, _First2, _Count);
    }

    static _CONSTEXPR20 _Elem* move(_Out_writes_all_(_Count) _Elem* const _First1,
        _In_reads_(_Count) const _Elem* const _First2, const size_t _Count) noexcept /* strengthened */ {
        // copy [_First2, _First2 + _Count) to [_First1, ...), allowing overlap
#if _HAS_MEMCPY_MEMMOVE_INTRINSICS
        __builtin_memmove(_First1, _First2, _Count * sizeof(_Elem));
#else // ^^^ _HAS_MEMCPY_MEMMOVE_INTRINSICS ^^^ / vvv !_HAS_MEMCPY_MEMMOVE_INTRINSICS vvv
#ifdef __cpp_lib_is_constant_evaluated
        if (_STD is_constant_evaluated()) {
            // dest: [_First1, _First1 + _Count)
            // src: [_First2, _First2 + _Count)
            // We need to handle overlapping ranges.
            // If _First1 is in the src range, we need a backward loop.
            // Otherwise, the forward loop works (even if the back of dest overlaps the front of src).

            if (_First1 == _First2) {
                return _First1; // Self-assignment; either loop would work, but returning immediately is faster.
            }

            // Usually, we would compare pointers with less-than, even though they could belong to different arrays.
            // However, we're not allowed to do that during constant evaluation, so we need a linear scan for equality.
            bool _Loop_forward = true;

            for (const _Elem* _Src = _First2; _Src != _First2 + _Count; ++_Src) {
                if (_First1 == _Src) {
                    _Loop_forward = false;
                    break;
                }
            }

            if (_Loop_forward) {
                for (size_t _Idx = 0; _Idx < _Count; ++_Idx) {
                    _First1[_Idx] = _First2[_Idx];
                }
            } else {
                for (size_t _Idx = 0; _Idx < _Count; ++_Idx) {
                    _First1[_Count - 1 - _Idx] = _First2[_Count - 1 - _Idx];
                }
            }

            return _First1;
        }
#endif // __cpp_lib_is_constant_evaluated

        _CSTD memmove(_First1, _First2, _Count * sizeof(_Elem));
#endif // ^^^ !_HAS_MEMCPY_MEMMOVE_INTRINSICS ^^^

        return _First1;
    }

    // For compare/length/find/assign, we can't uniformly call CRT functions (or their builtin versions).
    // 8-bit: memcmp/strlen/memchr/memset; 16-bit: wmemcmp/wcslen/wmemchr/wmemset; 32-bit: nonexistent

    _NODISCARD static _CONSTEXPR17 int compare(_In_reads_(_Count) const _Elem* _First1,
        _In_reads_(_Count) const _Elem* _First2, size_t _Count) noexcept /* strengthened */ {
        // compare [_First1, _First1 + _Count) with [_First2, ...)
        for (; 0 < _Count; --_Count, ++_First1, ++_First2) {
            if (*_First1 != *_First2) {
                return *_First1 < *_First2 ? -1 : +1;
            }
        }

        return 0;
    }

    _NODISCARD static _CONSTEXPR17 size_t length(_In_z_ const _Elem* _First) noexcept /* strengthened */ {
        // find length of null-terminated sequence
        size_t _Count = 0;
        while (*_First != _Elem()) {
            ++_Count;
            ++_First;
        }

        return _Count;
    }

    _NODISCARD static _CONSTEXPR17 const _Elem* find(
        _In_reads_(_Count) const _Elem* _First, size_t _Count, const _Elem& _Ch) noexcept /* strengthened */ {
        // look for _Ch in [_First, _First + _Count)
        for (; 0 < _Count; --_Count, ++_First) {
            if (*_First == _Ch) {
                return _First;
            }
        }

        return nullptr;
    }

    static _CONSTEXPR20 _Elem* assign(
        _Out_writes_all_(_Count) _Elem* const _First, size_t _Count, const _Elem _Ch) noexcept /* strengthened */ {
        // assign _Count * _Ch to [_First, ...)
#ifdef __cpp_lib_constexpr_string
        if (_STD is_constant_evaluated()) {
            for (_Elem* _Next = _First; _Count > 0; --_Count, ++_Next) {
                _STD construct_at(_Next, _Ch);
            }
        } else
#endif // __cpp_lib_constexpr_string
        {
            for (_Elem* _Next = _First; _Count > 0; --_Count, ++_Next) {
                *_Next = _Ch;
            }
        }

        return _First;
    }

    static _CONSTEXPR17 void assign(_Elem& _Left, const _Elem& _Right) noexcept {
#ifdef __cpp_lib_constexpr_string
        if (_STD is_constant_evaluated()) {
            _STD construct_at(_STD addressof(_Left), _Right);
        } else
#endif // __cpp_lib_constexpr_string
        {
            _Left = _Right;
        }
    }

    _NODISCARD static constexpr bool eq(const _Elem& _Left, const _Elem& _Right) noexcept {
        return _Left == _Right;
    }

    _NODISCARD static constexpr bool lt(const _Elem& _Left, const _Elem& _Right) noexcept {
        return _Left < _Right;
    }

    _NODISCARD static constexpr _Elem to_char_type(const int_type& _Meta) noexcept {
        return static_cast<_Elem>(_Meta);
    }

    _NODISCARD static constexpr int_type to_int_type(const _Elem& _Ch) noexcept {
        return static_cast<int_type>(_Ch);
    }

    _NODISCARD static constexpr bool eq_int_type(const int_type& _Left, const int_type& _Right) noexcept {
        return _Left == _Right;
    }

    _NODISCARD static constexpr int_type not_eof(const int_type& _Meta) noexcept {
        return _Meta != eof() ? _Meta : !eof();
    }

    _NODISCARD static constexpr int_type eof() noexcept {
        return static_cast<int_type>(EOF);
    }
};


template <class _Elem>
struct char_traits : _Char_traits<_Elem, long> {};

_HY_END