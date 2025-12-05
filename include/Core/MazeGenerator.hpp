#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <algorithm> //shuffle
#include <random> //mt19937, random_device
#include <chrono> //chrono::system_clock
#include <cmath>
#include <queue> 
#include <chrono> //seed random theo thoi gian
#include <cassert>
#include <fstream>
using namespace std;

struct position {
    int x, y;
    bool operator == (const position& other) const {
        return (x == other.x && y == other.y);
    }
};

class generate_maze {
private:
    int maze_size;
    int real_size;// = maze_size * 2 + 1;
    vector <string> maze_layout;
    struct dsu_edge {
        int x1, y1, x2, y2;
        bool is_connected;
    };
    vector <int> parent, component_size;

    std::mt19937 rng;

    int find_set(int a) {
        return (a == parent[a] ? parent[a] : parent[a] = find_set(parent[a]));
    }

    bool union_set(int a, int b) {
        a = find_set(a); b = find_set(b);
        if (a == b) {
            return false;
        }
        if (component_size[a] < component_size[b]) swap(a, b);
        parent[b] = a; // union b into a 
        component_size[a] += component_size[b];
        return true;
    }
    int get_id(int x, int y) {
        return y * real_size + x;
    }
    bool inside_real(int x, int y) {
        return (x >= 0 && x < maze_size && y >= 0 && y < maze_size);
    }
    void init_real_maze() {
        maze_layout.assign(real_size, string(real_size, ' '));
        for (int i = 0; i < real_size; i++) {
            for (int j = 0; j < real_size; j++) {
                if (!(i & 1) && !(j & 1)) {
                    maze_layout[i][j] = '+';
                }
            }
        }
        for (int i = 1; i < real_size; i += 2) {
            maze_layout[i][0] = '#';
            maze_layout[0][i] = '#';
            maze_layout[i][real_size - 1] = '#';
            maze_layout[real_size - 1][i] = '#';
        }
        for (int i = 2; i < real_size; i += 2) {
            for (int j = 1; j < real_size; j += 2) {
                maze_layout[i][j] = '#';
                maze_layout[j][i] = '#';
            }
        }
    }
public:
    position player, mummy, exit_cell;
    generate_maze(int input_size);
    void print_maze();
    void generate();
    int get_random_number(int minimum, int maximum);
    int can_solve_the_maze();

    //bool check if can win. (can out )
};
