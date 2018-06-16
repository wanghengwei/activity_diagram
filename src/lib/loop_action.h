#pragma once
#include "node.h"

template<typename Status>
class LoopAction : public Action<Status> {
public:
    explicit LoopAction(int64_t n = -1) : m_loopCount{n} {}

    rxcpp::observable<Status> performBy(Principal& p) const override {
        if (!m_innerStep) {
            return rxcpp::observable<>::empty<Status>();
        }

        return rxcpp::observable<>::just(m_innerStep.get()).repeat(m_loopCount).flat_map([&p](Action<Status>* step) {
            return step->performBy(p);
        });
    }

    void setInnerAction(std::shared_ptr<Action<Status>> inner) {
        m_innerStep.swap(inner);
    }

    void setLoopCount(int64_t n) {
        m_loopCount = n;
    }

    using Action<Status>::getNext;
private:
    std::shared_ptr<Action<Status>> m_innerStep;
    int64_t m_loopCount{-1};
};
