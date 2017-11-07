#ifndef KSR_ALGORITHM_TRANSFORM_TO_HPP
#define KSR_ALGORITHM_TRANSFORM_TO_HPP

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>

namespace ksr {

    template <typename t>
    t make_container(std::size_t) {
        return t{};
    }

    template <>
    std::vector<>


    template <typename t, typename input_it, typename unary_op>
    t transform_to(const input_it begin, const input_it end, unary_op op) {

        auto result = make_container<out_t>(std::distance(begin, end));
        std::transform(begin, end, std::inserter(result, std::end(result)));
        return result;
    }
}

#endif
