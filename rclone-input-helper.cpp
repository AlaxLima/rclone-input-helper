#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <stdio.h>
#include <chrono>
#include <thread>
#include <conio.h>
#include <limits>
#include <vector>
#include <sstream>
#include <cctype>
#include <Windows.h>
#include <algorithm>
#include <fstream>

bool loop;
std::string tabLength = "    ";

// presence bool is called on top due to further use of functions outside int main
bool rclonePresence = 0;
bool winfspPresence = 0;

std::string execCommand(const std::string &cmd)
{
    std::string result;
    char buffer[128];
    FILE *pipe = _popen(cmd.c_str(), "r");
    if (!pipe)
        return "ERROR";

    while (fgets(buffer, sizeof(buffer), pipe) != nullptr)
    {
        result += buffer;
    }

    _pclose(pipe);
    return result;
}

void millisecsDelay(int millisecs)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(millisecs));
}

std::vector<std::string> splitByLine(const std::string &input)
{
    std::vector<std::string> lines;
    std::stringstream ss(input);
    std::string line;
    while (std::getline(ss, line))
    {
        if (!line.empty())
            lines.push_back(line);
    }
    return lines;
}

void disclaimer()
{
    std::string note = R"(
         **NOTICE**
 
    - This program is developed for basic personal use and does not modify the rclone software.
    - Even if program offers fail-safe mechanisms, it is not a replacement for Rclone config.
    - Program is currently limited in functionalities and inconsistent code structure. 
    - Would be happy to receive contributions. Thank you! <3
          )";
    std::cout << "\r" << note << "\n";
    std::cout << "\r" << "Press Enter to start using...";
    std::cin.ignore();
    system("cls");
}

bool check4rclone()
{
    std::string result = execCommand("rclone version");
    bool checkResult = result.find("rclone") != std::string::npos && result.find("v") != std::string::npos;
    if (checkResult)
    {
        std::cout << "\r" << "Rclone present." << "\n";
        disclaimer();
        return 1;
    }
    else
    {
        std::cout << "\r" << "Rclone not found. Please run this program in the same directory with rclone.exe" << "\n";
        return 0;
    }
}

bool check4WinFSP()
{
    const std::string winfspDLL =
        "C:\\Program Files (x86)\\WinFsp\\bin\\winfsp-x64.dll";

    bool result = std::filesystem::exists(winfspDLL);
    return result;
}

int provideOptions()
{
    std::cout << "\r\n"
              << "    - rclone basic input utility v.1.0 -    " << "\n";
    unsigned int corresWidth = 8;
    std::cout << std::left;
    std::cout << tabLength << std::setw(corresWidth) << "[1]" << "Run remote config" << "\n";
    std::cout << tabLength << std::setw(corresWidth) << "[2]" << "View remotes" << "\n";
    std::cout << tabLength << std::setw(corresWidth) << "[3]" << "Sync" << "\n";
    std::cout << tabLength << std::setw(corresWidth) << "[4]" << "Mount" << "\n";
    std::cout << tabLength << std::setw(corresWidth) << "[5]" << "View rclone version info" << "\n";
    std::cout << tabLength << std::setw(corresWidth) << "[6]" << "Terminate running tasks" << "\n";
    std::cout << tabLength << std::setw(corresWidth) << "[ESC]" << "Quit" << "\n";

    std::string ask4option = R"(Press the corresponding key to select)";

    if (!winfspPresence)
    {
        std::cout << "\nWarning: WinFSP not detected in machine.\n";
        std::cout << "Mounting functionality will be unavailable.\n\n";
    }

    std::cout << "\n"
              << ask4option << std::flush;
    char optionSelect = _getch();
    if (optionSelect == 27)
    {
        return 27;
    }
    else if (optionSelect >= '1' && optionSelect <= '7')
    {
        system("cls");
        return optionSelect - '0'; // Convert char to integer
    }
    else
    {
        return 0;
    }
}

void returnEnter()
{
    std::cout << "\n";
    std::cout << "\r" << "Press Enter to return to the option menu...";
    bool lock = 1;
    while (lock)
    {
        if (_kbhit())
        {
            char askedInput = _getch();
            if (askedInput == 13)
            {
                lock = 0;
            }
        }
    }
}

