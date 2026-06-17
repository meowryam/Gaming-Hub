#include "Snake.h"
#include <cstdlib>
#include <ctime>
static string intToStr(int n)
{
    if (n == 0) return "0";
    string result = "";
    bool neg = (n < 0);
    if (neg) n = -n;
    while (n > 0)
    {
        result = (char)('0' + n % 10) + result;
        n /= 10;
    }
    if (neg) result = "-" + result;
    return result;
}

Snake::Snake(sf::RenderWindow& win, sf::Music& bgMusic)
    : window(win),
    music(bgMusic),
    babyPink(255, 182, 193),
    lightPink(255, 204, 229),
    darkPink(255, 105, 180),
    textColor(255, 240, 245),
    transparent(0, 0, 0, 0),
    score(0),
    gameOver(false),
    running(true),
    countdownActive(true),
    specialFoodActive(false),
    foodEatenSinceLastSpecial(0),
    specialEatSound(specialEatBuffer),
    specialFoodDuration(3.f),
    countdownValue(3),
    lastBeepValue(0),
    eatSound(eatBuffer),
    deathSound(deathBuffer),
    beepSound(beepBuffer),
    length(3),
    direction(1, 0),
    nextDirection(1, 0),
    moveInterval(0.15f)
{
    if (!font.openFromFile("assets/fonts/CherryBomb.ttf"))
        std::cout << "Font failed to load!\n";

    if (!bgTexture.loadFromFile("assets/images/background.jpeg"))
        std::cout << "Background failed to load!\n";
    bg.emplace(bgTexture);
    float scaleX = (float)window.getSize().x / bgTexture.getSize().x;
    float scaleY = (float)window.getSize().y / bgTexture.getSize().y;
    bg->setScale({ scaleX, scaleY });

    if (!headUpTex.loadFromFile("assets/images/head_up.png"))        std::cout << "Head up failed!\n";
    if (!headDownTex.loadFromFile("assets/images/head_down.png"))        std::cout << "Head up failed!\n";
    if (!tailUpTex.loadFromFile("assets/images/tail_up.png"))        std::cout << "Tail up failed!\n";
    if (!tailDownTex.loadFromFile("assets/images/tail_down.png"))        std::cout << "Tail down failed!\n";
    if (!headLeftTex.loadFromFile("assets/images/head_left.png"))        std::cout << "Head Left failed!\n";
    if (!headRightTex.loadFromFile("assets/images/head_right.png"))        std::cout << "Head Right failed!\n";
    if (!bodyHTex.loadFromFile("assets/images/body_horizontal.png"))        std::cout << "Body Horizontal failed!\n";
    if (!bodyVTex.loadFromFile("assets/images/body_vertical.png"))        std::cout << "Body Vertcal failed!\n";
    if (!bodyTLTex.loadFromFile("assets/images/body_topleft.png"))        std::cout << "Body top L failed!\n";
    if (!bodyTRTex.loadFromFile("assets/images/body_topright.png"))        std::cout << "Body top R failed!\n";
    if (!bodyBLTex.loadFromFile("assets/images/body_bottomleft.png"))        std::cout << "BODY BL failed!\n";
    if (!bodyBRTex.loadFromFile("assets/images/body_bottomright.png"))        std::cout << "BBR failed!\n";
    if (!tailLeftTex.loadFromFile("assets/images/tail_left.png"))        std::cout << "Tail L failed!\n";
    if (!tailRightTex.loadFromFile("assets/images/tail_right.png"))        std::cout << "Tail R failed!\n";
 
    if (!eatBuffer.loadFromFile("assets/sounds/brickbreak.wav"))
        std::cout << "Eat sound failed to load!\n";
    if (!deathBuffer.loadFromFile("assets/sounds/Fah.mp3"))
        std::cout << "Death sound failed to load!\n";
    if (!beepBuffer.loadFromFile("assets/sounds/beep.wav"))
        std::cout << "Beep sound failed to load!\n";
    if (!specialEatBuffer.loadFromFile("assets/sounds/specialfood.mp3"))
        std::cout << "Special food sound failed to load!\n";
    specialEatSound.setVolume(100.f);
    eatSound.setVolume(100.f);
    deathSound.setVolume(100.f);
    beepSound.setVolume(100.f);

    scoreText.emplace(font, "Score: 0", 30);
    scoreText->setFillColor(darkPink);
    scoreText->setOutlineColor(textColor);
    scoreText->setOutlineThickness(2.f);
    scoreText->setPosition({ 10.f, 10.f });

    gameOverText.emplace(font, "Game Over!", 80);
    gameOverText->setFillColor(darkPink);
    gameOverText->setOutlineColor(textColor);
    gameOverText->setOutlineThickness(3.f);
    gameOverText->setPosition({ window.getSize().x / 2.f - gameOverText->getGlobalBounds().size.x / 2.f, window.getSize().y / 2.f - 100.f   });

    playAgainText.emplace(font, "Press R to Play Again  |  Escape to Menu", 25);
    playAgainText->setFillColor(darkPink);
    playAgainText->setOutlineColor(textColor);
    playAgainText->setOutlineThickness(3.f);
    playAgainText->setPosition({ window.getSize().x / 2.f - playAgainText->getGlobalBounds().size.x / 2.f,  window.getSize().y / 2.f + 50.f  });

    countdownText.emplace(font, "3", 150);
    countdownText->setFillColor(darkPink);
    countdownText->setOutlineColor(textColor);
    countdownText->setOutlineThickness(4.f);
    countdownText->setPosition({ window.getSize().x / 2.f - countdownText->getGlobalBounds().size.x / 2.f, window.getSize().y / 2.f - 100.f  });
    //centre + right 
    body[0] = { GRID_COLS / 2,     GRID_ROWS / 2 };
    body[1] = { GRID_COLS / 2 - 1, GRID_ROWS / 2 };
    body[2] = { GRID_COLS / 2 - 2, GRID_ROWS / 2 };

    srand((unsigned int)time(nullptr));
    spawnFood();


    if (!speakerOnTex.loadFromFile("assets/images/sound_on.png"))
        std::cout << "Speaker on texture failed!\n";
    if (!speakerOffTex.loadFromFile("assets/images/sound_off.png"))
        std::cout << "Speaker off texture failed!\n";
    speakerSprite.emplace(speakerOnTex);
    speakerSprite->setColor(darkPink);
    speakerSprite->setScale({ 0.07f, 0.07f });
    speakerSprite->setPosition({ 950.f, 10.f });
}



