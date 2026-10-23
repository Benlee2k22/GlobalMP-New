#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>
#include <StringCompressor.h>
#include <RakSleep.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <unordered_map>
#include <string>
static constexpr unsigned char Manager = ID_USER_PACKET_ENUM + 1;
static constexpr unsigned char Join = 0, JoinReply = 1, Position = 3;
int main(int argc, char** argv) {
  const int port = argc > 1 ? std::atoi(argv[1]) : 8365;
  if(port < 1 || port > 65535) return 2;
  auto* peer = RakNet::RakPeerInterface::GetInstance();
  RakNet::SocketDescriptor socket(static_cast<unsigned short>(port), nullptr);
  if(peer->Startup(100, &socket, 1) != RakNet::RAKNET_STARTED) {
    std::puts("ERROR startup"); return 3;
  }
  peer->SetMaximumIncomingConnections(100);
  RakNet::StringCompressor::AddReference();
  std::unordered_map<std::string, unsigned int> players;
  unsigned int nextId = 1;
  std::printf("RELAY_LISTENING udp=%d\n",port); std::fflush(stdout);
  while(true) {
    for(auto* p=peer->Receive();p;p=peer->Receive()) {
      const auto addr = std::string(p->systemAddress.ToString(true, ':'));
      if(p->data[0] == ID_NEW_INCOMING_CONNECTION) {
        std::printf("PEER_CONNECTED %s\n",addr);
      } else if(p->data[0] == ID_CONNECTION_LOST || p->data[0] == ID_DISCONNECTION_NOTIFICATION) {
        players.erase(addr);
        std::printf("PEER_LEFT %s\n",addr);
      } else if(p->data[0] == Manager && p->length >= 2) {
        RakNet::BitStream payload(p->data,p->length,false);
        payload.IgnoreBytes(2);
        payload.AlignReadToByteBoundary();
        if(p->data[1] == Join) {
          char name[24]={};
          if(RakNet::StringCompressor::Instance()->DecodeString(name,24,&payload)) {
            if(!players.count(addr)) players[addr]=nextId++;
            RakNet::BitStream out;
            out.Write(static_cast<RakNet::MessageID>(Manager));
            out.Write(static_cast<RakNet::MessageID>(JoinReply));
            out.Write(static_cast<int>(12));out.Write(static_cast<int>(0));out.Write(static_cast<int>(0));
            peer->Send(&out,HIGH_PRIORITY,RELIABLE_ORDERED,0,p->systemAddress,false);
            std::printf("JOIN name=%s id=%u\n",name,players[addr]);
          }
        } else if(p->data[1] == Position && players.count(addr)) {
          unsigned int sequence=0;
          double x=0,y=0,z=0;
          if(payload.Read(sequence) && payload.Read(x) && payload.Read(y) && payload.Read(z)
              && std::isfinite(x) && std::isfinite(y) && std::isfinite(z)
              && std::fabs(x)<10000000 && std::fabs(y)<10000000 && std::fabs(z)<10000000) {
            RakNet::BitStream out;
            out.Write(static_cast<RakNet::MessageID>(Manager));
            out.Write(static_cast<RakNet::MessageID>(Position));
            out.Write(players[addr]);out.Write(sequence);
            out.Write(x);out.Write(y);out.Write(z);
            for(unsigned i=0;i<peer->NumberOfConnections();++i) {
              RakNet::SystemAddress remote;
              RakNet::RakNetGUID guid;
              if(peer->GetSystemAddressFromIndex(i) == p->systemAddress) continue;
              remote=peer->GetSystemAddressFromIndex(i);
              if(remote != RakNet::UNASSIGNED_SYSTEM_ADDRESS && players.count(remote.ToString(true, ':')))
                peer->Send(&out,MEDIUM_PRIORITY,UNRELIABLE_SEQUENCED,1,remote,false);
            }
          }
        }
      }
      peer->DeallocatePacket(p);
    }
    RakSleep(10);
  }
}
