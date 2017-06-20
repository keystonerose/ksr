#ifndef KSR_TRANSFORM_ITERATOR_HPP
#define KSR_TRANSFORM_ITERATOR_HPP

#include "type_traits.hpp"

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <utility>

namespace ksr {

    template <typename it, typename category>
    using is_iterator_category =
        std::is_base_of<category, typename std::iterator_traits<it>::iterator_category>;

    template <typename it, typename category>
    inline constexpr auto is_iterator_category_v = is_iterator_category<it, category>::value;

    template <typename fn, typename it>
    using is_iterator_transformation =
        invoke_result_trait<std::is_reference, fn, typename std::iterator_traits<it>::reference>;

    template <typename fn, typename it>
    inline constexpr auto is_iterator_transformation_v = is_iterator_transformation<fn, it>::value;

    namespace detail { namespace proxy_iterator {

        template <typename pred, typename it>
        using is_enabled_filter_pred = std::disjunction<
            std::is_void<pred>,
            std::is_invocable_r<bool, pred, typename std::iterator_traits<it>::reference>>;

        template <typename it, typename pred>
        class filter_policy {
        protected:

            filter_policy(const it end)
              : m_end{end} {}

            auto next(const it from) const -> it {
                return std::find_if(from, m_end, pred{});
            }

        private:
            it m_end;
        };

        template <typename it>
        class filter_policy<it, void> {
        protected:

            auto next(const it from) const -> it {
                return std::next(from);
            }
        };

        template <typename fn, typename it>
        using is_enabled_transform_fn = std::disjunction<
            std::is_void<fn>,
            is_iterator_transformation<fn, it>>;

        // TODO:DOC requires the transformation to return a genuine language reference, not a
        // reference-like proxy object of some kind.

        template <typename it, typename fn>
        class transform_policy {
        protected:

            // The SFINAE on `proxy_iterator` guarantees that invoking `fn` on the reference type of
            // `it` returns a reference in the sense of `std::is_reference`.

            using reference = std::invoke_result_t<fn, typename std::iterator_traits<it>::reference>;
            using value_type = std::remove_reference_t<reference>;
            using pointer = std::add_pointer_t<reference>;

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
        typename it, typename transform_fn, typename filter_pred,
        typename = std::enable_if_t<std::conjunction_v<
            is_iterator_category<it, std::forward_iterator_tag>,
            detail::proxy_iterator::is_enabled_transform_fn<transform_fn, it>,
            detail::proxy_iterator::is_enabled_filter_pred<filter_pred, it>>>
    >
    class proxy_iterator
      : private detail::proxy_iterator::transform_policy<it, transform_fn>,
        private detail::proxy_iterator::filter_policy<it, filter_pred> {

    private:

        using proxy_iterator_t = proxy_iterator<it, transform_fn, filter_pred>;
        using transform_policy_t = typename proxy_iterator_t::transform_policy;
        using filter_policy_t = typename proxy_iterator_t::filter_policy;

    public:

        using difference_type = typename std::iterator_traits<it>::difference_type;
        using iterator_category = std::forward_iterator_tag;

        using typename transform_policy_t::reference;
        using typename transform_policy_t::value_type;
        using typename transform_policy_t::pointer;

        template <typename... arg_ts>
        explicit proxy_iterator(const it iter, arg_ts&&... args)
            : filter_policy_t{std::forward<arg_ts>(args)...}, m_iter{next(iter)} {}

        auto operator*() const -> reference {
            return dereference(m_iter);
        }

        auto operator->() {
            return std::addressof(*(*this));
        }

        auto operator++() -> proxy_iterator& {
            m_iter = next(m_iter);
            return *this;
        }

        auto operator++(int) -> proxy_iterator& {
            auto orig = *this;
            ++(*this);
            return orig;
        }

        friend auto operator==(const proxy_iterator lhs, const proxy_iterator rhs) -> bool {
            return (lhs.m_iter == rhs.m_iter);
        }

        friend auto operator!=(const proxy_iterator lhs, const proxy_iterator rhs) -> bool {
            return !(lhs == rhs);
        }

    private:
        it m_iter;
    };
}

#endif
