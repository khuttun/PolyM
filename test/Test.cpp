#include "polym/Queue.hpp"
#include "Tester.hpp"
#include <functional>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

// Test that MsgUIDs generated in two different threads simultaneously are unique
void testMsgUID()
{
    const int N = 1000;
    std::vector<PolyM::MsgUID> uids1, uids2;

    auto createMsgs = [](int count, std::vector<PolyM::MsgUID>& uids)
    {
        for (int i = 0; i < count; ++i)
            uids.push_back(PolyM::Msg(1).getUniqueId());
    };

    std::thread t1(createMsgs, N, std::ref(uids1));
    std::thread t2(createMsgs, N, std::ref(uids2));
    t1.join();
    t2.join();

    for (auto uid1 : uids1)
    {
        for (auto uid2 : uids2)
        {
            TEST_NOT_EQUALS(uid1, uid2);
        }
    }
}

// Test that messages are received in order in 1-to-1 one-way messaging scenario
void testMsgOrder()
{
    const int N = 1000;
    PolyM::Queue queue;

    auto sender = [](int count, PolyM::Queue& q)
    {
        for (int i = 0; i < count; ++i)
            q.put(PolyM::Msg(i));
    };

    auto receiver = [](int count, PolyM::Queue& q)
    {
        for (int i = 0; i < count; ++i)
        {
            auto m = q.get();
            TEST_EQUALS(m->getMsgId(), i);
        }
    };

    std::thread t1(sender, N, std::ref(queue));
    std::thread t2(receiver, N, std::ref(queue));
    t1.join();
    t2.join();
}

// Test that messages are received in order in 2-to-1 one-way messaging scenario
void test2To1MsgOrder()
{
    const int N = 1000;
    PolyM::Queue queue;

    auto sender = [](int msgId, int count, PolyM::Queue& q)
    {
        for (int i = 0; i < count; ++i)
            q.put(PolyM::DataMsg<int>(msgId, i));
    };

    auto receiver = [](int count, PolyM::Queue& q)
    {
        int expectedData1 = 0;
        int expectedData2 = 0;

        for (int i = 0; i < count; ++i)
        {
            auto m = q.get();
            auto& dm = dynamic_cast<PolyM::DataMsg<int>&>(*m);

            if (dm.getMsgId() == 1)
            {
                TEST_EQUALS(dm.getPayload(), expectedData1);
                ++expectedData1;
            }
            else if (dm.getMsgId() == 2)
            {
                TEST_EQUALS(dm.getPayload(), expectedData2);
                ++expectedData2;
            }
            else
            {
                TEST_FAIL("Unexpected message id");
            }
        }
    };

    std::thread t1(sender, 1, N, std::ref(queue));
    std::thread t2(sender, 2, N, std::ref(queue));
    std::thread t3(receiver, 2 * N, std::ref(queue));
    t1.join();
    t2.join();
    t3.join();
}

// Test putting DataMsg through the queue
void testDataMsg()
{
    PolyM::Queue q;
    q.put(PolyM::DataMsg<std::string>(42, "foo"));
    auto m = q.get();
    auto& dm = dynamic_cast<PolyM::DataMsg<std::string>&>(*m);
    TEST_EQUALS(dm.getMsgId(), 42);
    TEST_EQUALS(dm.getPayload(), std::string("foo"));
    // Test modifying the payload data
    dm.getPayload() += "bar";
    TEST_EQUALS(dm.getPayload(), std::string("foobar"));
}

// Test timeout when getting message from the queue
void testReceiveTimeout()
{
    PolyM::Queue q;

    // Test first with a Msg in the queue that specifying timeout for get() doesn't have an effect
    q.put(PolyM::Msg(1));

    auto start = std::chrono::steady_clock::now();
    auto m = q.get(10);
    auto end = std::chrono::steady_clock::now();
    auto dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    TEST_EQUALS(m->getMsgId(), 1);
    //TEST_LESS_THAN(dur, 5LL);

    // Then test with empty queue
    start = std::chrono::steady_clock::now();
    auto m2 = q.get(10);
    end = std::chrono::steady_clock::now();
    dur = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    TEST_EQUALS(m2.get(), (PolyM::Msg*) nullptr);
    //TEST_LESS_THAN(5LL, dur);
    //TEST_LESS_THAN(dur, 15LL);
}

// Test tryGet() method
void testTryGet()
{
    PolyM::Queue q;

    auto m1 = q.tryGet();
    TEST_EQUALS(m1.get(), (PolyM::Msg*) nullptr);

    q.put(PolyM::Msg(1));

    auto m2 = q.tryGet();
    TEST_NOT_EQUALS(m2.get(), (PolyM::Msg*) nullptr);
}

// Test 2-to-1 request-response scenario
void testRequestResponse()
{
    const int N = 1000;

    PolyM::Queue queue;

    auto requester1 = [](int count, PolyM::Queue& q)
    {
        for (int i = 0; i < count; ++i)
        {
            TEST_EQUALS(q.request(PolyM::Msg(i))->getMsgId(), i + count);
        }
    };

    auto requester2 = [](int count, PolyM::Queue& q)
    {
        for (int i = 0; i < count; ++i)
        {
            TEST_EQUALS(q.request(PolyM::Msg(i + 2 * count))->getMsgId(), i + 3 * count);
        }
    };

    auto responder = [](int count, PolyM::Queue& q)
    {
        for (int i = 0; i < 2 * count; ++i)
        {
            auto m = q.get();
            q.respondTo(m->getUniqueId(), PolyM::Msg(m->getMsgId() + count));
        }
    };

    std::thread t1(requester1, N, std::ref(queue));
    std::thread t2(requester2, N, std::ref(queue));
    std::thread t3(responder, N, std::ref(queue));
    t1.join();
    t2.join();
    t3.join();
}

void testRequestExpired()
{
    PolyM::Queue queue;
    auto m1 = queue.request(PolyM::Msg(42), 50);
    TEST_EQUALS(m1.get(), (PolyM::Msg*) nullptr);

    auto m2 = queue.get();
    TEST_EQUALS(m2->getMsgId(), 42);
}

int main()
{
    // Statically assert that messages can't be copied or moved
    static_assert(!std::is_move_constructible<PolyM::Msg>::value, "Msg can't be copyable");
    static_assert(!std::is_move_assignable<PolyM::Msg>::value, "Msg can't be copyable");
    static_assert(!std::is_move_constructible<PolyM::DataMsg<int>>::value, "DataMsg can't be copyable");
    static_assert(!std::is_move_assignable<PolyM::DataMsg<int>>::value, "DataMsg can't be copyable");

    Tester tester("Test PolyM");
    tester.addTest(testMsgUID, "Test MsgUID generation");
    tester.addTest(testMsgOrder, "Test 1-to-1 message order");
    tester.addTest(test2To1MsgOrder, "Test 2-to-1 message order");
    tester.addTest(testDataMsg, "Test DataMsg");
    tester.addTest(testReceiveTimeout, "Test receive timeout");
    tester.addTest(testTryGet, "Test tryGet");
    tester.addTest(testRequestResponse, "Test 2-to-1 request-response");
    tester.addTest(testRequestExpired, "Test request() timeout");
    tester.runTests();
}
