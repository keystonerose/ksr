#ifndef KSR_UPDATE_FILTER_HPP
#define KSR_UPDATE_FILTER_HPP

#include "math.hpp"

#include <chrono>
#include <cstddef>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ksr {

    template <typename policy_t, typename... value_ts>
    class update_filter;

    template <std::size_t index, typename policy_t, typename... value_ts>
    decltype(auto) adl_get(const update_filter<policy_t, value_ts...>& self);

    namespace filter_policy {

        ///
        /// Policy class for an \ref update_filter that accepts updates when sufficient time has
        /// elapsed since the last such actioned update. The update interval may be specified upon
        /// construction. Applicable to \ref update_filter instantiations storing arbitrary data.
        ///

        class sampled {
        public:

            sampled(const std::chrono::milliseconds interval)
              : m_interval{interval} {}

        protected:

            ///
            /// Determines whether the elapsed time since the last actioned update is greater than
            /// the duration that this policy object was constructed with (and indicates that an
            /// update should be actioned if so).
            ///

            template <typename... value_ts>
            auto can_update(
                const std::tuple<value_ts...> &current_values,
                const std::tuple<value_ts...> &new_values) const -> bool {

                if (current_values == new_values) {
                    return false;
                }

                const auto now = std::chrono::steady_clock::now();
                const auto elapsed = now - m_last_update_time;

                // If this is the first time needs_update() has been called, m_last_update_time will
                // be set to the epoch of std::chrono::steady_clock, so in the absence of perverse
                // interval choices, the elapsed time will be large enough to ensure that an update
                // is triggered.

                if (elapsed >= m_interval) {
                    m_last_update_time = std::chrono::steady_clock::now();
                    return true;
                }

                return false;
            }

        private:

            std::chrono::milliseconds m_interval;
            mutable std::chrono::steady_clock::time_point m_last_update_time;
        };

        ///
        /// Policy class for an \ref update_filter tracking two arithmetic values that accepts
        /// updates when the integer percentage that those two data specify (when interpreted as the
        /// numerator and denominator of a quotient) changes. Only applicable to the
        /// \ref update_filter instantiation storing a pair of \p num_t values.
        ///

        template <typename num_t, typename = std::enable_if_t<std::is_arithmetic_v<num_t>>>
        class int_percentage {
        protected:

            ///
            /// Determines whether the integer percentage formed by \p old_values (when interpreted
            /// as the numerator and denominator of a quotient) is greater than that formed by
            /// \p new_values (and indicates that an update should be actioned if so).
            ///

            auto can_update(
                const std::tuple<num_t, num_t>& old_values,
                const std::tuple<num_t, num_t>& new_values) const -> bool {

                const auto& [old_num, old_denom] = old_values;
                const auto& [new_num, new_denom] = new_values;

                if (old_denom == 0) {
                    return true;
                }

                const auto old_percentage = ksr::int_percentage(old_num, old_denom);
                const auto new_percentage = ksr::int_percentage(new_num, new_denom);
                return new_percentage > old_percentage;
            }
        };
    }

    ///
    /// A generalised component for filtering updates. An \ref update_filter instance is created for
    /// a specified callback; then, when successive calls to update() are made, the argument data
    /// are compared with the last actioned data as per policy_t::can_update(). If the policy
    /// indicates that the new data are sufficiently different to the last actioned data, the
    /// callback function for the \ref update_filter instance is invoked with those new data.
    ///
    /// This is useful when there is a need to notify other components of updates from a
    /// long-running process, but the source data are changing rapidly and it is impractical to send
    /// that notification every time a change occurs. (For example, when sending signals over Qt
    /// connections.) Once the process that the \ref update_filter is responding to completes,
    /// sync() may be called to force an update with the final data.
    ///
    /// Each update_filter instance stores a tuple of data to manage updates to. New values for each
    /// element of this tuple are passed as the arguments to update(). Upon construction of the
    /// update_filter, the initial data are value-initialised. (Each type in \p value_ts must
    /// therefore support this mode of initialisation.) The last-actioned data stored in the
    /// update_filter may be retrieved at any time via ksr::get() or a decomposition declaration
    /// (just as for \c std::tuple).
    ///
    /// Policies must provide a member function with the signature
    /// <pre><![CDATA[
    /// bool can_update(
    ///     const std::tuple<value_ts...> &current_values,
    ///     const std::tuple<value_ts...> &new_values) const
    /// ]></pre>
    /// that specifies whether an update should be actioned when the data \p new_values are passed
    /// to an \ref update_filter whose last-actioned data are \p current_values. Additionally,
    /// policies that require data of their own should provide an appropriate constructor (refer to
    /// the constructor of \ref update_filter).
    ///

    template <typename policy_t, typename... value_ts>
    class update_filter : private policy_t {
    private:

        using update_fn = std::function<void(value_ts...)>;
        static constexpr auto needs_policy_data = !std::is_default_constructible_v<policy_t>;

    public:

        // We have to jump through hoops to implement ksr::get() as a free function, but consistency
        // with the standard library is nice. It's impractical to implement ksr::get() as an
        // out-of-line friend, but somewhat nicer to expose an adl_get() friend accessible only via
        // ADL that provides the implementation. (This also facilitates a single implementation of
        // ksr::get() for all tuple-like custom types.)

        template <std::size_t index>
        friend decltype(auto) adl_get(const update_filter& self) {
            return std::get<index>(self.m_last_values);
        }

        ///
        /// Depending on the type of filtering performed by this instantiation of
        /// \ref update_filter (which is determined by its \p policy_t template argument), it may
        /// be necessary to initialise it with policy-specific data that configures the filtering.
        /// If this is the case, \p policy_t should provide (only) a non-default constructor that
        /// offers an implicit conversion from these data; these can then be passed as the first
        /// argument to the \ref update_filter constructor.
        ///

        template <typename = std::enable_if<!needs_policy_data>>
        explicit update_filter(update_fn update)
          : m_update{std::move(update)} {}

        template <typename = std::enable_if<needs_policy_data>>
        explicit update_filter(const policy_t& policy_data, update_fn update)
          : policy_t{policy_data}, m_update{std::move(update)} {}

        ///
        /// Performs the update action of update() unconditionally (that is, without checking if the
        /// filter policy indicates that an update should be performed). This is useful when the
        /// process that the \ref update_filter is reponding to completes and the finalised data
        /// need to be actioned.
        ///

        void sync(const value_ts&... new_values) {
            m_update(new_values...);
            m_last_values = std::make_tuple(new_values...);
        }

        ///
        /// Compares \p new_values with the last actioned data stored within the \ref update_filter.
        /// If the filter policy indicates that these new data are sufficiently different to the
        /// last actioned data, the callback function passed to the constructor of this
        /// \ref update_filter instance is invoked for \p new_values and these new values replace
        /// the last actioned data within the \ref update_filter; otherwise, no action is taken.
        /// Policies should ensure that an update is always performed the first time this member
        /// function is called.
        ///

        auto update(const value_ts&... new_values) -> bool {

            auto new_tuple = std::make_tuple(new_values...);
            const auto needs_update = policy_t::can_update(m_last_values, new_tuple);

            if (needs_update) {
                m_update(new_values...);
                m_last_values = std::move(new_tuple);
            }

            return needs_update;
        }

    private:

        update_fn m_update;
        std::tuple<value_ts...> m_last_values;
    };

    template <std::size_t index, typename t>
    decltype(auto) get(const t& self) {
        return adl_get<index>(self);
    }

    template <typename num_t, typename = std::enable_if_t<std::is_arithmetic_v<num_t>>>
    using int_percentage_filter = update_filter<filter_policy::int_percentage<num_t>, num_t, num_t>;

    template <typename... value_ts>
    using sampled_filter = update_filter<filter_policy::sampled, value_ts...>;
}

// It looks like this is weirdness in the development version of libstdc++, in whose <utility>
// header std::tuple_element is declared variously as both a struct template and a class template.

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmismatched-tags"

namespace std {

    // These two specialisations allow an update_filter to be decomposed in the same way that would
    // be permitted for the tuple of data that it stores.

    template<std::size_t index, typename policy_t, typename... value_ts>
    struct tuple_element<index, ksr::update_filter<policy_t, value_ts...>> {
        using type = decltype(ksr::get<index>(std::declval<ksr::update_filter<policy_t, value_ts...>>()));
    };

    template <typename policy_t, typename... value_ts>
    struct tuple_size<ksr::update_filter<policy_t, value_ts...>>
        : public std::integral_constant<std::size_t, sizeof...(value_ts)> {};
}

#pragma clang diagnostic pop

namespace ksr {

    template <typename num_t, typename = std::enable_if_t<std::is_arithmetic_v<num_t>>>
    int int_percentage(const int_percentage_filter<num_t>& filter) {
        const auto& [num, denom] = filter;
        return int_percentage(num, denom);
    }
}

#endif
