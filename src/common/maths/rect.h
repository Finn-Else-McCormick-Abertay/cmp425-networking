#pragma once

#include <prelude.h>
#include <prelude/vec.h>
#include <alias/concepts.h>

template<typename T, size_t Size>
class rect {
public:
    vec<T, Size> origin; vec<T, Size> size;

    constexpr rect() : rect{vmath_hpp::zero_init} {}

    constexpr rect(vmath_hpp::no_init_t) {}
    constexpr rect(vmath_hpp::zero_init_t) : origin{T{0}}, size{T{0}} {}
    constexpr rect(vmath_hpp::unit_init_t) : origin{T{1}}, size{T{1}} {}
    
    template<convertible_to<T> U = T, convertible_to<T> V = T>
    constexpr rect(const vec<U, Size>& origin, const vec<V, Size>& size) : origin{origin}, size{size} {}

    [[nodiscard]] constexpr vec<T, Size>& operator[](size_t index) noexcept {
        switch (index) {
            default:
            case 0: return origin;
            case 1: return size;
        }
    }

    [[nodiscard]] constexpr const vec<T, Size>& operator[](size_t index) const noexcept {
        switch (index) {
            default:
            case 0: return origin;
            case 1: return size;
        }
    }

    [[nodiscard]] constexpr vec<T, Size>& at(size_t index) {
        VMATH_HPP_THROW_IF(index >= size, std::out_of_range("rect::at"));
        return (*this)[index];
    }

    [[nodiscard]] constexpr const vec<T, Size>& at(size_t index) const {
        VMATH_HPP_THROW_IF(index >= size, std::out_of_range("rect::at"));
        return (*this)[index];
    }

    template<convertible_to<T> U = T> requires (!unsigned_integral<T>)
    static rect<T, Size> centred(const vec<U, Size>& size) {
        rect<T, Size> ret { -size, size };
        ret.origin /= T{2};
        return ret;
    }
};

// -- Aliases --

template<typename T> using rect2 = rect<T, 2>;
using frect2 = rect2<float>;
using irect2 = rect2<int>;
using urect2 = rect2<unsigned>;

// -- Operators --

// addition

template <typename T, typename U, size_t Size>
rect<T, Size> operator +(const rect<T, Size>& r, const vec<U, Size>& v) {
    return rect<T, Size>{ r.origin + v, r.size };
}

template <typename T, typename U, size_t Size>
rect<T, Size>& operator +=(const rect<T, Size>& r, const vec<U, Size>& v) {
    r.origin += v;
    return r;
}

// subtraction

template <typename T, typename U, size_t Size>
rect<T, Size> operator -(const rect<T, Size>& r, const vec<U, Size>& v) {
    return rect<T, Size>{ r.origin - v, r.size };
}

template <typename T, typename U, size_t Size>
rect<T, Size>& operator -=(const rect<T, Size>& r, const vec<U, Size>& v) {
    r.origin -= v;
    return r;
}

// -- Free Functions --

namespace maths_impl {
    // Based on the following (because logicking it out properly was making my brain hurt for some reason):
    // https://scicomp.stackexchange.com/questions/26258/the-easiest-way-to-find-intersection-of-two-intervals
    // https://blogs.sas.com/content/sgf/2022/01/13/calculating-the-overlap-of-date-time-intervals/

    template<typename T, typename U>
    bool overlap(T start1, T end1, U start2, U end2) { return end1 >= start2 && end2 >= start1; }

    template<typename T, convertible_to<T> U>
    T overlap_by(T start1, T end1, U start2, U end2) {
        return max(0, min(end1, end2) - max(start1, start2) + 1);
    }
}

template<typename T, typename U, size_t Size>
bool overlap(const rect<T, Size>& lhs, const rect<U, Size>& rhs) {
    for (size_t i = 0; i < Size; ++i) {
        if (!maths_impl::overlap(
            lhs[0][i], lhs[0][i] + lhs[1][i],
            rhs[0][i], rhs[0][i] + rhs[1][i]
        )) return false;
    }
    return true;
}

template<typename T, convertible_to<T> U, size_t Size>
vec<T, Size> overlap_by(const rect<T, Size>& lhs, const rect<U, Size>& rhs) {
    vec<T, Size> ret {vmath_hpp::no_init};
    for (size_t i = 0; i < Size; ++i) {
        // Assert sizes are positive, and thus that these are valid AABBs
        assert(lhs.size[i] >= 0); assert(rhs.size[i] >= 0);
        ret[i] = maths_impl::overlap_by(
            lhs.origin[i], lhs.origin[i] + lhs.size[i],
            rhs.origin[i], rhs.origin[i] + rhs.size[i]
        );
    }
    return ret;
}