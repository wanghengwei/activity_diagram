#pragma once
#include "step.h"
// #include "action_step.h"

template<typename Status>
class LoopActionStep : public ActionStep<Status> {
public:
    std::tuple<rxcpp::observable<Step<Status>*>, rxcpp::observable<Status>> performBy(Principal& p, const Status& = Status{}) const override {
        if (!m_innerStep) {
            return std::make_tuple(rxcpp::observable<>::just<Step<Status>*>(getNext()), rxcpp::observable<>::empty<Status>());
        }

        auto sts = rxcpp::observable<>::just(m_innerStep.get()).repeat(m_loopCount).flat_map([&p](ActionStep<Status>* step) {
            auto [_, statuses] = step->performBy(p);
            return statuses;
        });

        return std::make_tuple(rxcpp::observable<>::just<Step<Status>*>(getNext()), sts);
    }

    void setInnerAction(std::shared_ptr<ActionStep<Status>> inner) {
        m_innerStep.swap(inner);
    }

    void setLoopCount(int64_t n) {
        m_loopCount = n;
    }

    using ActionStep<Status>::getNext;
private:
    std::shared_ptr<ActionStep<Status>> m_innerStep;
    int64_t m_loopCount{-1};
};
