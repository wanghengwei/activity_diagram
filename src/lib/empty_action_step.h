#pragma once
#include "step.h"

template<typename Status>
class EmptyActionStep : public ActionStep<Status> {
public:
    explicit EmptyActionStep(const Status& s = Status{}) : m_status{s} {}

    std::tuple<rxcpp::observable<Step<Status>*>, rxcpp::observable<Status>> performBy(Principal&, const Status& status = Status{}) const override {
        return std::make_tuple(rxcpp::observable<>::just<Step<Status>*>(getNext()), rxcpp::observable<>::just(m_status));
    }

    using ActionStep<Status>::getNext;
private:
    Status m_status;
};
