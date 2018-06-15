#include <gtest/gtest.h>
#include <step.h>
#include <empty_action_step.h>
#include <status_dispatcher.h>
#include <loop_action_step.h>
#include <composite_action_step.h>
#include <direct_dispatcher.h>
#include <rxcpp/rx.hpp>
#include <boost/log/trivial.hpp>

using namespace std::literals::chrono_literals;

// TEST(RxCpp, Subject) {
//     BOOST_LOG_TRIVIAL(info) << "start test";

//     rxcpp::subjects::subject<uint64_t> s;
//     s.get_observable().start_with(uint64_t(1))
//     // .observe_on(rxcpp::observe_on_event_loop())
//     // .observe_on(rxcpp::observe_on_event_loop())
//     .scan(uint64_t(1), [&s](uint64_t seed, uint64_t x) {
//         s.get_subscriber().on_next(seed);
//         return seed + x;
//     }).subscribe([](auto x) {
//         BOOST_LOG_TRIVIAL(info) << x;
//     });

//     std::this_thread::sleep_for(3s);
// }

class Tester : public Principal {};

TEST(BlueprintTest, EmptyAction) {
    EmptyAction<int> step;
    Tester tester;

    auto r = step.performBy(tester);
    int n = 0;
    r.as_blocking().subscribe([&n](int st) {
        ASSERT_EQ(st, 0);
        ++n;
    });
    ASSERT_EQ(n, 1);
}

TEST(BlueprintTest, CompisiteStep) {
    Tester tester;

    auto s1 = std::make_shared<EmptyAction<bool>>(true);
    auto s2 = std::make_shared<EmptyAction<bool>>(true);
    auto s3 = std::make_shared<EmptyAction<bool>>(true);

    s1->setNext(s2);
    s2->setNext(s3);

    auto entry = std::make_shared<DirectDispatcher<bool>>();
    entry->setNext(s1);

    CompositeActionStep<bool> step;
    step.setEntry(entry);

    BOOST_LOG_TRIVIAL(debug) << "start performing";
    auto statusSeq = step.performBy(tester);
    BOOST_LOG_TRIVIAL(debug) << "end performing";

    int n = 0;
    statusSeq
    .all([&n](bool s) {
        BOOST_LOG_TRIVIAL(debug) << "status: " << s;
        ++n;
        return s;
    })
    .as_blocking().subscribe([](bool b) {
        ASSERT_TRUE(b);
        // BOOST_LOG_TRIVIAL(debug) << "subscribe: " << b;
    });

    ASSERT_EQ(n, 3);
}

TEST(BlueprintTest, CompisiteStepWithError) {
    Tester tester;

    auto s1 = std::make_shared<EmptyAction<int>>(0);
    // The second action will FAILED!!
    auto s2 = std::make_shared<EmptyAction<int>>(1);
    auto s3 = std::make_shared<EmptyAction<int>>(1);

    auto d2to3 = std::make_shared<StatusDispatcher<int>>();
    d2to3->setFatalCondition([](int e) {
        return e == 1;
    });

    s1->setNext(s2);
    s2->setNext(d2to3);
    d2to3->setDefaultNext(s3);

    auto entry = std::make_shared<DirectDispatcher<int>>();
    entry->setNext(s1);

    CompositeActionStep<int> step;
    step.setEntry(entry);

    auto rez = step.performBy(tester);

    int n = 0;
    bool hasError = false;
    rez.as_blocking().subscribe([&n](int b) {
        BOOST_LOG_TRIVIAL(info) << "subscribed: " << b;
        // ASSERT_EQ(b, 0);
        ++n;
    }, [&hasError](std::exception_ptr) {
        hasError = true;
        BOOST_LOG_TRIVIAL(info) << "has error!";
    }, []() {
        ASSERT_TRUE(false);
        BOOST_LOG_TRIVIAL(info) << "completed";
    });

    ASSERT_EQ(n, 2);
    ASSERT_TRUE(hasError);
}

TEST(BlueprintTest, LoopStep) {
    Tester tester;

    auto s1 = std::make_shared<EmptyAction<int>>();
    auto s2 = std::make_shared<EmptyAction<int>>();
    auto s3 = std::make_shared<EmptyAction<int>>();

    s1->setNext(s2);
    s2->setNext(s3);

    auto entry = std::make_shared<DirectDispatcher<int>>();
    entry->setNext(s1);

    auto innerStep = std::make_shared<CompositeActionStep<int>>();
    innerStep->setEntry(entry);

    LoopActionStep<int> loop;
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

    auto s1 = std::make_shared<EmptyAction<int>>(0);
    auto s2 = std::make_shared<EmptyAction<int>>(1);
    auto s3 = std::make_shared<EmptyAction<int>>(2);

    auto sd = std::make_shared<StatusDispatcher<int>>();
    sd->addRule([](int b) {
        return b == 1;
    }, s1.get());

    s1->setNext(s2);
    s2->setNext(sd);
    sd->setDefaultNext(s3);

    auto entry = std::make_shared<DirectDispatcher<int>>();
    entry->setNext(s1);

    CompositeActionStep<int> step;
    step.setEntry(entry);

    auto statusSeq = step.performBy(tester);

    // int n = 0;
    std::vector<int> v;
    statusSeq.take(4).as_blocking().subscribe([&](int r) {
        v.push_back(r);
    });

    std::vector<int> expected{0, 1, 0, 1};
    ASSERT_EQ(v, expected);

    // ASSERT_EQ(n, 3);
}