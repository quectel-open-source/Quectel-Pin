#pragma once
#include <thread>    
#include <vector>
//RAII ��������,��ֹ�����˳����߳�û��join���±���
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
