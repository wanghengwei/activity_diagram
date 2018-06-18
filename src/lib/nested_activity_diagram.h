#pragma once
#include "node.h"
#include <boost/log/trivial.hpp>
#include <boost/type_index.hpp>

template<typename Status>
class NestedActivityDiagram : public Action<Status> {
public:

    rxcpp::observable<Status> performBy(Principal& p) const override {
        Node<Status>* entry = getInitial();

        using NodeWithDefaultState = std::pair<Node<Status>*, Status>;
        
        auto subject = std::make_shared<rxcpp::subjects::subject<NodeWithDefaultState>>();

        auto actionObservable = subject->get_observable();

        // seq<(node, seq<state>, lastState)>
        auto o1 = actionObservable.map([&p](Node<Status>* n) {
            auto stSeq = n->performBy(p).publish();
            stSeq.connect_forever();
            return std::make_tuple(n, stSeq.as_dynamic(), Status{});
        }).publish();

        // seq<(node, state)>
        o1
        .observe_on(rxcpp::observe_on_event_loop())
        // .subscribe_on(rxcpp::observe_on_event_loop())
        .flat_map([](auto x) {
            Node<Status>* cur = std::get<0>(x);
            Status lastState = std::get<2>(x);
            return std::get<1>(x).take_last(1).default_if_empty(lastState).map([cur](const Status& st) {
                return std::make_pair(cur, st);
            });
        })
        .subscribe([subject](auto pr) {
            Node<Status>* next = pr.first->getNext(pr.second);
            if (next) {
                BOOST_LOG_TRIVIAL(debug) << "add an new action";
                subject->get_subscriber().on_next(next);
            } else {
                BOOST_LOG_TRIVIAL(debug) << "no more actions, completed";
                subject->get_subscriber().on_completed();
            }
        })
        ;

        // seq<state>
        auto ret = o1
        .flat_map([](auto o) {
            return std::get<1>(o).tap([](auto) {
                BOOST_LOG_TRIVIAL(debug) << "22222";
            });
            // .map([](const auto& pr) {
            //     return std::get<1>(pr);
            // })
            ;
        });

        o1.connect_forever();
        subject->get_subscriber().on_next(entry);
        return ret;
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
