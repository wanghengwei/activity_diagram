#pragma once
#include "node.h"
// #include <rxcpp/operators/rx-observe_on.hpp>
// #include <boost/log/trivial.hpp>

template<typename Status>
class DelayAction : public Action<Status> {
public:
    explicit DelayAction(double sec) : m_delayDuration{sec} {}

    rxcpp::observable<Status> performBy(Principal&) const override {
        auto d = std::chrono::duration_cast<std::chrono::milliseconds>(m_delayDuration);
        // BOOST_LOG_TRIVIAL(debug) << "aaa";
        // auto d = std::chrono::seconds{2};
        return rxcpp::observable<>::timer(d).map([](int) {
            return Status{};
        }).ignore_elements();
    }

private:
    std::chrono::duration<double> m_delayDuration;
};