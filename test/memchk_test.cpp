///////////////////////////////////////////////////////
// SPDX-License-Identifier: MIT
// https://github.com/axelriet/memchk
// Copyright © Axel Rietschin.  All rights reserved.
//

#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

typedef std::chrono::high_resolution_clock Clock;

#include "../src/memchk.c"

namespace memchk_test
{
    TEST_CLASS(memchk_test)
    {
    public:

        TEST_METHOD(Test1)
        {
            Assert::AreEqual(0, memchk(nullptr, 0, 0));
            Assert::AreEqual(0, memchk((void*) "123", 0, 0));
        }

        TEST_METHOD(Test2)
        {
            try
            {
                #pragma warning(suppress: 6387) // _Param(1) could be '0'
                Assert::AreEqual(0, memchk(nullptr, 0, 1));
                Assert::Fail(L"Should have crashed.");
            }
            catch (...)
            {
                __noop; // Note: catching AV's requires /EHa
            }
        }

        TEST_METHOD(Test3)
        {
            constexpr int size = 128; // Even and >= 64
            auto buf = std::make_unique<char[]>(size + 1);

            buf[size] = 'X';

            for (size_t i = 0; i <= size / 2; ++i)
            {
                for (size_t j = 0; j <= size / 2; ++j)
                {
                    memset(buf.get(), 0, size);
                    memset(buf.get() + i, 'A', j);

                    Assert::AreEqual(0, memchk(buf.get() + i, 'A', j));

                    if (j > 0)
                    {
                        buf[i + j - 1] = 'B';
                        Assert::AreEqual(-1, memchk(buf.get() + i, 'A', j));
                    }
                }
            }

            Assert::AreEqual('X', buf[size]);
            Assert::AreEqual('B', buf[size - 1]);
            Assert::AreEqual('A', buf[size - 2]);
        }

        TEST_METHOD(Test4)
        {
            constexpr int size = 1024 * 1024;
            auto buf = std::make_unique<char[]>(size);

            memset(buf.get(), 0, size);

            auto t1 = Clock::now();

            //
            // Test the 1MB block 1024 times.
            //

            for (int x = 0; x < 1024; ++x)
            {
                Assert::AreEqual(0, memchk(buf.get(), 0, size));
            }

            auto t2 = Clock::now();
            auto elapsedTime = t2 - t1;

            std::stringstream s;
            s << "Duration: ~" << std::to_string(elapsedTime.count() / 1000000.0) << " ms";

            Logger::WriteMessage(s.str().c_str());
        }
    };
}
