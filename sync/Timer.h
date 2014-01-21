#include <sys/time.h>
#include <unistd.h>
#include <time.h>


class Timer
{
public:
  Timer();
  ~Timer();

  // Pause the program until the time target is reached
  bool SleepUntilTime(time_t target);

  // stop
  bool Stop();

private:
  bool running;
  
};

