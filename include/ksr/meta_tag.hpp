#ifndef KSR_META_TAG_HPP
#define KSR_META_TAG_HPP

#include <cstddef>
#include <type_traits>

/*[[[cog

import cog
import string

tag_configs = [{
        'name': 'value',
        'sig': 'auto',
        'tag_contents': 'static constexpr auto value = elem;',
    }, {
        'name': 'type',
        'sig': 'typename',
        'tag_contents': 'using type = elem;',
    }, {
        'name': 'value_func',
        'sig': 'template <auto...> class',
        'tag_contents': 'template <auto... args> using func = elem<args...>;',
    }, {
        'name': 'type_func',
        'sig': 'template <typename...> class',
        'tag_contents': 'template <typename... args> using func = elem<args...>;',
    }]

]]]*/
//[[[end]]]

namespace ksr { namespace meta {

    // TODO:DOC

    /*[[[cog

    with open('meta_tag_impl.in.hpp', 'r') as impl_file:
        impl_template = string.Template(impl_file.read())

    cog.outl()
    for config in tag_configs:
        cog.outl(impl_template.substitute(**config))

    ]]]*/

    template <typename t, template <auto...> class f>
    struct in_value_domain : std::false_type {};

    template <template <auto...> class f, auto... args>
    struct in_value_domain<f<args...>, f> : std::true_type {};

    template <typename t, template <auto...> class f>
    inline constexpr auto in_value_domain_v = is_value_domain<t, f>::value;

    template <auto elem>
    struct value_tag {
        static constexpr auto value = elem;
    };

    template <auto elem>
    constexpr auto operator==(value_tag<elem>, value_tag<elem>) -> bool {
        return true;
    }

    template <auto lhs_elem, auto rhs_elem>
    constexpr auto operator==(value_tag<lhs_elem>, value_tag<rhs_elem>) -> bool {
        return false;
    }

    template <auto lhs_elem, auto rhs_elem>
    constexpr auto operator!=(const value_tag<lhs_elem> lhs, const value_tag<rhs_elem> rhs) -> bool {
        return !(lhs == rhs);
    }

    template <auto...>
    struct value_seq;

    template <>
    struct value_seq<> {};

    template <auto head_elem, auto... tail_elems>
    struct value_seq<head_elem, tail_elems...> {
        static constexpr auto head = value_tag<head_elem>{};
        static constexpr auto tail = value_seq<tail_elems...>{};
    };

    template <auto... elems>
    constexpr auto size(value_seq<elems...>) -> std::size_t {
        return sizeof...(elems);
    }

    template <auto elem, auto... seq_elems>
    constexpr auto push_back(value_seq<seq_elems...>, value_tag<elem> = {}) {
        return value_seq<seq_elems..., elem>{};
    }

    template <auto elem, auto... seq_elems>
    constexpr auto push_front(value_seq<seq_elems...>, value_tag<elem> = {}) {
        return value_seq<elem, seq_elems...>{};
    }

    template <typename t, template <typename...> class f>
    struct in_type_domain : std::false_type {};

    template <template <typename...> class f, typename... args>
    struct in_type_domain<f<args...>, f> : std::true_type {};

    template <typename t, template <typename...> class f>
    inline constexpr auto in_type_domain_v = is_type_domain<t, f>::value;

    template <typename elem>
    struct type_tag {
        using type = elem;
    };

    template <typename elem>
    constexpr auto operator==(type_tag<elem>, type_tag<elem>) -> bool {
        return true;
    }

    template <typename lhs_elem, typename rhs_elem>
    constexpr auto operator==(type_tag<lhs_elem>, type_tag<rhs_elem>) -> bool {
        return false;
    }

    template <typename lhs_elem, typename rhs_elem>
    constexpr auto operator!=(const type_tag<lhs_elem> lhs, const type_tag<rhs_elem> rhs) -> bool {
        return !(lhs == rhs);
    }

    template <typename...>
    struct type_seq;

    template <>
    struct type_seq<> {};

    template <typename head_elem, typename... tail_elems>
    struct type_seq<head_elem, tail_elems...> {
        static constexpr auto head = type_tag<head_elem>{};
        static constexpr auto tail = type_seq<tail_elems...>{};
    };

    template <typename... elems>
    constexpr auto size(type_seq<elems...>) -> std::size_t {
        return sizeof...(elems);
    }

    template <typename elem, typename... seq_elems>
    constexpr auto push_back(type_seq<seq_elems...>, type_tag<elem> = {}) {
        return type_seq<seq_elems..., elem>{};
    }

    template <typename elem, typename... seq_elems>
    constexpr auto push_front(type_seq<seq_elems...>, type_tag<elem> = {}) {
        return type_seq<elem, seq_elems...>{};
    }

    template <typename t, template <template <auto...> class...> class f>
    struct in_value_func_domain : std::false_type {};

    template <template <template <auto...> class...> class f, template <auto...> class... args>
    struct in_value_func_domain<f<args...>, f> : std::true_type {};

    template <typename t, template <template <auto...> class...> class f>
    inline constexpr auto in_value_func_domain_v = is_value_func_domain<t, f>::value;

    template <template <auto...> class elem>
    struct value_func_tag {
        template <auto... args> using func = elem<args...>;
    };

