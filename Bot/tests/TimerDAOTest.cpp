#include <gtest/gtest.h>
#include <random>

#include "DAO/TimerDAO.h"

class TimerDAOTest : public ::testing::Test
{
public:
    TimerDAOTest() = default;

    ~TimerDAOTest() = default;

    void SetUp() override
    {
        dao = TimerDAO();
        gen = std::mt19937(rd());
    }

    void TearDown() override
    {
        // Remove directories
        std::filesystem::remove_all("data/timers");
    }

    int randInt(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(gen);
    }

    unsigned randUInt(unsigned min, unsigned max)
    {
        std::uniform_int_distribution<unsigned> dist(min, max);
        return dist(gen);
    }

    void expectSize(size_t size)
    {
        EXPECT_EQ(dao.findAll().size(), size);
        EXPECT_EQ(dao.getDataMap().size(), size);
    }

    void expectEmpty()
    {
        expectSize(0);
    }

    TimerDTO createMockTimerDTO(const std::string& id, const std::string& message)
    {
        TimerDTO timer = TimerDTO();
            timer.setName(id);
            timer.setChannel(dpp::snowflake(1234567890));
            timer.setMessage(message);
            timer.setInterval(60);
            auto now = std::chrono::system_clock::now();
            timer.setStart(now);
            timer.setEnd(now + std::chrono::hours(3));
        return timer;
    }

    TimerDTO createMockTimerDTO(const std::string& id)
    {
        return createMockTimerDTO(id, "Hey! This is a simple message.");
    }

    void addMockTimerDTO(const std::string& id, const std::string& message)
    {
        dao.add(id, createMockTimerDTO(id, message));
    }

    void addMockTimerDTO(const std::string& id)
    {
        dao.add(id, createMockTimerDTO(id));
    }

    void checkFileContent(const std::string& id, const TimerDTO& timer)
    {
        std::ifstream file("data/timers/" + id + ".txt");

        std::string name;
        dpp::snowflake channel;
        std::string message;
        uint64_t interval;
        int64_t start;
        int64_t end;
        std::string line;

        // Name
        std::getline(file, name);
        // Channel
        std::getline(file, line);
        channel = dpp::snowflake(std::stoull(line));
        // Interval
        std::getline(file, line);
        interval = std::stoull(line);
        // Message
        std::getline(file, message);
        // Start
        std::getline(file, line);
        start = std::stoll(line);
        // End
        std::getline(file, line);
        end = std::stoll(line);

        EXPECT_EQ(name, timer.getName());
        EXPECT_EQ(channel, timer.getChannel());
        EXPECT_EQ(interval, timer.getInterval());
        EXPECT_EQ(message, timer.getMessage());
    }

protected:
    TimerDAO dao;
    std::random_device rd;
    std::mt19937 gen;
};

TEST_F(TimerDAOTest, add)
{
    expectEmpty();

    size_t lastNumber = 0;

    for (size_t i = 0; i < 100; ++i)
    {
        lastNumber += randUInt(1, 100); // lastNumber is always < 100 * 100 = 10000 < size_t max
        std::string id = std::to_string(lastNumber);
        auto mockTimer = createMockTimerDTO(id);
        EXPECT_NO_THROW(dao.add(id, mockTimer));
        EXPECT_EQ(dao.findOne(id).getName(), mockTimer.getName());
        
        checkFileContent(id, mockTimer);
        expectSize(i + 1);
    }

    EXPECT_NO_THROW(addMockTimerDTO("some timer"));
    checkFileContent("some timer", createMockTimerDTO("some timer"));
    EXPECT_THROW(dao.add("some timer", createMockTimerDTO("never used name")), DAOIDAlreadyExists);
    EXPECT_THROW(dao.add("", createMockTimerDTO("never used name bis")), DAOBadID);
}

TEST_F(TimerDAOTest, update)
{
    auto timer = createMockTimerDTO("1", "A message");
    dao.add("1", timer);

    expectSize(1);
    EXPECT_EQ(dao.findOne("1").getMessage(), "A message");
    checkFileContent("1", timer);
    
    timer.setMessage("Updated message");

    EXPECT_NO_THROW(dao.update("1", timer));
    expectSize(1);
    EXPECT_EQ(dao.findOne("1").getMessage(), "Updated message");
    checkFileContent("1", timer);

    EXPECT_THROW(dao.update("2", timer), DAOIDNotFound);
    EXPECT_THROW(dao.update("", timer), DAOBadID);
}

