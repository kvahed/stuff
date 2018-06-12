#include <chrono>
#include <iostream>
#include <thread>
#include <vector>

class Timer {

  using clock = std::chrono::steady_clock;

public:
  Timer(bool s = true) {
    if (s) {
      start();
    }
  }
  
  void start() {
    lap_.push_back(clock::now());
    running_ = true;
  }
  
  template<typename T = std::milli, typename U = long> std::chrono::duration<U,T> elapsed() {
    if (running_) {
      auto n = std::chrono::steady_clock::now();
      lap_.push_back(n);
      return std::chrono::duration_cast<std::chrono::duration<U,T>>(n - lap_.front());
    } 
    return std::chrono::duration<U,T>(0);
  }
  
  template<typename T = std::milli, typename U = long> std::chrono::duration<U,T> lap() {
    if (running_) {
      auto n = std::chrono::steady_clock::now();
      auto ret = std::chrono::duration_cast<std::chrono::duration<U,T>>(n - lap_.back());
      lap_.push_back(n);
      return ret;
    } 
    return std::chrono::duration<U,T>(0);
  }

private:
  std::vector<clock::time_point> lap_;
  bool running_ = false;
};

