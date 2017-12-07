#include "ksr/lock_adaptor.hpp"

#include "catch/catch.hpp"

#include <mutex>
#include <string>

using namespace ksr;
namespace {

    class dual_lockable
    {
    public:

        std::string log() const { return m_log; }

        void log_char(char c) { m_log += c; }

        void lock_x() { m_log += '['; m_x_locked = true; }
        void lock_y() { m_log += '('; m_y_locked = true; }

        bool try_lock_x();
        bool try_lock_y();

        void unlock_x() noexcept { m_log += ']'; m_x_locked = false; }
        void unlock_y() noexcept { m_log += ')'; m_y_locked = false; }

        using x_mutex = ksr::mem_lockable<dual_lockable,
            &dual_lockable::lock_x,
            &dual_lockable::try_lock_x,
            &dual_lockable::unlock_x>;

        using y_mutex = ksr::mem_lockable<dual_lockable,
            &dual_lockable::lock_y,
            &dual_lockable::try_lock_y,
            &dual_lockable::unlock_y>;

        using x_lock_guard = ksr::lock_adaptor<std::lock_guard, x_mutex>;
        using y_lock_guard = ksr::lock_adaptor<std::lock_guard, y_mutex>;

        using unique_x_lock = ksr::lock_adaptor<std::unique_lock, x_mutex>;
        using unique_y_lock = ksr::lock_adaptor<std::unique_lock, y_mutex>;

        using scoped_xy_lock = ksr::lock_adaptor<std::scoped_lock, x_mutex, y_mutex>;
        using scoped_yx_lock = ksr::lock_adaptor<std::scoped_lock, y_mutex, x_mutex>;

    private:

        bool m_x_locked;
        bool m_y_locked;

        std::string m_log;
    };
}

bool dual_lockable::try_lock_x() {

    if (!m_x_locked) {
        lock_x();
        return true;
    }
    else {
        return false;
    }
}

bool dual_lockable::try_lock_y() {

    if (!m_y_locked) {
        lock_y();
        return true;
    }
    else {
        return false;
    }
}

TEST_CASE("lock_guard", "[lock_adaptor]") {

    auto inst = dual_lockable{};
    {
        auto _1 = dual_lockable::x_lock_guard{inst};
        auto _2 = dual_lockable::y_lock_guard{inst};
        inst.log_char('*');
    }
    CHECK(inst.log() == "[(*)]");
}

TEST_CASE("scoped_lock", "[lock_adaptor]") {

    auto inst = dual_lockable{};
    {
        auto _ = dual_lockable::scoped_xy_lock{inst};
        inst.log_char('*');
    }
    CHECK(inst.log() == "[(*)]");
}
