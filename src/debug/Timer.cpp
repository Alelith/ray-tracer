#include "Timer.hpp"
#include <iostream>
#include <iomanip>

Timer::Timer() : m_bRunning(false) {}

void Timer::start() {
    m_StartTime = std::chrono::high_resolution_clock::now();
    m_bRunning = true;
}

void Timer::stop() {
    m_EndTime = std::chrono::high_resolution_clock::now();
    m_bRunning = false;
}

double Timer::elapsedMilliseconds() const {
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;

    if (m_bRunning) {
        endTime = std::chrono::high_resolution_clock::now();
    } else {
        endTime = m_EndTime;
    }

    return std::chrono::duration_cast<std::chrono::milliseconds>(endTime - m_StartTime).count();
}

double Timer::elapsedSeconds() const {
    return elapsedMilliseconds() / 1000.0;
}

void Timer::printElapsed() const {
    long long totalSeconds = static_cast<long long>(elapsedSeconds());
    long long hours = totalSeconds / 3600;
    long long minutes = (totalSeconds % 3600) / 60;
    long long seconds = totalSeconds % 60;

    std::cerr << "Render completado en: " 
              << std::setfill('0') << std::setw(2) << hours << ":"
              << std::setfill('0') << std::setw(2) << minutes << ":"
              << std::setfill('0') << std::setw(2) << seconds << "\n";
}
