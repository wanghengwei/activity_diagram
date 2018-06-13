#include "step.h"
#include <boost/log/trivial.hpp>

//bool operator==(const Status& l, const Status& r) {
//    return false;
//}

//StatusSequence SendEventStep::performBy(Robot& robot) const {
//    BOOST_LOG_TRIVIAL(info) << "SendEventStep.performBy: robot sending event: id=" << m_eventId;
//    robot.sendEvent(Message{m_eventId});
//    return rxcpp::observable<>::just(Status{});
//}


//StatusSequence WaitEventStep::performBy(Robot& robot) const {
//    // robot.sendEvent()
//    BOOST_LOG_TRIVIAL(info) << "WaitEventStep.performBy: robot waiting event: id=" << m_eventId;
//    int id = m_eventId;
//    return robot.getMessageSequence().filter([id](const Message& msg) {
//        BOOST_LOG_TRIVIAL(debug) << "filtering message which id is " << id;
//        return msg.getId() == id;
//    }).take(1).map([](const Message& msg) {
//        // get status from msg
//        return Status{};
//    });
//}

//rxcpp::observable<Status> SleepStep::performBy(Robot&) const {
//    BOOST_LOG_TRIVIAL(debug) << "Sleeping for " << m_period.count() << " milliseconds";
//    return rxcpp::observable<>::just(Status{}).delay(m_period);
//}

//rxcpp::observable<Status> MultiStep::performBy(Robot& robot) const {
//    rxcpp::observable<>::iterate(m_innerSteps).map([&robot](auto step) {
//        return step->performBy(robot);
//    }).amb();
//}

//rxcpp::observable<Status> SequenceStep::performBy(Robot& robot) const {
//    rxcpp::observable<>::iterate(m_innerSteps).map([&robot](auto step) {
//        return step->performBy(robot);
//    }).concat();
//}

//rxcpp::observable<Status> LoopStep::performBy(Robot& robot) const {
//    if (m_loopCount == 0) {
//        return rxcpp::observable<>::empty<Status>();
//    } else {
//        auto o = rxcpp::observable<>::just(m_innerStep);
//        auto steps = m_loopCount < 0 ? rxcpp::observable<>::just(m_innerStep).repeat().as_dynamic() : rxcpp::observable<>::just(m_innerStep).repeat(m_loopCount).as_dynamic();
//        return steps.flat_map([&robot](auto step) {
//            return step->performBy(robot);
//        });
//    }
//}
