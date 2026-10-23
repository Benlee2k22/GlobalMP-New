#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include <StringCompressor.h>
#include <RakSleep.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>

static constexpr unsigned char PacketManagerId = ID_USER_PACKET_ENUM + 1;
static constexpr unsigned char JoinId = 0;
static constexpr unsigned char JoinReplyId = 1;

static void log(const char* message) { std::puts(message); std::fflush(stdout); }

int main(int argc, char** argv) {
    const char* host = argc > 1 ? argv[1] : "178.238.226.59";
    const int port = argc > 2 ? std::atoi(argv[2]) : 8365;
    const char* name = argc > 3 ? argv[3] : "GlobalMPTester";
    if (port < 1 || port > 65535 || std::strlen(name) > 22) {
        log("ERROR: Invalid port or nickname"); return 2;
    }
    auto* peer = RakNet::RakPeerInterface::GetInstance();
    RakNet::SocketDescriptor socket(0, nullptr);
    if (peer->Startup(1, &socket, 1) != RakNet::RAKNET_STARTED) {
        log("ERROR: RakNet startup failed"); RakNet::RakPeerInterface::DestroyInstance(peer); return 3;
    }
    RakNet::StringCompressor::AddReference();
    log("CONNECTING");
    if (peer->Connect(host, static_cast<unsigned short>(port), nullptr, 0) != RakNet::CONNECTION_ATTEMPT_STARTED) {
        log("ERROR: Connection could not start");
        peer->Shutdown(100); RakNet::StringCompressor::RemoveReference();
        RakNet::RakPeerInterface::DestroyInstance(peer); return 4;
    }
    bool joined = false;
    auto start = std::chrono::steady_clock::now();
    while (true) {
        for (RakNet::Packet* p = peer->Receive(); p; p = peer->Receive()) {
            const auto id = p->data[0];
            if (id == ID_CONNECTION_REQUEST_ACCEPTED) {
                log("RAKNET_CONNECTED");
                RakNet::BitStream payload;
                RakNet::StringCompressor::Instance()->EncodeString(name, 24, &payload);
                RakNet::BitStream packet;
                packet.Write(static_cast<RakNet::MessageID>(PacketManagerId));
                packet.Write(static_cast<RakNet::MessageID>(JoinId));
                packet.AlignWriteToByteBoundary();
                payload.ResetReadPointer();
                packet.Write(&payload);
                peer->Send(&packet, HIGH_PRIORITY, RELIABLE_ORDERED, 0, p->systemAddress, false);
                log("JOIN_SENT");
            } else if (id == PacketManagerId && p->length >= 2 && p->data[1] == JoinReplyId) {
                RakNet::BitStream data(p->data, p->length, false);
                data.IgnoreBytes(2);
                data.AlignReadToByteBoundary();
                int hour = 0, minute = 0, weather = 0;
                if (data.Read(hour) && data.Read(minute) && data.Read(weather)) {
                    std::printf("JOIN_CONFIRMED hour=%d minute=%d weather=%d\n", hour, minute, weather);
                    std::fflush(stdout);
                    joined = true;
                }
            } else if (id == ID_CONNECTION_ATTEMPT_FAILED || id == ID_NO_FREE_INCOMING_CONNECTIONS || id == ID_CONNECTION_BANNED) {
                log("ERROR: Connection rejected or timed out");
                peer->DeallocatePacket(p);
                peer->Shutdown(100);
                RakNet::StringCompressor::RemoveReference();
                RakNet::RakPeerInterface::DestroyInstance(peer);
                return 5;
            } else if (id == ID_CONNECTION_LOST || id == ID_DISCONNECTION_NOTIFICATION) {
                log("DISCONNECTED");
                peer->DeallocatePacket(p);
                peer->Shutdown(100);
                RakNet::StringCompressor::RemoveReference();
                RakNet::RakPeerInterface::DestroyInstance(peer);
                return 6;
            }
            peer->DeallocatePacket(p);
        }
        if (!joined && std::chrono::steady_clock::now() - start > std::chrono::seconds(15)) {
            log("ERROR: No join acknowledgement within 15 seconds");
            break;
        }
        RakSleep(30);
    }
    peer->Shutdown(100);
    RakNet::StringCompressor::RemoveReference();
    RakNet::RakPeerInterface::DestroyInstance(peer);
    return 7;
}
