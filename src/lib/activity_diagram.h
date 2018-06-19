#pragma once
#include "nested_activity_diagram.h"
#include "direct_decision.h"
#include <rxcpp/rx-observable.hpp>

class Principal;

template<typename Status>
class ActivityDiagram {
public:
    ActivityDiagram();

    rxcpp::observable<Status> performBy(Principal& p, rxcpp::schedulers::worker w) const;

    void add(std::shared_ptr<Node<Status>> node);

    void setFirst(Node<Status>* n);
private:
    NestedActivityDiagram<Status> m_impl;
};

template<typename Status>
rxcpp::observable<Status> ActivityDiagram<Status>::performBy(Principal& p, rxcpp::schedulers::worker w) const {
    return m_impl.performBy(p, w);
}

template<typename Status>
void ActivityDiagram<Status>::add(std::shared_ptr<Node<Status>> node) {
    m_impl.add(node);
}

template<typename Status>
void ActivityDiagram<Status>::setFirst(Node<Status>* n) {
    static_cast<DirectDecision<Status>*>(m_impl.getInitial())->setNext(n);
}

template<typename Status>
ActivityDiagram<Status>::ActivityDiagram() {
    auto init = std::make_shared<DirectDecision<Status>>();
    m_impl.setInitial(init);
}