void cleanedBufferReturn()
{
    std::cout << "\r" << "Press enter to return to the option menu...";
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

// Remote names parsing and index building
std::vector<std::string> getRemoteList()
{
    std::string output = execCommand("rclone listremotes");
    return output == "ERROR" ? std::vector<std::string>{} : splitByLine(output);
}

void printIndexedList(const std::vector<std::string> &remotes)
{
    int index = 1;
    for (const auto &name : remotes)
    {
        std::cout << "\r[" << index++ << "] " << name << "\n";
    }
}

int selectRemotePagedIndex(const std::vector<std::string> &remotes)
{
    const int pageSize = 9;
    int totalPages = (remotes.size() + pageSize - 1) / pageSize;
    int currentPage = 0;
    while (true)
    {
        system("cls");
        std::cout << "-- Select a Remote (Page " << (currentPage + 1) << "/" << totalPages << ") --\n";

        int baseIndex = currentPage * pageSize;
        for (int i = 0; i < pageSize; ++i)
        {
            int remoteIndex = baseIndex + i;
            if (remoteIndex >= remotes.size())
                break;
            std::cout << "[" << (i + 1) << "] " << remotes[remoteIndex] << "\n";
        }
        std::cout << "\nUse < or > to move to next/previous page, 1-9 to select, or ESC to cancel.";
        char key = _getch();
        if (key == 27)
            return -1; // ESC
        else if (key == '<' || key == ',')
        {
            if (currentPage > 0)
                --currentPage;
        }
        else if (key == '>' || key == '.')
        {
            if (currentPage < totalPages - 1)
                ++currentPage;
        }
        else if (key >= '1' && key <= '9')
        {
            int offset = key - '1';
            int selectedIndex = baseIndex + offset;
            if (selectedIndex < remotes.size())
                return selectedIndex;
        }
    }
}

void remoteList()
{
    std::cout << "\n-- Remote list --\n";
    auto remotes = getRemoteList();
    if (remotes.empty())
    {
        std::cout << "\rNo remotes found.\n";
        return;
    }
    printIndexedList(remotes);
}

void remoteConfig()
{
    std::this_thread::sleep_for(std::chrono::microseconds(500));
    system("start cmd /c rclone config");
}

void viewRemote()
{
    remoteList();
    returnEnter();
}

// Detect currently used drive letters
std::vector<char> getUsedDriveLetters()
{
    std::vector<char> used;
    DWORD drives = GetLogicalDrives();
    for (int i = 0; i < 26; ++i)
    {
        if (drives & (1 << i))
        {
            used.push_back('A' + i);
        }
    }
    return used;
}

// Prompt for available drive letter (returns '\0' if cancelled)
char getAvailableDriveLetter()
{
    auto used = getUsedDriveLetters();
    std::cout << "\nDrive letters currently in use: ";
    for (char c : used)
        std::cout << c << ": ";
    std::cout << "\nPress a letter (A-Z) to mount, or ESC to cancel: ";
    while (true)
    {
        char key = _getch();
        if (key == 27)
            return '\0'; // ESC
        key = std::toupper(key);

        if (key >= 'A' && key <= 'Z')
        {
            if (std::find(used.begin(), used.end(), key) != used.end())
            {
                std::cout << "\rDrive " << key << ": is already in use. Try another: ";
            }
            else
            {
                std::cout << "\nUsing drive " << key << ":\\\n";
                return key;
            }
        }
    }
}

// Select a remote index (returns -1 if ESC)
int selectRemoteIndex(const std::vector<std::string> &remotes)
{
    return selectRemotePagedIndex(remotes);
}

void mount()
{
    if (!winfspPresence)
    {
        std::cout << "\nMounting is unavailable because WinFSP is not found.\n";
        std::cout << "\nTo use this function, please install WinFSP.\n";
        returnEnter();
        return;
    }

    auto remotes = getRemoteList();
    if (remotes.empty())
    {
        std::cout << "\rNo remotes found.\n";
        return;
    }
    int selectedIndex = selectRemoteIndex(remotes);
    if (selectedIndex == -1)
    {
        std::cout << "\nCancelled.\n";
        return;
    }
    std::string selectedRemote = remotes[selectedIndex];
    char driveLetter = getAvailableDriveLetter();
    if (driveLetter == '\0')
    {
        std::cout << "\nMount cancelled.\n";
        return;
    }
    std::string mountCommandJoint = "start /B cmd /c rclone mount " +
                                    selectedRemote + " " + driveLetter + ":\\ --vfs-cache-mode full > NUL 2>&1";
    system(mountCommandJoint.c_str());
    system("cls");
    std::cout << "\r" << selectedRemote << " (" << driveLetter << ":\\) has been mounted.\n\n";
    millisecsDelay(500);
    returnEnter();
}

void sync()
{
    auto remotes = getRemoteList();
    if (remotes.size() < 2)
    {
        std::cout << "\rYou must have at least 2 remotes to perform a sync.\n";
        return;
    }
    std::cout << "\n-- Select Source Remote --\n";
    int sourceIndex = selectRemoteIndex(remotes);
    if (sourceIndex == -1)
    {
        std::cout << "\nCancelled.\n";
        return;
    }
    std::cout << "\n-- Select Destination Remote --\n";
    int destIndex = selectRemoteIndex(remotes);
    if (destIndex == -1)
    {
        std::cout << "\nCancelled.\n";
        return;
    }
    std::string sourceName = remotes[sourceIndex];
    std::string destinationName = remotes[destIndex];
    std::string confirmMessage = R"(
    Run in background?
    Press ENTER to run in background
    Press ESC to cancel
    Press any other key to run in a new CMD window.
    )";
    std::cout << "\n"
              << confirmMessage;
    char choice = _getch();
    if (choice == 27) // ESC key
    {
        std::cout << "\rSync operation cancelled.\n";
        return;
    }
    else if (choice == 13) // Enter key
    {
        std::string syncCommandJoint = "start /B cmd /c rclone sync " + sourceName + " " + destinationName + "> NUL 2>&1";
        system(syncCommandJoint.c_str());
        std::cout << "\rOperation has started in background.\n";
    }
    else
    {
        std::string syncCommandJoint = "start cmd /c rclone sync " + sourceName + " " + destinationName + " -P";
        system(syncCommandJoint.c_str());
        std::cout << "\rOperation has started.\n";
    }

    returnEnter();
}

