# ThreadLab - Лабораторная работа по потокам

Система параллельной обработки массивов с использованием POSIX Threads.

## Основные функции

- **Параллельные вычисления**: одновременный поиск минимума/максимума и среднего значения
- **Замена элементов**: автоматическая замена минимальных и максимальных значений на среднее
- **Тестирование**: модульные тесты всех компонентов системы

## Сборка

```bash
mkdir build && cd build
cmake ..
make
## Запуск
```bash
./thread_lab

##Тестирование
```bash
./thread_lab_test

##Пример работы
```text
Enter array size: 5
Enter 5 elements:
10 20 30 40 50

Original array: 10 20 30 40 50
Min: 10
Max: 50
Average: 30.0
Replaced 2 elements
Modified array: 30 20 30 40 30
Структура проекта

##main.cpp - точка входа программы
##ThreadManager.cpp - управление потоками и данными
##DataManager.cpp - работа с массивами данных
##DataProcessor.cpp - алгоритмы обработки
##ThreadController.cpp - контроллер потоков POSIX
##test_main.cpp - модульные тесты
