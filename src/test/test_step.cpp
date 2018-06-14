#include <gtest/gtest.h>
#include <step.h>
#include <statusdispatcher.h>
#include <loopactionstep.h>
#include <composite_action_step.h>

class Tester : public Principal {};

TEST(BlueprintTest, EmptyAction) {
    EmptyActionStep<bool> step{true};
    Tester tester;

    auto r = step.performBy(tester);
    int n = 0;
    r.as_blocking().subscribe([&n](bool st) {
        ASSERT_TRUE(st);
        ++n;
    });
    ASSERT_EQ(n, 1);
}

TEST(BlueprintTest, CompisiteStep) {
    Tester tester;

    auto s1 = std::make_shared<EmptyActionStep<bool>>(true);
    auto s2 = std::make_shared<EmptyActionStep<bool>>(true);
    auto s3 = std::make_shared<EmptyActionStep<bool>>(true);

    s1->setNext(s2);
    s2->setNext(s3);

    auto entry = std::make_shared<DirectDispatcher<bool>>();
    entry->setNext(s1);

    CompositeActionStep<bool> step;
    step.setEntry(entry);

    auto statusSeq = step.performBy(tester);

    int n = 0;
    statusSeq.all([&n](bool s) {
        ++n;
        return s;
    }).as_blocking().subscribe([](bool b) {
        ASSERT_TRUE(b);
    });

    ASSERT_EQ(n, 3);
}

TEST(BlueprintTest, CompisiteStepWithError) {
    Tester tester;

    auto s1 = std::make_shared<EmptyActionStep<bool>>(true);
    // The second action will FAILED!!
    auto s2 = std::make_shared<EmptyActionStep<bool>>(false);
    auto s3 = std::make_shared<EmptyActionStep<bool>>(false);

    auto d2to3 = std::make_shared<StatusDispatcher<bool>>();
    d2to3->setFatalCondition([](bool b) {
        return b == false;
    });

    s1->setNext(s2);
    s2->setNext(d2to3);
    d2to3->setDefaultNext(s3);

    auto entry = std::make_shared<DirectDispatcher<bool>>();
    entry->setNext(s1);

    CompositeActionStep<bool> step;
    step.setEntry(entry);

    auto statusSeq = step.performBy(tester);

    int n = 0;
    bool hasError = false;
    statusSeq.as_blocking().subscribe([&n](bool b) {
        ASSERT_TRUE(b);
        ++n;
    }, [&hasError](std::exception_ptr) {
        hasError = true;
    }, []() {
        ASSERT_TRUE(false);
    });

    ASSERT_EQ(n, 2);
    ASSERT_TRUE(hasError);
}

TEST(BlueprintTest, LoopStep) {
    Tester tester;

    auto s1 = std::make_shared<EmptyActionStep<bool>>();
    auto s2 = std::make_shared<EmptyActionStep<bool>>();
    auto s3 = std::make_shared<EmptyActionStep<bool>>();

    s1->setNext(s2);
    s2->setNext(s3);

    auto entry = std::make_shared<DirectDispatcher<bool>>();
    entry->setNext(s1);

    auto innerStep = std::make_shared<CompositeActionStep<bool>>();
    innerStep->setEntry(entry);

    LoopActionStep<bool> loop;
    loop.setInnerAction(innerStep);
    loop.setLoopCount(2);

    auto statusSeq = loop.performBy(tester);

    int n = 0;
    statusSeq.all([&n](bool s) {
        ++n;
        return !s;
    }).as_blocking().subscribe([](bool b) {
        ASSERT_TRUE(b);
    });

    ASSERT_EQ(n, 6);
}

TEST(BlueprintTest, Goto) {
    Tester tester;

    auto s1 = std::make_shared<EmptyActionStep<bool>>(true);
    auto s2 = std::make_shared<EmptyActionStep<bool>>(false);
    auto s3 = std::make_shared<EmptyActionStep<bool>>(false);

    auto sd = std::make_shared<StatusDispatcher<bool>>();
    sd->addRule([](bool b) {
        return !b;
    }, s1.get());

    s1->setNext(s2);
    s2->setNext(sd);
    sd->setDefaultNext(s3);

    auto entry = std::make_shared<DirectDispatcher<bool>>();
    entry->setNext(s1);

    CompositeActionStep<bool> step;
    step.setEntry(entry);

    auto statusSeq = step.performBy(tester);

    // int n = 0;
    statusSeq.take(3).last().as_blocking().subscribe([](bool b) {
        ASSERT_TRUE(b);
    });

    // ASSERT_EQ(n, 3);
}