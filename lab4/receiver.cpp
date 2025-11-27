#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <cstdlib>

#include "common.h"

using namespace std;

SharedData* shared;
int fd;
string fname;

void cleanup() {
    if (shared != nullptr) {
        munmap(shared, sizeof(SharedData));
    }
    if (fd != -1) {
        close(fd);
    }
    unlink(fname.c_str());
}

int main() {
    shared = nullptr;
    fd = -1;
    
    cout << "Receiver: Введите имя бинарного файла: ";
    getline(cin, fname);
    
    fd = open(fname.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd == -1) {
        perror("Receiver: Ошибка создания файла");
        return 1;
    }
    
    if (ftruncate(fd, sizeof(SharedData)) == -1) {
        perror("Receiver: Ошибка установки размера файла");
        cleanup();
        return 1;
    }
    
    shared = (SharedData*)mmap(nullptr, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared == MAP_FAILED) {
        perror("Receiver: Ошибка mmap");
        cleanup();
        return 1;
    }
    
    shared->msg.empty = true;
    shared->sender_ready = false;
    shared->receiver_ready = true;
    
    cout << "Receiver: Запуск процесса Sender..." << endl;
    pid_t pid = fork();
    
    if (pid == 0) {
        execl("./sender", "sender", fname.c_str(), nullptr);
        perror("Receiver: Ошибка запуска sender");
        return 1;
    } else if (pid > 0) {
        cout << "Receiver: Ожидание готовности Sender..." << endl;
        
        while (!shared->sender_ready) {
            usleep(100000);
        }
        
        cout << "Receiver: Sender готов. Начинаем работу." << endl;
        
        string cmd;
        while (true) {
            cout << "\nReceiver commands:" << endl;
            cout << "1. read - прочитать сообщение" << endl;
            cout << "2. exit - завершить работу" << endl;
            cout << "Введите команду: ";
            
            getline(cin, cmd);
            
            if (cmd == "read") {
                if (shared->msg.empty) {
                    cout << "Receiver: Файл пуст. Ожидаю сообщение..." << endl;
                    while (shared->msg.empty) {
                        usleep(100000);
                    }
                }
                
                cout << "Receiver: Получено сообщение: '" << shared->msg.text << "'" << endl;
                shared->msg.empty = true;
                
            } else if (cmd == "exit") {
                cout << "Receiver: Завершение работы." << endl;
                break;
            } else {
                cout << "Receiver: Неизвестная команда." << endl;
            }
        }
        
        wait(nullptr);
        cout << "Receiver: Sender завершил работу." << endl;
        
    } else {
        perror("Receiver: Ошибка fork");
        cleanup();
        return 1;
    }
    
    cleanup();
    return 0;
}