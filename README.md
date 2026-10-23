# GlobalMP Launcher v0.1

Bare-bones Windows WPF launcher targeting ETS2 1.61.1.9s (64-bit).

## Run

Install the .NET 8 SDK on Windows, then:

```powershell
dotnet run --project GlobalMP.Launcher
```

Choose the game's `bin/win_x64/eurotrucks2.exe` and click **Launch ETS2**.

The launcher stores your selected executable path in `%LOCALAPPDATA%/GlobalMP/launcher.json`.

## Scope

This release only starts ETS2. The displayed UDP server address is informational and no multiplayer connection or remote truck rendering exists yet. Those require a separate compatible game client and integration module.
