#pragma once

#include <unordered_map>
#include <vector>

#include "sys/epoll.h"
#include "base/Logger.h"

START_NAMESPACE

class Channel;
// using epoll
class Poller {
public:
    Poller();
    Timestamp poll(int timeoutMs,std::vector<Channel*>& activeChannels);

    void insert(const Channel& channel);
    void update(const Channel& channel) const;
    void remove(const Channel& channel);
    bool hasChannel(const Channel& channel);
    static std::string flagToString(int flag);

private:

    void poll_ctl(const Channel& channel, int opt) const;

    std::vector<epoll_event> events_;
    std::unordered_map<int, Channel*> channelsMap;
    int poller_;
};

END_NAMESPACE