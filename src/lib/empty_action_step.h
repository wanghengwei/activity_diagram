#pragma once
#include "step.h"

template<typename Status>
class EmptyAction : public Action<Status> {
public:
    explicit EmptyAction(const Status& s = Status{}) : m_status{s} {}

    rxcpp::observable<Status> performBy(Principal&) const override {
        return rxcpp::observable<>::just<Status>(m_status);
    }

private:
    Status m_status;
};
