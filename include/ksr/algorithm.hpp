#ifndef KSR_ALGORITHM_HPP
#define KSR_ALGORITHM_HPP

#include "range.hpp"
#include "type_util.hpp"

#include <algorithm>
#include <cstddef>
#include <functional>
#include <iterator>
#include <type_traits>

namespace ksr {

    template <typename input_it, typename t>
    auto contains(const input_it begin, const input_it end, const t& value) -> bool {
        return std::find(begin, end, value) != end;
    }

    template <typename range_t, typename t, typename = std::enable_if_t<is_range_v<range_t>>>
    auto contains(const range_t& range, const t& value) -> bool {
        return contains(adl_begin(range), adl_end(range), value);
    }

    /// Iterates through all elements in `container` and calls Container::erase() individually for
    /// each one that satisfies `pred` (when invoked as per `std::invoke()`. `container_t` should be
    /// an STL-style container type for which repeated single-element erasure is preferable to the
    /// standard erase-remove idiom (or for which the latter approach is not possible, e.g. the
    /// associative containers). `pred` should be a function object for which
    /// `std::invoke(pred, item)` is well-formed and convertible to `bool` when `item` as an object
    /// of type `container_t::value_type`.

    template <typename container_t, typename pred_t>
    void erase_if(container_t &container, pred_t pred) {

        auto iter = adl_begin(container);
        while (iter != adl_end(container)) {

            if (std::invoke(pred, *iter)) {
                iter = container.erase(iter);
            } else {
                ++iter;
            }
        }
    }

    /// Invokes `callback` on each `k`-element partial permutation of the sorted range
    /// `[begin, end)`, in lexicographic order. Elements of this range are permuted in-place; once
    /// this algorithm returns, the range is once more sorted as if by `std::sort()`. `callback`
    /// must be a function object for which `std::invoke(callback, r)` is well-formed when `r` is an
    /// object of type `range<bidir_it>`. Note that since `range` may (at the cost of a copy) be
    /// implicitly converted to a container type, this means that `callback` may take a parameter of
    /// any appropriate owning container type if a copy of this range is to be stored. `callback`
    /// must not assign to any element in the original range.

    template <typename bidir_it, typename callback_t>
    void k_permute(
        const bidir_it begin, const bidir_it end, const std::size_t k, callback_t callback) {

        auto mid = begin;
        std::advance(mid, k);

        do {
            std::invoke(callback, range{begin, mid});
            std::reverse(mid, end);
        } while (std::next_permutation(begin, end));
    }

    template <typename range_t, typename callback_t, typename = std::enable_if_t<is_range_v<range_t>>>
    void k_permute(range_t& range, const std::size_t k, callback_t callback) {
        k_permute(adl_begin(range), adl_end(range), k, callback);
    }

    /// Invokes `mutator` as if by `std::invoke()` on a copy of `value` alongside each item in the
    /// range `[begin, end)`, and returns the subsequent value of `value`. `mutator` must be a
    /// function object for which `std::invoke(mutator, value, rhs)` is well-formed when `rhs` is an
    /// object of the value type of `input_it`; it is expected that `mutator` take `value` as a
    /// mutable reference and (potentially) modify that value when called. Note the similarity to
    /// `std::accumulate`.

    template <typename input_it, typename t, typename mutator_t>
    auto mutate_for_each(input_it begin, const input_it end, t value, mutator_t mutator) -> t {

        for (; begin != end; ++begin) {
            std::invoke(mutator, value, *begin);
        }
        return value;
    }

    template <typename range_t, typename t, typename mutator_t, typename = std::enable_if_t<is_range_v<range_t>>>
    auto mutate_for_each(const range_t& range, const t& value, mutator_t mutator) -> t {
        return mutate_for_each(adl_begin(range), adl_end(range), value, mutator);
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

    template <typename range_t, typename callback_t, typename = std::enable_if_t<is_range_v<range_t>>>
    void sub_permute(range_t& range, callback_t callback) {
        sub_permute(adl_begin(range), adl_end(range), callback);
    }
}

#endif
