#pragma once
#include "principal.h"
#include <rxcpp/rx-observable.hpp>

template<typename Status>
class Step {
public:
    virtual ~Step() {}

    virtual rxcpp::observable<Status> performBy(Principal&) const = 0;

    virtual Step<Status>* getNext(const Status& status) const = 0;
};

template<typename Status>
class Dispatcher : public Step<Status> {
public:
    rxcpp::observable<Status> performBy(Principal&) const override {
        return rxcpp::observable<>::empty<Status>();
    }
};

template<typename Status>
class Action : public Step<Status> {
public:

    Step<Status>* getNext(const Status& status) const override {
        return m_next.get();
    }

    void setNext(std::shared_ptr<Step<Status>> next) {
        m_next = next;
    }

private:
    std::shared_ptr<Step<Status>> m_next;
};
