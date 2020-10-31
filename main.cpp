#include <iostream>
#include <thread>
#include <mutex>
#include "rw_lock.hpp"

const int READER_NUM = 2;
const int WRITER_NUM = 2;

ReadWriteLock cout_mutex;

void displayBalance(bool is_consume, unsigned int value){
    WriteLock lock(cout_mutex);
    std::cout << (is_consume ? "Consume:" : "My account balance:") << value << std::endl;
}

class Account{
private:
    ReadWriteLock mutex_;
    unsigned int balance_;
public:
    Account(){
        balance_ = 10000;
    }
    ~Account(){}
    unsigned int get_balance() {
        ReadLock lock(mutex_);
        return balance_;
    }
    unsigned int consume(){
        WriteLock lock(mutex_);
        int cost = rand() % 1000;
        balance_ -= cost;
        return cost;
    }
    void reset(){
        WriteLock lock(mutex_);
        balance_ = 10000;
    }
};

int main(){
    std::thread readers[READER_NUM];
    std::thread writers[WRITER_NUM];
    Account my_account;
    auto read = [&my_account](){
        while(1){
            std::this_thread::sleep_for(std::chrono::seconds(1));
            displayBalance(false, my_account.get_balance());
        }
    };
    auto write = [&my_account](){
        while(1){
            std::this_thread::sleep_for(std::chrono::seconds(1));
            displayBalance(true, my_account.consume());
        }
    };
    auto reset = [&my_account](){
        while(1){
            std::this_thread::sleep_for(std::chrono::seconds(5));
            my_account.reset();
        }

    };
    std::thread reset_thread = std::thread(reset);
    for(int i = 0; i < READER_NUM; i++){
        readers[i] = std::thread(read);
    }
    for(int i = 0; i < WRITER_NUM; i++){
        writers[i] = std::thread(write);
    }
    for(int i = 0; i < 2; i++){
        readers[i].join();
        writers[i].join();
    }
    reset_thread.join();

    return 0;
}
