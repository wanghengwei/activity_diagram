#pragma once
#include "node.h"
#include <boost/log/trivial.hpp>
#include <boost/type_index.hpp>

template<typename Status>
class NestedActivityDiagram : public Action<Status> {
public:

    // NestedActivityDiagram(rxcpp::)
    rxcpp::observable<Status> performBy(Principal& p, rxcpp::schedulers::worker w) const override {
        using ActionAndState = std::pair<Node<Status>*, Status>;

        // static auto worker = rxcpp::schedulers::make_event_loop().create_worker();

        auto _s = std::make_shared<rxcpp::subjects::behavior<ActionAndState>>(std::make_pair(getInitial(), Status{}));

        auto actionSeq = _s->get_observable();

        return actionSeq.subscribe_on(rxcpp::serialize_same_worker(w)).flat_map([&p, _s, w](auto x) {
            // BOOST_LOG_TRIVIAL(debug) << "flat_map";
            auto stateSeq = x.first->performBy(p, w);
            stateSeq
            .observe_on(rxcpp::serialize_same_worker(w))
            .take_last(1).default_if_empty(x.second).subscribe([cur = x.first, _s](const Status& ls) {
                auto next = cur->getNext(ls);
                // BOOST_LOG_TRIVIAL(debug) << "getNext: state=" << ls << ", next=" << next;
                if (next) {
                    _s->get_subscriber().on_next(std::make_pair(next, ls));
                } else {
                    _s->get_subscriber().on_completed();
                }
            });
            return stateSeq;
        });
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

    // static rxcpp::observable<Status> perform(Principal &p, Node<Status>* step, const Status& lastStatus) {
    //     if (!step) {
    //         return rxcpp::observable<>::empty<Status>().as_dynamic();
    //     }

    //     BOOST_LOG_TRIVIAL(debug) << "call performBy for " << step;
    //     auto statusOfCurrent = step->performBy(p).replay();
    //     // statusOfCurrent.connect();

    //     auto leftStatuses = statusOfCurrent.take_last(1).default_if_empty(lastStatus).flat_map([step, &p](const Status& currentStatus) {
    //         BOOST_LOG_TRIVIAL(debug) << "flat map on last status: " << currentStatus;
    //         Node<Status>* nextStep = step->getNext(currentStatus);
    //         return perform(p, nextStep, currentStatus);
    //     });

    //     auto r = statusOfCurrent.concat(leftStatuses);
    //     statusOfCurrent.connect_forever();
    //     return r;
    //     // return leftStatuses;
    // }

private:
    std::shared_ptr<Decision<Status>> m_entry;
    std::vector<std::shared_ptr<Node<Status>>> m_children;
};
