#pragma once
#include "step.h"
#include <functional>

template<typename Status>
class StatusDispatcher : public Dispatcher<Status>
{
public:
    Step<Status>* getNext(const Status& st) const override {
        if (m_fatalCond(st)) {
            throw std::runtime_error{""};
        }

        return m_defaultNext.get();
    }

    template<typename Cond>
    void setFatalCondition(const Cond& cond) {
        m_fatalCond = cond;
    }

    void setDefaultNext(std::shared_ptr<Step<Status>> n) {
        m_defaultNext.swap(n);
    }
private:
    std::function<bool(const Status&)> m_fatalCond;

    std::shared_ptr<Step<Status>> m_defaultNext;
};
