#include <gtest/gtest.h>
#include <random>

#include "Controllers/TimerController.h"

class TimerControllerTest : public ::testing::Test
{
public:
    TimerControllerTest() = default;

    ~TimerControllerTest() = default;

    void SetUp() override
    {
        
    }

    void TearDown() override
    {
        
    }

protected:
};

TEST_F(TimerControllerTest, IsDatePassed)
{
    TimerController::TimePoint_Type now = std::chrono::system_clock::now();
    TimerController::TimePoint_Type future = now + std::chrono::seconds(10);
    TimerController::TimePoint_Type past = now - std::chrono::seconds(10);

    EXPECT_FALSE(TimerController::IsDatePassed(future));
    EXPECT_TRUE(TimerController::IsDatePassed(past));
}

TEST_F(TimerControllerTest, ParseInteval)
{
    EXPECT_EQ(TimerController::ParseInteval("42s"), 42);
    EXPECT_EQ(TimerController::ParseInteval("42m"), 42 * 60);
    EXPECT_EQ(TimerController::ParseInteval("42h"), 42 * 60 * 60);
    EXPECT_EQ(TimerController::ParseInteval("42d"), 42 * 60 * 60 * 24);
    EXPECT_EQ(TimerController::ParseInteval("1d 2h 3m 4s"), 1 * 60 * 60 * 24 + 2 * 60 * 60 + 3 * 60 + 4);
    EXPECT_EQ(TimerController::ParseInteval("1d 2h"), 1 * 60 * 60 * 24 + 2 * 60 * 60);
    EXPECT_EQ(TimerController::ParseInteval("1d"), 1 * 60 * 60 * 24);
    EXPECT_EQ(TimerController::ParseInteval("2h 4s"), 2 * 60 * 60 + 4);
    EXPECT_EQ(TimerController::ParseInteval("2m 4s"), 2 * 60 + 4);

    EXPECT_THROW(TimerController::ParseInteval("42"), ParsingException);
    EXPECT_THROW(TimerController::ParseInteval("42x"), ParsingException);
    EXPECT_THROW(TimerController::ParseInteval("42s 42m"), ParsingException);
    EXPECT_THROW(TimerController::ParseInteval("42s 42m 42h"), ParsingException);
    EXPECT_THROW(TimerController::ParseInteval("42s 42m 42h 42d"), ParsingException);
    EXPECT_THROW(TimerController::ParseInteval("42s 42m 42h 42d 42x"), ParsingException);
}