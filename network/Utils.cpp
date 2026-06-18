//
// Created by  on 11/30/2025.
//

#include "Utils.h"

#include <chrono>

using namespace std;
using namespace chrono;

double GetTimeUtils() {
    auto now = system_clock::now();
    auto duration = now.time_since_epoch();
    double seconds = std::chrono::duration<double>(duration).count();
    return seconds - 1778427250;
}