#pragma once
#include "step.h"

template<typename Status>
class CompositeActionStep : public ActionStep<Status> {
public:

    rxcpp::observable<Status> performBy(Principal& p) const override {
        auto step = getEntry()->getNext(Status{});
        return performBy(p, step);
    }

    rxcpp::observable<Status> performBy(Principal &p, Step<Status>* step) const {
        if (!step) {
            return rxcpp::observable<>::empty<Status>();
        }

        auto s1 = step->performBy(p);
        auto s2 = s1.last().flat_map([this, &p, step](const Status& st) {
            auto n = step->getNext(st);
            return performBy(p, n);
        });
        return s1.concat(s2);
    }

    Dispatcher<Status>* getEntry() const {
        return m_entry.get();
    }

    void setEntry(std::shared_ptr<Dispatcher<Status>> entry) {
        m_entry.swap(entry);
    }
private:
    std::shared_ptr<Dispatcher<Status>> m_entry;
};
