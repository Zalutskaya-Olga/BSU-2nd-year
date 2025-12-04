#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <cstdlib>
#include <vector>
#include <signal.h>
#include <iomanip>
#include <ctime>

using namespace std;

const int MAX_LEN = 20;
const int MAX_FILE = 100;

struct Msg {
    char text[MAX_LEN + 1];
    bool is_valid;
};

struct SharedData {
    Msg messages[MAX_FILE];
    int read_index;
    int write_index;
    int count;
    int max_messages;
    int sender_count;
    int ready_senders;
    bool receiver_ready;
    bool shutdown;
};

SharedData* shared = nullptr;
int fd = -1;
string fname;
vector<pid_t> sender_pids;

void cleanup() {
    cout << "\nЗавершение работы Receiver..." << endl;
    
    if (shared != nullptr) {
        shared->shutdown = true;
    }
    
    sleep(2);
    
    for (pid_t pid : sender_pids) {
        if (pid > 0) {
            kill(pid, SIGTERM);
        }
    }
    
    for (pid_t pid : sender_pids) {
        if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
        }
    }
    
    if (shared != nullptr && shared != MAP_FAILED) {
        munmap(shared, sizeof(SharedData));
    }
    if (fd != -1) {
        close(fd);
    }
    unlink(fname.c_str());
    
    cout << "Все ресурсы освобождены." << endl;
}

void signal_handler(int sig) {
    if (sig == SIGINT) {
        cleanup();
        exit(0);
    }
}

void print_header() {
    cout << "=======================================" << endl;
    cout << "       СИСТЕМА ОБМЕНА СООБЩЕНИЯМИ" << endl;
    cout << "=======================================" << endl;
}

void print_menu() {
    cout << "\nМЕНЮ RECEIVER:" << endl;
    cout << "1 - Прочитать сообщение" << endl;
    cout << "2 - Показать статус" << endl;
    cout << "3 - Завершить работу" << endl;
    cout << "Введите команду: ";
}

void print_status() {
    cout << "\n--- СТАТУС СИСТЕМЫ ---" << endl;
    cout << "Сообщений в буфере: " << shared->count << "/" << shared->max_messages << endl;
    cout << "Индекс чтения: " << shared->read_index << endl;
    cout << "Индекс записи: " << shared->write_index << endl;
    cout << "Процессов Sender: " << shared->sender_count << endl;
    cout << "Готово процессов: " << shared->ready_senders << "/" << shared->sender_count << endl;
    
    if (shared->count > 0) {
        cout << "\nСообщения в буфере:" << endl;
        for (int i = 0; i < shared->count; i++) {
            int idx = (shared->read_index + i) % shared->max_messages;
            if (shared->messages[idx].is_valid) {
                cout << "  " << (i + 1) << ". " << shared->messages[idx].text << endl;
            }
        }
    } else {
        cout << "\nБуфер пуст" << endl;
    }
    cout << "----------------------" << endl;
}

void read_message() {
    if (shared->count == 0) {
        cout << "\nБуфер пуст. Ожидаю сообщение от Sender процессов..." << endl;
        
        time_t start = time(nullptr);
        while (shared->count == 0 && !shared->shutdown) {
            if (time(nullptr) - start > 10) {
                cout << "Таймаут ожидания." << endl;
                return;
            }
            usleep(300000);
        }
        
        if (shared->count == 0) return;
    }
    
    int idx = shared->read_index;
    
    if (!shared->messages[idx].is_valid) {
        cout << "Ошибка чтения сообщения" << endl;
        return;
    }
    
    time_t now = time(nullptr);
    char time_str[20];
    strftime(time_str, sizeof(time_str), "%H:%M:%S", localtime(&now));
    
    cout << "\n[" << time_str << "] Получено: \"" << shared->messages[idx].text << "\"" << endl;
    
    shared->messages[idx].is_valid = false;
    memset(shared->messages[idx].text, 0, MAX_LEN + 1);
    
    shared->read_index = (shared->read_index + 1) % shared->max_messages;
    shared->count--;
    
    cout << "Осталось сообщений: " << shared->count << "/" << shared->max_messages << endl;
}

