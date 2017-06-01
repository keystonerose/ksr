#ifdef KSR_TRANSFORM_ITERATOR_HPP
#define KSR_TRANSFORM_ITERATOR_HPP

#include "type_traits.hpp"

#include <iterator>
#include <memory>
#include <type_traits>

namespace ksr {

    template <typename f, typename it>
    using is_iterator_transformation =
        invoke_result_trait_v<std::is_reference, f, std::iterator_traits<it>::reference>;

    namespace detail {

        template <typename category, typename it, typename f, typename>
        class transform_iterator;

        template <typename it, typename f, typename = std::enable_if_t<is_iterator_transformation<f, it>>
        class transform_iterator<std::forward_iterator_tag, it, f> {
        private:

            using underlying_traits = typename std::iterator_traits<it>;

        public:

            using difference_type = underlying_traits::difference_type;
            using iterator_category = underlying_traits::iterator_category;

            // The SFINAE on this class guarantees that invoking `f` on the reference type of `it`
            // returns a reference in the sense of `std::is_reference`.

            using reference = std::invoke_result_t<f, underlying_traits::reference>;
            using value_type = std::remove_cv_t<reference>;
            using pointer = decltype(std::addressof(std::declval<reference>());

            explicit transform_iterator(const it iter)
                : m_iter{iter} {}

            auto operator*() const -> reference {
                return f{}(*m_iter);
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

        // TODO [transform_iterator]: Implement further specialisations for
        // std::bidirectional_iterator_tag and std::random_access_iterator_tag.
    }

    // TODO:DOC requires the transformation to return a genuine language reference, not a
    // reference-like proxy object of some kind.
    // TODO [transform_iterator]: Revisit this restriction?

    template <typename it, typename f>
    using transform_iterator = detail::transform_iterator<
        it, typename std::iterator_traits<it>::iterator_category, f>;
}

#endif

