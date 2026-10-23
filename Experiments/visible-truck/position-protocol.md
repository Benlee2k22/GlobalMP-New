# Experimental POSITION packet v1

This is a proposal for a server/client protocol extension, not an implemented feature.

- Packet framing: ID_PACKET_MANAGER (RakNet ID_USER_PACKET_ENUM+1), custom packet ID 3 (after join, join reply, chat).
- Payload: protocolVersion uint8 (1), senderId uint32, sequence uint32, position x/y/z float64, orientation quaternion x/y/z/w float32, timestampMs uint64.
- Send rate: 10 Hz initial, unreliable sequenced; discard older sequence numbers.
- Validate finite coordinates and quaternion, reject oversized/malformed packets; assign senderId on server (do not trust client-supplied IDs).
- Server broadcasts position to all connected clients except sender.
- Receiver interpolates between latest updates and times out remote player after 3 seconds.

Coordinate conventions and SCS telemetry fields must be verified against actual SDK output before coding the serializer. No spawning API is implied by this packet.
