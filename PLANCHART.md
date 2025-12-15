# System Maze Game - Project Architecture

This project is a 2D Maze Game developed using **C++** and **SFML**. The game features a dynamic maze generation algorithm, an intelligent enemy AI (Mummy), and a robust State Management system.

## 🏗️ System Architecture

The project follows the **State Pattern** to manage different game screens (Menu, Gameplay, Settings). Below are the high-level architectural diagrams.

### 1. Core Structure
The `Game` class acts as the engine, managing the window and the stack of States.

```text
+-----------------------+
|       Main.cpp        |
+-----------------------+
            |
            v (Initializes)
+-----------------------+         +--------------------------+
|       Game.cpp        | <-----> |       GameData.hpp       |
| (Window Management &  |         | (Global Settings,        |
|  State Stack)         |         |  Volume, Assets)         |
+-----------------------+         +--------------------------+
            |
            | (Manages)
            v
    +----------------+
    |  State Stack   |
    +----------------+
            |
            v
  +-------------------+
  |   MainMenuState   |
  +-------------------+
```
### 2. Game Navigation Flow (State Machine)

This diagram illustrates the transitions between different game states. The **MainMenu** serves as the central hub, allowing access to various sub-systems like customization, settings, and the gameplay loop.

```text
                                   +----------------------+
                                   |    MainMenuState     |
                                   +----------------------+
                                    /     |        |     \
                  (Click "Play")   /      |        |      \ (Click "Resume")
                                  /       |        |       \
                                 v        |        |        v
 +-------------------+    +-------------+ |        |    +-------------+
 |  ModeSelectState  |    | Sub-Menus   | |        |    |  GameState  |
 | (Select 6x6, 8x8) |    |             | |        |    | (Load Data) |
 +-------------------+    +-------------+ |        |    +-------------+
          |               | SettingState|<+        |           ^
          | (Generate)    +-------------+          |           |
          |               |CustomizeState|<--------+           |
          v               +-------------+          |           |
 +-------------------+    |HowToPlayState|<--------+           |
 |     GameState     |    +-------------+                      |
 |  (New Gameplay)   |                                         |
 +-------------------+                                         |
          |                                                    |
          +------------------- (Victory / Defeat) -------------+
                     (Returns to Menu or Replays)
```
### 3. Gameplay Architecture (GameState)

Inside the main game session, `GameState` acts as the central controller. It manages the interactions between the player, the enemy AI, the environment, and the time-travel mechanics.

```text
+-----------------------------------------------------------------------+
|                        GameState (Controller)                         |
|   - Handles Input (Keyboard/Mouse)                                    |
|   - Updates Game Loop (Delta Time)                                    |
|   - Checks Win/Lose Conditions                                        |
+-----------------------------------------------------------------------+
       |                  |                        |
       | (Updates)        | (Manages)              | (Renders)
       v                  v                        v
+-------------+    +------------------+    +----------------------+
|  Entities   |    |   Core Systems   |    |    User Interface    |
+-------------+    +------------------+    +----------------------+
|             |    |                  |    |                      |
|  [Player] <----->|  [TimeControl]   |    |  [Buttons]           |
|     ^       |    |   (Undo/Redo)    |    |   - Undo / Redo      |
|     |       |    |   (Stack Data)   |    |   - Save / Exit      |
|   (Hunts)   |    |                  |    |                      |
|     |       |    +------------------+    |  [Popup Panel]       |
|  [Mummy]    |                            |   - Exit Confirm     |
|     ^       |                            |   - Win/Lose Msg     |
|     |       |                            |                      |
|  (Checks)   |                            +----------------------+
|     v       |
|  [Map/Maze] |
| (Collision) |
+-------------+
```
## 📂 Project Structure & Responsibilities

The codebase is organized into modular components to ensure separation of concerns.

| Component Group | File Name | Description & Responsibility |
| :--- | :--- | :--- |
| **1. Core Engine** | `Game.cpp` | **Application Lifecycle:** Manages the main game loop, window creation, event polling, and global resource initialization (Music). |
| | `State.hpp` | **Base Class:** Abstract class defining the contract for all game screens (Init, Update, Render). |
| | `GameData.hpp` | **Global Context:** Stores shared settings accessible across states (Volume levels, Current Theme index). |
| | `TimeControl.hpp` | **System:** Implements the *Memento Pattern* to handle the **Undo/Redo** mechanics by storing game snapshots. |
| **2. Game States** | `MainMenuState` | **Navigation Hub:** The entry point handling transitions to Gameplay, Settings, or Help screens. |
| | `ModeSelectState` | **Level Setup:** Handles difficulty selection and triggers the procedural maze generation logic. |
| | `GameState` | **Gameplay Controller:** The core logic hub. Manages the turn-based system, collision detection, win/lose conditions, and save/load operations. |
| | `Setting/Customize` | **User Preferences:** Handles audio volume sliders and theme switching logic. |
| **3. Entities** | `Player.cpp` | **Character Logic:** Handles user input, grid-based movement, and smooth visual interpolation. |
| | `Mummy.cpp` | **Enemy AI:** Implements pathfinding algorithms (Greedy/Random) to chase the player based on Manhattan distance. |
| **4. World Generation** | `Maze.cpp (Map)` | **Environment:** Renders the grid, walls, and floors. Handles coordinate conversion (Grid Space ↔ Pixel Space). |
| | `MazeGenerator.hpp` | **Algorithm:** Uses *DFS (Depth-First Search)* or *Prim's Algorithm* to generate solvable mazes dynamically. |
| **5. GUI** | `Button.cpp` | **UI Component:** A reusable widget class handling hover effects, click events, audio feedback, and breathing animations. |

## 🛠️ Technologies & Algorithms

This project demonstrates the use of Modern C++ standards and professional game development patterns.

### 1. Core Stack
* **Language:** C++17 (Modern C++)
    * *Features used:* Smart Pointers (`std::unique_ptr`), Lambdas, `std::optional`, `std::filesystem`, STL Containers.
* **Library:** SFML (Simple and Fast Multimedia Library)
    * *Modules:* `sf::Graphics`, `sf::Audio`, `sf::Window`, `sf::System`.

### 2. Key Algorithms
* **Procedural Generation:** The mazes are generated dynamically using **Randomized Depth-First Search (DFS)** (Backtracking), ensuring a solvable path every time.
* **Enemy AI (Pathfinding):** The "Mummy" entity uses a **Greedy Best-First Search** strategy based on **Manhattan Distance** to calculate the most efficient path to the player.
* **Game Loop:** Implements a fixed time-step game loop for consistent physics and logic updates (`dt` calculation).

### 3. Design Patterns
* **State Pattern:** Manages game screens (`MainMenu`, `Game`, `Settings`) efficiently, allowing for easy transitions and resource management.
* **Memento Pattern:** Implemented in the `TimeControl` class to handle the **Undo/Redo** functionality (saving and restoring game snapshots).
* **Component-based UI:** The `Button` class encapsulates logic, rendering, and audio, making the UI code modular and reusable.

---
*Developed by Hoàng Đức Vinh - 2025*