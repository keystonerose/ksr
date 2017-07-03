template <typename t, template <$sig...> class f>
struct in_${name}_domain : std::false_type {};

template <template <$sig...> class f, $sig... args>
struct in_${name}_domain<f<args...>, f> : std::true_type {};

template <typename t, template <$sig...> class f>
inline constexpr auto in_${name}_domain_v = is_${name}_domain<t, f>::value;

template <$sig elem>
struct ${name}_tag {
    $tag_contents
};

template <$sig elem>
constexpr auto operator==(${name}_tag<elem>, ${name}_tag<elem>) -> bool {
    return true;
}

template <$sig lhs_elem, $sig rhs_elem>
constexpr auto operator==(${name}_tag<lhs_elem>, ${name}_tag<rhs_elem>) -> bool {
    return false;
}

template <$sig lhs_elem, $sig rhs_elem>
constexpr auto operator!=(const ${name}_tag<lhs_elem> lhs, const ${name}_tag<rhs_elem> rhs) -> bool {
    return !(lhs == rhs);
}

template <$sig...>
struct ${name}_seq;

template <>
struct ${name}_seq<> {};

template <$sig head_elem, $sig... tail_elems>
struct ${name}_seq<head_elem, tail_elems...> {
    static constexpr auto head = ${name}_tag<head_elem>{};
    static constexpr auto tail = ${name}_seq<tail_elems...>{};
};

template <$sig... elems>
constexpr auto size(${name}_seq<elems...>) -> std::size_t {
    return sizeof...(elems);
}

template <$sig elem, $sig... seq_elems>
constexpr auto push_back(${name}_seq<seq_elems...>, ${name}_tag<elem> = {}) {
    return ${name}_seq<seq_elems..., elem>{};
}

template <$sig elem, $sig... seq_elems>
constexpr auto push_front(${name}_seq<seq_elems...>, ${name}_tag<elem> = {}) {
    return ${name}_seq<elem, seq_elems...>{};
}
