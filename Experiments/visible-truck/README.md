# GlobalMP visible-truck experiment 001

## Goal
Determine whether a second truck can be rendered in ETS2 1.61.1.9s (64-bit) **without** injecting unknown game-engine hooks. Do not claim success from telemetry or RakNet alone.

## First candidate: official SCS SDK telemetry + local ghost overlay
This candidate can render a truck-shaped marker in an **external overlay**, not a native ETS2 truck. It is a controlled test of real-time position exchange and an explicit stepping stone toward an in-game renderer.

## Native in-game vehicle requirement
A genuine remote truck requires a compatible ETS2 1.61 vehicle creation, update and removal mechanism. The SCS Telemetry SDK exposes no such mechanism. The original ETS2MP x86 1.3.1 client is incompatible with ETS2 1.61 x64. We will not reuse its hardcoded offsets.

## Experiment gates
1. Two Windows RakNet clients both join Ubuntu server: proven for one client; repeat for two.
2. Obtain two independent live truck positions using a compatible SCS telemetry plugin.
3. Relay positions to each other, confirm timing and interpolation.
4. Research/test a version-specific, consent-based, isolated in-game rendering integration on a test profile, or an explicitly external overlay.
5. Count the experiment successful **only** if a remote vehicle is visibly rendered in the game world.

Do not run third-party DLL injection binaries from unknown sources. Back up ETS2 profiles before any native game integration experiment.

## Immediate action
Build and run the 2-player RakNet test on separate machines. The existing server handles join packets but does not relay positions; implement a typed POSITION packet next, without changing the working JOIN protocol.
