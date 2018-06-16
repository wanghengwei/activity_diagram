#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <system_error>
#include <activity_diagram.h>
#include <delay_action.h>
#include <node.h>
#include <empty_action.h>
#include <status_decision.h>
#include <loop_action.h>
#include <nested_activity_diagram.h>
#include <direct_decision.h>
#include <rxcpp/rx.hpp>
#include <boost/log/trivial.hpp>

using namespace std::literals::chrono_literals;

// class Tester : public Principal {};
class DummyPlayer : public Principal {};

// TEST(ActivityDiagramTest, EmptyAction) {
//     Tester tester;

//     EmptyAction<int> step{2};

//     auto r = step.performBy(tester);
//     r.sequence_equal(rxcpp::observable<>::from(2)).as_blocking().subscribe([](bool b) {
//         ASSERT_TRUE(b);
//     });
// }

// /*

//  +---------------------------------------------+
//  |                                             |
//  |         +-----+      +-----+      +-----+   |
// +++        |     |      |     |      |     |   |
// | +------> |  0  +----> |  0  +----> |  0  |   |
// +++        |     |      |     |      |     |   |
//  |         +-----+      +-----+      +-----+   |
//  |                                             |
//  +---------------------------------------------+


// */
// TEST(ActivityDiagramTest, SimpleNested) {
//     Tester tester;

//     auto s1 = std::make_shared<EmptyAction<int>>(0);
//     auto s2 = std::make_shared<EmptyAction<int>>(0);
//     auto s3 = std::make_shared<EmptyAction<int>>(0);

//     s1->setNext(s2);
//     s2->setNext(s3);

//     auto entry = std::make_shared<DirectDecision<int>>();
//     entry->setNext(s1);

//     NestedActivityDiagram<int> step;
//     step.setEntry(entry);

//     auto statusSeq = step.performBy(tester);

//     statusSeq.sequence_equal(rxcpp::observable<>::from(0, 0, 0)).as_blocking().subscribe([](bool b) {
//         ASSERT_TRUE(b);
//     });
// }

// /*

//                               Error
//                                 ^
//                                 |
//                                 |
//  +-----------------------------------------------+
//  |                              |                |
//  |      +------+   +------+     |     +------+   |
// +++     |      |   |      |   +-+-+   |      |   |
// | +----->  0   +--->  1   +--->   +--->  2   |   |
// +++     |      |   |      |   +---+   |      |   |
//  |      +------+   +------+           +------+   |
//  |                                               |
//  +-----------------------------------------------+

// */
// TEST(ActivityDiagramTest, CompisiteStepWithError) {
//     Tester tester;

//     auto s1 = std::make_shared<EmptyAction<int>>(0);
//     // The second action will FAILED!!
//     auto s2 = std::make_shared<EmptyAction<int>>(1);
//     auto s3 = std::make_shared<EmptyAction<int>>(2);

//     auto d2to3 = std::make_shared<StatusDecision<int>>();
//     d2to3->setFatalCondition([](int e) {
//         return e == 1;
//     });

//     s1->setNext(s2);
//     s2->setNext(d2to3);
//     d2to3->setDefaultNext(s3);

//     auto entry = std::make_shared<DirectDecision<int>>();
//     entry->setNext(s1);

//     NestedActivityDiagram<int> step;
//     step.setEntry(entry);

//     auto rez = step.performBy(tester);

//     rez.take(2).sequence_equal(rxcpp::observable<>::from(0, 1)).as_blocking().subscribe([](bool b) {
//         ASSERT_TRUE(b);
//     });

//     rez.skip(2).as_blocking().subscribe([](auto) {
//         FAIL();
//     }, [](std::exception_ptr) {
//         SUCCEED();
//     }, []() {
//         FAIL();
//     });
// }

// TEST(ActivityDiagramTest, LoopStep) {
//     Tester tester;

//     auto s1 = std::make_shared<EmptyAction<int>>(0);
//     auto s2 = std::make_shared<EmptyAction<int>>(1);
//     auto s3 = std::make_shared<EmptyAction<int>>(2);

//     s1->setNext(s2);
//     s2->setNext(s3);

//     auto entry = std::make_shared<DirectDecision<int>>();
//     entry->setNext(s1);

//     auto innerStep = std::make_shared<NestedActivityDiagram<int>>();
//     innerStep->setEntry(entry);

//     LoopAction<int> loop;
//     loop.setInnerAction(innerStep);
//     loop.setLoopCount(2);

//     auto statusSeq = loop.performBy(tester);

//     statusSeq.sequence_equal(rxcpp::observable<>::from(0, 1, 2, 0, 1, 2)).as_blocking().subscribe([](bool b) {
//         ASSERT_TRUE(b);
//     });
// }

// TEST(ActivityDiagramTest, Goto) {
//     Tester tester;

//     auto s1 = std::make_shared<EmptyAction<int>>(0);
//     auto s2 = std::make_shared<EmptyAction<int>>(1);
//     auto s3 = std::make_shared<EmptyAction<int>>(2);

//     auto sd = std::make_shared<StatusDecision<int>>();
//     sd->addRule([](int b) {
//         return b == 1;
//     }, s1.get());

//     s1->setNext(s2);
//     s2->setNext(sd);
//     sd->setDefaultNext(s3);

//     auto entry = std::make_shared<DirectDecision<int>>();
//     entry->setNext(s1);

