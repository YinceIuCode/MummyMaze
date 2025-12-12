#pragma once
#include <vector>
#include <string>
#include <random>

// Struct lưu tọa độ
struct position {
    int x, y;
    bool operator == (const position& other) const {
        return (x == other.x && y == other.y);
    }
};

class generate_maze {
private:
    int maze_size;
    int real_size;
    std::vector<std::string> maze_layout;

    // DSU (Disjoint Set Union) components
    struct dsu_edge {
        int x1, y1, x2, y2;
        bool is_connected;
    };
    std::vector<int> parent, component_size;

    std::mt19937 rng;

    // Private Helpers
    int find_set(int a);
    bool union_set(int a, int b);
    int get_id(int x, int y) const;
    bool inside_real(int x, int y) const;
    void init_real_maze();

public:
    position player, mummy, exit_cell;

    generate_maze(int input_size);

    void generate();
    void print_maze();
    int can_solve_the_maze();
    int get_random_number(int minimum, int maximum);
};