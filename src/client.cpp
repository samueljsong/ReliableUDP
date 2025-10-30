#include "../include/packet.h"
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <chrono>

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in server{};
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &server.sin_addr);

    socklen_t len = sizeof(server);
    char buffer[1024];

    Packet syn(1, 0, true, false);
    auto out = serialize(syn);
    sendto(sock, out.data(), out.size(), 0, (sockaddr*)&server, len);
    std::cout << "Sent SYN\n";

    std::this_thread::sleep_for(std::chrono::seconds(2));

    ssize_t bytes = recvfrom(sock, buffer, sizeof(buffer), 0,
                             (sockaddr*)&server, &len);
    Packet packet = deserialize(buffer, bytes);
    if (packet.syn && packet.ack) {
        std::cout << "Received SYN+ACK\n";
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));

    Packet ack(2, packet.sequence + 1, false, true);
    out = serialize(ack);
    sendto(sock, out.data(), out.size(), 0, (sockaddr*)&server, len);
    std::cout << "Connection Established: Three-way Handshake Complete.\n";

    std::this_thread::sleep_for(std::chrono::seconds(2));

    std::string message = "Hello from client!";
    Packet dataPkt(3, 0, false, true, false, true, message);
    out = serialize(dataPkt);
    sendto(sock, out.data(), out.size(), 0, (sockaddr*)&server, len);
    std::cout << "Sent data.\n";

    std::this_thread::sleep_for(std::chrono::seconds(2));

    recvfrom(sock, buffer, sizeof(buffer), 0, (sockaddr*)&server, &len);
    packet = deserialize(buffer, bytes);
    std::cout << "ACK received.\n";

    std::this_thread::sleep_for(std::chrono::seconds(2));

    Packet fin(4, 0, false, true, true);
    out = serialize(fin);
    sendto(sock, out.data(), out.size(), 0, (sockaddr*)&server, len);
    std::cout << "Sent FIN.\n";

    close(sock);
}