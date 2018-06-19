#pragma once
#include "node.h"

template<typename Status>
class EmptyAction : public Action<Status> {
public:
    explicit EmptyAction(const Status& s = Status{}) : m_status{s} {}

    rxcpp::observable<Status> performBy(Principal&) const override {
        BOOST_LOG_TRIVIAL(debug) << "EmptyAction.performBy";
        auto r = rxcpp::observable<>::just<Status>(m_status).tap([this](const Status& st) {
            BOOST_LOG_TRIVIAL(debug) << "EmptyAction.onNext: this=" << this << ", state=" << st;
        });
        // r.connect();
        return r;
    }

private:
    Status m_status;
};
