#include <iostream>
#include <vector>
#include <pthread.h>
#include <unistd.h>
#include <limits>
#include <stdexcept>

class ThreadManager {
private:
    static const int kMinMaxSleepMs = 7;
    static const int kAverageSleepMs = 12;
    
    std::vector<int> data_;
    int min_value_;
    int max_value_;
    double average_value_;

    // Prevent copying
    ThreadManager(const ThreadManager&);
    ThreadManager& operator=(const ThreadManager&);

public:
    ThreadManager() : min_value_(0), max_value_(0), average_value_(0.0) {}

    void readInputData() {
        int size = 0;
        std::cout << "Enter array size: ";
        std::cin >> size;
        
        if (size <= 0) {
            throw std::invalid_argument("Array size must be positive");
        }
        
        data_.resize(size);
        std::cout << "Enter " << size << " elements:" << std::endl;
        for (int i = 0; i < size; ++i) {
            if (!(std::cin >> data_[i])) {
                throw std::invalid_argument("Invalid input");
            }
        }
    }

    static void* minMaxThread(void* arg) {
        ThreadManager* manager = static_cast<ThreadManager*>(arg);
        manager->findMinMax();
        return NULL;
    }

    static void* averageThread(void* arg) {
        ThreadManager* manager = static_cast<ThreadManager*>(arg);
        manager->calculateAverage();
        return NULL;
    }

    void findMinMax() {
        min_value_ = data_[0];
        max_value_ = data_[0];

        for (size_t i = 1; i < data_.size(); ++i) {
            if (data_[i] < min_value_) min_value_ = data_[i];
            if (data_[i] > max_value_) max_value_ = data_[i];
            usleep(kMinMaxSleepMs * 1000);
        }
        
        std::cout << "Min: " << min_value_ << "\nMax: " << max_value_ << std::endl;
    }

    void calculateAverage() {
        long long sum = 0;
        for (size_t i = 0; i < data_.size(); ++i) {
            sum += data_[i];
            usleep(kAverageSleepMs * 1000);
        }
        
        average_value_ = static_cast<double>(sum) / data_.size();
        std::cout << "Average: " << average_value_ << std::endl;
    }

    void replaceElements() {
        int replace_count = 0;
        for (size_t i = 0; i < data_.size(); ++i) {
            if (data_[i] == min_value_ || data_[i] == max_value_) {
                data_[i] = static_cast<int>(average_value_);
                ++replace_count;
            }
        }
        std::cout << "Replaced " << replace_count << " elements" << std::endl;
    }

    void printArray(const std::string& title) const {
        std::cout << title << ": ";
        for (size_t i = 0; i < data_.size(); ++i) {
            std::cout << data_[i] << " ";
        }
        std::cout << std::endl;
    }

    void execute() {
        readInputData();
        printArray("Original array");

        pthread_t min_max_thread, average_thread;
        
        if (pthread_create(&min_max_thread, NULL, minMaxThread, this) != 0) {
            throw std::runtime_error("Cannot create min_max thread");
        }
        
        if (pthread_create(&average_thread, NULL, averageThread, this) != 0) {
            pthread_join(min_max_thread, NULL);
            throw std::runtime_error("Cannot create average thread");
        }

        pthread_join(min_max_thread, NULL);
        pthread_join(average_thread, NULL);

        replaceElements();
        printArray("Modified array");
    }
};

int main() {
    try {
        ThreadManager manager;
        manager.execute();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}




