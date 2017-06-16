#ifndef KSR_TRANSFORM_ITERATOR_HPP
#define KSR_TRANSFORM_ITERATOR_HPP

#include "meta.hpp"
#include "type_traits.hpp"

#include <iterator>
#include <memory>
#include <type_traits>

namespace ksr {

    template <typename it, typename category>
    using is_iterator_category =
        std::is_base_of_v<category, typename std::iterator_traits<it>::iterator_category>;

    template <typename it, typename category>
    inline constexpr auto is_iterator_category_v = is_iterator_category<it, category>::value;

    template <typename fn, typename it>
    using is_iterator_transformation =
        invoke_result_trait<std::is_reference, fn, typename std::iterator_traits<it>::reference>;

    template <typename fn, typename it>
    inline constexpr auto is_iterator_transformation_v = is_iterator_transformation<fn, it>::value;

    namespace iterator_policy {

        template <typename pred>
        class filter {
        };

        template <>
        class filter<void> {
        };

        template <typename fn>
        class transform {
        };

        template <>
        class transform<void> {
        };
    }

    template <
        typename it, typename transform_fn, typename filter_pred,
        typename = std::enable_if_t<
            is_iterator_category_v<it, std::forward_iterator_tag> &&
            is_iterator_transformation_v<transform_fn, it>> &&
            std::is_invocable_r_v<bool, filter_pred, typename std::iterator_traits<it>::reference>>
    >
    class transform_iterator {
    private:

        using underlying_traits = typename std::iterator_traits<it>;

    public:

        using difference_type = underlying_traits::difference_type;
        using iterator_category = std::forward_iterator_tag;

        // The SFINAE on this class guarantees that invoking `f` on the reference type of `it`
        // returns a reference in the sense of `std::is_reference`.

        using reference = std::invoke_result_t<f, underlying_traits::reference>;
        using value_type = std::remove_cv_t<reference>;
        using pointer = decltype(std::addressof(std::declval<reference>());

        explicit transform_iterator(const it iter)
            : m_iter{iter} {

            while (!filter_pred{}(*)) {

            }
        }

        auto operator*() const -> reference {
            return transform_fn{}(*m_iter);
        }

        auto operator->() {
            return std::addressof(*(*this));
        }

        auto operator++() -> transform_iterator& {
            ++m_iter;
            return *this;
        }

        auto operator++(int) -> transform_iterator& {
            auto orig = *this;
            ++(*this);
            return orig;
        }

        friend auto operator==(const transform_iterator lhs, const transform_iterator rhs) -> bool {
            return (rhs.m_iter == rhs.m_iter);
        }

        friend auto operator!=(const transform_iterator lhs, const transform_iterator rhs) -> bool {
            return !(lhs == rhs);
        }

    protected:

        it m_iter;
    };

    // TODO:DOC requires the transformation to return a genuine language reference, not a
    // reference-like proxy object of some kind.
    // TODO [transform_iterator]: Revisit this restriction?

    template <typename it, typename fn>
    using transform_iterator = detail::transform_iterator<
        it, typename std::iterator_traits<it>::iterator_category, f>;
}

#endif
