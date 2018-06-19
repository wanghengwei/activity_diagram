#pragma once
#include "node.h"
#include <rxcpp/operators/rx-observe_on.hpp>
#include <boost/log/trivial.hpp>

template<typename Status>
class DelayAction : public Action<Status> {
public:
    explicit DelayAction(double sec) : m_delayDuration{sec} {}

    rxcpp::observable<Status> performBy(Principal&, rxcpp::schedulers::worker w) const override {
        // BOOST_LOG_TRIVIAL(debug) << "begin";
        auto d = std::chrono::duration_cast<std::chrono::milliseconds>(m_delayDuration);
        return rxcpp::observable<>::just<Status>(Status{}).subscribe_on(rxcpp::serialize_same_worker(w)).delay(d)
        // .tap([](auto) {
        //     // BOOST_LOG_TRIVIAL(debug) << "onNext";
        // }, []() {
        //     // BOOST_LOG_TRIVIAL(debug) << "onCompleted";
        // })
        .publish().connect_forever();
    }

private:
    std::chrono::duration<double> m_delayDuration;
};