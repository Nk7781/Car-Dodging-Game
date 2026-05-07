# Car Dodging Game 🚗💨

A fast-paced **2D Car Dodging Game** developed in **C++ using SFML** where the player controls a car, avoids incoming traffic, and survives as long as possible while the game difficulty continuously increases.

---

## 🎮 Features

* Smooth car movement system
* Multiple enemy cars
* Dynamic difficulty scaling
* Easy / Medium / Hard modes
* Increasing game speed over time
* Collision detection system
* Infinite scrolling road effect
* Score and speed tracking
* Restart functionality
* Object-Oriented game architecture

---

## 📂 Project Structure

```bash id="w4g6r3"
CAR-DODGING-GAME/
│
├── Assets/
│   ├── fonts/
│   ├── graphics/
│   └── screenshots/
│
├── include/
│   ├── Car.h
│   ├── PlayerCar.h
│   └── EnemyCar.h
│
├── Car.cpp
├── PlayerCar.cpp
├── EnemyCar.cpp
├── Game.cpp
│
└── README.md
```

---

## 🛠 Technologies Used

* **C++**
* **SFML 2.5+**
* Object-Oriented Programming
* Real-time Game Development

---

## 🎯 Gameplay

The player controls a white car and must avoid incoming enemy vehicles while the game speed gradually increases. The longer you survive, the higher your score becomes.

---

## 🎮 Controls

| Key           | Action            |
| ------------- | ----------------- |
| `LEFT ARROW`  | Move Left         |
| `RIGHT ARROW` | Move Right        |
| `ENTER`       | Start Game        |
| `R`           | Restart Game      |
| `1`           | Easy Difficulty   |
| `2`           | Medium Difficulty |
| `3`           | Hard Difficulty   |

---

## ⚙️ Difficulty Modes

| Difficulty | Spawn Rate | Enemy Speed | Overall Difficulty |
| ---------- | ---------- | ----------- | ------------------ |
| Easy       | Slow       | Slow        | Beginner Friendly  |
| Medium     | Balanced   | Moderate    | Standard Gameplay  |
| Hard       | Fast       | Very Fast   | Challenging        |

---

## 🧠 OOP Concepts Used

* Inheritance
* Encapsulation
* Polymorphism
* Class-based Design
* Arrays of Objects
* Game State Management

---

## 🚗 Class Structure

### `Car` Class

Base class for all vehicles.

Features:

* Position handling
* Speed handling
* Sprite rendering
* Movement updates

Files:

* `Car.h` 
* `Car.cpp` 

---

### `PlayerCar` Class

Derived from `Car`.

Features:

* Player controls
* Boundary checking
* Lane restriction

Files:

* `PlayerCar.h` 
* `PlayerCar.cpp` 

---

### `EnemyCar` Class

Derived from `Car`.

Features:

* Enemy movement
* Off-screen detection
* Enemy lane management
* Collision positioning

Files:

* `EnemyCar.h` 
* `EnemyCar.cpp` 

---

## 🎮 Main Game Logic

`Game.cpp` handles:

* Game loop
* Difficulty selection
* Enemy spawning system
* Dynamic speed scaling
* Collision detection
* Score system
* UI rendering
* Restart logic
* Road animation

Source: 

---

## 📸 Screenshots

### Starting Window


![Starting Window](Assets/Screenshots/Starting_Window.png)


### Difficulty Selection


![Difficulty Window](Assets/Screenshots/Difficulty_Level_Window.png)


### Gameplay

![Gameplay](Assets/Screenshots/GamePlay.png)


### Game Over Screen


![Game Over](Assets/Screenshots/Game_Over.png)


---

## ▶️ How to Run

### 1️⃣ Install SFML

Download SFML from:

[SFML Official Website](https://www.sfml-dev.org/?utm_source=chatgpt.com)

---

### 2️⃣ Compile the Project

Using **g++**:

```bash id="9v5nsq"
g++ Game.cpp Car.cpp PlayerCar.cpp EnemyCar.cpp -o Game ^
-lsfml-graphics -lsfml-window -lsfml-system
```

---

### 3️⃣ Run the Game

```bash id="jlwmmo"
./Game
```

Or run:

```bash id="k7ebud"
Game.exe
```

---

## 🚀 Future Improvements

* Sound effects and background music
* High score saving system
* Pause menu
* More enemy vehicle types
* Nitro boost system
* Multiplayer mode
* Mobile support
* Better animations and particle effects

---

## 👨‍💻 Author

**Neelakantha Sahu**

---

## 📜 License

This project is developed for educational and learning purposes.
