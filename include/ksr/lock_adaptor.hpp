#include <functional>
#include <mutex>
#include <utility>

// Dependencies: none

namespace ksr {

    namespace impl {

        template <typename t>
        using lock_mem_fn = void (t::*)();

        template <typename t>
        using try_lock_mem_fn = bool (t::*)();

        template <typename t>
        using unlock_mem_fn = void (t::*)() noexcept;
    }

    template <
        template <typename...> class std_lock,
        typename... lockables>
    class lock_adaptor
      : private lockables..., public std_lock<lockables...> {

    public:

        template <typename... arg_ts>
        explicit lock_adaptor(arg_ts&&... args)
        : lockables{std::forward<arg_ts>(args)...}...,
            std_lock<lockables...>{static_cast<lockables&>(*this)...} {}
    };

    template <
        typename t,
        impl::lock_mem_fn<t> lock_fn,
        impl::unlock_mem_fn<t> unlock_fn>
    class mem_basic_lockable {
    public:

        explicit mem_basic_lockable(t& target) noexcept : m_target{target} {}

        mem_basic_lockable(const mem_basic_lockable& rhs) = delete;
        mem_basic_lockable& operator=(const mem_basic_lockable& rhs) = delete;

        void lock()            { (m_target.*lock_fn)(); }
        void unlock() noexcept { (m_target.*unlock_fn)(); }

    private:

        t& m_target;
    };

    template<
        typename t,
        impl::lock_mem_fn<t> lock_fn,
        impl::try_lock_mem_fn<t> try_lock_fn,
        impl::unlock_mem_fn<t> unlock_fn>
    class mem_lockable {
    public:

        explicit mem_lockable(t& target) noexcept : m_target{target} {}

        mem_lockable(const mem_lockable& rhs) = delete;
        mem_lockable& operator=(const mem_lockable& rhs) = delete;

        void lock()            { (m_target.*lock_fn)(); }
        bool try_lock()        { (m_target.*try_lock_fn)(); }
        void unlock() noexcept { (m_target.*unlock_fn)(); }

    private:

        t& m_target;
    };
}
