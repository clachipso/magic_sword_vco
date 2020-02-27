# magic_sword_vco
A chiptune-style voltage controlled oscillator for modular synthesizers based on Arduino.

This module was inspired by classic chiptune sound generators. You can think of
it as a baby version of Edges.

## Features
- 5 selectable chiptune wave forms:
    1. 50% duty cycle pulse wave
    2. 25% duty cycle pulse wave
    3. 12.5% duty cycle pulse wave 
    4. NES style triangle wave
    5. 4 bit sawtooth wave
- 1V/OCT input with nearly 5 full octaves (0V-5V)
- No tuning necessary with stable 5V regulator
- Low parts count, fairly easy/inexpensive to DIY
- Simple design

## Compromises 
- The 1V/OCT sample rate is fairly low in order to get required precision, so 
audio rate FM won't work
- Only 1 output
