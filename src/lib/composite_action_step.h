#pragma once
#include "step.h"
#include <boost/log/trivial.hpp>
#include <boost/type_index.hpp>

template<typename Status>
class CompositeActionStep : public Action<Status> {
public:

    rxcpp::observable<Status> performBy(Principal& p) const override {
        Step<Status>* entry = getEntry();
        return perform(p, entry, Status{});
    }

    Dispatcher<Status>* getEntry() const {
        return m_entry.get();
    }

    void setEntry(std::shared_ptr<Dispatcher<Status>> entry) {
        m_entry.swap(entry);
    }

    using Action<Status>::getNext;

private:

    static rxcpp::observable<Status> perform(Principal &p, Step<Status>* step, const Status& lastStatus) {
        if (!step) {
            return rxcpp::observable<>::empty<Status>().as_dynamic();
        }

        auto statusOfCurrent = step->performBy(p);

        auto leftStatuses = statusOfCurrent.take_last(1).default_if_empty(lastStatus).flat_map([step, &p](const Status& currentStatus) {
            Step<Status>* nextStep = step->getNext(currentStatus);
            return perform(p, nextStep, currentStatus);
        });

        return statusOfCurrent.concat(leftStatuses);
    }

private:
    std::shared_ptr<Dispatcher<Status>> m_entry;
};
