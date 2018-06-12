#include "Blueprint.h"
#include <fruit/fruit.h>
#include <boost/log/trivial.hpp>

class DemoBlueprint : public Blueprint {
public:
    INJECT(DemoBlueprint()) {}

    std::shared_ptr<Step> getFirstStep() const override {
        BOOST_LOG_TRIVIAL(debug) << "getFirstStep";
        // return std::shared_ptr<Step>{new SendEventStep{1}};
        return std::shared_ptr<Step>{new SleepStep{1000}};
    }

    std::shared_ptr<Step> getNextStep(const std::shared_ptr<Step>& current, Status stat) const {
        {
            auto p = dynamic_cast<const SendEventStep*>(current.get());
            if (p) {
                int sentId = p->m_eventId;
                BOOST_LOG_TRIVIAL(debug) << "next step is WaitEvent: " << sentId;
                return std::shared_ptr<Step>{new WaitEventStep{sentId}};
            }
        }

        {
            auto p = dynamic_cast<const WaitEventStep*>(current.get());
            if (p) {
                int waitId = p->m_eventId;
                BOOST_LOG_TRIVIAL(debug) << "next step is SendEvent: " << waitId + 1;
                return std::shared_ptr<Step>{new SendEventStep{waitId + 1}};
            }
        }

        {
            auto p = dynamic_cast<const SleepStep*>(current.get());
            if (p) {
                return std::shared_ptr<Step>{new SleepStep{p->m_period.count()}};
            }
        }

        throw std::runtime_error{"unexpected step type"};
    }
private:

};

fruit::Component<Blueprint> getBlueprint() {
    return fruit::createComponent()
        .bind<Blueprint, DemoBlueprint>();
}