sf::Texture& Snake::getSegmentTexture(int i)
{
    if (i == 0) //head
    {
        if (direction == sf::Vector2i(0, -1)) return headUpTex;
        if (direction == sf::Vector2i(0, 1)) return headDownTex;
        if (direction == sf::Vector2i(-1, 0)) return headLeftTex;
        return headRightTex;
    }
    else if (i == length - 1) //tail
    {
        sf::Vector2i tailDir = normalizeDir(body[i] - body[i - 1]);
        if (tailDir == sf::Vector2i(0, -1)) return tailUpTex;
        if (tailDir == sf::Vector2i(0, 1)) return tailDownTex;
        if (tailDir == sf::Vector2i(-1, 0)) return tailLeftTex;
        return tailRightTex;
    }
    else //body
    {
        sf::Vector2i prev = normalizeDir(body[i - 1] - body[i]);
        sf::Vector2i next = normalizeDir(body[i + 1] - body[i]);

        //straight
        if (prev.x != 0 && next.x != 0) return bodyHTex;
        if (prev.y != 0 && next.y != 0) return bodyVTex;

        //corners
        if ((prev == sf::Vector2i(0, -1) && next == sf::Vector2i(1, 0)) ||
            (prev == sf::Vector2i(1, 0) && next == sf::Vector2i(0, -1))) return bodyTRTex;

        if ((prev == sf::Vector2i(0, -1) && next == sf::Vector2i(-1, 0)) ||
            (prev == sf::Vector2i(-1, 0) && next == sf::Vector2i(0, -1))) return bodyTLTex;

        if ((prev == sf::Vector2i(0, 1) && next == sf::Vector2i(1, 0)) ||
            (prev == sf::Vector2i(1, 0) && next == sf::Vector2i(0, 1))) return bodyBRTex;

        if ((prev == sf::Vector2i(0, 1) && next == sf::Vector2i(-1, 0)) ||
            (prev == sf::Vector2i(-1, 0) && next == sf::Vector2i(0, 1))) return bodyBLTex;

        return bodyHTex;
    }
}





