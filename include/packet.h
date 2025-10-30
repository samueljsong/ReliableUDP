#pragma once
#include <string>
#include <cstring>
#include <cstdint>
#include <vector>

struct Packet {
    uint32_t sequence;
    uint32_t acknowledgement;
    bool syn;
    bool ack;
    bool fin;
    bool psh;
    std::string data;

    Packet(uint32_t seq = 0, uint32_t ackn = 0,
           bool s = false, bool a = false, bool f = false, bool p = false,
           const std::string& d = "")
        : sequence(seq), acknowledgement(ackn),
          syn(s), ack(a), fin(f), psh(p), data(d) {}
};

inline std::vector<char> serialize(const Packet& p) {
    std::vector<char> buffer(sizeof(Packet) + p.data.size());
    std::memcpy(buffer.data(), &p.sequence, sizeof(uint32_t));
    std::memcpy(buffer.data() + 4, &p.acknowledgement, sizeof(uint32_t));
    buffer[8] = p.syn;
    buffer[9] = p.ack;
    buffer[10] = p.fin;
    buffer[11] = p.psh;

    std::memcpy(buffer.data() + 12, p.data.c_str(), p.data.size());
    return buffer;
}

inline Packet deserialize(const char* data, size_t len) {
    Packet p;
    if (len < 12) return p;

    std::memcpy(&p.sequence, data, sizeof(uint32_t));
    std::memcpy(&p.acknowledgement, data + 4, sizeof(uint32_t));
    p.syn = data[8];
    p.ack = data[9];
    p.fin = data[10];
    p.psh = data[11];
    p.data = std::string(data + 12, len - 12);

    return p;
}