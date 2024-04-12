#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore>

const int k = 3; // Liczba palaczy
const int l = 2; // Liczba ubijaczy
const int m = 3; // Liczba pudełek zapałek

std::mutex screen_mutex; // Semafor do wykluczania dostępu do ekranu

std::semaphore tamper(l); // Semafor dla ubijaczy
std::semaphore matches(m); // Semafor dla pudełek zapałek

void smoker(int id) {
    while (true) {
        // Żądanie ubijacza
        tamper.wait();

        // Ubijanie
        std::cout << "[Smoker " << id << "] Ubijam fajkę" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Zwrot ubijacza
        tamper.signal();

        // Żądanie pudełka zapałek
        matches.wait();

        // Zapalenie fajki
        std::cout << "[Smoker " << id << "] Zapalam fajkę" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Zwrot pudełka zapałek
        matches.signal();

        // Palenie fajki
        std::cout << "[Smoker " << id << "] Palę fajkę" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));

        std::cout << "[Smoker " << id << "] Skończyłem palić" << std::endl;
    }
}

int main() {
    std::thread smokers[k];

    for (int i = 0; i < k; i++) {
        smokers[i] = std::thread(smoker, i);
    }

    for (int i = 0; i < k; i++) {
        smokers[i].join();
    }

    return 0;
}