#pragma once
#include "step.h"

template<typename Status>
class ActionStep : public Step<Status> {
public:
    Step<Status>* getNext() const {
        return m_next.get();
    }

    void setNext(std::shared_ptr<Step<Status>> next) {
        m_next = next;
    }

private:
    std::shared_ptr<Step<Status>> m_next;
};

template<typename Status>
class EmptyActionStep : public ActionStep<Status> {
public:
    explicit EmptyActionStep(const Status& s = Status{}) : m_status{s} {}

    std::tuple<Step<Status>*, rxcpp::observable<Status>> performBy(Principal&, rxcpp::observable<Status> lastStatus = rxcpp::observable<>::empty<Status>()) const override {
        return std::make_tuple(getNext(), lastStatus.concat(rxcpp::observable<>::just(m_status)));
    }
protected:
    using ActionStep<Status>::getNext;
private:
    Status m_status;
};
