#ifndef KSR_FUNCTION_VIEW_HPP
#define KSR_FUNCTION_VIEW_HPP

#include "type_traits.hpp"

#include <memory>
#include <type_traits>
#include <utility>

namespace ksr {

    ///
    /// A lightweight, type-erased reference to a callable object. This is intended to be used in
    /// passing function-like objects as parameters to other functions: the traditional approach of
    /// templating the receiving function can become cumbersome or simply not possible in certain
    /// circumstances (e.g. for virtual functions). \ref function_view is cheap to construct, copy
    /// and move, and can therefore by passed by value. Note that unlike \c std::function,
    /// \ref function_view takes no ownership of the function object passed to its constructor; it
    /// is the responsibility of calling code to ensure that (like \c std::string_view and any other
    /// reference type) the \ref function_view does not outlive the callable object it points to.
    ///

    template <typename>
    class function_view;

    template <typename ret_t, typename... arg_ts>
    class function_view<ret_t(arg_ts...)> {
    public:

        template <
            typename function_t,
            typename = std::enable_if_t<!matches_special_ctr_v<function_view, function_t>>::value>
        >
        function_view(function_t&& function) noexcept
          : m_invoke{&function_view::invoke<function_t>}, m_ptr{std::addressof(function)} {}

        // With C++17, it is possible to get the noexcept specification of operator() correct for
        // the erased function. However, std::function still doesn't provide this behaviour, and it
        // seems a little more effort than it's worth for now.

        ret_t operator()(arg_ts&&... args) const {
            return (this->*m_invoke)(std::forward<arg_ts>(args)...);
        }

    private:

        template <typename t>
        ret_t invoke(arg_ts&&... args) const {
            const auto function = reinterpret_cast<std::add_pointer_t<t>>(m_ptr);
            return (*function)(std::forward<arg_ts>(args)...);
        }

        using invoke_inst = ret_t (function_view::*)(arg_ts&&...) const;
        invoke_inst m_invoke;
        void* m_ptr;
    };
}

#endif
