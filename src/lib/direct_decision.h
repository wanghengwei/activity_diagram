#pragma once
#include "node.h"

template<typename Status>
class DirectDecision : public Decision<Status> {
public:
    Node<Status>* getNext(const Status& status) const override {
        return m_next.get();
    }

    void setNext(std::shared_ptr<Node<Status>> next) {
        m_next.swap(next);
    }
private:
    std::shared_ptr<Node<Status>> m_next;
};
