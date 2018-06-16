#pragma once
#include "node.h"

template<typename Status>
class DirectDecision : public Decision<Status> {
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
