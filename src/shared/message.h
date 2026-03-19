#pragma once

#include <cstdint>
#include <vector>

#include "shared/tcp.h"

enum class MessageType : uint32_t {
    FRAME_SNAPSHOT,  // game -> tools
    PAUSE,           // tools -> game
    RESUME,          // tools -> game
    STEP,            // tools -> game (advance one frame, then pause)
};

struct MessageHeader {
    MessageType type;
    uint32_t payloadSize;
};

inline bool sendMessage(TcpConnection &conn, MessageType type, const void *payload = nullptr, uint32_t size = 0) {
    MessageHeader header{type, size};
    if (!conn.send(&header, sizeof(header))) return false;
    if (size > 0 && payload) {
        if (!conn.send(payload, size)) return false;
    }
    return true;
}

// Blocking recv
inline bool recvMessage(TcpConnection &conn, MessageHeader &header, std::vector<uint8_t> &payload) {
    if (!conn.recv(&header, sizeof(header))) return false;
    payload.resize(header.payloadSize);
    if (header.payloadSize > 0) {
        if (!conn.recv(payload.data(), header.payloadSize)) return false;
    }
    return true;
}

// Non-blocking: returns false if no data available (doesn't mean disconnected)
inline bool tryRecvMessage(TcpConnection &conn, MessageHeader &header, std::vector<uint8_t> &payload) {
    if (!conn.hasData()) return false;
    return recvMessage(conn, header, payload);
}
