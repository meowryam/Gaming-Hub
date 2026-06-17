#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <optional>

using namespace std;

class Snake
{
private:
    sf::RenderWindow& window;

    sf::Color babyPink, lightPink, darkPink, textColor, transparent;

    sf::Texture bgTexture;
    std::optional<sf::Sprite> bg;

    sf::Texture headUpTex, headDownTex, headLeftTex, headRightTex;
    sf::Texture bodyHTex, bodyVTex;
    sf::Texture bodyTLTex, bodyTRTex, bodyBLTex, bodyBRTex;
    sf::Texture tailUpTex, tailDownTex, tailLeftTex, tailRightTex;
    sf::Texture foodTex, specialFoodTex;
    std::optional<sf::Sprite> foodSprite, specialFoodSprite;
    
    sf::Font font;
    std::optional<sf::Text> scoreText;
    std::optional<sf::Text> countdownText;
    std::optional<sf::Text> gameOverText;
    std::optional<sf::Text> playAgainText;

    sf::SoundBuffer eatBuffer;
    sf::SoundBuffer deathBuffer;
    sf::SoundBuffer beepBuffer;
    sf::SoundBuffer specialEatBuffer; 
    sf::Sound eatSound;
    sf::Sound deathSound;
    sf::Sound beepSound;
    sf::Sound specialEatSound;
    sf::Music& music;
    sf::Texture speakerOnTex, speakerOffTex;
    std::optional<sf::Sprite> speakerSprite;
    sf::Vector2i food;       
    sf::RectangleShape foodShape; 
  
    sf::Vector2i specialFood;
    sf::RectangleShape specialFoodShape;

    bool specialFoodActive;
    int foodEatenSinceLastSpecial;
    sf::Clock specialFoodClock;
    float specialFoodDuration;
 
    int score;
    bool gameOver;
    bool running;
    bool countdownActive;
    int countdownValue;
    int lastBeepValue;
    sf::Clock countdownClock;

    static const int CELL_SIZE = 40;            // each grid cell 20x20 pixels
    static const int GRID_COLS = 25;           // 1000 / 20 = 50 columns
    static const int GRID_ROWS = 20;  
   
    static const int MAX_LENGTH = GRID_COLS * GRID_ROWS;
    sf::Vector2i body[MAX_LENGTH];
    int length;

    sf::Vector2i direction;
    sf::Vector2i nextDirection;

    sf::Clock moveClock;
    float moveInterval;

public:
    Snake(sf::RenderWindow& win, sf::Music& bgMusic);
    ~Snake() {}
    void run();
    int getFinalScore() const { return score; }

private:
    void handleEvents();
    void update();
    void render();
    void spawnFood();
    void resetSnake();
    void spawnSpecialFood();
    sf::Texture& getSegmentTexture(int i);
    sf::Vector2i normalizeDir(sf::Vector2i d);
};