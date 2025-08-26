#pragma once
#include <thread>    
#include <vector>
//RAII 守卫对象,防止意外退出，线程没有join导致崩溃
class ThreadGuard {
public:
    explicit ThreadGuard(std::vector<std::thread>& threads)
        : m_threads(threads) {}

    ~ThreadGuard() {
        for (auto& t : m_threads) {
            if (t.joinable()) {
                t.join();
            }
        }
    }

private:
    std::vector<std::thread>& m_threads;
};
