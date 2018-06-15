#pragma once
#include "step.h"
#include <boost/log/trivial.hpp>
#include <boost/type_index.hpp>

template<typename Status>
class CompositeActionStep : public ActionStep<Status> {
public:

    std::tuple<rxcpp::observable<Step<Status>*>, rxcpp::observable<Status>> performBy(Principal& p, const Status& status = Status{}) const override {
        Step<Status>* entry = getEntry();
        rxcpp::observable<Status> statuses = perform(p, entry, status);
        return std::make_tuple(rxcpp::observable<>::just<Step<Status>*>(ActionStep<Status>::getNext()), statuses);
    }


    Dispatcher<Status>* getEntry() const {
        return m_entry.get();
    }

    void setEntry(std::shared_ptr<Dispatcher<Status>> entry) {
        m_entry.swap(entry);
    }

private:

    static rxcpp::observable<Status> perform(Principal &p, Step<Status>* step, const Status& lastStatus) {
        if (!step) {
            BOOST_LOG_TRIVIAL(debug) << "ending perform because step is null";
            return rxcpp::observable<>::empty<Status>().as_dynamic();
        }
        BOOST_LOG_TRIVIAL(debug) << "performing, step=" << boost::typeindex::type_id_runtime(*step) << ", lastStatus=" << lastStatus;

        auto [next, statusOfCurrent] = step->performBy(p, lastStatus);

        BOOST_LOG_TRIVIAL(debug) << "performed for step " << boost::typeindex::type_id_runtime(*step);

        auto leftStatuses = next.zip([&p](Step<Status>* nextStep, const Status& st) {
            return perform(p, nextStep, st);
        }, statusOfCurrent.take_last(1).default_if_empty(lastStatus)).merge();

        return statusOfCurrent.concat(leftStatuses);
    }

private:
    std::shared_ptr<Dispatcher<Status>> m_entry;
};
