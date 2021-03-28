#include <sys/timerfd.h>

#include "Util.h"
#include "Channel.h"

START_NAMESPACE

class Timer {
public:

    using Runnable = std::function<void()>;

    Timer(double interval);

    void enable();

    void disable();

    ~Timer();

    void setOnAlarm(Runnable&& func);

    void handleRead() {
        if(onAlarm_)
            onAlarm_();
    }

private:
    
    Runnable onAlarm_;
    Channel channel_;

 };

END_NAMESPACE