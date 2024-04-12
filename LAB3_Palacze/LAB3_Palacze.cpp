#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore>
#include <vector>
#include <cstdlib> // Do czyszczenia konsoli

const int k = 4; // Liczba palaczy
const int l = 1; // Liczba ubijaczy
const int m = 1; // Liczba pudełek zapałek

std::mutex screen_mutex; // Semafor do wykluczania dostępu do ekranu

std::counting_semaphore<l> tamper(l); // Semafor dla ubijaczy
std::counting_semaphore<m> matches(m); // Semafor dla pudełek zapałek

enum ActionType {
    ACTION_NONE,
    ACTION_TAMPER,
    ACTION_LIGHT,
    ACTION_SMOKE
};

std::string actionToString(ActionType action) {
    switch (action) {
    case ACTION_TAMPER:
        return "Ubijam ";
    case ACTION_LIGHT:
        return "Zapalam";
    case ACTION_SMOKE:
        return "Pale   ";
    default:
        return "Czekam ";
    }
}

void clearConsole() {
#ifdef _WIN32
    std::system("cls");
#endif
}

void printTable(std::vector<ActionType>& actions) {
    std::lock_guard<std::mutex> lock(screen_mutex);

    clearConsole(); // Czyszczenie konsoli

    std::cout << "+-------------+-------------+-------------+------------+" << std::endl;
    std::cout << "| Palacz #1   | Palacz #2   | Palacz #3   | Palacz #4  |" << std::endl;
    std::cout << "+-------------+-------------+-------------+------------+" << std::endl;
    std::cout << "| " << actionToString(actions[0]) << "     | " << actionToString(actions[1]) << "     | " << actionToString(actions[2]) << "     | "  << actionToString(actions[3])<< "    |" << std::endl;
    std::cout << "+-------------+-------------+-------------+----------- +" << std::endl;
}

void smoker(int id, std::vector<ActionType>& actions) {
    while (true) {
        // Żądanie ubijacza
        tamper.acquire();

        actions[id] = ACTION_TAMPER;
        printTable(actions);

        // Ubijanie
        std::this_thread::sleep_for(std::chrono::seconds(3));

        // Zwrot ubijacza
        tamper.release();

        // Żądanie pudełka zapałek
        matches.acquire();

        actions[id] = ACTION_LIGHT;
        printTable(actions);

        // Zapalenie fajki
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Zwrot pudełka zapałek
        matches.release();

        // Palenie fajki
        actions[id] = ACTION_SMOKE;
        printTable(actions);
        std::this_thread::sleep_for(std::chrono::seconds(4));

        actions[id] = ACTION_NONE;
        printTable(actions);
    }
}

int main() {
    std::vector<ActionType> actions(k, ACTION_NONE);
    std::thread smokers[k];

    for (int i = 0; i < k; i++) {
        smokers[i] = std::thread(smoker, i, std::ref(actions));
    }

    for (int i = 0; i < k; i++) {
        smokers[i].join();
    }

    return 0;
}