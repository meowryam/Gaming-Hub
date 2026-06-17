#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <optional>
#include "LeaderBoard.h"
using namespace std;

class Menu
{
private:
    sf::Clock clock;
    sf::RenderWindow window;
    sf::Color babyPink, lightPink, darkPink, textColor, pyaraclr, stuble, transparent;
    sf::Font font1;
    std::optional<sf::Text> creditText;
    sf::Texture bgTexture, mascotTexture;
    std::optional<sf::Text> title, titleShadow;
    std::optional<sf::Text> game1, game2, game3, title2, title3;
    sf::RectangleShape button1, button2, button3, button4, button5, overlay;
    std::optional<sf::Sprite> bg, mascot;
    sf::CircleShape particles[10];
    sf::Texture speakerOnTex, speakerOffTex;
    std::optional<sf::Sprite> speakerSprite;
    sf::Music music;
    int gameState;
    LeaderboardManager leaderboard;
    string lbFilter;
    
public:
    Menu();
    ~Menu() {}
    void run();
    string getPlayerName(const string& prompt);
};