#pragma once
#include "step.h"

template<typename Status>
class LoopActionStep : public ActionStep<Status> {
public:
    rxcpp::observable<Status> performBy(Principal& p) const override {
        if (!m_innerStep) {
            return rxcpp::observable<>::empty<Status>();
        }

        return rxcpp::observable<>::just(m_innerStep).repeat(m_loopCount).flat_map([&p](std::shared_ptr<Step<Status>> step) {
            return step->performBy(p);
        });
    }

    void setInnerAction(std::shared_ptr<ActionStep<Status>> inner) {
        m_innerStep.swap(inner);
    }

    void setLoopCount(int64_t n) {
        m_loopCount = n;
    }
private:
    std::shared_ptr<ActionStep<Status>> m_innerStep;
    int64_t m_loopCount{-1};
};
