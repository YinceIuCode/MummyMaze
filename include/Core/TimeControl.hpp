#pragma once
#include <iostream> 
#include <stack>
#include "../Entities/Player.hpp"
#include "../Entities/Mummy.hpp"


using namespace std;

struct GameSnapshot {
    Player player;
    Mummy mummy;
    int stepCount;
};

class time_controller {
private:
    stack <GameSnapshot> undo_stack;
    stack <GameSnapshot> redo_stack;
public:
    void push_state(Player input_state_player, Mummy input_state_mummy, int steps) {
        GameSnapshot snapshot;
        snapshot.player = input_state_player;
        snapshot.mummy = input_state_mummy;
        snapshot.stepCount = steps;

        undo_stack.push(snapshot);
    }
    GameSnapshot undo_state(Player current_player, Mummy current_mummy, int current_steps) {
        if (undo_stack.empty()) {
            return { current_player, current_mummy, current_steps };
        }

        GameSnapshot currentSnapshot;
        currentSnapshot.player = current_player;
        currentSnapshot.mummy = current_mummy;
        currentSnapshot.stepCount = current_steps;
        redo_stack.push(currentSnapshot);

        GameSnapshot oldState = undo_stack.top();
        undo_stack.pop();

        return oldState;
    }
    GameSnapshot redo_state(Player current_player, Mummy current_mummy, int current_steps) {
        if (redo_stack.empty()) {
            return { current_player, current_mummy, current_steps };
        }

        GameSnapshot currentSnapshot;
        currentSnapshot.player = current_player;
        currentSnapshot.mummy = current_mummy;
        currentSnapshot.stepCount = current_steps;
        undo_stack.push(currentSnapshot);

        GameSnapshot futureState = redo_stack.top();
        redo_stack.pop();

        return futureState;
    }
    void reset() {
        while (!undo_stack.empty()) undo_stack.pop();
        while (!redo_stack.empty()) redo_stack.pop();
    }
    void resetredo() {
        while (!redo_stack.empty()) redo_stack.pop();
	}

    bool canUndo() const { return !undo_stack.empty(); }
    bool canRedo() const { return !redo_stack.empty(); }
};