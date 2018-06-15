#pragma once
#include "step.h"

template<typename Status>
class DirectDispatcher : public Dispatcher<Status> {
public:
    std::tuple<rxcpp::observable<Step<Status>*>, rxcpp::observable<Status>> performBy(Principal&, const Status& status = Status{}) const override {
        return std::make_tuple(rxcpp::observable<>::just<Step<Status>*>(m_next.get()), rxcpp::observable<>::empty<Status>());
    }

    Step<Status>* getNext(const Status&) const {
        return m_next.get();
    }

    void setNext(std::shared_ptr<Step<Status>> next) {
        m_next.swap(next);
    }
private:
    std::shared_ptr<Step<Status>> m_next;
};