void ver()
{
    std::cout << "\n";
    system("rclone --version");
    returnEnter();
}

void taskTerminate()
{
    std::string confirmationMsg = R"(
    All running operations will be terminated. Make sure not to halt any important things.
    Press ENTER to confirm or ESC to cancel...
    )";
    std::cout << '\n'
              << confirmationMsg;
    while (true)
    {
        char input = _getch();
        if (input == 13)
        { // ENTER
            system("taskkill /IM rclone.exe /F > NUL 2>&1");
            std::cout << "\rAll rclone tasks have been terminated.\n";
            returnEnter();
            break;
        }
        else if (input == 27)
        { // ESC
            std::cout << "\rCancelled.\n";
            millisecsDelay(2000);
            break;
        }
    }
}

// input values corresponding to its functions
int optionExecution(int selectValue)
{
    switch (selectValue)
    {
    case 1:
        remoteConfig();
        millisecsDelay(3000);
        return 0;
    case 2:
        viewRemote();
        millisecsDelay(1000);
        return 0;
    case 3:
        sync();
        millisecsDelay(500);
        return 0;
    case 4:
        mount();
        millisecsDelay(500);
        return 0;
    case 5:
        ver();
        millisecsDelay(500);
        return 0;
    case 6:
        taskTerminate();
        millisecsDelay(500);
        return 0;
    case 27: // ESC key
        return -1;
    case 0:
        return 0;
    default:
        return -1;
    }
}

int main()
{
    loop = true;
    rclonePresence = check4rclone();
    if (rclonePresence)
    {
    }
    while (loop)
    {
        int returnedOption;
        if (rclonePresence)
        {
            returnedOption = provideOptions();
        }
        else
        {
            loop = false;
            millisecsDelay(2000);
        }
        int optionExecDone;
        optionExecDone = optionExecution(returnedOption);
        if (optionExecDone == -1)
        {
            loop = false;
        }
        else if (optionExecDone == 0)
        {
            system("cls");
        }
    }
    system("cls");
    std::cout << "\r\n"
              << "Exiting..." << "\n";
    millisecsDelay(1000);
    return 0;
}
