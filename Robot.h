#pragma once
#include "Step.h"
#include "Message.h"
#include <rxcpp/rx-observable.hpp>
#include <rxcpp/rx-subjects.hpp>

class Blueprint;

class Robot {
public:
    explicit Robot(Blueprint* bp, int id) : m_blueprint{bp}, m_id{id} {}

    void start();

    // StepPtr getFirstStep();

    // StepPtr getNextStep(const StepPtr& current, Status st);

    rxcpp::observable<Message> getMessageSequence() const;

    void sendEvent(const Message& msg);

    int getId() const {
        return m_id;
    }

    std::string toString() const;

private:
    Blueprint* m_blueprint{};
    int m_id{};
    // rxcpp::observable<StepPtr> m_steps;
    rxcpp::subjects::subject<StepPtr> m_steps;
    // rxcpp::observable<Message> m_msgs;
    rxcpp::subjects::behavior<Message> m_msgs{Message{}};
};
