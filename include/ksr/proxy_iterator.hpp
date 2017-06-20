#ifndef KSR_TRANSFORM_ITERATOR_HPP
#define KSR_TRANSFORM_ITERATOR_HPP

#include "type_traits.hpp"

#include <algorithm>
#include <iterator>
#include <memory>
#include <type_traits>
#include <utility>

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

    namespace detail { namespace proxy_iterator {

        template <
            typename it, typename pred,
            typename = std::enable_if_t<
                std::is_void_v<pred> ||
                std::is_invocable_r_v<bool, filter_pred, typename std::iterator_traits<it>::reference>>
        >
        class filter_policy {
        protected:

            filter_policy(const it end)
              : m_end{end} {}

            auto next(const it from) const -> it {
                return std::find_if(from, end, pred{});
            }

        private:
            it m_end;
        };

        template <typename it, typename enable_if_t>
        class filter_policy<it, void, enable_if_t> {
        protected:

            auto next(const it from) const -> it {
                return std::next(from);
            }
        };

        // TODO:DOC requires the transformation to return a genuine language reference, not a
        // reference-like proxy object of some kind.

        template <
            typename it, typename fn,
            typename = std::enable_if_t<std::is_void_v<fn> || is_iterator_transformation_v<fn, it>>
        >
        class transform_policy {
        protected:

            // The SFINAE on this class guarantees that invoking `fn` on the reference type of `it`
            // returns a reference in the sense of `std::is_reference`.

            using reference = std::invoke_result_t<fn, typename std::iterator_traits<it>::reference>;
            using value_type = std::remove_cv_t<reference>;
            using pointer = decltype(std::addressof(std::declval<reference>());

            auto dereference(const it iter) const -> reference {
                return fn{}(*iter);
            }
        };

        template <typename it>
        class transform_policy<it, void> {
        protected:

            using reference = typename std::iterator_traits<it>::reference;
            using value_type = typename std::iterator_traits<it>::value_type;
            using pointer = typename std::iterator_traits<it>::pointer;

            auto dereference(const it iter) const -> reference {
                return *iter;
            }
        };

    }}

    template <
        typename it, typename transform_fn, typename filter_pred
        typename = std::enable_if_t<is_iterator_category_v<it, std::forward_iterator_tag>>
    >
    class proxy_iterator
      : private detail::proxy_iterator::transform_policy<it, transform_fn>,
        private detail::proxy_iterator::filter_policy<it, filter_pred> {

    public:

        using difference_type = typename std::iterator_traits<it>::difference_type;
        using iterator_category = std::forward_iterator_tag;

        using transform_policy::reference;
        using transform_policy::value_type;
        using transform_policy::pointer;

        template <typename... arg_ts>
        explicit transform_iterator(const it iter, arg_ts&&... args)
            : filter_policy{std::forward<arg_ts>(args)...}, m_iter{next(iter)} {}

        auto operator*() const -> reference {
            return dereference(m_iter);
        }

        auto operator->() {
            return std::addressof(*(*this));
        }

        auto operator++() -> transform_iterator& {
            m_iter = next(m_iter);
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

    private:
        it m_iter;
    };
}

#endif
