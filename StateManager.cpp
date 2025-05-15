#include "StateManager.h"

void StateManager::pushState(std::unique_ptr<GameState> state) {
    // move the state into the stack
    m_states.push(std::move(state));
}

void StateManager::popState() {
    // remove the current state from the stack
    if (!m_states.empty())
        m_states.pop();
}

void StateManager::changeState(std::unique_ptr<GameState> state) {
    // remove the current state from the stack
    popState();
    // push the new state onto the stack
    pushState(std::move(state));
}

GameState* StateManager::getCurrentState() {
    // return the current state
    // if the stack is empty, return nullptr
    return m_states.empty() ? nullptr : m_states.top().get();
}
