#pragma once
#include "node.h"
#include <rxcpp/operators/rx-observe_on.hpp>
#include <boost/log/trivial.hpp>

template<typename Status>
class DelayAction : public Action<Status> {
public:
    explicit DelayAction(double sec) : m_delayDuration{sec} {}

    rxcpp::observable<Status> performBy(Principal&) const override {
        BOOST_LOG_TRIVIAL(debug) << "begin";
        auto d = std::chrono::duration_cast<std::chrono::milliseconds>(m_delayDuration);
        return rxcpp::observable<>::timer(d).map([](int) {
            BOOST_LOG_TRIVIAL(debug) << "RelayAction.onNext";
            return Status{};
        }).ignore_elements();
        // return rxcpp::observable<>::just<Status>(Status{}).delay(d, rxcpp::observe_on_new_thread()).tap([](auto) {
        //     BOOST_LOG_TRIVIAL(debug) << "onNext";
        // }, []() {
        //     BOOST_LOG_TRIVIAL(debug) << "onCompleted";
        // });
    }

private:
    std::chrono::duration<double> m_delayDuration;
};