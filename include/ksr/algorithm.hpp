#ifndef KSR_ALGORITHM_HPP
#define KSR_ALGORITHM_HPP

#include "type_util.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace ksr {

    /// Combines a pair of begin and past-the-end iterators of type `iter` into an object modeling
    /// the `Range` concept (as it exists in C++17) that can be iterated over in a range-based `for`
    /// loop, operated upon by generic algorithms, and implicitly copied to any type constructible
    /// from an iterator range (such as the standard library containers). Will likely be superseded
    /// by a standardised utility once the Ranges TS is merged.

    template <typename iter>
    class range {
    public:

        constexpr explicit range(const iter begin, const iter end)
            : m_begin{begin}, m_end{end} {}

        template <typename t, typename = std::enable_if_t<std::is_constructible_v<t, iter, iter>>>
        operator t() const {
            return t(m_begin, m_end);
        }

        constexpr auto begin() const noexcept -> iter {
            return m_begin;
        }

        constexpr auto end() const noexcept -> iter {
            return m_end;
        }

    private:

        iter m_begin;
        iter m_end;
    };

    template <typename iter>
    range(iter, iter) -> range<iter>;

    /// Iterates through all elements in `container` and calls Container::erase() individually for
    /// each one that satisfies `pred`. `Container` should be an STL-style container for which
    /// repeated single-element erasure is preferable to the standard erase-remove idiom (or for
    /// which the latter approach is not possible, e.g. the associative containers). `Predicate`
    /// should be a function type compatible with bool(const Container::value_type &).

    template <typename container_t, typename pred_t>
    void erase_if(container_t &container, pred_t pred) {

        using std::begin;
        using std::end;

        for (auto iter = begin(container); iter != end(container);) {
            if (pred(*iter)) {
                iter = container.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    /// Invokes `callback` on each `k`-element partial permutation of the sorted range
    /// `[begin, end)`, in lexicographic order. Elements of this range are permuted in-place; once
    /// this algorithm returns, the range is once more sorted as if by `std::sort()`. `callback`
    /// must be a function object compatible with the following signature:
    /// ```c++
    /// void callback(const range<bidir_it>&);
    /// ```
    /// Note that since `range` may (at the cost of a copy) be implicitly converted to a container
    /// type, `callback` may instead take an argument of an appropriate owning container type if a
    /// copy of this range is to be stored. `callback` must not assign to any element in the
    /// original range.

    template <typename bidir_it, typename callback_t>
    void k_permute(
        const bidir_it begin, const bidir_it end, const std::size_t k, callback_t callback) {

        auto mid = begin;
        std::advance(mid, k);

        do {
            callback(range{begin, mid});
            std::reverse(mid, end);
        } while (std::next_permutation(begin, end));
    }

    /// Invokes `callback` on each k-element partial permutation of the sorted range `[begin, end)`
    /// for all values of `k` from zero to the size of this range, as per `k_permute()`.

    template <typename bidir_it, typename callback_t>
    void sub_permute(const bidir_it begin, const bidir_it end, callback_t callback) {

        const auto size = narrow_cast<std::size_t>(std::distance(begin, end));
        for (auto k = std::size_t{0}; k <= size; ++k) {
            k_permute(begin, end, k, callback);
        }
    }
}

#endif
