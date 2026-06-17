#include "Pong.h"
using namespace std;


static string intToStr(int n)
{   //to_string
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


Pong::Pong(sf::RenderWindow& win, sf::Music& bgMusic, const string& p1, const string& p2)
    : window(win),
    music(bgMusic),
    paddleLeft({ 15.f, 100.f }),
    paddleRight({ 15.f, 100.f }),
    ballVelocity(6.f, 6.f),
    scoreLeft(0),
    babyPink(255, 182, 193),
    lightPink(255, 204, 229),
    darkPink(255, 105, 180),
    textColor(255, 240, 245),
    transparent(0, 0, 0, 0),
    stuble(255, 182, 193, 150),
    pyaraclr(255, 182, 193, 80),
    scoreRight(0),
    countdownValue(3),
    lastBeepValue(0),
    hitSound(hitBuffer),
    bounceSound(bounceBuffer),
    beepSound(beepBuffer),
    scoreSound(scoreBuffer),
    countdownActive(true),
    gameOver(false),
    running(true),
    p1Name(p1),
    p2Name(p2),
    winnerName("")

{

    if (!font.openFromFile("assets/fonts/CherryBomb.ttf"))
        std::cout << "Font failed to load!\n";

    if (!ballTexture.loadFromFile("assets/images/bowl.png"))
        std::cout << "Ball texture failed to load!\n";
    ball.emplace(ballTexture);
    ball->setColor(darkPink);
    
    sf::Vector2u texSize = ballTexture.getSize();
    ball->setScale({ 40.f / texSize.x, 40.f / texSize.y });

    if (!bgTexture.loadFromFile("assets/images/background.jpeg"))
        std::cout << "Background failed to load!\n";
    if (!scoreBuffer.loadFromFile("assets/sounds/pongdeath.mp3"))
        std::cout << "Score sound failed to load!\n";
    scoreSound.setVolume(100.f);

    bg.emplace(bgTexture);
    float scaleX = (float)window.getSize().x / bgTexture.getSize().x;
    float scaleY = (float)window.getSize().y / bgTexture.getSize().y;
    bg->setScale({ scaleX, scaleY });
   
    labelLeft.emplace(font, p1 , 30);
    labelLeft->setFillColor(sf::Color(darkPink));
    labelLeft->setOutlineColor(sf::Color(textColor));
    labelLeft->setOutlineThickness(2.f);
    labelLeft->setPosition({ 40.f, 10.f });
    
    labelRight.emplace(font, p2 , 30);
    labelRight->setFillColor(sf::Color(darkPink));
    labelRight->setOutlineColor(sf::Color(textColor));
    labelRight->setOutlineThickness(2.f);
    labelRight->setPosition({ window.getSize().x - labelRight->getGlobalBounds().size.x - 80.f, 10.f });
  
    if (!hitBuffer.loadFromFile("assets/sounds/bounce.flac"))
        std::cout << "Hit sound failed to load!\n";

    if (!bounceBuffer.loadFromFile("assets/sounds/bounce.flac"))
        std::cout << "Bounce sound failed to load!\n";
    if (!beepBuffer.loadFromFile("assets/sounds/beep.wav"))
        std::cout << "Beep sound failed to load!\n";
    beepSound.setVolume(100.f);
    hitSound.setVolume(100.f);
    bounceSound.setVolume(100.f);
    
    ball->setPosition({ window.getSize().x / 2.f - 20.f, window.getSize().y / 2.f - 20.f }); //(centre)

    
    paddleLeft.setFillColor(sf::Color(darkPink));  
    paddleLeft.setPosition({ 20.f, window.getSize().y / 2.f - 50.f });

    paddleRight.setFillColor(sf::Color(darkPink));  
    paddleRight.setPosition({ window.getSize().x - 35.f, window.getSize().y / 2.f - 50.f  });

    scoreText.emplace(font, "0     0", 50);
    scoreText->setFillColor(sf::Color(darkPink));
    scoreText->setOutlineColor(sf::Color(textColor));
    scoreText->setOutlineThickness(2.f);
    scoreText->setPosition({ window.getSize().x / 2.f - 60.f, 10.f });

    winText.emplace(font, "", 70);
    winText->setFillColor(sf::Color(darkPink));
    winText->setOutlineColor(sf::Color(textColor));
    winText->setOutlineThickness(3.f);

    int numDashes = 20;
    float dashHeight = 20.f;
    float dashWidth = 6.f;
    float gap = (window.getSize().y - (numDashes * dashHeight)) / (numDashes - 1);

    for (int i = 0; i < numDashes; i++)
    {
        sf::RectangleShape dash({ dashWidth, dashHeight });
        dash.setFillColor(sf::Color(darkPink));  
        dash.setPosition({ window.getSize().x / 2.f - dashWidth / 2.f,  i * (dashHeight + gap)});
        divider.push_back(dash);
    }

    playAgainText.emplace(font, "Press R to Play Again  |  Escape to Menu", 25);
    playAgainText->setFillColor(sf::Color(darkPink));
    playAgainText->setOutlineColor(sf::Color(textColor));
    playAgainText->setOutlineThickness(1.f);
    playAgainText->setPosition({window.getSize().x / 2.f - playAgainText->getGlobalBounds().size.x / 2.f, window.getSize().y / 2.f + 50.f });
    playAgainText->setFillColor(sf::Color(darkPink));
    playAgainText->setOutlineColor(sf::Color(textColor));   
    playAgainText->setOutlineThickness(3.f);               

    countdownText.emplace(font, "3", 150);
    countdownText->setFillColor(sf::Color(255, 105, 180));
    countdownText->setOutlineColor(sf::Color(255, 240, 245));
    countdownText->setOutlineThickness(4.f);
    countdownText->setPosition({ window.getSize().x / 2.f - countdownText->getGlobalBounds().size.x / 2.f, window.getSize().y / 2.f - 100.f});

    if (!speakerOnTex.loadFromFile("assets/images/sound_on.png"))
        std::cout << "Speaker on texture failed!\n";
    if (!speakerOffTex.loadFromFile("assets/images/sound_off.png"))
        std::cout << "Speaker off texture failed!\n";
    speakerSprite.emplace(speakerOnTex);
    speakerSprite->setColor(darkPink);
    speakerSprite->setScale({ 0.07f, 0.07f });
    speakerSprite->setPosition({ 950.f, 10.f });
}

void Pong::handleEvents()
{
    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
            window.close();

        if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == sf::Keyboard::Key::Escape)
            {
                running = false; //main menu
            }
            if (keyPressed->code == sf::Keyboard::Key::M)
            {
                if (music.getStatus() == sf::Music::Status::Playing)
                    music.pause();
                else
                    music.play();
            }
            if (keyPressed->code == sf::Keyboard::Key::R && gameOver)
            { //reset full game
                scoreLeft = 0;
                scoreRight = 0;
                winnerName = "";   
                gameOver = false;
                countdownActive = true;       
                countdownClock.restart();      
                countdownValue = 3;           
                countdownText->setString("3"); 
                lastBeepValue = 0;   
                ball->setPosition({ window.getSize().x / 2.f - 20.f,   window.getSize().y / 2.f - 20.f });
                ballVelocity = { 6.f, 6.f };
                paddleLeft.setPosition({ 20.f, window.getSize().y / 2.f - 50.f });
                paddleRight.setPosition({  window.getSize().x - 35.f, window.getSize().y / 2.f - 50.f  });
                scoreText->setString("0     0");
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

void Pong::update()
{
    if (countdownActive)
    {
        int elapsed = (int)countdownClock.getElapsedTime().asSeconds();
        if (elapsed < 3)
        {
            countdownValue = 3 - elapsed;
            countdownText->setString(intToStr(countdownValue));
            countdownText->setPosition({ window.getSize().x / 2.f - countdownText->getGlobalBounds().size.x / 2.f,window.getSize().y / 2.f - 100.f });
            if (countdownValue != lastBeepValue)
            { beepSound.play();lastBeepValue = countdownValue;}
        }
        else
        { countdownActive = false;   }

    return;   // don't update anything else during countdown
    }

    if (gameOver) return; 
    

    ball->move(ballVelocity);
    if (ball->getPosition().y <= 0) //top-wall
    {
        ballVelocity.y = std::abs(ballVelocity.y);
        bounceSound.play();    
    }
    
    if (ball->getPosition().y + 40.f >= window.getSize().y) //bottomwall
    {
        ballVelocity.y = -std::abs(ballVelocity.y);
        bounceSound.play();    
    }

    if (ball->getGlobalBounds().findIntersection(paddleLeft.getGlobalBounds()))
    {
        ballVelocity.x = std::abs(ballVelocity.x);
        hitSound.play();   
    }

    if (ball->getGlobalBounds().findIntersection(paddleRight.getGlobalBounds()))
    {
        ballVelocity.x = -std::abs(ballVelocity.x);
        hitSound.play();   
    }
    
    if (ball->getPosition().x <= 0) //right scores
    {
        scoreRight++;
        scoreSound.play();
        ball->setPosition({
            window.getSize().x / 2.f - 20.f,
            window.getSize().y / 2.f - 20.f
            });
        ballVelocity = { 6.f, 6.f };
        scoreText->setString(intToStr(scoreLeft) + "     " + intToStr(scoreRight));
    }

    if (ball->getPosition().x + 40.f >= window.getSize().x) //left scores
    {
        scoreLeft++;
        scoreSound.play();
        ball->setPosition({
            window.getSize().x / 2.f - 20.f,
            window.getSize().y / 2.f - 20.f
            });
        ballVelocity = { -6.f, 6.f };
        scoreText->setString(intToStr(scoreLeft) + "     " + intToStr(scoreRight));
    }
    
    if (scoreLeft >= 7)
    {
        gameOver = true;
        winnerName = p1Name;
        winText->setString(p1Name + " Wins!  " +  intToStr(scoreLeft) + " - " + intToStr(scoreRight));
        winText->setPosition({ window.getSize().x / 2.f - winText->getGlobalBounds().size.x / 2.f,  window.getSize().y / 2.f - 50.f  });
    }
    if (scoreRight >= 7)
    {
        gameOver = true;
        winnerName = p2Name;
        winText->setString(p2Name + " Wins!  " + intToStr(scoreLeft) + " - " + intToStr(scoreRight));
        winText->setPosition({window.getSize().x / 2.f - winText->getGlobalBounds().size.x / 2.f,  window.getSize().y / 2.f - 50.f  });
    }
  
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        if (paddleLeft.getPosition().y > 0)
            paddleLeft.move({ 0.f, -6.f });
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        if (paddleLeft.getPosition().y + 100.f < window.getSize().y)
            paddleLeft.move({ 0.f, 6.f });
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up))
        if (paddleRight.getPosition().y > 0)
            paddleRight.move({ 0.f, -6.f });
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
        if (paddleRight.getPosition().y + 100.f < window.getSize().y)
            paddleRight.move({ 0.f, 6.f });
}




void Pong::render()
{
    window.clear(sf::Color(20, 0, 10));
    window.draw(*bg);

    if (countdownActive)
    {
        window.draw(*countdownText);
        window.display();
        return;
    }
    if (!gameOver)
        for (auto& dash : divider)
            window.draw(dash);

    
    window.draw(paddleLeft);
    window.draw(paddleRight);
    window.draw(*ball);

    window.draw(*labelLeft);
    window.draw(*labelRight);
    window.draw(*scoreText);

    if (gameOver)
    {
        window.draw(*winText);
        window.draw(*playAgainText);
    }
    if (music.getStatus() == sf::Music::Status::Playing)
        speakerSprite->setTexture(speakerOnTex);
    else
        speakerSprite->setTexture(speakerOffTex);
    window.draw(*speakerSprite);
    window.display();
}



void Pong::run()
{
    window.setFramerateLimit(60);    
    countdownClock.restart();
    while(window.isOpen() && running)
    {
        handleEvents();
        update();
        render();
    }
}