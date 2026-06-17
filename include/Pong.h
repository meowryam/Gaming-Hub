#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
using namespace std;

class Pong
{
private:
    sf::RenderWindow& window;

    sf::Texture ballTexture;
    std::optional<sf::Sprite> ball;
    sf::Vector2f ballVelocity;
    sf::Texture bgTexture;
    sf::Color babyPink, lightPink, darkPink, textColor, pyaraclr, stuble, transparent;
    std::optional<sf::Sprite> bg;
    std::optional<sf::Text> labelLeft;
    std::optional<sf::Text> labelRight;
    sf::SoundBuffer hitBuffer;
    sf::SoundBuffer bounceBuffer;
    sf::SoundBuffer scoreBuffer;
    sf::Sound scoreSound;
    sf::Music& music;
    sf::SoundBuffer beepBuffer;
    sf::Sound beepSound;
    int lastBeepValue;
    sf::Sound hitSound;
    sf::Sound bounceSound;

    sf::RectangleShape paddleLeft;
    sf::RectangleShape paddleRight;
    int scoreLeft;
    int scoreRight;
    std::vector<sf::RectangleShape> divider;
    std::optional<sf::Text> playAgainText;
    sf::Font font;
    std::optional<sf::Text> scoreText;
    std::optional<sf::Text> winText;
    std::optional<sf::Text> countdownText;
    sf::Clock countdownClock;
    sf::Texture speakerOnTex, speakerOffTex;
    std::optional<sf::Sprite> speakerSprite;
    int countdownValue;
    bool countdownActive;
    bool gameOver;
    bool running;
    
public:
    Pong(sf::RenderWindow& win, sf::Music& bgMusic, const string& p1 = "Player1", const string& p2 = "Player2");
    ~Pong() {}
    void run();
    string getWinner() const { return winnerName; }
    int getWinnerScore() const
    {
        if (winnerName.empty()) return 0;
        return (winnerName == p1Name) ? scoreLeft : scoreRight;
    }
private:
    void handleEvents();
    void update();
    void render();
    string p1Name;
    string p2Name;
    string winnerName;  
};