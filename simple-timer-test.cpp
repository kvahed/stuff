#include <simple-timer.h>

int main() {
  Timer t1, t2(false);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "lap[ms]: " << t1.lap().count() << ", elapsed[ms]: "
            << t1.elapsed().count() << std::endl;
  std::cout << "lap[ms]: " << t2.lap().count() << ", elapsed[ms]: "
            << t2.elapsed().count() << std::endl;
  t2.start();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "lap[ms]: " << t1.lap().count() << ", elapsed[ms]: "
            << t1.elapsed().count() << std::endl;

  std::cout << "lap[us]: " << t1.lap<std::micro>().count() << ", elapsed[us]: "
            << t1.elapsed<std::micro,int>().count() << std::endl;

  std::cout << "lap[ns]: " << t1.lap<std::nano>().count() << ", elapsed[ns]: "
            << t1.elapsed<std::nano,float>().count() << std::endl;

  std::cout << "lap[fs]: " << t1.lap<std::femto>().count() << ", elapsed[fs]: "
  << t1.elapsed<std::femto,double>().count() << std::endl;

  std::cout << "lap[ms]: " << t2.lap().count() << ", elapsed[ms]: "
            << t2.elapsed().count() << std::endl;
}

