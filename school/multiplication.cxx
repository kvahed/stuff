#include <iostream>
#include <chrono>
#include <random>
#include <csignal>
#include <string>
#include <vector>

template<typename T> struct vec {
  std::vector<T> _v;
  vec() {
    _v.resize(121);
  }
  T& operator[](size_t i) {
    return _v[i];
  }
  T const& operator[](size_t i) const {
    return _v[i];
  }
  vec<float> operator/(vec<T> const& other) {
    vec<float> ret;
    for (size_t i = 0; i < 100; ++i) {
      if (other._v[i] != 0) {
        ret[i] = 100.0*(float)_v[i]/(float)other._v[i];
      }
    }
    return ret;
  }
};

std::ostream& operator<< (std::ostream& os, vec<float> v) {
  std::cout << "       1   2   3   4   5   6   7   8   9  10" 
            << std::endl << std::endl;
  for (size_t i = 0; i < 100; i=i+10) {
    printf
      ("%2d   %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f %3.0f\n",
       i/10+1, v[i+0], v[i+1], v[i+2], v[i+3], v[i+4], v[i+5], v[i+6], v[i+7],
       v[i+8], v[i+9], v[i+9]);
  }
  return os;
}

int main() {

  std::random_device r;
  std::default_random_engine e(r());
  std::uniform_int_distribution<> ud(1,10);
  
  std::string line;
  int x = ud(e), y = ud(e), z;

  std::vector<std::chrono::duration<double>> times;
  vec<size_t> samples, correct;
  
  bool first = true;
    
  while (first || std::getline(std::cin, line)) {
    if(!first) {
      try {
        z = std::stoi(line);
        samples[(y-1)*10+(x-1)]++;
        if (z == x*y) {
          correct[(y-1)*10+x-1]++;
          x = ud(e), y = ud(e);
        } 
      } catch (std::exception const& e) {
        break;
      }
    }
    printf("%2d x %2d = ", x, y);
    fflush(stdout);
    first=false;
  }

  std::cout << std::endl;
  std::cout << correct/samples << std::endl;

  return 0;

}
