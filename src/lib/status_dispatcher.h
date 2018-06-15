#pragma once
#include "step.h"
#include <functional>
#include <boost/log/trivial.hpp>

template<typename Status>
class StatusDispatcher : public Dispatcher<Status>
{
public:
    std::tuple<rxcpp::observable<Step<Status>*>, rxcpp::observable<Status>> performBy(Principal&, const Status& status) const override {
        BOOST_LOG_TRIVIAL(debug) << "dispatching by status: " << status;
        // 严重错误直接中断序列
        if (m_fatalCond && m_fatalCond(status)) {
            return std::make_tuple(rxcpp::observable<>::empty<Step<Status>*>(), rxcpp::observable<>::error<Status>(std::runtime_error{"FatalError"}));
        }

        // 检查状态码，看看有没有合适的分支
        for (const auto& p : m_gotos) {
            if (p.first(status)) {
                return std::make_tuple(rxcpp::observable<>::just(p.second), rxcpp::observable<>::empty<Status>());
            }
        }

        // 如果没有合适的goto，返回默认的下一个节点
        return std::make_tuple(rxcpp::observable<>::just(m_defaultNext.get()), rxcpp::observable<>::empty<Status>());
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
