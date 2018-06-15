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

    // steps: The next step to be performed
    // currentStatus: current status which used to be passed to performBy
    // return: all statues of the next step and all remained steps
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

        // rxcpp::observable<Status> allStatusesAfterNext = nextStep.flat_map([&p, currentStatus](Step<Status>* step) {
        //     // perform next step:
        //     BOOST_LOG_TRIVIAL(debug) << "start zip, step is " << step << ", last status is " << currentStatus;
        //     if (!step) {
        //         BOOST_LOG_TRIVIAL(debug) << "step is null, no more statuses";
        //         return rxcpp::observable<>::empty<Status>().as_dynamic();
        //     }
        //     auto [nextOfNext, statusOfNext] = step->performBy(p, currentStatus);

        //     // 递归调用本函数
        //     auto allStatusesAfterNext = statusOfNext.default_if_empty(currentStatus).last().flat_map([&p, nextOfNext](const Status& st) {
        //         // get all statuses after next step
        //         return perform(p, nextOfNext, st);
        //     });

        //     return allStatusesAfterNext;
        // });

        // BOOST_LOG_TRIVIAL(debug) << "perform return";
        // return rxcpp::observable<>::just<Status>(currentStatus).concat(allStatusesAfterNext);
    }

private:
    std::shared_ptr<Dispatcher<Status>> m_entry;
    // rxcpp::subjects::subject<Step<Status>*> m_stepSeq;
};
