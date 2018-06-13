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
    auto n = clock::now();
    auto d = n - stop_;
    for (auto& i : lap_) {
      i += d;
    }
    lap_.push_back(n);
    running_ = true;
  }
  
  template<typename T = std::milli, typename U = long>
  std::chrono::duration<U,T> elapsed() {
    if (running_) {
      auto n = std::chrono::steady_clock::now();
      return std::chrono::duration_cast<std::chrono::duration<U,T>>(
        n - lap_.front());
    } 
    return std::chrono::duration<U,T>(0);
  }
  
  template<typename T = std::milli, typename U = long>
  std::chrono::duration<U,T> lap() {
    if (running_) {
      auto n = std::chrono::steady_clock::now();
      auto ret = std::chrono::duration_cast<std::chrono::duration<U,T>>(
        n - lap_.back());
      lap_.push_back(n);
      return ret;
    } 
    return std::chrono::duration<U,T>(0);
  }

  template<typename T = std::milli, typename U = long>
  std::vector<std::chrono::duration<U,T>> laps() {
    std::vector<std::chrono::duration<U,T>> ret;
    if (lap_.size() > 0) {
      for (auto i = 1; i < lap_.size(); ++i) {
        ret.push_back(std::chrono::duration_cast<std::chrono::duration<U,T>>(
                        lap_[i] - lap_[i-1]));
      }
    }
    return ret;
  }

  void stop() {
    stop_ = clock::now();
    running_ = false;
  }

  void reset() {
    lap_.clear();
    running_ = false;
  }
  
private:
  clock::time_point stop_;
  std::vector<clock::time_point> lap_;
  bool running_ = false;
};

template<typename T>
inline std::ostream& operator<< (std::ostream& os, std::vector<T> const& vt) {
  os << "[ ";
  for (auto const& i : vt)
    os << i.count() << " ";
  os << "]";
  return os;
}
