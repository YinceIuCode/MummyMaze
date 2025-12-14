# MUMMY MAZE - SYSTEM PROGRAMMING PROJECT 🏺

> **Đồ án môn học: Nhập môn Game Development / Lập trình C++**
> *Một tựa game giải đố chiến thuật được xây dựng từ đầu (from scratch) với C++ và SFML.*

## 📖 Giới Thiệu (Overview)

**Mummy Maze** là dự án tái hiện tựa game giải đố kinh điển của PopCap. Trong game, người chơi vào vai nhà thám hiểm bị kẹt trong kim tự tháp và phải tìm cách thoát ra ngoài trước khi bị Xác ướp bắt kịp.

Dự án này tập trung vào việc áp dụng các kiến thức Lập trình hướng đối tượng (OOP), Quản lý bộ nhớ và Cấu trúc dữ liệu giải thuật trong C++.

### 🛠 Công Nghệ Sử Dụng
* **Ngôn ngữ:** C++ (Standard 17)
* **Thư viện đồ họa:** SFML 3.0
* **Mô hình kiến trúc:** State Pattern (Quản lý các màn hình chơi)
* **Công cụ:** CMake, Visual Studio / VS Code

---

## ✨ Các Tính Năng Kỹ Thuật (Key Features)

Dưới đây là các tính năng chính đã được hiện thực trong Source Code:

### 1. Hệ Thống Gameplay & AI
* **Pathfinding AI:** Xác ướp (Mummy) sử dụng thuật toán tìm đường (A* / Greedy) để tự động truy đuổi người chơi sau mỗi lượt đi.
* **Logic Va Chạm:** Xử lý va chạm chuẩn xác giữa Player, Mummy, Tường và Cửa thoát hiểm.

### 2. Cấu Trúc Dữ Liệu & Thuật Toán
* **Undo/Redo System:** Sử dụng cấu trúc dữ liệu **Stack (Ngăn xếp)** để quản lý lịch sử di chuyển (phím `U` / `I`), cho phép hoàn tác không giới hạn.
* **Save/Load System:** Hệ thống lưu trữ trạng thái màn chơi hiện tại (vị trí nhân vật, map, theme) vào file text/binary (phím `P`).

### 3. Hệ Thống Giao Diện (UI/UX)
* **State Machine:** Chuyển đổi mượt mà giữa các trạng thái: *MainMenu, GameLoop, Settings, Customize, Credits*.
* **Assets Management:** Quản lý tài nguyên tập trung.
* **Theme Customization:** Hỗ trợ thay đổi Skin nhân vật và Tileset bản đồ (Ai Cập, Rừng Rậm...).

---

## 📂 Cấu Trúc Source Code (Project Structure)

```text
MummyMaze/
├── src/                    # Chứa mã nguồn (.cpp)
│   ├── Core/               # Các lớp lõi (Game, GameData, Map...)
│   ├── Entities/           # Các thực thể (Player, Mummy...)
│   ├── States/             # Các màn hình (MenuState, GameState...)
│   └── GUI/                # Các thành phần giao diện (Button...)
├── include/                # Chứa file header (.hpp)
├── assets/                 # Tài nguyên game (Ảnh, Nhạc, Font, Map levels)
├── CMakeLists.txt          # Cấu hình build hệ thống
└── README.md               # Tài liệu dự án
```
## 🎮 Điều Khiển (Controls)

| Phím (Key) | Chức Năng | Mô Tả |
| :--- | :--- | :--- |
| <kbd>W</kbd> <kbd>A</kbd> <kbd>S</kbd> <kbd>D</kbd> | **Di chuyển** | Điều khiển nhân vật đi Lên, Trái, Xuống, Phải (Hoặc dùng phím Mũi tên) |
| <kbd>U</kbd> | **Undo** | Quay lại nước đi trước đó (Hoàn tác không giới hạn) |
| <kbd>I</kbd> | **Redo** | Đi lại nước vừa hoàn tác (Nếu lỡ bấm Undo nhầm) |
| <kbd>P</kbd> | **Save Game** | Lưu lại vị trí và màn chơi hiện tại vào bộ nhớ |
| <kbd>R</kbd> | **Reset Game** | Tải lại màn chơi ban đầu |
| <kbd>🖱️ Chuột</kbd> | **Tương tác UI** | Click chọn Menu, Cài đặt, Đổi Skin, Tạm dừng |
---

## 👥 Thành Viên Thực Hiện (Credits)

Dự án được thực hiện bởi nhóm 5 thành viên lớp CNTT - KHTN:

### 👨‍💻 Developers (Core & Gameplay Logic)
* **Nguyễn Quốc Tuấn** - *Team Leader, AI Algorithm (Mummy Logic)*
* **Trịnh Trần Hương Mai** - *Maze Generate (Logic)*

### 🖥️ UI Programmers (System & Interface)
* **Hoàng Đức Vinh** - *Menu System, Settings, Sound Manager, State Management, Input Handling*

### 🎨 UI Designer (Assets & Graphics)
* **Thái Mạc Tường Vi** - *Pixel Art, UI Design, Map Level Design*
* **Võ Lê Hoàng** - *Pixel Art, UI Design, Map Level Design*

---

## 🔗 Liên Kết & Mã Nguồn

Source code được lưu trữ và quản lý phiên bản tại GitHub:

👉 **[https://github.com/YinceluCode/MummyMaze](https://github.com/YinceluCode/MummyMaze)**

---

## 📝 License & Note
*Dự án này được thực hiện cho mục đích học tập và nghiên cứu.*
*Source code thuộc về nhóm tác giả.*
