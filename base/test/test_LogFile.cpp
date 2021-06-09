#include <gtest/gtest.h>
#include "../LogFile.h"
#include "../Timestamp.h"
using PROJECT_NAME::LogFile;
using PROJECT_NAME::Timestamp;

int main(int argc, char* argv[])
{
    testing::InitGoogleTest();
    auto ret = RUN_ALL_TESTS();
    getchar();
}

TEST(MyFILE, write)
{
    auto cur = Timestamp::now();
    auto time = cur.format(false);
    const auto fileName = "/home/owen/myFile-test-" + time + ".log";

    const char* context = "this is a test.\n";
    const auto len = strlen(context);
    {
        LogFile::MyFile file(fileName);
        file.write(context, len);
        file.flush();
    }
    auto f= fopen(fileName.c_str(), "r");
    EXPECT_NE(f, nullptr);
    char buffer[32];
    bzero(buffer, sizeof buffer);
    auto ret = fread(buffer, sizeof(char), sizeof buffer, f);
    EXPECT_EQ(ret, len);
    EXPECT_EQ(strcmp(buffer, context), 0);

    {
        LogFile::MyFile file(fileName);
        file.write(context, len);
        EXPECT_EQ(file.bytesWrite(), len);
    }

    bzero(buffer, sizeof buffer);
    ret = fread(buffer, sizeof(char), sizeof buffer, f);
    EXPECT_EQ(ret, len);
    EXPECT_EQ(strcmp(buffer, context), 0);
}

TEST(LogFile, write)
{
    // auto cur = Timestamp::now();
    // auto time = cur.format(false);
    const auto baseName = "/home/owen/LogFile-test-";
    const auto fileName = baseName + Timestamp::now().format(false) + ".log";
    const std::string context = "logFileTest\n";

    LogFile logFile(baseName, 1000 * 1000 * 500);
    for(int i = 0; i < 100; ++i)
    {
        logFile.write(context.c_str(), context.size());
    }

    logFile.flush();
    // std::cout << "fileName is: " << fileName << std::endl;

    auto f = fopen(fileName.c_str(), "r");
    EXPECT_NE(f, nullptr);
    if (f)
    {
        char buffer[2048 * 10];
        auto ret = fread(buffer, sizeof(char), sizeof buffer, f);
        EXPECT_EQ(ret, context.size() * 100);
        buffer[context.length()] = '\0';
        EXPECT_EQ(strcmp(context.c_str(), buffer), 0);
    }
}