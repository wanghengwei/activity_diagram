#pragma once
#include "step.h"

template<typename Status>
class CompositeActionStep : public ActionStep<Status> {
public:

    rxcpp::observable<Status> performBy(Principal& p) const override {
        auto step = getEntry();
        auto steps = m_stepSeq.get_observable();
        auto sss = steps.flat_map([&p](Step<Status>* step) {
            return step->performBy(p);
        });

        sss.flat_map([](auto ss) {
            return ss.take_last(1);
        }).subscribe([](const Status& s) {
            
        });

    }

    static void perform(Principal &p, Step<Status>* step, rxcpp::observable<Status>& output) {
        if (!step) {
            return;
        }

        auto statusOfCurrentStep = step->performBy(p);
        auto t = output.concat(statusOfCurrentStep);
        auto lastStatus = t.take_last(1);
        output = lastStatus.flat_map([&p, step, t](const Status& st) {
            auto n = step->getNext(st);
            rxcpp::observable<Status> o = t;
            perform(p, n, o);
            return o;
        });
    }

    Dispatcher<Status>* getEntry() const {
        return m_entry.get();
    }

    void setEntry(std::shared_ptr<Dispatcher<Status>> entry) {
        m_entry.swap(entry);
    }
private:
    std::shared_ptr<Dispatcher<Status>> m_entry;
    rxcpp::subjects::subject<Step<Status>*> m_stepSeq;
};
