#include <simple-timer.h>
#include <iomanip>

int main() {
  std::cout << "starting t1" << std::endl;
    std::cout << "initialising t2" << std::endl;
  Timer t1, t2(false);
  using one = std::ratio<1>;
  std::cout << "sleeping 1s" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "  lap[ms]: " << t1.lap().count() << ", elapsed[ms]: "
            << t1.elapsed().count() << std::endl;
  std::cout << "  lap[ms]: " << t2.lap().count() << ", elapsed[ms]: "
            << t2.elapsed().count() << std::endl;
  std::cout << "starting t2" << std::endl;
  std::cout << "sleeping 1s" << std::endl;
  t2.start();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "  lap[ms]: " << t1.lap().count() << ", elapsed[ms]: "
            << t1.elapsed().count() << std::endl;

  std::cout << "  lap[us]: " << t1.lap<std::micro>().count() << ", elapsed[us]: "
            << t1.elapsed<std::micro,int>().count() << std::endl;

  std::cout << "  lap[ns]: " << t1.lap<std::nano>().count() << ", elapsed[ns]: "
            << t1.elapsed<std::nano,float>().count() << std::endl;

  std::cout << "  lap[fs]: " << t1.lap<std::femto>().count() << ", elapsed[fs]: "
  << t1.elapsed<std::femto,double>().count() << std::endl;

  std::cout << "  lap[ms]: " << t2.lap().count() << ", elapsed[ms]: "
            << t2.elapsed().count() << std::endl;
  
  t1.stop();
  std::cout << "pausing t1" << std::endl;
  std::cout << "sleeping 1s" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "continuing 1s" << std::endl;
  t1.start();
  std::cout << "  lap[ms]: " << t1.lap().count() << ", elapsed[ms]: "
            << t1.elapsed().count() << std::endl;
  std::cout << "sleeping 1s" << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  
  std::cout << "  lap[ms]: " << t1.lap().count() << ", elapsed[ms,long]: "
            << t1.elapsed().count() << " elapsed[s,float]: "
            << t1.elapsed<one,float>().count() << std::endl << std::endl;

  std::cout << "  lap times[ns,long]:" << t1.laps<std::nano>() << std::endl;
  std::cout << "  lap times[ms,float]:" << t1.laps<std::milli,float>() << std::endl;
  std::cout.precision(8*std::numeric_limits<long double>::digits10);
  std::cout << "  lap times[s,long double]:" << t1.laps<one,long double>() << std::endl;
  
}

