# GlobalMP Launcher v0.2

A bare-bones Windows WPF launcher for ETS2 1.61.1.9s (64-bit), plus an experimental RakNet client for the original ETS2MP Ubuntu server.

## Requirements
- Windows 10/11
- .NET 8 SDK
- Git
- A running ETS2MP server listening on UDP port 8365

## Build launcher
```powershell
git clone https://github.com/Benlee2k22/GlobalMP-New.git
cd GlobalMP-New
dotnet build GlobalMP.Launcher -c Release
dotnet run --project GlobalMP.Launcher
```

## Build networking client
GitHub Actions > **Build Windows RakNet client** > **Run workflow**. Once successful, download the `globalmp-client-windows` artifact, unzip it, and place `globalmp-client.exe` in:
`GlobalMP.Launcher/bin/Debug/net8.0-windows/` for `dotnet run`, or
`GlobalMP.Launcher/bin/Release/net8.0-windows/` for Release builds.

Then run the launcher and click **CONNECT TO GLOBALMP SERVER**. A successful server join reply is reported as `JOIN_CONFIRMED`. The launcher can launch ETS2 independently; networking is not integrated into the game yet.

This Windows build is experimental and not yet verified; inspect GitHub Actions output if it fails. The original server only handles join/chat/time and does not synchronize trucks.
