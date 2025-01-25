
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

Use the included Makefile to build.
Then run `chmod 100 pengWin` and `chown root pengWin` to make the hack unaccessible for VAC.

# Usage
`sudo ./pengWin [options]`
Possible options are:
--aimbot
--bhop
--debug
--radar
--trigger
