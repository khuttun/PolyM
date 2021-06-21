#pragma once

#include <cstdlib>
#include <iostream>
#include <functional>
#include <utility>
#include <string>
#include <vector>

// This file contains an implementation for *very* simple testing framework.

// Use these macros in your test functions to test for various things.
// The test program will print an error message and exit if the tests fail.

/** Test that boolean condition a is true */
#define TEST(a) test((a), #a, __FILE__, __LINE__);
/** Test that a == b */
#define TEST_EQUALS(a, b) testEquals((a), (b), __FILE__, __LINE__);
/** Test that a != b */
#define TEST_NOT_EQUALS(a, b) testNotEquals((a), (b), __FILE__, __LINE__);
/** Test that a < b */
#define TEST_LESS_THAN(a, b) testLessThan((a), (b), __FILE__, __LINE__);
/** Fail the test with error message msg */
#define TEST_FAIL(msg) testFail((msg), __FILE__, __LINE__);

void test(bool a, const char* exp, const char* file, int line)
{
    if (!a)
    {
        std::cout << file << ":" << line << ": TEST FAILED: " << exp << std::endl;
        exit(1);
    }
}

template <class C> void testEquals(C a, C b, const char* file, int line)
{
    if (a != b)
    {
        std::cout << file << ":" << line << ": TEST FAILED: " << a << " == " << b << std::endl;
        exit(1);
    }
}

template <class C> void testNotEquals(C a, C b, const char* file, int line)
{
    if (a == b)
    {
        std::cout << file << ":" << line << ": TEST FAILED: " << a << " != " << b << std::endl;
        exit(1);
    }
}

template <class C> void testLessThan(C a, C b, const char* file, int line)
{
    if (a >= b)
    {
        std::cout << file << ":" << line << ": TEST FAILED: " << a << " < " << b << std::endl;
        exit(1);
    }
}

void testFail(const char* msg, const char* file, int line)
{
    std::cout << file << ":" << line << ": TEST FAILED: " << msg << std::endl;
    exit(1);
}

/**
 * Tester is a container for tests.
 * Use addTest() to add new test.
 * Use runTests() to run all added tests.
 */
class Tester
{
public:
    /**
     * Construct a Tester.
     *
     * @param name Name for the Tester instance. Will be printed when running tests.
     */
    Tester(const std::string& name)
      : name_(name), tests_()
    {
    }

    /**
     * Add test.
     *
     * @param testFunc Function containing the test.
     * @param testDesc Description for the test. Will be printed when running the test.
     */
    void addTest(const std::function<void()>& testFunc, const std::string& testDesc)
    {
        tests_.push_back(std::make_pair(testFunc, testDesc));
    }

    /**
     * Run all added tests.
     */
    void runTests() const
    {
        std::cout << name_ << ": Running " << tests_.size() << " tests" << std::endl;
        for (auto& test : tests_)
        {
            std::cout << test.second << "... ";
            test.first();
            std::cout << "OK!" << std::endl;
        }
    }

private:
    std::string name_;
    std::vector<std::pair<std::function<void()>, std::string>> tests_;
};
