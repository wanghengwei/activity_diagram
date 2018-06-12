#include "Step.h"
#include "Robot.h"
#include <boost/log/trivial.hpp>

bool operator==(const Status& l, const Status& r) {
    return false;
}

StatusSequence SendEventStep::performBy(Robot& robot) const {
    BOOST_LOG_TRIVIAL(info) << "SendEventStep.performBy: robot sending event: id=" << m_eventId;
    robot.sendEvent(Message{m_eventId});
    return rxcpp::observable<>::just(Status{});
}


StatusSequence WaitEventStep::performBy(Robot& robot) const {
    // robot.sendEvent()
    BOOST_LOG_TRIVIAL(info) << "WaitEventStep.performBy: robot waiting event: id=" << m_eventId;
    int id = m_eventId;
    return robot.getMessageSequence().filter([id](const Message& msg) {
        BOOST_LOG_TRIVIAL(debug) << "filtering message which id is " << id;
        return msg.getId() == id;
    }).take(1).map([](const Message& msg) {
        // get status from msg
        return Status{};
    });
}

rxcpp::observable<Status> SleepStep::performBy(Robot&) const {
    BOOST_LOG_TRIVIAL(debug) << "Sleeping for " << m_period.count() << " milliseconds";
    return rxcpp::observable<>::just(Status{}).delay(m_period);
}
