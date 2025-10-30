#include "../include/packet.h"
#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <chrono>

int main() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    sockaddr_in server{}, client{};
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);
    server.sin_addr.s_addr = INADDR_ANY;
    bind(sock, (sockaddr*)&server, sizeof(server));

    std::cout << "Server listening on port 8080...\n";

    char buffer[1024];
    socklen_t clientLen = sizeof(client);
    Packet packet;

    while (true)
    {
        ssize_t bytes = recvfrom(sock, buffer, sizeof(buffer), 0,
                                 (sockaddr*)&client, &clientLen);
        packet = deserialize(buffer, bytes);
        if (packet.syn) {
            std::cout << "Received SYN, seq=" << packet.sequence << "\n";
        }
    
        std::this_thread::sleep_for(std::chrono::seconds(3));
    
        Packet synAck(100, packet.sequence + 1, true, true);
        auto out = serialize(synAck);
        sendto(sock, out.data(), out.size(), 0, (sockaddr*)&client, clientLen);
        std::cout << "Sent SYN+ACK\n";
    
        std::this_thread::sleep_for(std::chrono::seconds(3));
    
        bytes = recvfrom(sock, buffer, sizeof(buffer), 0,
                         (sockaddr*)&client, &clientLen);
        packet = deserialize(buffer, bytes);
        if (packet.ack) {
            std::cout << "Handshake complete.\n";
        }
    
        std::this_thread::sleep_for(std::chrono::seconds(3));
    
        while (true) {
            bytes = recvfrom(sock, buffer, sizeof(buffer), 0,
                             (sockaddr*)&client, &clientLen);
            packet = deserialize(buffer, bytes);
    
            if (packet.fin) {
                std::cout << "Received FIN, closing...\n";
                Packet finAck(400, packet.sequence + 1, false, true, true);
                out = serialize(finAck);
                sendto(sock, out.data(), out.size(), 0, (sockaddr*)&client, clientLen);
                break;
            }
    
            if (packet.psh) {
                std::cout << "Data: " << packet.data << "\n";
                Packet ack(packet.sequence + 1, 0, false, true);
                out = serialize(ack);
                sendto(sock, out.data(), out.size(), 0, (sockaddr*)&client, clientLen);
            }
        }

        std::cout << "Server is waiting for the next connection..." << std::endl;
    }

    close(sock);
}