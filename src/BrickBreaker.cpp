#include "BrickBreaker.h"

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

Brick::Brick()
{
    active = false;
    hitsRequired = 1;
}

void Brick::init(float x, float y, float width, float height, int hits, sf::Color color)
{
    shape.setSize({ width, height });
    shape.setPosition({ x, y });
    shape.setFillColor(color);
    shape.setOutlineColor(sf::Color(255, 240, 245));
    shape.setOutlineThickness(1.5f);
    hitsRequired = hits;
    active = true;
}

void Brick::hit()
{
    if (!active) return;

    hitsRequired--;

    if (hitsRequired == 1)
        shape.setFillColor(sf::Color(255, 182, 193)); 

    if (hitsRequired <= 0)
        active = false;
}

bool Brick::isActive() const {return active;}
sf::RectangleShape& Brick::getShape() {return shape;}
sf::FloatRect Brick::getBounds() const  {return shape.getGlobalBounds();}


BrickBreaker::BrickBreaker(sf::RenderWindow& win, sf::Music& bgMusic)
    : window(win),
    music(bgMusic),
    babyPink(255, 182, 193),
    lightPink(255, 204, 229),
    darkPink(255, 105, 180),
    textColor(255, 240, 245),
    transparent(0, 0, 0, 0),
    stuble(255, 182, 193, 150),
    pyaraclr(255, 182, 193, 80),
    paddle({ 120.f, 15.f }),
    ballVelocity(8.f, -8.f),
    score(0),
    lives(3),
    gameOver(false),
    won(false),
    running(true),
    countdownActive(true),
    countdownValue(3),
    lastBeepValue(0),
    hitBrickSound(hitBrickBuffer),
    paddleHitSound(paddleHitBuffer),
    wallBounceSound(wallBounceBuffer),
    outSound(outBuffer),
    beepSound(beepBuffer)
{
    
    if (!font.openFromFile("assets/fonts/CherryBomb.ttf"))
        std::cout << "Font failed to load!\n";
    if (!bgTexture.loadFromFile("assets/images/background.jpeg"))
        std::cout << "Background failed to load!\n";
    bg.emplace(bgTexture);
    float scaleX = (float)window.getSize().x / bgTexture.getSize().x;
    float scaleY = (float)window.getSize().y / bgTexture.getSize().y;
    bg->setScale({ scaleX, scaleY });

    if (!ballTexture.loadFromFile("assets/images/bowl.png"))
        std::cout << "Ball texture failed to load!\n";
    ball.emplace(ballTexture);
    ball->setColor(darkPink);  
    sf::Vector2u texSize = ballTexture.getSize();
    ball->setScale({ 25.f / texSize.x, 25.f / texSize.y });

    paddle.setFillColor(darkPink);
    paddle.setOutlineColor(textColor);
    paddle.setOutlineThickness(1.5f);
    paddle.setPosition({
        window.getSize().x / 2.f - 60.f,
        window.getSize().y - 40.f
        });

    if (!hitBrickBuffer.loadFromFile("assets/sounds/brickbreak.wav"))
        std::cout << "Hit brick sound failed to load!\n";
    if (!paddleHitBuffer.loadFromFile("assets/sounds/bounce.flac"))
        std::cout << "Paddle hit sound failed to load!\n";
    if (!wallBounceBuffer.loadFromFile("assets/sounds/bounce.flac"))
        std::cout << "Wall bounce sound failed to load!\n";
    if (!beepBuffer.loadFromFile("assets/sounds/beep.wav"))
        std::cout << "Beep sound failed to load!\n";
    if (!outBuffer.loadFromFile("assets/sounds/brickdeath.wav"))
        std::cout << "death sound failed to load!\n";
    hitBrickSound.setVolume(100.f);
    paddleHitSound.setVolume(100.f);
    wallBounceSound.setVolume(100.f);
    beepSound.setVolume(100.f);
    outSound.setVolume(100.f);

    scoreText.emplace(font, "Score: 0", 30);
    scoreText->setFillColor(darkPink);
    scoreText->setOutlineColor(textColor);
    scoreText->setOutlineThickness(2.f);
    scoreText->setPosition({ 10.f, 10.f });

    if (!heartTexture.loadFromFile("assets/images/food_heart.png"))
        std::cout << "Heart texture failed to load!\n";

    // Scale hearts to ~40x40 pixels
    sf::Vector2u hSize = heartTexture.getSize();
    float hScale = 40.f / hSize.x;

    heart1.emplace(heartTexture);
    heart2.emplace(heartTexture);
    heart3.emplace(heartTexture);

    heart1->setScale({ hScale, hScale });
    heart2->setScale({ hScale, hScale });
    heart3->setScale({ hScale, hScale });

    heart1->setPosition({ window.getSize().x - 200.f, 10.f });
    heart2->setPosition({ window.getSize().x - 150.f, 10.f });
    heart3->setPosition({ window.getSize().x - 100.f,  10.f });
    
    winText.emplace(font, "You Win!", 80);
    winText->setFillColor(darkPink);
    winText->setOutlineColor(textColor);
    winText->setOutlineThickness(3.f);
    winText->setPosition({ window.getSize().x / 2.f - winText->getGlobalBounds().size.x / 2.f,  window.getSize().y / 2.f - 100.f  });

    loseText.emplace(font, "Game Over!", 80);
    loseText->setFillColor(darkPink);
    loseText->setOutlineColor(textColor);
    loseText->setOutlineThickness(3.f);
    loseText->setPosition({  window.getSize().x / 2.f - loseText->getGlobalBounds().size.x / 2.f,  window.getSize().y / 2.f - 100.f });

    playAgainText.emplace(font, "Press R to Play Again  |  Escape to Menu", 25);
    playAgainText->setFillColor(darkPink);
    playAgainText->setOutlineColor(textColor);
    playAgainText->setOutlineThickness(3.f);
    playAgainText->setPosition({ window.getSize().x / 2.f - playAgainText->getGlobalBounds().size.x / 2.f,  window.getSize().y / 2.f + 50.f });

    countdownText.emplace(font, "3", 150);
    countdownText->setFillColor(darkPink);
    countdownText->setOutlineColor(textColor);
    countdownText->setOutlineThickness(4.f);
    countdownText->setPosition({window.getSize().x / 2.f - countdownText->getGlobalBounds().size.x / 2.f, window.getSize().y / 2.f - 100.f  });
   
    initBricks();
    resetBall();

    if (!speakerOnTex.loadFromFile("assets/images/sound_on.png"))
        std::cout << "Speaker on texture failed!\n";
    if (!speakerOffTex.loadFromFile("assets/images/sound_off.png"))
        std::cout << "Speaker off texture failed!\n";
    speakerSprite.emplace(speakerOnTex);
    speakerSprite->setColor(darkPink);
    speakerSprite->setScale({ 0.07f, 0.07f });
    speakerSprite->setPosition({ 950.f, 10.f });
}
    
    void BrickBreaker::initBricks()
    {
        float brickWidth = 84.f;
        float brickHeight = 25.f;
        float padding = 8.f;
        float startX = 28.f;
        float startY = 80.f;   

        // two-hit brick colors 
        sf::Color twoHitColors[BRICK_ROWS] = {
            sf::Color(180, 50,  120),   // row 0 — deep magenta
            sf::Color(200, 80,  140),   // row 1
            sf::Color(220, 100, 160),   // row 2
            sf::Color(240, 130, 180),   // row 3
            sf::Color(255, 160, 200),   // row 4 — lightest
        };
        // one-hit brick colors 
        sf::Color oneHitColors[BRICK_ROWS] = {
            sf::Color(255, 105, 180),   // row 0 — darkPink
            sf::Color(255, 130, 190),   // row 1
            sf::Color(255, 155, 205),   // row 2
            sf::Color(255, 175, 215),   // row 3
            sf::Color(255, 200, 230),   // row 4 — babyPink
        };

        for (int row = 0; row < BRICK_ROWS; row++)
        {
            for (int col = 0; col < BRICK_COLS; col++)
            {
                int index = row * BRICK_COLS + col;

                float x = startX + col * (brickWidth + padding);
                float y = startY + row * (brickHeight + padding);

                // top 2 rows are two-hit bricks, rest are one-hit
                int hits = (row < 2) ? 2 : 1;
                sf::Color color = (row < 2) ? twoHitColors[row] : oneHitColors[row];

                bricks[index].init(x, y, brickWidth, brickHeight, hits, color);
            }
        }
    }


    void BrickBreaker::handleEvents()
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
                    // reset state
                    outSound.stop();
                    hitBrickSound.stop();
                    paddleHitSound.stop();
                    wallBounceSound.stop();
                    beepSound.stop();
                    score = 0;
                    lives = 3;
                    gameOver = false;
                    won = false;
                    countdownActive = true;
                    countdownValue = 3;
                    lastBeepValue = 0;
                    heart1->setColor(sf::Color::White);
                    heart2->setColor(sf::Color::White);
                    heart3->setColor(sf::Color::White);
                    countdownClock.restart();
                    countdownText->setString("3");
                    scoreText->setString("Score: 0");
                    paddle.setPosition({ window.getSize().x / 2.f - 60.f, window.getSize().y - 40.f });

                    resetBall();
                    initBricks();
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


    void BrickBreaker::update()
    {
        if (countdownActive)
        {
            int elapsed = (int)countdownClock.getElapsedTime().asSeconds();

            if (elapsed < 3)
            {
                countdownValue = 3 - elapsed;
                countdownText->setString(intToStr(countdownValue));
                countdownText->setPosition({  window.getSize().x / 2.f - countdownText->getGlobalBounds().size.x / 2.f, window.getSize().y / 2.f - 100.f    });

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
        ball->move(ballVelocity);

        if (ball->getPosition().x <= 0)
        {
            ballVelocity.x = std::abs(ballVelocity.x);
            wallBounceSound.play();
        }
        if (ball->getPosition().x + 25.f >= window.getSize().x)
        {
            ballVelocity.x = -std::abs(ballVelocity.x);
            wallBounceSound.play();
        }

        if (ball->getPosition().y <= 0)
        {
            ballVelocity.y = std::abs(ballVelocity.y);
            wallBounceSound.play();
        }
        if (ball->getPosition().y + 25.f >= window.getSize().y)
        {
            lives--;
            outSound.play();
            heart1->setColor(lives >= 1 ? sf::Color::White : sf::Color::Transparent);
            heart2->setColor(lives >= 2 ? sf::Color::White : sf::Color::Transparent);
            heart3->setColor(lives >= 3 ? sf::Color::White : sf::Color::Transparent);
            if (lives <= 0)
            {
                gameOver = true;
                return;
            }
            resetBall();
            return;
        }

        
        if (ball->getGlobalBounds().findIntersection(paddle.getGlobalBounds()))
        {
            ballVelocity.y = -std::abs(ballVelocity.y); 
            float paddleCenter = paddle.getPosition().x + 60.f;
            float ballCenter = ball->getPosition().x + 12.5f;
            float offset = ballCenter - paddleCenter;  // -60 to +60
            ballVelocity.x = offset * 0.08f;

            paddleHitSound.play();
        }
        for (int i = 0; i < MAX_BRICKS; i++)
        {
            if (!bricks[i].isActive()) continue;

            if (ball->getGlobalBounds().findIntersection(bricks[i].getBounds()))
            {
                bricks[i].hit();
                hitBrickSound.play();

                if (!bricks[i].isActive())  
                    score += 10;

                scoreText->setString("Score: " + intToStr(score));

                float ballCenterX = ball->getPosition().x + 12.5f;
                float ballCenterY = ball->getPosition().y + 12.5f;
                float brickLeft = bricks[i].getBounds().position.x;
                float brickRight = brickLeft + bricks[i].getBounds().size.x;
                float brickTop = bricks[i].getBounds().position.y;
                float brickBottom = brickTop + bricks[i].getBounds().size.y;

                float overlapLeft = ballCenterX - brickLeft;
                float overlapRight = brickRight - ballCenterX;
                float overlapTop = ballCenterY - brickTop;
                float overlapBottom = brickBottom - ballCenterY;

                float minOverlap = overlapLeft;
                if (overlapRight < minOverlap) minOverlap = overlapRight;
                if (overlapTop < minOverlap) minOverlap = overlapTop;
                if (overlapBottom < minOverlap) minOverlap = overlapBottom;

                if (minOverlap == overlapLeft || minOverlap == overlapRight)
                    ballVelocity.x = -ballVelocity.x;  
                else
                    ballVelocity.y = -ballVelocity.y; 

                break;
            }
        }
        bool anyActive = false;
        for (int i = 0; i < MAX_BRICKS; i++)
        {
            if (bricks[i].isActive())
            {
                anyActive = true;
                break;
            }
        }
        if (!anyActive)
        {
            gameOver = true;
            won = true;
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
            if (paddle.getPosition().x > 0)
                paddle.move({ -6.f, 0.f });

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
            if (paddle.getPosition().x + 120.f < window.getSize().x)
                paddle.move({ 6.f, 0.f });
    }
    void BrickBreaker::render()
    {
        window.clear(sf::Color(20, 0, 10));
        window.draw(*bg);
        if (countdownActive)
        {
            window.draw(*countdownText);
            window.display();
            return;
        }
        sf::RectangleShape uiDivider({ (float)window.getSize().x, 2.f });
        uiDivider.setPosition({ 0.f, 60.f });
        uiDivider.setFillColor(sf::Color(255, 105, 180, 150));
        window.draw(uiDivider);
        for (int i = 0; i < MAX_BRICKS; i++)
        {
            if (bricks[i].isActive())
                window.draw(bricks[i].getShape());
        }
        window.draw(paddle);
        window.draw(*ball);
        window.draw(*scoreText);
        window.draw(*heart1);
        window.draw(*heart2);
        window.draw(*heart3);
        if (gameOver)
        {
            sf::RectangleShape overlay({ (float)window.getSize().x, (float)window.getSize().y });
            overlay.setFillColor(sf::Color(0, 0, 0, 150));
            window.draw(overlay);

            if (won)
                window.draw(*winText);
            else
                window.draw(*loseText);

            window.draw(*playAgainText);
        }
        if (music.getStatus() == sf::Music::Status::Playing)
            speakerSprite->setTexture(speakerOnTex);
        else
            speakerSprite->setTexture(speakerOffTex);
        window.draw(*speakerSprite);
        window.display();
    }


void BrickBreaker::resetBall()
{
    ball->setPosition({ window.getSize().x / 2.f - 12.5f,  window.getSize().y / 2.f   });
    ballVelocity = { 8.f, -8.f };
}



void BrickBreaker::run()
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
