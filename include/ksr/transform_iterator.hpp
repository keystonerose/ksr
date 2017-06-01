#ifndef KSR_TRANSFORM_ITERATOR_HPP
#define KSR_TRANSFORM_ITERATOR_HPP

#include <iterator>

template <typename underlying_it, typename transform_func>
class transform_iterator {
private:

    using underlying_traits = typename std::iterator_traits<underlying_it>;

public:

    using difference_type = underlying_traits::difference_type;
    using value_type =
    using pointer =
    using reference =
    using iterator_category = underlying_traits::iterator_category;

private:
};

#endif

