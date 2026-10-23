# Two-client relay test

This is a real position relay test, NOT in-game truck rendering. Position coordinates are synthetic until telemetry integration is built.

## Ubuntu
Stop the old globalmp-server process before starting the new relay (both bind UDP 8365).

```bash
cd /root/GlobalMP-New
git pull
cmake -S NativeServer -B native-server-build -DETS2MP_SOURCE=/root/ETS2MP
cmake --build native-server-build -j4
./native-server-build/globalmp-relay 8365
```

If you have not cloned GlobalMP-New on the VPS yet:
```bash
cd /root
git clone https://github.com/Benlee2k22/GlobalMP-New.git
```

## Windows
Pull the repo and download the new native client artifact from GitHub Actions. Place globalmp-client.exe next to GlobalMP.Launcher.exe.

Run two client terminals with unique nicknames:
```powershell
.\globalmp-client.exe 178.238.226.59 8365 Alice 1
.\globalmp-client.exe 178.238.226.59 8365 Bob 2
```
The optional final argument is a synthetic test-position offset. Both clients should show `JOIN_CONFIRMED` and `REMOTE_POSITION` output. The relay should show two JOIN lines.

**This does not yet display a second truck inside ETS2.**
