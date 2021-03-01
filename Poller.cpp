#include "Poller.h"

#include <strings.h>
#include "sys/epoll.h"

#include "Channel.h"
USE_NAMESPACE
Poller::Poller(): poller_(epoll_create1(EPOLL_CLOEXEC)),events_(4) {}

Timestamp Poller::poll(int timeoutMs, std::vector<Channel*>& activeChannels) {
    LOG_TRACE << "poller file count = " << channelsMap.size();

    const auto activeCount = epoll_wait(poller_, events_.data(), static_cast<int>(events_.size()), timeoutMs);
    const auto err = errno;
    if(activeCount > 0) {
        LOG_TRACE << "epoll_wait return, " << activeCount << " events active";
        for (int i = 0; i < activeCount; ++i) {
            const auto& event = events_[i];
            auto* const channel = static_cast<Channel*>(event.data.ptr);
            channel->actionEvents() = event.events;
            activeChannels.push_back(channel);
        }
        if(static_cast<int>(events_.size()) == activeCount) {
            events_.reserve(activeCount * 2);
        }
    }
    else if(activeCount == 0) {
        LOG_TRACE <<"epoll_wait return, " << "no events active";
    }
    else {
        if(err != EINTR) {
            errno = err;
            LOG_FATAL << "Poller::poll()";
        }
    }
    return Timestamp::now();
}

void Poller::insert(const Channel& channel) {
    LOG_TRACE << "insert channel: " << channel.toString();
    poll_ctl(channel, EPOLL_CTL_ADD);
    auto fd = channel.fd();
    channelsMap[fd] = const_cast<Channel*>(&channel);
}


void Poller::update(const Channel& channel) const {
    LOG_TRACE << "update channel: " << channel.toString();
    poll_ctl(channel, EPOLL_CTL_MOD);
}

void Poller::remove(const Channel& channel) {
    LOG_TRACE << "remove channel: fd = " << channel.fd();
    poll_ctl(channel, EPOLL_CTL_DEL);
    channelsMap.erase(channel.fd());
}

bool Poller::hasChannel(const Channel& channel) {
    const auto it = channelsMap.find(channel.fd());
    return it != channelsMap.cend();
}

std::string Poller::flagToString(int flag) {
    switch (flag) {
    case EPOLL_CTL_ADD:
        return "EPOLL_CTL_ADD";
    case EPOLL_CTL_DEL:
        return "EPOLL_CTL_DEL";
    case EPOLL_CTL_MOD:
        return "EPOLL_CTL_MOD";
    default:
        return "Unknown Operation";
    }
}

void Poller::poll_ctl(const Channel& channel, int opt) const {
    LOG_TRACE << "epoll_ctl flag = " << flagToString(opt)
              << ", "  << channel.toString() ;
    int ret;
    if (opt == EPOLL_CTL_DEL)
        ret = epoll_ctl(poller_, opt, channel.fd(), nullptr);
    else {

        epoll_event event{};
        bzero(&event, sizeof event);
        event.events = channel.event() | EPOLLET;
        event.data.ptr = const_cast<Channel*>(&channel);

        ret = epoll_ctl(poller_, opt, channel.fd(), &event);
    }
    if (ret < 0) {
        LOG_FATAL << "epoll_ctl flag = " << flagToString(opt) << " fd = " << channel.fd();
    }
}
