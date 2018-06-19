#pragma once
#include "node.h"
#include <boost/log/trivial.hpp>

template<typename Status>
class DirectDecision : public Decision<Status> {
public:
    Node<Status>* getNext(const Status& status) const override {
        // BOOST_LOG_TRIVIAL(debug) << "DirectDecision.getNext: state=" << status;
        return m_next;
    }

    void setNext(Node<Status>* next) {
        m_next = next;
    }
private:
    Node<Status>* m_next{};
};
