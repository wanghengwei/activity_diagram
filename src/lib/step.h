#pragma once
#include "principal.h"
#include <rxcpp/rx-observable.hpp>

template<typename Status>
class Step {
public:
    virtual ~Step() {}

    virtual std::tuple<rxcpp::observable<Step<Status>*>, rxcpp::observable<Status>> performBy(Principal&, const Status& status) const = 0;
};

template<typename Status>
class Dispatcher : public Step<Status> {
public:

};

template<typename Status>
class ActionStep : public Step<Status> {
public:

    virtual std::tuple<rxcpp::observable<Step<Status>*>, rxcpp::observable<Status>> performBy(Principal&, const Status& status = Status{}) const = 0;

    Step<Status>* getNext() const {
        return m_next.get();
    }

    void setNext(std::shared_ptr<Step<Status>> next) {
        m_next = next;
    }

private:
    std::shared_ptr<Step<Status>> m_next;
};
