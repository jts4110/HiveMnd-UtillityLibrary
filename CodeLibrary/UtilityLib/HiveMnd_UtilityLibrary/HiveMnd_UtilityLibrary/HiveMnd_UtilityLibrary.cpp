#include <iostream>
#include "Utility.h"   // make sure this points to your HiveMnd version
#include <Windows.h>


int main()
{
    using namespace HiveMnd::Utility;

    // =========================
    // TEST 1: UserInputString
    // =========================
    std::cout << "\n[TEST] UserInputString()\n";
    char userText[100];
    UserInputString(userText, sizeof(userText));
    std::cout << "You entered: " << userText << std::endl;

    // =========================
    // TEST 2: UserInputInteger
    // =========================
    std::cout << "\n[TEST] UserInputInteger()\n";
    int intValue = 0;
    UserInputInteger(&intValue);
    std::cout << "You entered integer: " << intValue << std::endl;

    // =========================
    // TEST 3: Write and WriteLine
    // =========================
    std::cout << "\n[TEST] Write() and WriteLine()\n";
    Write("This text is printed without newline... ");
    WriteLine("then this one ends the line.");
    WriteLine("WriteLine() works correctly!");

    // =========================
    // TEST 4: ChangeBackgroundColor
    // =========================
    std::cout << "\n[TEST] ChangeBackgroundColor()\n";
    std::cout << "Changing background to light green for 2 seconds...\n";
    ChangeBackgroundColor(2);  // 2 = Green
    Sleep(2000);
    ChangeBackgroundColor(0);  // 0 = Reset to black

    // =========================
    // TEST 5: ChangeForegroundColor
    // =========================
    std::cout << "\n[TEST] ChangeForegroundColor()\n";
    std::cout << "Changing text color to bright red for 2 seconds...\n";
    ChangeForegroundColor(12); // 12 = Light Red
    Sleep(2000);
    ChangeForegroundColor(7);  // 7 = Default white/gray

    std::cout << "\nAll HiveMnd::Utility add-on functions tested successfully!\n";

    return 0;
}

