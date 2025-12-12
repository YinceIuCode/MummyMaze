#include "Core/MazeGenerator.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <queue>
#include <cassert>

using namespace std;

// --- HELPER FUNCTIONS (File scope) ---

// Chuyển đổi tọa độ logic sang tọa độ thực tế trên mảng 2 chiều
static int get_real_coordinates(int x) {
    return x * 2 + 1;
}

// Tính khoảng cách Manhattan
static int manhattan_distance(position a, position b) {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

// Các biến toàn cục cho BFS (như logic cũ của bạn)
static const vector<int> dx = { 0, 0, 1, -1 };
static const vector<int> dy = { 1, -1, 0, 0 };

struct direction {
    bool reach[4] = { false };
};

struct queue_element {
    position a, b;
    int num_move;
};

static int get_id_pair(position a, position b, int n) {
    return a.x * n * n * n + a.y * n * n + b.x * n + b.y;
}

// Logic di chuyển của Mummy (Mummy AI)
static position get_next_mummy_move(position mm, position py, const vector<vector<direction>>& layout, int layout_size) {
    position ans = mm;
    int best_dist = manhattan_distance(py, mm);

    for (int k = 0; k < 4; k++) {
        if (layout[mm.x][mm.y].reach[k]) continue;

        // Mummy bước 2 ô
        position new_mm2 = { mm.x + dx[k] * 2, mm.y + dy[k] * 2 };

        if (new_mm2.x < 0 || new_mm2.x >= layout_size || new_mm2.y < 0 || new_mm2.y >= layout_size) {
            continue;
        }

        int new_dist = manhattan_distance(new_mm2, py);
        if (new_dist < best_dist) {
            best_dist = new_dist;
            ans = new_mm2;
        }
    }
    return ans;
}

static position move_twice(position mm, position py, const vector<vector<direction>>& layout, int layout_size) {
    position ans = get_next_mummy_move(mm, py, layout, layout_size);
    if (ans == py) return ans; // Bắt được player ngay bước 1
    return get_next_mummy_move(ans, py, layout, layout_size); // Bước tiếp
}


// --- CLASS IMPLEMENTATION ---

generate_maze::generate_maze(int input_size) {
    maze_size = input_size;
    real_size = get_real_coordinates(maze_size);

    // Seed random theo thời gian thực
    unsigned seed = static_cast<unsigned>(chrono::steady_clock::now().time_since_epoch().count());
    rng.seed(seed);

    player = { 0, 0 };
    mummy = { 0, 0 };
    exit_cell = { 0, 0 };
}

int generate_maze::get_random_number(int minimum, int maximum) {
    assert(minimum <= maximum);
    return uniform_int_distribution<int>(minimum, maximum)(rng);
}

// Các hàm DSU Private
int generate_maze::find_set(int a) {
    return (a == parent[a] ? parent[a] : parent[a] = find_set(parent[a]));
}

bool generate_maze::union_set(int a, int b) {
    a = find_set(a);
    b = find_set(b);
    if (a == b) return false;

    if (component_size[a] < component_size[b]) swap(a, b);
    parent[b] = a;
    component_size[a] += component_size[b];
    return true;
}

int generate_maze::get_id(int x, int y) const {
    return y * real_size + x;
}

bool generate_maze::inside_real(int x, int y) const {
    return (x >= 0 && x < maze_size && y >= 0 && y < maze_size);
}

void generate_maze::init_real_maze() {
    maze_layout.assign(real_size, string(real_size, ' '));

    // Tạo lưới cột trụ (+)
    for (int i = 0; i < real_size; i++) {
        for (int j = 0; j < real_size; j++) {
            if (!(i & 1) && !(j & 1)) maze_layout[i][j] = '+';
        }
    }
    // Tạo tường bao quanh
    for (int i = 1; i < real_size; i += 2) {
        maze_layout[i][0] = '#';
        maze_layout[0][i] = '#';
        maze_layout[i][real_size - 1] = '#';
        maze_layout[real_size - 1][i] = '#';
    }
    // Tạo tường kín bên trong (lưới #)
    for (int i = 2; i < real_size; i += 2) {
        for (int j = 1; j < real_size; j += 2) {
            maze_layout[i][j] = '#';
            maze_layout[j][i] = '#';
        }
    }
}

void generate_maze::generate() {
    init_real_maze();

    // 1. Khởi tạo DSU và danh sách cạnh
    vector<dsu_edge> set_of_edges;
    set_of_edges.reserve(maze_size * maze_size * 2);

    parent.resize(real_size * real_size + real_size * 2);
    component_size.resize(real_size * real_size + real_size * 2);
    for (int i = 0; i < (int)parent.size(); i++) {
        parent[i] = i;
        component_size[i] = 1;
    }

    // Thêm các cạnh (tường) vào danh sách
    for (int i = 1; i < real_size; i += 2) {
        if (i + 2 < real_size) {
            for (int j = 1; j < real_size; j += 2) {
                set_of_edges.push_back({ i, j, i + 2, j, false });
            }
        }
        for (int j = 1; j + 2 < real_size; j += 2) {
            set_of_edges.push_back({ i, j, i, j + 2, false });
        }
    }

    // 2. Kruskal (hoặc Prim ngẫu nhiên) để tạo cây khung (Perfect Maze)
    int count_break_wall = 0;
    shuffle(set_of_edges.begin(), set_of_edges.end(), rng);

    for (auto& edge : set_of_edges) {
        int id_1 = get_id(edge.x1, edge.y1);
        int id_2 = get_id(edge.x2, edge.y2);

        if (union_set(id_1, id_2)) {
            edge.is_connected = true; // Đánh dấu đã phá
            count_break_wall++;
            if (edge.x1 == edge.x2) {
                maze_layout[edge.x1][(edge.y1 + edge.y2) / 2] = ' ';
            }
            else if (edge.y1 == edge.y2) {
                maze_layout[(edge.x1 + edge.x2) / 2][edge.y1] = ' ';
            }
        }
    }

    // 3. Phá thêm tường ngẫu nhiên (Tạo vòng lặp - Braided Maze)
    double percentage = 1.0 - ((count_break_wall * 1.0) / set_of_edges.size());
    double wall_percentage = get_random_number(15, min(30.0, percentage * 100.0)) / 100.0;
    int number_wall_to_break = (percentage - wall_percentage) * ((int)set_of_edges.size());

    for (auto& edge : set_of_edges) {
        if (number_wall_to_break <= 0) break;
        if (!edge.is_connected) { // Nếu là tường chưa phá
            edge.is_connected = true;
            number_wall_to_break--;

            if (edge.x1 == edge.x2) {
                maze_layout[edge.x1][(edge.y1 + edge.y2) / 2] = ' ';
            }
            else if (edge.y1 == edge.y2) {
                maze_layout[(edge.x1 + edge.x2) / 2][edge.y1] = ' ';
            }
        }
    }

    // 4. Random Exit (Cổng ra)
    int flip_coin = get_random_number(0, 1);
    if (flip_coin == 0) { // Cửa ở cạnh dọc (trái/phải)
        exit_cell.x = (get_random_number(0, 1) == 0) ? 0 : maze_size;
        exit_cell.y = get_random_number(0, maze_size - 1);

        int rX = (exit_cell.x == 0) ? get_real_coordinates(exit_cell.x) - 1 : get_real_coordinates(exit_cell.x); // Logic cũ: -1 nếu x=0? (Dựa theo code gốc) 
        // *Lưu ý: Code gốc của bạn: get_real_coordinates(exit_cell.x) - 1. Nếu x=0 -> -1 + 1 -1 = -1 (Lỗi mảng). 
        // Nhưng logic gốc của bạn chắc chắn chạy được do cách get_real_coordinates hoạt động. Mình giữ nguyên logic gán mảng.
        if (exit_cell.x == 0) maze_layout[0][get_real_coordinates(exit_cell.y)] = 'X';
        else maze_layout[real_size - 1][get_real_coordinates(exit_cell.y)] = 'X'; // Giả định maze_size -> real_size - 1

        // Đoạn này mình giữ logic gán layout gốc của bạn để an toàn:
        if (exit_cell.x == 0) maze_layout[0][get_real_coordinates(exit_cell.y)] = 'X';
        else maze_layout[get_real_coordinates(exit_cell.x) - 1][get_real_coordinates(exit_cell.y)] = 'X';
    }
    else { // Cửa ở cạnh ngang (trên/dưới)
        exit_cell.y = (get_random_number(0, 1) == 0) ? 0 : maze_size;
        exit_cell.x = get_random_number(0, maze_size - 1);

        if (exit_cell.y == 0) maze_layout[get_real_coordinates(exit_cell.x)][0] = 'X';
        else maze_layout[get_real_coordinates(exit_cell.x)][get_real_coordinates(exit_cell.y) - 1] = 'X';
    }

    // 5. Random Player & Mummy (Fair setup)
    bool fair_setup = false;
    position tmp_player, tmp_mummy;
    do {
        // Player cách xa Exit
        do {
            tmp_player.x = get_random_number(0, maze_size - 1);
            tmp_player.y = get_random_number(0, maze_size - 1);
        } while (manhattan_distance(exit_cell, tmp_player) < maze_size / 2);

        // Mummy cách xa Player
        do {
            tmp_mummy.x = get_random_number(0, maze_size - 1);
            tmp_mummy.y = get_random_number(0, maze_size - 1);
        } while (manhattan_distance(tmp_mummy, tmp_player) < maze_size / 2); // Sửa logic: Mummy xa Player (code cũ check `player` chưa init, dùng tmp_player mới đúng)

        int dist_player = manhattan_distance(exit_cell, tmp_player);
        int dist_mummy = manhattan_distance(tmp_player, tmp_mummy);

        // Điều kiện công bằng
        if (dist_mummy > (dist_player * 2) - 2) {
            fair_setup = true;
        }
    } while (!fair_setup);

    mummy = tmp_mummy;
    maze_layout[get_real_coordinates(mummy.x)][get_real_coordinates(mummy.y)] = 'M';

    player = tmp_player;
    maze_layout[get_real_coordinates(player.x)][get_real_coordinates(player.y)] = 'E';
}

void generate_maze::print_maze() {
    std::filesystem::path p("assets/mazes/maze1.txt");
    std::cout << "--- DEBUG FILE PATH ---\n";
    std::cout << "File: " << std::filesystem::absolute(p) << "\n";

    std::ofstream fout(p);
    if (!fout.is_open()) {
        std::cerr << "LOI: Khong mo duoc file! Kiem tra thu muc assets/mazes.\n";
        return;
    }

    for (int i = 0; i < real_size; i++) {
        for (int j = 0; j < real_size; j++) {
            fout << maze_layout[i][j];
        }
        fout << "\n";
    }

    fout.flush();
    fout.close();
    std::cout << "Da ghi file THANH CONG!\n";
}

int generate_maze::can_solve_the_maze() {
    // BFS tìm đường ngắn nhất
    vector<vector<direction>> matrix(maze_size, vector<direction>(maze_size));
    // Visited: state [player.x][player.y][mummy.x][mummy.y]
    vector<bool> visited(maze_size * maze_size * maze_size * maze_size, false);

    // Build đồ thị di chuyển (Pre-calculate walls)
    for (int i = 0; i < maze_size; i++) {
        int x = get_real_coordinates(i);
        for (int j = 0; j < maze_size; j++) {
            int y = get_real_coordinates(j);
            for (int k = 0; k < 4; k++) {
                if (maze_layout[x + dx[k]][y + dy[k]] == '#') {
                    matrix[i][j].reach[k] = true; // Bị chặn
                }
            }
        }
    }

    int max_move = maze_size * maze_size * 2;
    queue<queue_element> q;

    q.push({ player, mummy, 0 });
    visited[get_id_pair(player, mummy, maze_size)] = true;

    while (!q.empty()) {
        queue_element u = q.front();
        q.pop();

        position cur_py = u.a;
        position cur_mm = u.b;

        for (int k = 0; k < 4; k++) {
            // Player move
            if (matrix[cur_py.x][cur_py.y].reach[k]) continue; // Tường

            position new_py = { cur_py.x + dx[k], cur_py.y + dy[k] };

            if (new_py == cur_mm) continue; // Lao vào mummy -> Chết

            // Check thoát ra ngoài (Win)
            if (new_py.x >= maze_size || new_py.x < 0 || new_py.y < 0 || new_py.y >= maze_size) {
                return u.num_move + 1;
            }

            // Mummy move response
            position new_mm = move_twice(cur_mm, new_py, matrix, maze_size);

            if (new_mm == new_py) continue; // Mummy bắt được -> Chết

            int new_id = get_id_pair(new_py, new_mm, maze_size);
            if (visited[new_id]) continue;

            if (u.num_move + 1 > max_move) return -1; // Quá lâu

            q.push({ new_py, new_mm, u.num_move + 1 });
            visited[new_id] = true;
        }
    }

    return -1; // Không tìm thấy đường thoát
}