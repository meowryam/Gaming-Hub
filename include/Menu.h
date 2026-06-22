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
    sf::Color creamWhite, softPink, pillBorder;
    sf::Font font1;
    std::optional<sf::Text> creditText;
    sf::Texture bgTexture, mascotTexture;
    std::optional<sf::Text> title, titleShadow, subtitle;
    std::optional<sf::Text> game1, game2, game3, title2, title3;
    sf::RectangleShape button1, button2, button3, button4, button5, overlay;
    sf::CircleShape pillCapL[5], pillCapR[5];
    sf::CircleShape pillCapLBorder[5], pillCapRBorder[5];
    sf::RectangleShape buttonBorder[5];
    std::optional<sf::Sprite> bg, mascot;
    // bg decoration: diamonds, dots, hearts, sparkles
    static const int NUM_SPRINKLES = 150;
    sf::ConvexShape sprinkles[NUM_SPRINKLES];
    int sprinkleKind[NUM_SPRINKLES];
    float sprinkleAngle[NUM_SPRINKLES];
    float sprinkleSpeed[NUM_SPRINKLES];
    float sprinkleX[NUM_SPRINKLES];
    float sprinkleY[NUM_SPRINKLES];
    // soft glow blobs behind title
    static const int NUM_BLOBS = 5;
    sf::CircleShape blobs[NUM_BLOBS];
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

private:
    void drawPillButton(int idx, bool hovered);
    void drawStar(float cx, float cy, float r, float angle, sf::Color col);
};