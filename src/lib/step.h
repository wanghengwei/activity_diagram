#pragma once
#include "principal.h"
#include <rxcpp/rx-observable.hpp>

template<typename Status>
class Step {
public:
    virtual ~Step() {}

    virtual rxcpp::observable<Status> performBy(Principal&) const = 0;

    virtual Step* getNext(const Status& st) const = 0;
};

template<typename Status>
class ActionStep : public Step<Status> {
public:
    Step<Status>* getNext(const Status& st) const override {
        return m_next.get();
    }

    void setNext(std::shared_ptr<Step<Status>> next) {
        m_next.swap(next);
    }
private:
    std::shared_ptr<Step<Status>> m_next;
};

template<typename Status>
class Dispatcher : public Step<Status> {
public:
    rxcpp::observable<Status> performBy(Principal&) const override {
        return rxcpp::observable<>::empty<Status>();
    }
};

template<typename Status>
class EmptyActionStep : public ActionStep<Status> {
public:
    explicit EmptyActionStep(const Status& s = Status{}) : m_status{s} {}

    rxcpp::observable<Status> performBy(Principal&) const override {
        return rxcpp::observable<>::just(m_status);
    }
private:
    Status m_status;
};

template<typename Status>
class DirectDispatcher : public Dispatcher<Status> {
public:
    Step<Status>* getNext(const Status&) const {
        return m_next.get();
    }

    void setNext(std::shared_ptr<Step<Status>> next) {
        m_next.swap(next);
    }
private:
    std::shared_ptr<Step<Status>> m_next;
};

//template<typename Status>
//class LoopStep : public CompositeStep<Status> {
//public:
//    Dispatcher<Status>* getEntry() const override {
//        return m_entry.get();
//    }
//private:
//    std::shared_ptr<Dispatcher<Status>> m_entry{new DirectDispatcher<Status>};
//    std::shared_ptr<Step<Status>> m_innerStep;
//};

//class SendEventStep : public Step {
//public:
//    explicit SendEventStep(int id) : m_eventId{id} {}

//    StatusSequence performBy(Robot& robot) const override;

//    int m_eventId;
//};

//class WaitEventStep : public Step {
//public:
//    explicit WaitEventStep(int id) : m_eventId{id} {}

//    StatusSequence performBy(Robot& robot) const override;

//    int m_eventId;
//};

//class SleepStep : public Step {
//public:
//    explicit SleepStep(int64_t ms) : m_period{ms} {}

//    rxcpp::observable<Status> performBy(Robot&) const override;

//    std::chrono::milliseconds m_period;
//};

//class MultiStep : public Step {
//public:
//    rxcpp::observable<Status> performBy(Robot&) const override;
    
//    std::vector<std::shared_ptr<Step>> m_innerSteps;
//};

//class LoopStep : public Step {
//public:
//    rxcpp::observable<Status> performBy(Robot&) const override;

//    std::shared_ptr<SequenceStep> m_innerStep;
//    int64_t m_loopCount{};
//};
