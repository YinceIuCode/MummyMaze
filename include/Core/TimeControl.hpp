#pragma once
#include <iostream> 
#include <stack>
#include "../Entities/Player.hpp"
#include "../Entities/Mummy.hpp"


using namespace std;

class time_controller {
private:
    stack <pair <Player, Mummy> > undo_stack;
    stack <pair <Player, Mummy> > redo_stack;
public:
    void push_state(Player input_state_player, Mummy input_state_mummy);
    pair <Player, Mummy> undo_state(Player current_player, Mummy current_mummy); //return top() state
    pair <Player, Mummy> redo_state();
};