TEST_F(TimerDAOTest, deleteByID)
{
    auto timer = createMockTimerDTO("1", "A message");
    dao.add("1", timer);

    expectSize(1);
    EXPECT_EQ(dao.findOne("1").getMessage(), "A message");
    checkFileContent("1", timer);

    EXPECT_NO_THROW(dao.deleteByID("1"));
    expectEmpty();
    EXPECT_FALSE(std::filesystem::exists("data/timers/1.txt"));

    EXPECT_THROW(dao.deleteByID("1"), DAOIDNotFound);
    EXPECT_THROW(dao.deleteByID(""), DAOBadID);
}

TEST_F(TimerDAOTest, findOne)
{
    auto timer = createMockTimerDTO("1", "A message");
    dao.add("1", timer);

    expectSize(1);
    auto retrievedTimer = dao.findOne("1");
    EXPECT_EQ(retrievedTimer.getMessage(), "A message");
    checkFileContent("1", retrievedTimer);

    EXPECT_THROW(dao.findOne("2"), DAOIDNotFound);
    EXPECT_THROW(dao.findOne(""), DAOBadID);
}

TEST_F(TimerDAOTest, findAll)
{
    expectEmpty();

    for (size_t i = 0; i < 100; ++i)
    {
        std::string id = std::to_string(i);
        auto mockTimer = createMockTimerDTO(id, "Hey! This is a simple message. My id is " + id);
        dao.add(id, mockTimer);
        expectSize(i + 1);
    }

    auto timers = dao.findAll();
    EXPECT_EQ(timers.size(), 100);

    // findAll is not expected to give the elements in order, so we sort them by name in order to compare
    std::sort(
        timers.begin(),
        timers.end(),
        [](const TimerDTO& a, const TimerDTO& b) {
            return std::stoull(a.getName()) < std::stoull(b.getName());
        }
    );
    for (size_t i = 0; i < 100; ++i)
    {
        std::string id = std::to_string(i);
        EXPECT_EQ(timers[i].getName(), id);
        EXPECT_EQ(timers[i].getMessage(), "Hey! This is a simple message. My id is " + id);
        checkFileContent(id, timers[i]);
    }
}

TEST_F(TimerDAOTest, idExists)
{
    expectEmpty();

    for (size_t i = 0; i < 100; ++i)
    {
        std::string id = std::to_string(i);
        auto mockTimer = createMockTimerDTO(id);
        dao.add(id, mockTimer);
        expectSize(i + 1);
    }

    for (size_t i = 0; i < 100; ++i)
    {
        std::string id = std::to_string(i);
        EXPECT_TRUE(dao.idExists(id));
    }

    EXPECT_FALSE(dao.idExists("100"));
    EXPECT_FALSE(dao.idExists("blahblah"));
    EXPECT_FALSE(dao.idExists(""));
}

TEST_F(TimerDAOTest, isIDValid)
{
    EXPECT_FALSE(dao.isIDValid(""));
    EXPECT_TRUE(dao.isIDValid(" "));
    EXPECT_TRUE(dao.isIDValid("1"));
    EXPECT_TRUE(dao.isIDValid("1234567890"));
    EXPECT_TRUE(dao.isIDValid("blahblah"));
    EXPECT_TRUE(dao.isIDValid("a"));
    EXPECT_TRUE(dao.isIDValid("a1"));
    EXPECT_TRUE(dao.isIDValid("a1b2c3d4e5f6g7h8i9j0"));
}

TEST_F(TimerDAOTest, loadTimers)
{
    expectEmpty();

    // Creating a few timers
    for (size_t i = 0; i < 100; ++i)
    {
        std::string id = std::to_string(i);
        auto mockTimer = createMockTimerDTO(id, "Hey! This is a simple message. My id is " + id);
        dao.add(id, mockTimer);
        expectSize(i + 1);
        checkFileContent(id, mockTimer);
    }

    // Times should be saved. Let's create a new DAO and load the timers again.
    dao = TimerDAO();
    expectEmpty();

    dao.loadTimers();
    expectSize(100);

    // Sorting for the same reason as in the findAll test
    auto timers = dao.findAll();
    std::sort(
        timers.begin(),
        timers.end(),
        [](const TimerDTO& a, const TimerDTO& b) {
            return std::stoull(a.getName()) < std::stoull(b.getName());
        }
    );

    // Now we should find the same values as before, with the same generation algorithm
    for (size_t i = 0; i < 100; ++i)
    {
        std::string id = std::to_string(i);
        EXPECT_EQ(timers[i].getName(), id);
        EXPECT_EQ(timers[i].getMessage(), "Hey! This is a simple message. My id is " + id);
        checkFileContent(id, timers[i]);
    }
}