# FancyWatchOS
Custom OS for the LilyGo TTGO-TWatch 2020.

## Building

Project is built and deployed using [platformio](https://platformio.org). Use command `pio run -t upload` while the watch is connected.

## Creating new apps

At present, apps for the watch are designed to run on the same thread, but using a game-engine like component design so multiple apps can operate seemingly simultaneously. Input handling is all done using an event system similar to SDL 2, which I'm in the process of implementing.
