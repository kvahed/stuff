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
  std::cout << "lap[ms]: " << t2.lap().count() << ", elapsed[ms]: "
            << t2.elapsed().count() << std::endl;
}

