//
// Created by ManTkai on 2023/2/6.
//

#ifndef EXTERNALRENDER_NETWORK_H
#define EXTERNALRENDER_NETWORK_H

#include <mutex>

#define SERVER_HOST "127.0.0.1"
#define SERVER_PORT "61311"

namespace Network {

    [[noreturn]] void ConnetServer(std::mutex &documentLock);

}

#endif //EXTERNALRENDER_NETWORK_H
