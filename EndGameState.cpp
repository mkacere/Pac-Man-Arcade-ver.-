#include "EndGameState.h"
#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cstddef>
#include "MainMenuState.h"

// Reduce character size until the text fits the allowed width.
// Keeps numbers readable when scores get very large.
void fitTextWidth(sf::Text& txt, float maxWidth) {
    unsigned size = txt.getCharacterSize();
    while (txt.getLocalBounds().width > maxWidth && size > 10) {
        size -= 2;
        txt.setCharacterSize(size);
    }
}

EndGameState::EndGameState(StateManager& manager,
    const sf::RenderWindow& window,
    int finalScore)
    : m_manager(manager),
    m_window(window),
    m_finalScore(finalScore) {

    // load fonts once
    m_titleFont.loadFromFile("assets/fonts/pacfont.ttf");
    m_monoFont.loadFromFile("assets/fonts/JetBrainsMono.ttf");

    float cx = static_cast<float>(m_window.getSize().x) / 2.f;
    float maxWidth = static_cast<float>(m_window.getSize().x) - 40.f;         // 20‑pixel margin each side

    // banner
    m_titleText.setFont(m_titleFont);
    m_titleText.setString("GAME OVER");
    m_titleText.setCharacterSize(72);
    fitTextWidth(m_titleText, maxWidth);
    centerOrigin(m_titleText);
    m_titleText.setPosition(cx, 80.f);

    // final score
    m_scoreText.setFont(m_monoFont);
    m_scoreText.setString("Score: " + std::to_string(m_finalScore));
    m_scoreText.setCharacterSize(48);
    fitTextWidth(m_scoreText, maxWidth);
    centerOrigin(m_scoreText);
    m_scoreText.setPosition(cx, 180.f);

    // prompt
    m_promptText.setFont(m_monoFont);
    m_promptText.setString("Enter initials and press Enter:");
    m_promptText.setCharacterSize(32);
    fitTextWidth(m_promptText, maxWidth);
    centerOrigin(m_promptText);
    m_promptText.setPosition(cx, 260.f);

    // 3‑char name entry
    m_nameText.setFont(m_monoFont);
    m_nameText.setCharacterSize(56);
    m_nameText.setString("___");
    centerOrigin(m_nameText);
    m_nameText.setPosition(cx, 330.f);

    // high‑score table rows
    m_tableRows.resize(5);
    for (auto& t : m_tableRows) {
        t.setFont(m_monoFont);
        t.setCharacterSize(40);
    }

    loadScores();     // pull scores.txt into vector
    buildTable();     // render top 5 rows
}

void EndGameState::handleEvents(sf::RenderWindow&) {}

// key‑latch polling → avoids draining the main event queue
void EndGameState::update(sf::Time) {
    static std::array<bool, sf::Keyboard::KeyCount> latch{};

    auto pressedOnce = [&](sf::Keyboard::Key k) {
        bool now = sf::Keyboard::isKeyPressed(k);
        bool once = now && !latch[static_cast<std::size_t>(k)];
        latch[static_cast<std::size_t>(k)] = now;
        return once;
        };

    if (!m_scoreSubmitted) {
        if (pressedOnce(sf::Keyboard::BackSpace) && !m_playerName.empty())
            m_playerName.pop_back();

        if (pressedOnce(sf::Keyboard::Enter) && m_playerName.size() == 3) {
            addCurrentScore();   // commit to file
            buildTable();        // refresh table with new entry
            m_scoreSubmitted = true;
            m_promptText.setString("Press Space to return");
            centerOrigin(m_promptText);
        }

        // build initials (A‑Z 0‑9)
        if (m_playerName.size() < 3) {
            for (int k = sf::Keyboard::A; k <= sf::Keyboard::Z; ++k)
                if (pressedOnce(static_cast<sf::Keyboard::Key>(k)))
                    m_playerName += static_cast<char>('A' + k - sf::Keyboard::A);

            for (int k = sf::Keyboard::Num0; k <= sf::Keyboard::Num9; ++k)
                if (pressedOnce(static_cast<sf::Keyboard::Key>(k)))
                    m_playerName += static_cast<char>('0' + k - sf::Keyboard::Num0);
        }

        // live underscore placeholders
        std::string display = m_playerName;
        while (display.size() < 3) display += '_';
        m_nameText.setString(display);
        centerOrigin(m_nameText);
    }
    else { // after submission: Space returns to main menu
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            m_manager.changeState(std::make_unique<MainMenuState>(m_manager, m_window));
            return;
        }
    }
}

void EndGameState::render(sf::RenderWindow& window) {
    window.draw(m_titleText);
    window.draw(m_scoreText);
    window.draw(m_promptText);
    window.draw(m_nameText);
    for (auto& t : m_tableRows) window.draw(t);
}

// set origin to centre for easy positioning
void EndGameState::centerOrigin(sf::Text& text) {
    auto b = text.getLocalBounds();
    text.setOrigin(b.left + b.width / 2.f, b.top + b.height / 2.f);
}

// render 5 best scores (after sorting)
void EndGameState::buildTable() {
    float cx = static_cast<float>(m_window.getSize().x) / 2.f;
    float maxWidth = static_cast<float>(m_window.getSize().x) - 40.f;
    for (std::size_t i = 0; i < 5; ++i) {
        std::ostringstream ss;
        if (i < m_scores.size())
            ss << std::left << std::setw(3) << m_scores[i].first << "  " << m_scores[i].second;

        m_tableRows[i].setString(ss.str());
        fitTextWidth(m_tableRows[i], maxWidth);
        centerOrigin(m_tableRows[i]);
        m_tableRows[i].setPosition(cx, 420.f + static_cast<float>(i) * 50.f);
    }
}

// read whole file into m_scores
void EndGameState::loadScores() {
    std::ifstream in("scores.txt");
    std::string line;
    while (std::getline(in, line)) {
        if (line.size() < 4) continue;
        std::string name = line.substr(0, 3);
        int value = std::stoi(line.substr(3));
        m_scores.emplace_back(name, value);
    }
}

// overwrite scores.txt with current vector
void EndGameState::saveScores() {
    std::ofstream out("scores.txt", std::ios::trunc);
    for (auto& e : m_scores) out << e.first << e.second << '\n';
}

// push current score, sort, cap at 10, persist
void EndGameState::addCurrentScore() {
    m_scores.emplace_back(m_playerName, m_finalScore);
    std::sort(m_scores.begin(), m_scores.end(),
        [](const auto& a, const auto& b) { return a.second > b.second; });
    if (m_scores.size() > 10) m_scores.resize(10);
    saveScores();
}
