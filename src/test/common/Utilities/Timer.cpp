/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Timer.h"
#include "gtest/gtest.h"

#include <thread>

TEST(AcoreTime, Now)
{
    auto before = std::chrono::steady_clock::now();
    auto now = Acore::Time::Now();
    auto after = std::chrono::steady_clock::now();
    EXPECT_GE(now, before);
    EXPECT_LE(now, after);
}

TEST(AcoreTime, Elapsed)
{
    auto start = Acore::Time::Now();
    std::this_thread::sleep_for(50ms);
    auto elapsed = Acore::Time::Elapsed(start);
    EXPECT_GE(elapsed, 50ms);
}

TEST(AcoreTime, HasElapsedTrue)
{
    auto start = Acore::Time::Now();
    std::this_thread::sleep_for(50ms);
    EXPECT_TRUE(Acore::Time::HasElapsed(start, 25ms));
}

TEST(AcoreTime, HasElapsedFalse)
{
    auto start = Acore::Time::Now();
    EXPECT_FALSE(Acore::Time::HasElapsed(start, 10s));
}

TEST(AcoreTime, HasElapsedWithSeconds)
{
    auto start = Acore::Time::Now();
    EXPECT_FALSE(Acore::Time::HasElapsed(start, 1s));
}

TEST(AcoreTime, HasElapsedWithMicroseconds)
{
    auto start = Acore::Time::Now();
    std::this_thread::sleep_for(100us);
    EXPECT_TRUE(Acore::Time::HasElapsed(start, Microseconds(1)));
}
