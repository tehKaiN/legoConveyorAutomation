# LEGO Conveyor Automation

These are remains of a project i did during my automation studies. It's an attempt to use real electronic parts and motors to control a LEGO model. It's mostly complete but needs some additional work. This project consists of:

- [LDCad](http://www.melkert.net/LDCad/) model of a simple conveyor-belt base production facility
- [KiCad](http://kicad-pcb.org/) schematic of electronic wiring
- [PlatformIO](https://platformio.org/) project for LOLIN32-lite board (any Arduino will do)

Some photos are available here: https://imgur.com/a/gFlp7bf. Unfortunately I haven't recorded it in action before disassembly of electronics, but you have my word that conveyor and ball dispensers are moving as intended.

## Notes on building LEGO model

LDCad model from this repo is an attempt to digitize a structure which I hand-built without too much planning. During transition to digital model I've realised there are lots of inconsistencies which result in stress put into LEGO parts - many things don't fit nicely, so this should be fixed.

If you plan to build this thing I suggest you take a look at LDCad model first and try to fix it - those tensions result in excessive friction in conveyor and ball dispenser mechanisms.
