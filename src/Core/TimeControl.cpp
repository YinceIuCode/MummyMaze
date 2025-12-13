#include "../include/Core/TimeControl.hpp"


void time_controller::push_state(Player input_state_player, Mummy input_state_mummy) {
    undo_stack.push({ input_state_player, input_state_mummy });
    while (!redo_stack.empty()) {
        redo_stack.pop();
    }

}

pair <Player, Mummy> time_controller::undo_state(Player current_player, Mummy current_mummy) {
    if (undo_stack.size() == 1) return undo_stack.top();

    pair <Player, Mummy> temp = undo_stack.top();
    redo_stack.push({ current_player, current_mummy });
    //cout << undo_stack.size() << endl;
    undo_stack.pop();

    //cout << undo_stack.size() << "\n";

    return temp;
}

pair <Player, Mummy> time_controller::redo_state() {
    if (redo_stack.empty()) {
        return undo_stack.top();
    }
    cout << redo_stack.size() << "\n";
    pair <Player, Mummy> temp = redo_stack.top();
    undo_stack.push(temp);
    redo_stack.pop();
    return temp;
}

