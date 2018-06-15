#pragma once
#include "step.h"

template<typename Status>
class DirectDispatcher : public Dispatcher<Status> {
public:
    Step<Status>* getNext(const Status& status) const override {
        return m_next.get();
    }

    void setNext(std::shared_ptr<Step<Status>> next) {
        m_next.swap(next);
    }
private:
    std::shared_ptr<Step<Status>> m_next;
};
