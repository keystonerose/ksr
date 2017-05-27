#ifndef KSR_RANGE_HPP
#define KSR_RANGE_HPP

#include "type_traits.hpp"

#include <iterator>
#include <type_traits>
#include <utility>

namespace ksr {

    namespace detail { namespace with_adl {

        using std::begin;
        using std::end;

        template <typename t>
        constexpr auto is_range(int)
            -> decltype(begin(std::declval<t>()), end(std::declval<t>()), bool{}) { return true; }

        template <typename t>
        constexpr auto is_range(...) -> bool { return false; }

    }}

    /// Determines whether the type `t` is a range in the C++17 sense; that is, are the following
    /// expressions well-formed when `x` is an object of type `t`?
    /// ```c++
    /// using std::begin;
    /// begin(x);
    ///
    /// using std::end;
    /// end(x);
    /// ```
    /// Will likely be superseded by a standardised utility once the Ranges TS is merged.

    template <typename t>
    struct is_range : std::bool_constant<detail::with_adl::is_range<t>(int{})> {};

    template <typename t>
    inline constexpr auto is_range_v = is_range<t>::value;

    namespace detail { namespace with_adl {

        // These implementations are hoisted into the detail namespace so that it's possible to get
        // the noexcept specifications of the public versions correct.

        template <typename range_t, typename = std::enable_if_t<is_range_v<range_t>>>
        constexpr auto begin_(range_t& range) noexcept(noexcept(begin(range)))
            -> decltype(auto) { return begin(range); }

        template <typename range_t, typename = std::enable_if_t<is_range_v<range_t>>>
        constexpr auto end_(range_t& range) noexcept(noexcept(end(range)))
            -> decltype(auto) { return end(range); }
    }}

    /// Equivalent to the expressions `begin(range)` and `end(range)` when the following
    /// declarations are in scope:
    /// ```c++
    /// using std::begin;
    /// using std::end;
    /// ```

    template <typename range_t, typename = std::enable_if_t<is_range_v<range_t>>>
    constexpr auto adl_begin(range_t& range)
        noexcept(noexcept(detail::with_adl::begin_(range)))
        -> decltype(auto) { return detail::with_adl::begin_(range); }

    template <typename range_t, typename = std::enable_if_t<is_range_v<range_t>>>
    constexpr auto adl_end(range_t& range)
        noexcept(noexcept(detail::with_adl::end_(range)))
        -> decltype(auto) { return detail::with_adl::end_(range); }

    /// Archetype of the `is_range` type trait: combines a pair of begin and past-the-end iterators
    /// of type `iter` into an object modeling the `Range` concept (as it exists in C++17) that can
    /// be succinctly iterated over in a range-based `for` loop. Will likely be superseded by a
    /// standardised utility once the Ranges TS is merged.

    template <typename iter>
    class range {
    public:

        constexpr explicit range(const iter begin, const iter end)
            noexcept(std::is_nothrow_copy_constructible_v<iter>)
          : m_begin{begin}, m_end{end} {}

        constexpr auto begin() const noexcept(std::is_nothrow_copy_constructible_v<iter>) -> iter {
            return m_begin;
        }

        constexpr auto end() const noexcept(std::is_nothrow_copy_constructible_v<iter>) -> iter {
            return m_end;
        }

    private:

        iter m_begin;
        iter m_end;
    };

    template <typename iter>
    range(iter, iter) -> range<iter>;
}

#endif
