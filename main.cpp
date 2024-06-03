#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <memory>
#include <atomic>

std::mutex mtx;

void incrementar(int & total, int inc) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
    std::unique_lock ul(mtx);
//    std::lock_guard lg(mtx);
//    mtx.lock();
    total += inc;

//    mtx.unlock();
}

void incrementar_atomic(std::atomic<int>& total, int inc) {
    total += inc;
}

int incrementar_total(int n, int inc) {
    std::atomic<int> total = 0;
    std::vector<std::jthread> vt;
    vt.reserve(n);
    for(int i = 0; i < n; ++i) {
//        vt.emplace_back(incrementar, std::ref(total), inc);
//        vt.emplace_back([&]{ total += inc; });
        vt.emplace_back(incrementar_atomic,std::ref(total), inc);
    }
    for (auto & t: vt) {
        t.join();
    }
    return total;
}

void ejercicio_1() {
    int total = incrementar_total(20, 1);
    std::cout << total << std::endl;
}

void ejercicio_2() {
    int m = 100;
    int n = 20;
    std::vector<int> totales(m);
    for (int i = 0; i < m; ++i)
        totales[i] = incrementar_total(n, 1);
    for (const auto& item: totales)
        std::cout << item << " ";
}

class Guardia {
    std::mutex& mtx;
public:
    explicit Guardia(std::mutex &mtx)
        : mtx(mtx) {
        mtx.lock();
    }
    ~Guardia() {
        mtx.unlock();
    }
};

class Client {
    std::mutex mtx;
    // double balance = 0;
    std::atomic<double> balance = 0;
public:
    Client() = default;
    explicit Client(double initial_balance): balance(initial_balance) {}
    void add(double amount) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        // Guardia g(mtx); // std::lock_guard lg(mtx), std::unique_lock ul(mtx)
        balance += amount;
    }
    void substract(double amount) {
        // Guardia g(mtx);
        balance -= amount;
    }
    friend std::ostream& operator << (std::ostream& os, const Client& client) {
        os << client.balance;
        return os;
    }
};

void ejercicio_3() {
    Client amazon;
    int nd = 100;
    int nr = 50;
    std::vector<std::jthread> vt;
    vt.reserve(nd + nr);
    for (int i = 0; i < nd; ++i) {
        vt.emplace_back(&Client::add, &amazon, 1);
//        vt.emplace_back([&amazon] { amazon.add(1); });
    }

    for (int i = 0; i < nr; ++i) {
        vt.emplace_back(&Client::substract, &amazon, 1);
//        vt.emplace_back([&amazon] { amazon.substract(1); });
    }

    for (int i = 0; i < nd + nr; ++i) {
        vt[i].join();
    }
    std::cout << amazon << std::endl;
}

int main() {
//    ejercicio_1();
//    ejercicio_2();
    ejercicio_3();
    return 0;
}
