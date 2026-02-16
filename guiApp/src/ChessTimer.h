#pragma once
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <string>
#include <sstream>
#include <iomanip>

inline std::string formatTimeMMSS(std::chrono::milliseconds ms) {
    if (ms.count() < 0)
        ms = std::chrono::milliseconds(0);

    auto totalSeconds = std::chrono::duration_cast<std::chrono::seconds>(ms).count();

    int minutes = static_cast<int>(totalSeconds / 60);
    int seconds = static_cast<int>(totalSeconds % 60);

    std::ostringstream oss;
    oss << minutes << ":"
        << std::setw(2) << std::setfill('0') << seconds;

    return oss.str();
}

class ChessTimer {
    using clock = std::chrono::steady_clock;

    std::thread worker;
    mutable std::mutex mtx;
    std::condition_variable cv;

    bool running = false;
    bool stop = false;

    std::chrono::milliseconds remaining;
    clock::time_point lastStart;

    void threadFunc() {
        std::unique_lock<std::mutex> lock(mtx);

        while (!stop) {
            // Wait until timer is started
            cv.wait(lock, [&] { return running || stop; });
            if (stop) break;

            lastStart = clock::now();

            // Wait until pause or time runs out
            if (cv.wait_for(lock, remaining, [&] { return !running || stop; })) {
                // We woke up early (paused or stopped)
                auto now = clock::now();
                auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastStart);
                remaining -= elapsed;

                // Prevent negative drift
                if (remaining.count() < 0) remaining = std::chrono::milliseconds(0);
            }
            else {
                // Time expired naturally
                remaining = std::chrono::milliseconds(0);
                running = false;
            }
        }
    }

public:
    explicit ChessTimer(int minutes = 15)
        : remaining(std::chrono::minutes(minutes)) {
        worker = std::thread(&ChessTimer::threadFunc, this);
    }

    ~ChessTimer() {
        {
            std::lock_guard<std::mutex> lock(mtx);
            stop = true;
            running = false; // Ensure we don't block on the first wait
        }
        cv.notify_all();
        if (worker.joinable()) {
            worker.join();
        }
    }

    void start() {
        std::lock_guard<std::mutex> lock(mtx);
        if (!running && remaining.count() > 0) {
            running = true;
            // IMPORTANT: Reset lastStart here to now, otherwise there is a 
            // split-second race before the thread wakes up where getTime() 
            // might calculate using an old timestamp.
            lastStart = clock::now();
            cv.notify_all();
        }
    }

    void pause() {
        std::lock_guard<std::mutex> lock(mtx);
        if (running) {
            running = false;
            cv.notify_all();
        }
    }

    void reset(int minutes = 15) {
        std::lock_guard<std::mutex> lock(mtx);
        running = false;
        remaining = std::chrono::minutes(minutes);
        cv.notify_all(); // Wake up thread to reset logic if needed
    }

    // FIX: Calculate live time if running
    std::chrono::milliseconds getTime() const {
        std::lock_guard<std::mutex> lock(mtx);
        if (running) {
            auto now = clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastStart);
            auto currentRemaining = remaining - elapsed;
            return (currentRemaining.count() < 0) ? std::chrono::milliseconds(0) : currentRemaining;
        }
        return remaining;
    }

    bool isRunning() const {
        std::lock_guard<std::mutex> lock(mtx);
        return running;
    }

    std::string getFormattedTime() const {
        // Fix: Call getTime() once to avoid double locking and inconsistency
        return formatTimeMMSS(getTime());
    }

    bool isTimeUp() const {
        // Reuse getTime logic to ensure accuracy while running
        return getTime().count() <= 0;
    }
};