void Snake::resetSnake()
{
    length = 3;
    direction = { 1, 0 };
    nextDirection = { 1, 0 };
    body[0] = { GRID_COLS / 2,     GRID_ROWS / 2 };
    body[1] = { GRID_COLS / 2 - 1, GRID_ROWS / 2 };
    body[2] = { GRID_COLS / 2 - 2, GRID_ROWS / 2 };
    moveInterval = 0.15f;
    moveClock.restart();
    spawnFood();
    specialFoodActive = false;
    foodEatenSinceLastSpecial = 0;
}


sf::Vector2i Snake::normalizeDir(sf::Vector2i d)
{
    if (d.x != 0)
    {
        if (d.x > GRID_COLS / 2) d.x = -1;
        else if (d.x < -GRID_COLS / 2) d.x = 1;
        else    d.x = (d.x > 0) ? 1 : -1;
    }
    if (d.y != 0)
    {
        if (d.y > GRID_ROWS / 2) d.y = -1;
        else if (d.y < -GRID_ROWS / 2) d.y = 1;
        else    d.y = (d.y > 0) ? 1 : -1;
    }
    return d;
}


void Snake::spawnFood()
{
    
    bool onSnake = true; //occupied by snake
    while (onSnake)
    {
        food.x = rand() % GRID_COLS;
        food.y = rand() % GRID_ROWS;

        onSnake = false;
        for (int i = 0; i < length; i++)
        {
            if (body[i] == food)
            {
                onSnake = true;
                break;
            }
        }
    }

    if (!foodTex.loadFromFile("assets/images/food_heart.png"))        cout << "food_heart failed!\n";
    foodSprite.emplace(foodTex);
    foodSprite->setScale({ CELL_SIZE / (float)foodTex.getSize().x, CELL_SIZE / (float)foodTex.getSize().y });
    foodSprite->setPosition({ food.x * (float)CELL_SIZE, food.y * (float)CELL_SIZE }); 
        
}


void Snake::spawnSpecialFood()
{
    bool invalid = true;
    while (invalid)
    {
        specialFood.x = rand() % GRID_COLS;
        specialFood.y = rand() % GRID_ROWS;

        invalid = false;

        //not on snake
        for (int i = 0; i < length; i++)
            if (body[i] == specialFood) { invalid = true; break; }

        //not on normal food
        if (specialFood == food) invalid = true;
    }
   
    if (!specialFoodTex.loadFromFile("assets/images/food_bow.png"))   std::cout << "food_bow failed!\n";
    specialFoodSprite.emplace(specialFoodTex);
    specialFoodSprite->setScale({ CELL_SIZE / (float)specialFoodTex.getSize().x, CELL_SIZE / (float)specialFoodTex.getSize().y });
    specialFoodSprite->setPosition({ specialFood.x * (float)CELL_SIZE, specialFood.y * (float)CELL_SIZE });
    specialFoodActive = true;
    specialFoodClock.restart();
}





void Snake::handleEvents()
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == sf::Keyboard::Key::Escape)
                running = false;

            if (keyPressed->code == sf::Keyboard::Key::M)
            {
                if (music.getStatus() == sf::Music::Status::Playing)
                    music.pause();
                else
                    music.play();
            }

            if (keyPressed->code == sf::Keyboard::Key::R && gameOver)
            {
                score = 0;
                gameOver = false;
                countdownActive = true;
                countdownValue = 3;
                lastBeepValue = 0;
                countdownClock.restart();
                countdownText->setString("3");
                scoreText->setString("Score: 0");
                resetSnake();
            }

            if (!gameOver && !countdownActive)
            {
                if ((keyPressed->code == sf::Keyboard::Key::Up ||
                    keyPressed->code == sf::Keyboard::Key::W)
                    && direction.y != 1)
                    nextDirection = { 0, -1 };

                if ((keyPressed->code == sf::Keyboard::Key::Down ||
                    keyPressed->code == sf::Keyboard::Key::S)
                    && direction.y != -1)
                    nextDirection = { 0, 1 };

                if ((keyPressed->code == sf::Keyboard::Key::Left ||
                    keyPressed->code == sf::Keyboard::Key::A)
                    && direction.x != 1)
                    nextDirection = { -1, 0 };

                if ((keyPressed->code == sf::Keyboard::Key::Right ||
                    keyPressed->code == sf::Keyboard::Key::D)
                    && direction.x != -1)
                    nextDirection = { 1, 0 };
            }
        }
               if (const auto* mp = event->getIf<sf::Event::MouseButtonPressed>())
               {
                   if (mp->button == sf::Mouse::Button::Left)
                      {
                        if (speakerSprite->getGlobalBounds().contains(sf::Vector2f(mousePos)))
                           {
                             if (music.getStatus() == sf::Music::Status::Playing)
                             music.pause();
                             else
                             music.play();
                        }
                   }
               }


    }
}

