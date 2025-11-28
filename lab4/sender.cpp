#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <cstdlib>
#include <signal.h>

#include "common.h"

using namespace std;

SharedData* shared = nullptr;
int fd = -1;

void cleanup() {
    if (shared != nullptr && shared != MAP_FAILED) {
        munmap(shared, sizeof(SharedData));
    }
    if (fd != -1) {
        close(fd);
    }
}

void signal_handler(int sig) {
    cleanup();
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }
    
    signal(SIGINT, signal_handler);
    
    string fname = argv[1];
    int sender_id = getpid();
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    cout << "Текущая директория: " << cwd << endl;
    
    fd = open(fname.c_str(), O_RDWR);
    if (fd == -1) {
        cout << "Ошибка открытия файла: " << fname << endl;
        return 1;
    }
    
    shared = (SharedData*)mmap(nullptr, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared == MAP_FAILED) {
        cout << "Ошибка mmap" << endl;
        close(fd);
        return 1;
    }
    
    shared->ready_senders++;
    cout << "=======================================" << endl;
    cout << "         SENDER " << sender_id << " ЗАПУЩЕН" << endl;
    cout << "=======================================" << endl;
    cout << "Ожидаю другие процессы..." << endl;
    
    while (shared->ready_senders < shared->sender_count && !shared->shutdown) {
        usleep(100000);
    }
    
    if (shared->shutdown) {
        cout << "Receiver завершил работу. Выход." << endl;
        cleanup();
        return 0;
    }
    
    cout << "Все процессы готовы!" << endl;
    cout << "Максимальная длина сообщения: " << MAX_LEN << " символов" << endl;
    cout << "Размер буфера: " << shared->max_messages << " сообщений" << endl;
    cout << "Всего процессов: " << shared->sender_count << endl;
    
    string cmd;
    string message;
    
    while (!shared->shutdown) {
        cout << "\n---------------------------------------" << endl;
        cout << "             SENDER " << sender_id << endl;
        cout << "---------------------------------------" << endl;
        cout << "Команды:" << endl;
        cout << "   'send' - отправить сообщение" << endl;
        cout << "   'exit' - завершить работу" << endl;
        cout << "Введите команду: ";
        
        getline(cin, cmd);
        
        if (cmd == "send") {
            if (shared->count >= shared->max_messages) {
                cout << "Буфер заполнен! Ожидаю освобождения..." << endl;
                while (shared->count >= shared->max_messages && !shared->shutdown) {
                    usleep(500000);
                    cout << "Ожидание..." << endl;
                }
                if (shared->shutdown) break;
            }
            
            cout << "Введите сообщение (до " << MAX_LEN << " символов): ";
            getline(cin, message);
            
            if (message.length() > MAX_LEN) {
                cout << "Ошибка: сообщение слишком длинное! Максимум " << MAX_LEN << " символов." << endl;
                continue;
            }
            
            if (message.empty()) {
                cout << "Сообщение не может быть пустым!" << endl;
                continue;
            }
            
            int current_index = shared->write_index;
            strncpy(shared->messages[current_index].text, message.c_str(), MAX_LEN);
            shared->messages[current_index].text[MAX_LEN] = '\0';
            shared->messages[current_index].is_valid = true;
            
            shared->write_index = (shared->write_index + 1) % shared->max_messages;
            shared->count++;
            
            cout << "Сообщение отправлено: \"" << message << "\"" << endl;
            cout << "В буфере: " << shared->count << "/" << shared->max_messages << " сообщений" << endl;
            
        } else if (cmd == "exit") {
            cout << "Завершение работы Sender " << sender_id << endl;
            break;
        } else if (!cmd.empty()) {
            cout << " Неизвестная команда. Используйте 'send' или 'exit'" << endl;
        }
    }
    
    cleanup();
    
    cout << "\n Нажмите Enter для закрытия окна...";
    cin.get();
    
    return 0;
}
