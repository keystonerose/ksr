#include <algorithm>
#include <functional>
#include <iterator>

namespace ksr {

    namespace detail::proxy_sort {

        template <typename index_t, typename fn>
        class handle {
        public:

            explicit handle(const index_t& index, fn move_assign)
              : m_move_assign{std::move(move_assign)}, m_index{index} {}

            ~handle() = default;

            handle(handle&& rhs) noexcept
              : m_move_assign{std::move(rhs.m_move_assign)}, m_index{rhs.m_index} {

                // TODO:JD:X need to call back some action here!
            }

            handle& operator=(handle&& rhs) noexcept {
                if (m_index.get() != rhs.m_index.get()) {
                    m_move_assign(m_index.get(), rhs.m_index.get());
                    m_index = rhs.m_index;
                }
            }

            handle(const handle& rhs) = delete;
            handle& operator=(const handle& rhs) = delete;

            friend bool operator<(const handle& lhs, const handle& rhs) {
                return lhs.m_index.get() < rhs.m_index.get();
            }

        private:

            fn m_move_assign;
            std::reference_wrapper<const index_t> m_index;
        };
    }

    template <typename ran_it, typename swap_fn>
    void proxy_sort(const ran_it begin, const ran_it end, swap_fn swap) {

        using index_t = typename std::iterator_traits<ran_it>::value_type;
        using handle = detail::proxy_sort::handle<index_t, swap_fn>;

        // TODO:X provide ksr::transform_to() to make the below more generic

        auto handles = std::vector<handle>{};
        handles.reserve(std::distance(begin, end));
        std::transform(
            begin, end, std::back_inserter(handles),
            [swap] (const index_t& index) { return handle{index, swap}; });

        std::sort(std::begin(handles), std::end(handles));
    }
}

// ...

#include <cstddef>
#include <iostream>
#include <memory>
#include <vector>

struct proxy {
    std::size_t idx;
    int order;
};

bool operator==(const proxy& lhs, const proxy& rhs) {
    return lhs.order == rhs.order;
}

bool operator!=(const proxy& lhs, const proxy& rhs) {
    return !(lhs == rhs);
}

bool operator<(const proxy& lhs, const proxy& rhs) {
    return lhs.order < rhs.order;
}

int main() {

    auto v = std::vector<std::unique_ptr<char>>{};
    v.push_back(std::make_unique<char>('1'));
    v.push_back(std::make_unique<char>('2'));
    v.push_back(std::make_unique<char>('3'));

    const auto i = std::vector{proxy{1, -1}, proxy{0, 1}, proxy{2, 0}};

    ksr::proxy_sort(std::begin(i), std::end(i), [&v](const proxy& from, const proxy& to) {
        v[to.idx] = std::move(v[from.idx]);
    });

    for (auto&& elem : v) {
        std::cout << *elem << '\n';
    }

    std::cout << std::flush;
    return EXIT_SUCCESS;
}