void Snake::update()
{
    if (countdownActive)
    {
        int elapsed = (int)countdownClock.getElapsedTime().asSeconds();

        if (elapsed < 3)
        {
            countdownValue = 3 - elapsed;
            countdownText->setString(intToStr(countdownValue));
            countdownText->setPosition({
                window.getSize().x / 2.f - countdownText->getGlobalBounds().size.x / 2.f,
                window.getSize().y / 2.f - 100.f
                });

            if (countdownValue != lastBeepValue)
            {
                beepSound.play();
                lastBeepValue = countdownValue;
            }
        }
        else
            countdownActive = false;

        return; 
    }

    if (gameOver) return;

    if (moveClock.getElapsedTime().asSeconds() < moveInterval)
        return;

    moveClock.restart();
    direction = nextDirection;
    
    for (int i = length - 1; i > 0; i--)   body[i] = body[i - 1]; //body
    body[0].x += direction.x;   body[0].y += direction.y;  //head
  
    //wall
    if (body[0].x < 0)          body[0].x = GRID_COLS - 1;
    if (body[0].x >= GRID_COLS) body[0].x = 0;
    if (body[0].y < 0)          body[0].y = GRID_ROWS - 1;
    if (body[0].y >= GRID_ROWS) body[0].y = 0;


    if (body[0] == food)
    {
        length++;           
        score += 10;
        scoreText->setString("Score: " + intToStr(score));
        eatSound.play();
        spawnFood();
        foodEatenSinceLastSpecial++;
        if (foodEatenSinceLastSpecial >= 5)
        {
            foodEatenSinceLastSpecial = 0;
            spawnSpecialFood();
        }
       
        if (moveInterval > 0.005f)
            moveInterval -= 0.005f;
    }


        if (specialFoodActive && body[0] == specialFood)
        {
            specialFoodActive = false;
            foodEatenSinceLastSpecial = 0;
            score += 50;
            scoreText->setString("Score: " + intToStr(score));
            specialEatSound.play();
            if (moveInterval > 0.05f)
                moveInterval -= 0.04f;
          

        }

        if (specialFoodActive && specialFoodClock.getElapsedTime().asSeconds() >= specialFoodDuration)
            specialFoodActive = false;


   
         for (int i = 1; i < length; i++)
         {
             if (body[0] == body[i])  //self collision
             {
                 gameOver = true;
                 deathSound.play();
                 break;
             }
         }
}

void Snake::render()
{
    window.clear(sf::Color(20, 0, 10));
    window.draw(*bg);
    if (countdownActive)
    {
        window.draw(*countdownText);
        window.display();
        return;
    }
    window.draw(*scoreText);
    window.draw(*foodSprite);
    if (specialFoodActive)
        window.draw(*specialFoodSprite);
    for (int i = 0; i < length; i++)
    {
        sf::Sprite segment(getSegmentTexture(i));
        sf::Vector2u texSize = getSegmentTexture(i).getSize();
        segment.setScale({(float)CELL_SIZE / texSize.x,(float)CELL_SIZE / texSize.y });
        segment.setPosition({ body[i].x * (float)CELL_SIZE,   body[i].y * (float)CELL_SIZE });
        segment.setColor(babyPink);  
        window.draw(segment);
    }
   
    if (gameOver)
    {
        sf::RectangleShape overlay({ (float)window.getSize().x, (float)window.getSize().y });
        overlay.setFillColor(sf::Color(0, 0, 0, 150));
        window.draw(overlay);
        window.draw(*gameOverText);
        window.draw(*playAgainText);
    }
    if (music.getStatus() == sf::Music::Status::Playing)
        speakerSprite->setTexture(speakerOnTex);
    else
        speakerSprite->setTexture(speakerOffTex);
    window.draw(*speakerSprite);
    window.display();
}

void Snake::run()
{
    window.setFramerateLimit(60);
    countdownClock.restart();

    while (window.isOpen() && running)
    {
        handleEvents();
        update();
        render();
    }
}