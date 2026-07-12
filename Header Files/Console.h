#pragma once

#include "Config.h"

namespace FNLauncher {

void setupConsole();
void clearScreen();
void printHeader();
void printStatus(const Config& cfg, bool backendUp);
void printMenu();

int readChoice(int minVal, int maxVal);
int readPort(int current);
std::wstring readLine();
void wait(int seconds);

}
