#include "Robot.h"
#include "Blueprint.h"
#include <boost/log/trivial.hpp>
#include <fmt/format.h>

void Robot::start() {

    static rxcpp::schedulers::worker singleWorker = rxcpp::schedulers::make_event_loop().create_worker();

    rxcpp::observable<StepPtr> steps = m_steps.get_observable();
    rxcpp::subscriber<StepPtr> stepSink = m_steps.get_subscriber();

    steps.start_with(m_blueprint->getFirstStep()).observe_on(rxcpp::identity_same_worker(singleWorker)).flat_map([this](StepPtr step) {
        BOOST_LOG_TRIVIAL(debug) << toString() << " performing one step...";
        rxcpp::observable<Status> statusSeq = step->performBy(*this);
        BOOST_LOG_TRIVIAL(debug) << "get a status sequence from Step";
        return statusSeq.map([step](Status st) {
            return std::make_pair(step, st);
        });
    }).observe_on(rxcpp::observe_on_event_loop()).subscribe([this, stepSink](const std::pair<std::shared_ptr<Step>, Status>& stat) {
        BOOST_LOG_TRIVIAL(debug) << "get status";
        StepPtr n = m_blueprint->getNextStep(stat.first, stat.second);
        BOOST_LOG_TRIVIAL(debug) << "get next step by status and current step";
        stepSink.on_next(n);
        BOOST_LOG_TRIVIAL(debug) << "pushed next step into step sequence";
    }, []() {
        BOOST_LOG_TRIVIAL(info) << "no more steps";
    });
}

std::string Robot::toString() const {
    return fmt::format("[{}]", getId());
}

rxcpp::observable<Message> Robot::getMessageSequence() const {
    return m_msgs.get_observable();
}

void Robot::sendEvent(const Message& msg) {
    auto s = m_msgs.get_subscriber();
    BOOST_LOG_TRIVIAL(debug) << "Robot.sendEvent: push message " << msg.getId() << " into MessageSequence";
    s.on_next(msg);
}