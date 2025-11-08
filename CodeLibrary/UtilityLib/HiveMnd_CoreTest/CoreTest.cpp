#include <iostream>
#include "Core/TimeUtils.h"
#include "Core/FileUtils.h"
#include "Core/SystemUtils.h"
#include "Core/StringUtils.h"
#include "Core/PathUtils.h"
#include "Core/EnvUtils.h"
#include "Core/DebugUtils.h"
#include "Core/LogUtils.h"

using namespace HiveMnd::Core;

int main()
{
    std::cout << "=== HiveMnd Core Utility Test ===" << std::endl;

    // Initialize logging to file
    LogUtils::Initialize(true, "HiveMnd_Core_TestLog.txt");
    LogUtils::Info("=== Starting Core Utility Validation ===");

    // --------------------------------------------------------------
    // TIME UTILS TEST
    // --------------------------------------------------------------
    LogUtils::Info("Testing TimeUtils...");
    std::cout << "Timestamp: " << TimeUtils::GetTimestamp() << std::endl;
    std::cout << "Milliseconds: " << TimeUtils::GetMillisec() << std::endl;
    TimeUtils::SleepMs(200);
    std::cout << "Sleep test complete." << std::endl;

    // --------------------------------------------------------------
    // FILE UTILS TEST
    // --------------------------------------------------------------
    LogUtils::Info("Testing FileUtils...");
    std::string testPath = "CoreTestFile.txt";
    FileUtils::WriteText(testPath, "HiveMnd Core File Test!");
    std::string readBack = FileUtils::ReadText(testPath);
    std::cout << "File Read: " << readBack << std::endl;

    if (FileUtils::Exists(testPath))
        LogUtils::Info("FileUtils - File exists successfully.");

    // --------------------------------------------------------------
    // SYSTEM UTILS TEST
    // --------------------------------------------------------------
    LogUtils::Info("Testing SystemUtils...");
    std::cout << "User: " << SystemUtils::GetUserName() << std::endl;
    std::cout << "Computer: " << SystemUtils::GetComputerName() << std::endl;

    // --------------------------------------------------------------
    // STRING UTILS TEST
    // --------------------------------------------------------------
    LogUtils::Info("Testing StringUtils...");
    std::string dirty = "   HiveMnd Rocks!   ";
    std::cout << "Trimmed: [" << StringUtils::Trim(dirty) << "]" << std::endl;
    std::cout << "Upper: " << StringUtils::ToUpper("hivemnd") << std::endl;
    std::cout << "Joined: " << StringUtils::Join({ "Core", "Utils", "Test" }, '-') << std::endl;

    // --------------------------------------------------------------
    // PATH UTILS TEST
    // --------------------------------------------------------------
    LogUtils::Info("Testing PathUtils...");
    std::string joinedPath = PathUtils::Combine("C:/HiveMnd", "Logs", "output.txt");
    std::cout << "Joined path: " << joinedPath << std::endl;
    std::cout << "Base name: " << PathUtils::GetFileName(joinedPath) << std::endl;
    std::cout << "Extension: " << PathUtils::GetExtension(joinedPath) << std::endl;

    // --------------------------------------------------------------
    // ENVIRONMENT UTILS TEST
    // --------------------------------------------------------------
    LogUtils::Info("Testing EnvUtils...");
    std::cout << "OS Name: " << EnvUtils::GetOSName() << std::endl;
    std::cout << "CPU Count: " << EnvUtils::GetCPUCount() << std::endl;
    std::string pathVar = EnvUtils::GetEnvVar("PATH");
    std::cout << "PATH variable (truncated): " << pathVar.substr(0, 60) << "..." << std::endl;

    // --------------------------------------------------------------
    // DEBUG UTILS TEST
    // --------------------------------------------------------------
    LogUtils::Info("Testing DebugUtils...");
    DebugUtils::Trace("This is a debug trace test.");
    DebugUtils::Assert(true, "This assertion should pass!");
    std::vector<int> nums = { 10, 20, 30, 40 };
    DebugUtils::PrintVector(nums);

    int testData[4] = { 1, 2, 3, 4 };
    DebugUtils::HexDump(testData, sizeof(testData));

    // --------------------------------------------------------------
    // LOG UTILS TEST
    // --------------------------------------------------------------
    LogUtils::Info("Testing LogUtils levels...");
    LogUtils::Info("Info message example.");
    LogUtils::Warning("Warning message example.");
    LogUtils::Error("Error message example.");
    LogUtils::Debug("Debug message example.");

    // --------------------------------------------------------------
    // CLEANUP
    // --------------------------------------------------------------
    LogUtils::Info("Core test complete. Shutting down...");
    LogUtils::Shutdown();

    std::cout << "\n=== HiveMnd Core Utility Test Complete ===" << std::endl;
    return 0;
}
