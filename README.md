# FruitNinja
Win32API game which was written as a project for Programming in Graphical Environments course at Warsaw University of Technology. The basic idea was to create a game with a logic of popular moblie game FruitNinja.
# 1. Game logic 🎮
Main idea of the game is to cut flying objects-fruits with the cursor-knife and collect as many points as possible. The fruits can be cut into smaller and smaller and smaller... pieces.
The duration of the game is set to 30 seconds (but as well as other parameters, can be easily changed in `FruitNinja.h` file).
# 2. Functionalities 📄
- the main window has fixed size and positioning and is always on top of other windows
- after not moving mouse for 3 seconds, the window becomes transparent
- game window has the **main board**, **progress bar** (which indicates the left time) and **score** (placed in right top corner)
- there are 3 optional board sizes- **small**, **medium**, **large** which can be choosen from menu
- the choice of board size is saved to `.init` file and restore with a new game
- in the menu bar there is also **new game** option which starts a new game
- the gravity is acting on moving fruits
- after end of the game, all objects which were moving stop and user can see the final score
# 3. Remarks 👋
The game uses mostly *GDI objects* (not *GDI+*) as it was one of the conditions for the task 🙂