//     NestedActivityDiagram<int> step;
//     step.setEntry(entry);

//     auto statusSeq = step.performBy(tester);

//     // int n = 0;
//     std::vector<int> v;
//     statusSeq.take(4).as_blocking().subscribe([&](int r) {
//         v.push_back(r);
//     });

//     std::vector<int> expected{0, 1, 0, 1};
//     ASSERT_EQ(v, expected);

//     // ASSERT_EQ(n, 3);
// }

// TEST(ActivityDiagramTest, GotoOuterNodeFromLoop) {
//     Tester tester;

//     auto node_A = std::make_shared<EmptyAction<int>>(3);
//     auto node_B = std::make_shared<EmptyAction<int>>(4);

//     //begin make loop action
//     auto s1 = std::make_shared<EmptyAction<int>>(0);
//     auto s2 = std::make_shared<EmptyAction<int>>(1);
//     auto d = std::make_shared<StatusDecision<int>>();
//     auto s3 = std::make_shared<EmptyAction<int>>(2);

//     s1->setNext(s2);
//     s2->setNext(d);
//     d->setDefaultNext(s3);

//     d->addRule([](int r) {
//         return r == 1;
//     }, node_B.get());

//     auto entry = std::make_shared<DirectDecision<int>>();
//     entry->setNext(s1);

//     auto innerStep = std::make_shared<NestedActivityDiagram<int>>();
//     innerStep->setEntry(entry);

//     auto loop = std::make_shared<LoopAction<int>>();
//     loop->setInnerAction(innerStep);
//     loop->setLoopCount(2);
//     //end make loop action

//     auto node_C = std::make_shared<EmptyAction<int>>(5);

//     node_A->setNext(node_B);
//     node_B->setNext(loop);
//     loop->setNext(node_C);

//     //start make a whole diagram
//     NestedActivietyDiagram<int> g;
//     auto init = std::make_shared<DirectDecision<int>>();
//     init->setNext(node_A);
//     g.setEntry(init);

//     auto statusSeq = g.performBy(tester);

//     statusSeq.take(8).map([](int x) {
//         // BOOST_LOG_TRIVIAL(debug) << "item: " << x;
//         return x;
//     }).sequence_equal(rxcpp::observable<>::from(3, 4, 0, 1, 4, 0, 1, 4)).as_blocking().subscribe([](bool b) {
//         ASSERT_TRUE(b);
//     });
// }

struct ActivityDiagramTest : ::testing::Test {
    DummyPlayer player;
};

// A -> B -> C -> D ->o
//           ^--------|
// int C(loop):
//   C_I_A -> C_I_B
TEST_F(ActivityDiagramTest, Complext) {
    // DummyPlayer player;

    auto g = std::make_shared<ActivityDiagram<int>>();

    auto nodeA = std::make_shared<EmptyAction<int>>(1);
    auto nodeB = std::make_shared<EmptyAction<int>>(2);
    auto nodeC = std::make_shared<LoopAction<int>>(2);
    auto nodeD = std::make_shared<EmptyAction<int>>(3);
    auto d01 = std::make_shared<StatusDecision<int>>();

    auto nodeC_I = std::make_shared<NestedActivityDiagram<int>>();
    auto nodeC_I_I = std::make_shared<DirectDecision<int>>();
    auto nodeC_I_A = std::make_shared<EmptyAction<int>>(4);
    auto nodeC_I_B = std::make_shared<EmptyAction<int>>(5);

    nodeA->setNext(nodeB.get());
    nodeB->setNext(nodeC.get());
    nodeC->setNext(nodeD.get());
    nodeD->setNext(d01.get());
    d01->setDefaultNext(nodeC.get());

    nodeC->setInnerAction(nodeC_I);
    nodeC_I->setInitial(nodeC_I_I);
    nodeC_I->add(nodeC_I_A);
    nodeC_I->add(nodeC_I_B);
    nodeC_I_I->setNext(nodeC_I_A.get());
    nodeC_I_A->setNext(nodeC_I_B.get());

    g->add(nodeA);
    g->add(nodeB);
    g->add(nodeC);
    g->add(nodeD);
    g->add(d01);

    g->setFirst(nodeA.get());

    auto results = g->performBy(player);
    auto expected = rxcpp::observable<>::from(1, 2, 4, 5, 4, 5, 3, 4, 5, 4, 5, 3).as_dynamic();

    results.take(12).sequence_equal(expected).as_blocking().subscribe([](bool ok) {
        ASSERT_TRUE(ok);
    });
}

TEST_F(ActivityDiagramTest, Sleep) {
    ActivityDiagram<int> g;
    
    auto nodeA = std::make_shared<DelayAction<int>>(2);

    g.add(nodeA);
    g.setFirst(nodeA.get());

    auto results = g.performBy(player);
    // auto results = rxcpp::observable<>::timer(2s).ignore_elements();
    auto now = std::chrono::steady_clock::now();
    BOOST_LOG_TRIVIAL(info) << "before delay";
    results.is_empty().as_blocking().subscribe([](auto ok) {
        // BOOST_LOG_TRIVIAL(info) << "item: " << ok;
        ASSERT_TRUE(ok);
    });
    BOOST_LOG_TRIVIAL(info) << "after delay";
    auto elapsed = std::chrono::steady_clock::now() - now;
    ASSERT_TRUE(elapsed > 1s && elapsed < 3s);
    // std::this_thread::sleep_for(5s);
}