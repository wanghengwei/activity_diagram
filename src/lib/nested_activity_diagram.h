#pragma once
#include "node.h"
#include <boost/log/trivial.hpp>
#include <boost/type_index.hpp>

template<typename Status>
class NestedActivityDiagram : public Action<Status> {
public:

    rxcpp::observable<Status> performBy(Principal& p) const override {
        Node<Status>* entry = getInitial();
        return perform(p, entry, Status{});
    }

    Decision<Status>* getInitial() const {
        return m_entry.get();
    }

    void setInitial(std::shared_ptr<Decision<Status>> entry) {
        m_entry.swap(entry);
    }

    void add(std::shared_ptr<Node<Status>> child) {
        m_children.push_back(child);
    }

    using Action<Status>::getNext;

private:

    static rxcpp::observable<Status> perform(Principal &p, Node<Status>* step, const Status& lastStatus) {
        if (!step) {
            return rxcpp::observable<>::empty<Status>().as_dynamic();
        }

        auto statusOfCurrent = step->performBy(p);

        auto leftStatuses = statusOfCurrent.take_last(1).default_if_empty(lastStatus).flat_map([step, &p](const Status& currentStatus) {
            Node<Status>* nextStep = step->getNext(currentStatus);
            return perform(p, nextStep, currentStatus);
        });

        return statusOfCurrent.concat(leftStatuses);
    }

private:
    std::shared_ptr<Decision<Status>> m_entry;
    std::vector<std::shared_ptr<Node<Status>>> m_children;
};
