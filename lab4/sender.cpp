#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <cstdlib>

#include "common.h"

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }
    
    string fname = argv[1];
    
    int fd = open(fname.c_str(), O_RDWR);
    if (fd == -1) {
        perror("Sender: Ошибка открытия файла");
        return 1;
    }
    
    SharedData* shared = (SharedData*)mmap(nullptr, sizeof(SharedData), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shared == MAP_FAILED) {
        perror("Sender: Ошибка mmap");
        close(fd);
        return 1;
    }
    
    cout << "Sender: Процесс запущен. Сигнализирую о готовности..." << endl;
    shared->sender_ready = true;
    
    string cmd;
    string msg_text;
    
    while (true) {
        cout << "\nSender commands:" << endl;
        cout << "1. send - отправить сообщение" << endl;
        cout << "2. exit - завершить работу" << endl;
        cout << "Введите команду: ";
        
        getline(cin, cmd);
        
        if (cmd == "send") {
            if (!shared->msg.empty) {
                cout << "Sender: Файл занят. Ожидаю освобождения..." << endl;
                while (!shared->msg.empty) {
                    usleep(100000);
                }
            }
            
            cout << "Sender: Введите сообщение (до " << MAX_LEN << " символов): ";
            getline(cin, msg_text);
            
            if (msg_text.length() > MAX_LEN) {
                cout << "Sender: Сообщение слишком длинное! Максимум " << MAX_LEN << " символов." << endl;
                continue;
            }
            
            strcpy(shared->msg.text, msg_text.c_str());
            shared->msg.empty = false;
            cout << "Sender: Сообщение '" << msg_text << "' отправлено." << endl;
            
        } else if (cmd == "exit") {
            cout << "Sender: Завершение работы." << endl;
            break;
        } else {
            cout << "Sender: Неизвестная команда." << endl;
        }
    }
    
    munmap(shared, sizeof(SharedData));
    close(fd);
    return 0;
}