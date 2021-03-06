#pragma once
#include "principal.h"
#include <rxcpp/rx.hpp>

template<typename Status>
class Node {
public:
    virtual ~Node() {}

    virtual rxcpp::observable<Status> performBy(Principal&, rxcpp::schedulers::worker w) const = 0;

    virtual Node<Status>* getNext(const Status& status) const = 0;
};

template<typename Status>
class Decision : public Node<Status> {
public:
    rxcpp::observable<Status> performBy(Principal&, rxcpp::schedulers::worker w) const override {
        return rxcpp::observable<>::empty<Status>();
    }
};

template<typename Status>
class Action : public Node<Status> {
public:

    Node<Status>* getNext(const Status& status) const override {
        return m_next;
    }

    void setNext(Node<Status>* next) {
        m_next = next;
    }

private:
    Node<Status>* m_next{};
};
