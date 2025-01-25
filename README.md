
# pengWin
pengWin is a fully external hack for Counter-Strike\: 2.
In comparison to internal cheats (e.g. AimTux) it does not inject any code into the game.
Unlike other external hacks it does not write to the games memory at all.
You also have the option to use a kernel module for accessing the game's memory which further increases security.

# Current Features:
- External Radar
- Aimbot
- Visibility check (BSP parsing)
- Loading settings from file
- Defining Hotkeys
- Panic key

# Todo:
- Sound ESP
- BlockBot

# Building
Dependencies:
- boost
- xdotool
- python3 + matplotlib + numpy

The project uses CMake and you can build by executing `cmake -S . -B build` and then `cmake --build build/ --target all`.
Run `chmod 100 build/src/pengWin` and `chown root build/src/pengWin` to make the hack unaccessible for VAC.

# Usage
In case you want to use the kernel module for instead of reading the memory via `process_vm_read`
you first nee to load the kernel module with `insmod build/kernel_module/mem_access.ko`.
Then run `sudo ./pengWin [options]`

Possible options are:
--aimbot
--bhop
--debug
--radar
--trigger
