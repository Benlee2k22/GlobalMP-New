# GlobalMP / hry-core compatibility probe

This is a minimal native hry-core **child plugin**, not a standalone SCS telemetry DLL.
It uses the actual hry-core `Plugin` / `INIT_PLUGIN` interface and `onImGuiRender` callback.
No ETS2 internal functions are hooked, no memory is modified, and no remote vehicle is spawned.

## Build
The upstream hry-core project is a Windows CMake C++17 project, built historically with
clang-cl / MSVC. Its upstream `CMakeLists.txt` already builds an example child plugin.
For an initial proof of concept:

1. Clone https://github.com/Hary309/hry-core with its required vendor dependencies.
2. Copy `GlobalMPProbe.hpp` and `GlobalMPProbe.cpp` into a new folder named
   `globalmp-probe` in that checkout.
3. Append to the upstream root `CMakeLists.txt`:

   ```cmake
   add_library(globalmp-probe SHARED globalmp-probe/GlobalMPProbe.cpp)
   target_link_libraries(globalmp-probe PRIVATE hry-core)
   target_include_directories(globalmp-probe PRIVATE vendor/imgui)
   ```

4. Configure/build with Visual Studio CMake tools. Deploy hry-core using its
   upstream instructions, then install the child plugin where that version of
   hry-core expects it. Do **not** assume the child DLL alone works in ETS2's
   native telemetry plugins directory.

## Pass / fail
Pass: ETS2 1.61 starts, hry-core loads, and a GlobalMP status window appears.
Fail: incompatible API/ABI, rendering hook or loading process.

The framework explicitly says it is *not* a game binding and still requires
game-specific function discovery for vehicle spawning and Convoy internals.
This test only validates its loader, UI and native plugin path on the user's
specific ETS2 build. **Not yet compiled or tested.**
