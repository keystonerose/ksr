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

    /// `value_tag`, `type_tag` and `func_tag` wrap integral constants, types, or type-functions
    /// (i.e. class templates), respectively, in objects that facilitate the deduction of
    /// corresponding template parameters and can be both be passed to or returned from `constexpr`
    /// functions. The **kind** of such a tag object designates whether it wraps a value, type or
    /// type-function.

    /// Tags an integral constant value `v`, which can later be retrieved via the `value` member.
    /// While integral constants can of course be managed more naturally without the tagging
    /// mechanism, this is necessary in some cases to facilitate the deduction of a corresponding
    /// template parameter. `value_tag` objects are considered identical when they represent values
    /// that themselves compare equal with `operator==`.

    /// Tags a single type `t`, which can later be retrieved from the `type_tag` object via the
    /// `type` member type. `type_tag` objects are considered identical when they represent types
    /// that themselves satisfy the `std::is_same` trait.

    /// Tags a single template class with type arguments `f`, which can later be retrieved from the
    /// `func_tag` object via the `func` member template. `func_tag` objects are considered
    /// identical when they represent the same class template, as if by an extension of
    /// `std::is_same` to templates rather than types.

    /// `value_seq`, `type_seq` and `func_seq` are sequence counterparts of `type_tag`, `value_tag`
    /// and `func_tag`: each wraps a pack of arbitrary integral constants, types or type-functions,
    /// and provides operations for processing that pack in terms of tag objects of the
    /// corresponding kind. Sequence objects that tag non-empty packs are typically processed by
    /// splitting into a head (which is a tagged value, type or type-function) and a tail (which is
    /// another sequence, and may be empty) via the `head` and `tail` members. These components are
    /// available only when the pack is non-empty; whether or not this is the case may be determined
    /// via the nonmember `empty()` predicate.

    // TODO:DOC
    /// Given a class template `value_func` with non-type (value) arguments only, determines whether
    /// `t` is an instantiation of `value_func`.

    /// Given a class template `f` with type arguments only, determines whether `t` is an
    /// instantiation of `f`.

    /// Given a class template `f` with type-based template template arguments only, determines
    /// whether `t` is an instantiation of `f`.

    /*[[[cog

    with open('meta_tag_impl.in.hpp', 'r') as impl_file:
        impl_template = string.Template(impl_file.read())

    cog.outl()
    for config in tag_configs:
        cog.outl(impl_template.substitute(**config))

    ]]]*/
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
    //[[[end]]]

    template <typename... ts>
    inline constexpr auto is_seq_v = is_seq<ts...>::value;

}}

#endif

