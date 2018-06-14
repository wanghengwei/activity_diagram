#pragma once
#include "step.h"
#include <functional>

template<typename Status>
class StatusDispatcher : public Dispatcher<Status>
{
public:
    Step<Status>* getNext(const Status& st) const override {
        if (m_fatalCond && m_fatalCond(st)) {
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

    template<typename Cond>
    void addRule(const Cond& cond, Step<Status>* tar) {
        m_gotos.push_back(std::make_pair(cond, tar));
    }
private:
    std::function<bool(const Status&)> m_fatalCond;

    std::shared_ptr<Step<Status>> m_defaultNext;

    std::vector<std::pair<std::function<bool(const Status&)>, Step<Status>*>> m_gotos;
};
