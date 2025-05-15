#ifndef STATEMANAGER_H
#define STATEMANAGER_H

#include "GameState.h"
#include <memory>
#include <stack>

class StateManager {
public:

    /**
     * @brief adds the state to the stack to be used next frame/iteration
     * 
     * @param state possible game state child class
     */
    void pushState(std::unique_ptr<GameState> state);

    /**
     * @brief removes the current state from the stack
     * 
     */
    void popState();

    /**
     * @brief changes the current state to the new state
     * 
     * @param state possible game state child class
     */
    void changeState(std::unique_ptr<GameState> state);

    /**
     * @brief returns the current state
     * 
     * @return GameState* pointer to the current state
     */
    GameState* getCurrentState();

private:
    std::stack<std::unique_ptr<GameState>> m_states;
};

#endif // STATEMANAGER_H
