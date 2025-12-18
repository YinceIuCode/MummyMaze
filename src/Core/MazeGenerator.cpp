#include "../../include/Core/MazeGenerator.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <queue>
#include <cassert>

int generate_maze::get_random_number(int minimum, int maximum) {
    assert(minimum <= maximum);
    return uniform_int_distribution<int>(minimum, maximum)(rng);

}
int get_real_coordinates(int x) {
    return x * 2 + 1;
}
generate_maze::generate_maze(int input_size) { //init map
    maze_size = input_size;
    real_size = get_real_coordinates(maze_size);

    // phụ thuộc thời gian hệ thống, có thể khi kích thước mê cung quá nhỏ dẫn đến việc 
    //chương trình chạy nhanh -> seed random giống nhau
    // unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    // std::mt19937 rng(seed);

    unsigned seed = (chrono::steady_clock::now().time_since_epoch().count());
    rng.seed(seed);
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

int manhattan_distance(position a, position b) { //mahattan distance from (x,y) to (u, v)
    return abs(a.x - b.x) + abs(a.y - b.y);
}

vector <int> dx = { 0, 0, 1, -1 };
vector <int> dy = { 1, -1, 0, 0 };

int get_id_maze(int x, int y, int n) {
    return x * n + y;
}

void distance_from_cell(vector <int>& distance_exit, position a, int maze_size, const vector <string>& maze_layout) {
    int ans = 0;
    queue <pair<position, int> > q;
    //cout << a.x <<" " << a.y << endl;
    distance_exit[get_id_maze(a.x, a.y, maze_size)] = 0;
    q.push({ a, 0 });
    while (!q.empty()) {
        auto u = q.front();
        int currx = u.first.x, curry = u.first.y;
        // cout << currx << " " << curry << endl;
        int curr_distance = u.second;
        q.pop();
        for (int i = 0; i < 4; i++) {
            int newx = currx + dx[i], newy = curry + dy[i];

            if (newx < 0 || newx >= maze_size || newy < 0 || newy >= maze_size) {
                continue;
            }
            if (maze_layout[get_real_coordinates(currx) + dx[i]][get_real_coordinates(curry) + dy[i]] == '#') {
                continue;
            }
            if (distance_exit[get_id_maze(newx, newy, maze_size)] <= curr_distance + 1) {
                continue;
            }
            distance_exit[get_id_maze(newx, newy, maze_size)] = curr_distance + 1;
            q.push({ {newx,newy}, curr_distance + 1 });
        }
    }
}

void generate_maze::build_maze() {

    init_real_maze();

    vector <dsu_edge> set_of_edges; //init dsu
    set_of_edges.resize(maze_size * maze_size * 2);
    parent.resize(real_size * real_size + real_size * 2);
    component_size.resize(real_size * real_size + real_size * 2);
    for (int i = 0; i < parent.size(); i++) parent[i] = i, component_size[i] = 1;

    for (int i = 1; i < real_size; i += 2) {
        if (i + 2 < real_size) {
            for (int j = 1; j < real_size; j += 2) {
                set_of_edges.push_back({ i,j,i + 2,j,false });
            }
        }
        for (int j = 1; j + 2 < real_size; j += 2) {
            set_of_edges.push_back({ i,j,i,j + 2, false });
        }
    }


    int count_break_wall = 0;

    shuffle(set_of_edges.begin(), set_of_edges.end(), rng);
    for (auto& [a, b, c, d, f] : set_of_edges) {
        int id_1 = get_id(a, b), id_2 = get_id(c, d);
        bool is_connected = union_set(id_1, id_2);
        if (is_connected) {
            f = true; //mark destroyed
            count_break_wall++;
            if (a == c) {
                maze_layout[a][(b + d) / 2] = ' ';
            }
            else if (b == d) {
                maze_layout[(a + c) / 2][b] = ' ';
            }
        }
    }
    //get percentage of wall 
    double percentage = 1.0 - ((count_break_wall * 1.0) / set_of_edges.size() * 1.0); //% tường đang chiếm hiện tại
    double wall_percentage = get_random_number(15.0, min(25.0, percentage * 100)) / 100.0; //có từ 15% -> 40% là tường
    int number_wall = (percentage - wall_percentage) * ((int)set_of_edges.size()); //số tường cần phá thêm
    for (auto& [a, b, c, d, f] : set_of_edges) {
        if (number_wall <= 0) break;
        if (f == false) { // if wall
            f = true;
            number_wall--;
            count_break_wall++;
            int id_1 = get_id(a, b), id_2 = get_id(c, d);
            if (a == c) {
                maze_layout[a][(b + d) / 2] = ' ';
            }
            else if (b == d) {
                maze_layout[(a + c) / 2][b] = ' ';
            }
        }
    }

    //---random exit---
    int flip_coin = get_random_number(0, 1);
    //0-coin => picks random exit cell in vertical
    //1-coin => picks random exit cell in horizontal
    if (flip_coin == 0) {
        //0: (0,0) -> (0,n-1) , 1; (n,0) -> (n-1,n-1)
        flip_coin = get_random_number(0, 1);
        if (flip_coin == 0) {
            exit_cell.x = 0;
        }
        else {
            exit_cell.x = maze_size;
        }

        exit_cell.y = get_random_number(0, maze_size - 1);
        maze_layout[get_real_coordinates(exit_cell.x) - 1][get_real_coordinates(exit_cell.y)] = 'X';
    }
    else {
        //0: (0,0) -> (n-1,0),  1: (0,n-1) -> (n-1,n-1);
        if (flip_coin == 0) {
            exit_cell.y = 0;
        }
        else {
            exit_cell.y = maze_size;
        }
        exit_cell.x = get_random_number(0, maze_size - 1);
        maze_layout[get_real_coordinates(exit_cell.x)][get_real_coordinates(exit_cell.y) - 1] = 'X';
    }
    //---random exit---

}
struct direction {
    bool reach[4] = { false };
    //0: east
    //1: west
    //2: north
    //4: south
};
struct queue_element {
    position a, b;
    int num_move;
};

int get_id_pair(position a, position b, int n) {
    return a.x * n * n * n + a.y * n * n + b.x * n + b.y;
}


position get_next_mummy_move(position mm, position py, vector <vector <direction > > layout, int layout_size) {
    position ans;

    ans = mm;
    int best_dist = manhattan_distance(py, mm);

    for (int k = 0; k < 4; k++) {
        if (layout[mm.x][mm.y].reach[k] == true) {
            continue;
        }
        position new_mm2 = { mm.x + dx[k] * 2, mm.y + dy[k] * 2 };
        if (new_mm2.x < 0 || (new_mm2.x >= layout_size) || new_mm2.y < 0 || new_mm2.y >= layout_size) {
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

position move_twice(position mm, position py, vector <vector <direction > > layout, int layout_size) {
    position ans = get_next_mummy_move(mm, py, layout, layout_size);
    if (ans == py) {
        return ans;
    }
    ans = get_next_mummy_move(ans, py, layout, layout_size);
    return ans;
}

int generate_maze::can_solve_the_maze() {
    vector < vector <direction> > matrix(maze_size, vector <direction>(maze_size));
    vector <bool> visited(maze_size * maze_size * maze_size * maze_size, false);

    vector <string> tmp_maze;
    tmp_maze = maze_layout;

    queue <queue_element> q;

    for (int i = 0; i < maze_size; i++) {
        int x = get_real_coordinates(i);
        for (int j = 0; j < maze_size; j++) {
            int y = get_real_coordinates(j);
            for (int k = 0; k < 4; k++) {
                if (tmp_maze[x + dx[k]][y + dy[k]] == '#') {
                    matrix[i][j].reach[k] = true;
                }
            }
        }
    }
    int max_move = maze_size * maze_size * 2;
    q.push({ player, mummy, 0 });
    visited[get_id_pair(player, mummy, maze_size)] = true;

    int start_id = get_id_pair(player, mummy, maze_size);

    while (!q.empty()) {
        queue_element u = q.front();
        q.pop();
        position cur_py = { u.a.x, u.a.y };
        position cur_mm = { u.b.x, u.b.y };
        for (int k = 0; k < 4; k++) {

            position new_mm = move_twice(cur_mm, cur_py, matrix, maze_size);
            if (new_mm == cur_py) {
                continue;
            }
            position new_py = { cur_py.x + dx[k], cur_py.y + dy[k] };


            //reach the win cell
            if (matrix[cur_py.x][cur_py.y].reach[k] == true) {
                continue;
            };
            if (new_py == cur_mm) { // player catch the mummy =))))
                continue;
            }
            if (new_py.x >= maze_size || new_py.x < 0 || new_py.x < 0 || new_py.y >= maze_size) {
                return u.num_move + 1;
            }


            if (new_mm == new_py) { //catch the player
                continue;
            }
            int new_id = get_id_pair(new_py, new_mm, maze_size);
            if (visited[new_id] == true) {
                continue;
            }
            if (u.num_move + 1 > max_move) {
                return -1;
            }
            q.push({ new_py, new_mm, u.num_move + 1 });
            visited[new_id] = true;
        }
    }


    return -1;
}
void generate_maze::reset_data() {
    init_real_maze();
    parent.clear();
    parent.resize(real_size * real_size + real_size * 2);
    component_size.clear();
    component_size.resize(real_size * real_size + real_size * 2);
    for (int i = 0; i < parent.size(); i++) parent[i] = i, component_size[i] = 1;

}

void generate_maze::generate() {

    bool success = false;
    while (!success) {
        build_maze();
        vector<int> distance_from_exit(maze_size * maze_size * maze_size, INT_MAX);
        distance_from_cell(distance_from_exit, exit_cell, maze_size, maze_layout);
        int max_try = 50;

        while (max_try--) {
            position tmp_player, tmp_mummy;
            bool valid_player = false, valid_mummy = false;
            //player 
            // do {

            // } while ();
            int p_try = 20;
            while (p_try--) {
                tmp_player.x = get_random_number(0, maze_size - 1);
                tmp_player.y = get_random_number(0, maze_size - 1);
                int d = distance_from_exit[get_id_maze(tmp_player.x, tmp_player.y, maze_size)];
                if (d != INT_MAX && d > maze_size / 2) {
                    valid_player = true;
                    break;
                }
            }
            //player
            if (!valid_player) continue;

            int d_player_to_exit = distance_from_exit[get_id_maze(tmp_player.x, tmp_player.y, maze_size)];

            vector<int> dist_from_player(maze_size * maze_size, INT_MAX);
            distance_from_cell(dist_from_player, tmp_player, maze_size, maze_layout);
            int m_try = 20;
            while (m_try--) {
                tmp_mummy.x = get_random_number(0, maze_size - 1);
                tmp_mummy.y = get_random_number(0, maze_size - 1);

                int d = dist_from_player[get_id_maze(tmp_mummy.x, tmp_mummy.y, maze_size)];
                int d_e = distance_from_exit[get_id_maze(tmp_mummy.x, tmp_mummy.y, maze_size)];
                if (d_e <= 4) {
                    continue;
                }
                if (d != INT_MAX && d > maze_size / 2) {
                    valid_mummy = true;
                    break;
                }
            }
            if (!valid_mummy) continue;

            int d_mummy_to_player = dist_from_player[get_id_maze(tmp_mummy.x, tmp_mummy.y, maze_size)];
            int d_mummy_to_exit = distance_from_exit[get_id_maze(tmp_mummy.x, tmp_mummy.y, maze_size)];
            double safety_factor = 1.5;
            if (d_mummy_to_player < d_player_to_exit * safety_factor) {
                continue;
            }

            this->player = tmp_player;
            this->mummy = tmp_mummy;

            maze_layout[get_real_coordinates(player.x)][get_real_coordinates(player.y)] = 'E';
            maze_layout[get_real_coordinates(mummy.x)][get_real_coordinates(mummy.y)] = 'M';

            int steps = can_solve_the_maze();

            if (steps != -1) {
                success = true;
                return;
            }
            maze_layout[get_real_coordinates(player.x)][get_real_coordinates(player.y)] = ' ';
            maze_layout[get_real_coordinates(mummy.x)][get_real_coordinates(mummy.y)] = ' ';
        }
    }
}