    template <template <auto...> class elem>
    constexpr auto operator==(value_func_tag<elem>, value_func_tag<elem>) -> bool {
        return true;
    }

    template <template <auto...> class lhs_elem, template <auto...> class rhs_elem>
    constexpr auto operator==(value_func_tag<lhs_elem>, value_func_tag<rhs_elem>) -> bool {
        return false;
    }

    template <template <auto...> class lhs_elem, template <auto...> class rhs_elem>
    constexpr auto operator!=(const value_func_tag<lhs_elem> lhs, const value_func_tag<rhs_elem> rhs) -> bool {
        return !(lhs == rhs);
    }

    template <template <auto...> class...>
    struct value_func_seq;

    template <>
    struct value_func_seq<> {};

    template <template <auto...> class head_elem, template <auto...> class... tail_elems>
    struct value_func_seq<head_elem, tail_elems...> {
        static constexpr auto head = value_func_tag<head_elem>{};
        static constexpr auto tail = value_func_seq<tail_elems...>{};
    };

    template <template <auto...> class... elems>
    constexpr auto size(value_func_seq<elems...>) -> std::size_t {
        return sizeof...(elems);
    }

    template <template <auto...> class elem, template <auto...> class... seq_elems>
    constexpr auto push_back(value_func_seq<seq_elems...>, value_func_tag<elem> = {}) {
        return value_func_seq<seq_elems..., elem>{};
    }

    template <template <auto...> class elem, template <auto...> class... seq_elems>
    constexpr auto push_front(value_func_seq<seq_elems...>, value_func_tag<elem> = {}) {
        return value_func_seq<elem, seq_elems...>{};
    }

    template <typename t, template <template <typename...> class...> class f>
    struct in_type_func_domain : std::false_type {};

    template <template <template <typename...> class...> class f, template <typename...> class... args>
    struct in_type_func_domain<f<args...>, f> : std::true_type {};

    template <typename t, template <template <typename...> class...> class f>
    inline constexpr auto in_type_func_domain_v = is_type_func_domain<t, f>::value;

    template <template <typename...> class elem>
    struct type_func_tag {
        template <typename... args> using func = elem<args...>;
    };

    template <template <typename...> class elem>
    constexpr auto operator==(type_func_tag<elem>, type_func_tag<elem>) -> bool {
        return true;
    }

    template <template <typename...> class lhs_elem, template <typename...> class rhs_elem>
    constexpr auto operator==(type_func_tag<lhs_elem>, type_func_tag<rhs_elem>) -> bool {
        return false;
    }

    template <template <typename...> class lhs_elem, template <typename...> class rhs_elem>
    constexpr auto operator!=(const type_func_tag<lhs_elem> lhs, const type_func_tag<rhs_elem> rhs) -> bool {
        return !(lhs == rhs);
    }

    template <template <typename...> class...>
    struct type_func_seq;

    template <>
    struct type_func_seq<> {};

    template <template <typename...> class head_elem, template <typename...> class... tail_elems>
    struct type_func_seq<head_elem, tail_elems...> {
        static constexpr auto head = type_func_tag<head_elem>{};
        static constexpr auto tail = type_func_seq<tail_elems...>{};
    };

    template <template <typename...> class... elems>
    constexpr auto size(type_func_seq<elems...>) -> std::size_t {
        return sizeof...(elems);
    }

    template <template <typename...> class elem, template <typename...> class... seq_elems>
    constexpr auto push_back(type_func_seq<seq_elems...>, type_func_tag<elem> = {}) {
        return type_func_seq<seq_elems..., elem>{};
    }

    template <template <typename...> class elem, template <typename...> class... seq_elems>
    constexpr auto push_front(type_func_seq<seq_elems...>, type_func_tag<elem> = {}) {
        return type_func_seq<elem, seq_elems...>{};
    }

    //[[[end]]]

    /// Trait used to constrain templates that operate upon the tagged sequence types defined in
    /// `ksr::meta`. Evaluates to `std::true_type` if every type in `ts` is an instantiation of the
    /// same kind of sequence template, or to `std::false_type` otherwise.

    /*[[[cog

    is_seq_template = string.Template('std::conjunction<in_${name}_domain<ts, ${name}_seq>...>')
    is_seq_disjuncts = ',\n    '.join([is_seq_template.substitute(name=config['name']) for config in tag_configs])

    cog.outl(f'''
    template <typename... ts>
    struct is_seq : std::disjunction<
        {is_seq_disjuncts}> {{}};
    ''')

    ]]]*/

    template <typename... ts>
    struct is_seq : std::disjunction<
        std::conjunction<in_value_domain<ts, value_seq>...>,
        std::conjunction<in_type_domain<ts, type_seq>...>,
        std::conjunction<in_value_func_domain<ts, value_func_seq>...>,
        std::conjunction<in_type_func_domain<ts, type_func_seq>...>> {};

    //[[[end]]]

    /// Determines whether `seq` is an empty sequence. Nonempty sequences provide `head` and `tail`
    /// members. `seq_t` must be a sequence type in the sense of the `is_seq` trait.

    template <typename seq_t, typename = std::enable_if_t<is_seq_v<seq_t>>>
    constexpr auto empty(const seq_t seq) -> bool {
        return size(seq) == 0;
    }

}}

#endif

