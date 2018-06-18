#pragma once
#include "node.h"

template<typename Status>
class EmptyAction : public Action<Status> {
public:
    explicit EmptyAction(const Status& s = Status{}) : m_status{s} {}

    rxcpp::observable<Status> performBy(Principal&) const override {
        BOOST_LOG_TRIVIAL(debug) << "EmptyAction.performBy";
        auto r = rxcpp::observable<>::just<Status>(m_status).tap([](auto) {
            BOOST_LOG_TRIVIAL(debug) << "EmptyAction.onNext";
        });
        // r.connect();
        return r;
    }

private:
    Status m_status;
};
