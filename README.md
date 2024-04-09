# BasketBlitz

BasketBlitz is a two-player basketball game written in C for the DE1-SoC board designed to offer players an immersive experience of competing against each other in a timed basketball shooting contest. This game utilizes the PS2 keyboard with interrupts for controls, as well as the hardware timer with interrupts to handle game time. Double buffering is utilized to dynamically show graphics on the VGA.

![ezgif com-video-to-gif-converter](https://github.com/ashleyleal/BasketBlitz/assets/69736735/f93a1ba3-67e8-4dbb-a8be-e7bedebaad20)

Full Demo: https://www.youtube.com/watch?v=Kv7HDWRfsds&ab_channel=AshleyL

## Setup
> Go to CPUlator Nios II System Simulator: https://cpulator.01xz.net/?sys=nios-de1soc

![image](https://github.com/ashleyleal/BasketBlitz/assets/69736735/0c85bb7b-1015-4e28-beea-d9169c15c9b0)

> Change the language in the dropdown menu from Nios II to C

![image](https://github.com/ashleyleal/BasketBlitz/assets/69736735/96da300c-418c-430f-800d-29b8845876e0)

> Disable (uncheck) all Debugging Checks warnings in the Settings (on the left menu)

![image](https://github.com/ashleyleal/BasketBlitz/assets/69736735/2da5fb67-f66a-45da-92e2-b4790d32c4ee)

> Download BasketBlitz.c and open it in through the File dropdown

![image](https://github.com/ashleyleal/BasketBlitz/assets/69736735/6a5e6f1d-6e99-4276-940b-4b930018f8eb)

> Press Compile and Load (or press F5), then click Continue (or press F3) to start the program

![image](https://github.com/ashleyleal/BasketBlitz/assets/69736735/d657d5d6-80e1-4954-8e03-fc8b7c9d7ffd)
![image](https://github.com/ashleyleal/BasketBlitz/assets/69736735/013cfeb2-ead3-4fe3-b076-c54226a7bcf4)

> View the game graphics through the VGA pixel buffer and use the PS/2 keyboard for controls (on the right menu)

![image](https://github.com/ashleyleal/BasketBlitz/assets/69736735/72a71781-557d-44be-b7eb-71be5f523a37)

## How to Play
### Controls
- Left Arrow Key: Increase launch angle
- Right Arrow Key: Decrease launch angle
- Up Arrow Key: Increase shooting power
- Down Arrow Key: Decrease shooting power
- W Key: Move basketball up
- A Key: Move basketball left
- S Key: Move basketball down
- D Key: Move basketball right
- Spacebar: Shoot basketball!

### Gameplay
- Player 1 plays Round 1 and Player 2 plays Round 2.
- Score as many points as possible to win against your opponent!
- Utilize the backboard to your advantage for rebound shots (tip).

## Team
| Ashley Leal | Zuha Mujib |
| ------------- | ------------- |
| ![image](https://github.com/ashleyleal/BasketBlitz/assets/69736735/3869dfae-9f6b-4278-abcb-f244c72e499b) | ![image](https://github.com/ashleyleal/BasketBlitz/assets/69736735/ece84653-b93d-4aa6-9ae1-89e938d3ce8e)|
| [@ashleyleal](https://github.com/ashleyleal) | [@zuhamujib](https://github.com/zuhamujib) |
