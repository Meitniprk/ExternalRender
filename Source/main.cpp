#include <thread>
#include "Window.h"
#include "Network.h"

int main() {
    std::mutex documentLock;

    std::thread network(Network::ConnetServer, std::ref(documentLock));
    std::thread render(Window::CreateExternalWindow, std::ref(documentLock));

    network.join();
    render.join();

    return 0;
}
