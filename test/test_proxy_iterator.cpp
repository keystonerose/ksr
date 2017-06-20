#include "ksr/proxy_iterator.hpp"

#include "catch/catch.hpp"

#include <algorithm>
#include <functional>
#include <iterator>
#include <map>
#include <memory>

using namespace ksr;
namespace {

    struct item {
        explicit item(const int key, const bool value) : key{key}, value{value} {}
        int key;
        bool value;
    };

    class item_set {
    private:

        using item_container = std::map<int, std::unique_ptr<item>>;
        static constexpr auto filter = [](auto&& value) { return (value.second != nullptr); };
        static constexpr auto transform = [](auto&& value) { return *value.second; };

        template <typename it>
        using transform_iterator = ksr::proxy_iterator<it, decltype(transform), decltype(filter)>;

    public:

        using iterator = transform_iterator<item_container::iterator>;
        using const_iterator = transform_iterator<item_container::const_iterator>;

        auto begin() const {
            return const_iterator{std::cbegin(m_items), std::cend(m_items)};
        }

        auto end() const {
            return const_iterator{std::cend(m_items), std::cend(m_items)};
        }

        auto begin() {
            return iterator{std::begin(m_items), std::end(m_items)};
        }

        auto end() {
            return iterator{std::end(m_items), std::end(m_items)};
        }

        item* emplace(const int key, const bool value) {
            const auto [iter, success] = m_items.insert({key, std::make_unique<item>(key, value)});
            return success ? iter->second.get() : nullptr;
        }

    private:

        item_container m_items;
    };

    item_set make_set() {

        item_set set;
        set.emplace(0, true);
        set.emplace(1, false);
        set.emplace(2, true);
        set.emplace(3, false);

        return set;
    }
}

TEST_CASE("access", "[transform_iterator]") {
    const auto set = make_set();
    CHECK(std::count_if(std::cbegin(set), std::cend(set), std::mem_fn(&item::value)) == 2);
}
