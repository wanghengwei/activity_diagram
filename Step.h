#pragma once
#include <rxcpp/rx-observable.hpp>

class Status {};

bool operator==(const Status& l, const Status& r);

typedef rxcpp::observable<Status> StatusSequence;

class Robot;

class Step {
public:
    virtual rxcpp::observable<Status> performBy(Robot&) const = 0;
};

typedef std::shared_ptr<Step> StepPtr;

class SendEventStep : public Step {
public:
    explicit SendEventStep(int id) : m_eventId{id} {}

    StatusSequence performBy(Robot& robot) const override;

    int m_eventId;
};

class WaitEventStep : public Step {
public:
    explicit WaitEventStep(int id) : m_eventId{id} {}

    StatusSequence performBy(Robot& robot) const override;

    int m_eventId;
};

class SleepStep : public Step {
public:
    explicit SleepStep(int64_t ms) : m_period{ms} {}

    rxcpp::observable<Status> performBy(Robot&) const override;

    std::chrono::milliseconds m_period;
};

class MultiStep : public Step {
public:
    rxcpp::observable<Status> performBy(Robot&) const override;
    
    std::vector<std::shared_ptr<Step>> m_innerSteps;
};

class SequenceStep : public Step {
public:
    rxcpp::observable<Status> performBy(Robot&) const override;

    std::vector<std::shared_ptr<Step>> m_innerSteps;
};

class LoopStep : public Step {
public:
    rxcpp::observable<Status> performBy(Robot&) const override;

    std::shared_ptr<SequenceStep> m_innerStep;
    int64_t m_loopCount{};
};