int main() {
    signal(SIGINT, signal_handler);
    
    system("clear");
    print_header();
    
    cout << "Введите имя файла (по умолчанию buffer.bin): ";
    getline(cin, fname);
    if (fname.empty()) fname = "buffer.bin";
    
    int max_messages;
    cout << "Размер буфера (1-100): ";
    while (!(cin >> max_messages) || max_messages < 1 || max_messages > 100) {
        cout << "Ошибка! Введите число 1-100: ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    cin.ignore();
    
    int sender_count;
    cout << "Количество процессов Sender, которые будут подключены: ";
    while (!(cin >> sender_count) || sender_count < 1) {
        cout << "Ошибка! Введите число больше 0: ";
        cin.clear();
        cin.ignore(10000, '\n');
    }
    cin.ignore();
    
    cout << "\nСоздание буфера..." << endl;
    
    fd = open(fname.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        cout << "Ошибка создания файла!" << endl;
        return 1;
    }
    
    if (ftruncate(fd, sizeof(SharedData)) == -1) {
        cout << "Ошибка установки размера!" << endl;
        close(fd);
        return 1;
    }
    
    shared = (SharedData*)mmap(nullptr, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared == MAP_FAILED) {
        cout << "Ошибка mmap!" << endl;
        close(fd);
        return 1;
    }
    
    memset(shared, 0, sizeof(SharedData));
    shared->max_messages = max_messages;
    shared->sender_count = sender_count;
    shared->receiver_ready = true;
    
    for (int i = 0; i < MAX_FILE; i++) {
        shared->messages[i].is_valid = false;
    }
    
    cout << "\n=======================================" << endl;
    cout << "          ИНСТРУКЦИЯ ДЛЯ ЗАПУСКА" << endl;
    cout << "=======================================" << endl;
    cout << "Файл буфера: " << fname << endl;
    cout << "Размер буфера: " << max_messages << " сообщений" << endl;
    cout << "Ожидаемое количество Sender процессов: " << sender_count << endl;
    cout << "\nДля запуска Sender процессов:" << endl;
    cout << "1. Откройте " << sender_count << " новых терминалов" << endl;
    cout << "2. В каждом терминале выполните команду:" << endl;
    cout << "   ./sender " << fname << endl;
    cout << "3. Вернитесь в этот терминал" << endl;
    cout << "=======================================" << endl;
    
    cout << "\nОжидание подключения " << sender_count << " Sender процессов..." << endl;
    
    int last_ready = 0;
    while (shared->ready_senders < sender_count && !shared->shutdown) {
        if (shared->ready_senders != last_ready) {
            cout << "Подключено: " << shared->ready_senders << "/" << sender_count << " процессов" << endl;
            last_ready = shared->ready_senders;
        }
        usleep(500000);
        
        if (shared->ready_senders > 0 && last_ready == 0) {
            cout << "\nПервый Sender подключен! Система готова к работе." << endl;
            cout << "Вы можете начать читать сообщения, даже если не все процессы подключены." << endl;
        }
    }
    
    system("clear");
    print_header();
    cout << "Система запущена!" << endl;
    cout << "Подключено " << shared->ready_senders << "/" << sender_count << " Sender процессов" << endl;
    cout << "Используйте меню ниже для работы с сообщениями" << endl;
    
    string cmd;
    while (true) {
        print_menu();
        getline(cin, cmd);
        
        if (cmd == "1" || cmd == "read") {
            read_message();
        } else if (cmd == "2" || cmd == "status") {
            print_status();
        } else if (cmd == "3" || cmd == "exit") {
            break;
        } else if (!cmd.empty()) {
            cout << "Неизвестная команда! Используйте 1, 2 или 3" << endl;
        }
    }
    
    cleanup();
    return 0;
}
