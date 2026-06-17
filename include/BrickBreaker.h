#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <optional>
using namespace std;

class Brick
{
private:
    sf::RectangleShape shape;
    bool active;
    int hitsRequired;

public:
    Brick();
    void init(float x, float y, float width, float height, int hits, sf::Color color);
    void hit();
    bool isActive() const;
    sf::RectangleShape& getShape();
    sf::FloatRect getBounds() const;
};

class BrickBreaker
{
private:
    sf::RenderWindow& window;
    
    sf::Color babyPink, lightPink, darkPink, textColor, pyaraclr, stuble, transparent;

 
    sf::Texture bgTexture;
    std::optional<sf::Sprite> bg;

    
    sf::Texture ballTexture;
    std::optional<sf::Sprite> ball;
    sf::Vector2f ballVelocity;

    sf::Texture heartTexture;
    std::optional<sf::Sprite> heart1, heart2, heart3;
    
    sf::RectangleShape paddle;
    
    static const int BRICK_ROWS = 5;
    static const int BRICK_COLS = 10;
    static const int MAX_BRICKS = BRICK_ROWS * BRICK_COLS;
    Brick bricks[MAX_BRICKS];

    sf::Font font;
    sf::Font font1;
    std::optional<sf::Text> scoreText;
    std::optional<sf::Text> livesText;
    std::optional<sf::Text> winText;
    std::optional<sf::Text> loseText;
    std::optional<sf::Text> countdownText;
    std::optional<sf::Text> playAgainText;

    sf::SoundBuffer hitBrickBuffer;
    sf::SoundBuffer paddleHitBuffer;
    sf::SoundBuffer wallBounceBuffer;
    sf::SoundBuffer beepBuffer;
    sf::SoundBuffer outBuffer;
    sf::Sound hitBrickSound;
    sf::Sound paddleHitSound;
    sf::Sound wallBounceSound;
    sf::Sound beepSound;
    sf::Sound outSound;
    sf::Music& music;
    sf::Texture speakerOnTex, speakerOffTex;
    std::optional<sf::Sprite> speakerSprite;
    int score;
    int lives;
    bool gameOver;
    bool won;
    bool running;
    bool countdownActive;
    int countdownValue;
    int lastBeepValue;

    sf::Clock countdownClock;

public:
    BrickBreaker(sf::RenderWindow& win, sf::Music& bgMusic);
    ~BrickBreaker() {}
    void run();
    int  getFinalScore() const { return score; }
    bool playerWon()     const { return won; }
private:
    void initBricks();
    void handleEvents();
    void update();
    void render();
    void resetBall();
};