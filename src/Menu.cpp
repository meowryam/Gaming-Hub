#include "Menu.h"
#include "Pong.h"
#include "BrickBreaker.h"
#include "Snake.h"
using namespace std;

static int strLen(const string& s)
{
    int len = 0;
    while (s[len] != '\0') len++;
    return len;
}

static void strPop(string& s)
{
    string temp = "";
    int len = strLen(s);
    for (int i = 0; i < len - 1; i++)
        temp += s[i];
    s = temp;
}

Menu::Menu()
    : window(sf::VideoMode({ 1000, 800 }), "Pookie Gaming Hub"),
    babyPink(255, 182, 193),
    lightPink(255, 204, 229),
    darkPink(255, 105, 180),
    textColor(255, 240, 245),
    transparent(0, 0, 0, 0),
    stuble(255, 182, 193, 150),
    pyaraclr(255, 182, 193, 80),
    button1({ 340.f, 80.f }),
    button2({ 340.f, 80.f }),
    button3({ 340.f, 80.f }),
    button4({ 340.f, 80.f }),
    button5({ 340.f, 80.f }),
    overlay({ 340.f, 400.f }),
    lbFilter("All"),
    gameState(0)
{
    if (!font1.openFromFile("assets/fonts/CherryBomb.ttf"))
        std::cout << "Font1 failed to load\n";

    title.emplace(font1, "GAMING HUB", 75);
    titleShadow.emplace(font1, "GAMING HUB", 75);
    game1.emplace(font1, "Ping Pong", 40);
    game2.emplace(font1, "Feed The Snake", 40);
    game3.emplace(font1, "Brick Breaker", 40);
    title2.emplace(font1, "LeaderBoard", 40);
    title3.emplace(font1, "Exit", 40);

    title->setPosition({ 265.f, 60.f });
    title->setFillColor(darkPink);
    title->setOutlineColor(textColor);
    title->setOutlineThickness(3.f);
    titleShadow->setPosition({ 264.f, 64.f });
    titleShadow->setFillColor(sf::Color(150, 80, 120));
    
    game1->setFillColor(textColor);
    game1->setOutlineColor(darkPink);
    game1->setOutlineThickness(3.f);

    game2->setFillColor(textColor);
    game2->setOutlineColor(darkPink);
    game2->setOutlineThickness(3.f);

    game3->setFillColor(textColor);
    game3->setOutlineColor(darkPink);
    game3->setOutlineThickness(3.f);
  
    title2->setFillColor(textColor);
    title2->setOutlineColor(darkPink);
    title2->setOutlineThickness(3.f);

    title3->setFillColor(textColor);
    title3->setOutlineColor(darkPink);
    title3->setOutlineThickness(3.f);

    game1->setPosition({ 120.f, 258.f });
    game2->setPosition({ 120.f, 338.f });
    game3->setPosition({ 120.f, 418.f });
    title2->setPosition({ 120.f, 498.f });
    title3->setPosition({ 120.f, 578.f });

    button1.setPosition({ 95.f, 240.f });
    button2.setPosition({ 95.f, 320.f });
    button3.setPosition({ 95.f, 400.f });
    button4.setPosition({ 95.f, 480.f });
    button5.setPosition({ 95.f, 560.f });

    overlay.setPosition({ 95.f, 240.f });
    overlay.setFillColor(pyaraclr);

    button1.setFillColor(transparent);
    button2.setFillColor(transparent);
    button3.setFillColor(transparent);
    button4.setFillColor(transparent);
    button5.setFillColor(transparent);

    if (!music.openFromFile("assets/sounds/Meow.mp3"))
        std::cout << "Music failed to load\n";
    else
    {
        music.setLooping(true);
        music.setVolume(10);
        music.play();
    }

    if (!bgTexture.loadFromFile("assets/images/background.jpeg"))
        std::cout << "Background failed to load\n";
    bg.emplace(bgTexture);
    float scaleX = (float)window.getSize().x / bgTexture.getSize().x;
    float scaleY = (float)window.getSize().y / bgTexture.getSize().y;
    bg->setScale({ scaleX, scaleY });

    if (!mascotTexture.loadFromFile("assets/images/food_heart.png"))
        std::cout << "Mascot failed to load\n";
    mascot.emplace(mascotTexture);
    mascot->setPosition({ 550.f, 250.f });
    mascot->setScale({ 8.f, 8.f });
    mascot->setColor(stuble);
    if (!speakerOnTex.loadFromFile("assets/images/sound_on.png"))
        std::cout << "Speaker on texture failed!\n";
    if (!speakerOffTex.loadFromFile("assets/images/sound_off.png"))
        std::cout << "Speaker off texture failed!\n";
    speakerSprite.emplace(speakerOnTex);
    speakerSprite->setColor(darkPink);
    speakerSprite->setScale({ 0.07f, 0.07f });
    speakerSprite->setPosition({ 950.f, 10.f });

    creditText.emplace(font1, "meow meow meow", 20);
    creditText->setFillColor(stuble);
    creditText->setPosition({ 650.f, 750.f });
    
    for (int i = 0; i < 10; i++)
    {
        particles[i].setRadius(5.f + (i % 3) * 3.f);
        particles[i].setFillColor(sf::Color(255, 105, 180, 60 + (i * 15)));
        particles[i].setPosition({ 500.f + (i % 5) * 80.f, 580.f + (i % 2) * 80.f });
    }
}

string Menu::getPlayerName(const string& prompt)
{
    string name = "";

    sf::Text promptText(font1, prompt, 35);
    promptText.setFillColor(darkPink);
    promptText.setOutlineColor(textColor);
    promptText.setOutlineThickness(2.f);
    promptText.setPosition({ 150.f, 350.f });

   
    sf::Text nameDisplay(font1, "_", 40);
    nameDisplay.setFillColor(darkPink);
    nameDisplay.setOutlineColor(textColor);
    nameDisplay.setOutlineThickness(2.f);
    nameDisplay.setPosition({ 150.f, 420.f });

    while (window.isOpen())
    {
        while (const optional ev = window.pollEvent())
        {
            if (ev->is<sf::Event::Closed>()) window.close();

            if (const auto* kp = ev->getIf<sf::Event::KeyPressed>())
            {
                if (kp->code == sf::Keyboard::Key::Enter && name != "")
                    return name;

                    if (kp->code == sf::Keyboard::Key::Backspace && name != "")
                        strPop(name);
            }

            if (const auto* te = ev->getIf<sf::Event::TextEntered>())
            {
                char c = (char)te->unicode;
                if (c >= 32 && c < 127 && strLen(name) < 16)
                    name += c;
            }
        }

        window.clear(sf::Color(20, 0, 10));
        window.draw(*bg);
        window.draw(promptText);
        nameDisplay.setString(name + "_");   // blinking cursor effect
        window.draw(nameDisplay);
        window.display();
    }
    return name;
}






void Menu::run()
{
    while (window.isOpen())
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* kp = event->getIf<sf::Event::KeyPressed>())
            {
                if (kp->code == sf::Keyboard::Key::M)
                {
                    if (music.getStatus() == sf::Music::Status::Playing)
                        music.pause();
                    else
                        music.play();
                }
            }
            if (gameState == 4)
            {
                if (const auto* kp = event->getIf<sf::Event::KeyPressed>())
                {
                    if (kp->code == sf::Keyboard::Key::Escape) gameState = 0;
                    if (kp->code == sf::Keyboard::Key::Num1)   lbFilter = "All";
                    if (kp->code == sf::Keyboard::Key::Num2)   lbFilter = "Snake";
                    if (kp->code == sf::Keyboard::Key::Num3)   lbFilter = "BrickBreaker";
                    if (kp->code == sf::Keyboard::Key::Num4)   lbFilter = "Pong";
                }
            }

            if (event->is<sf::Event::MouseButtonPressed>())
            {
                if (event->getIf<sf::Event::MouseButtonPressed>()->button == sf::Mouse::Button::Left)
                {
                    if (game1->getGlobalBounds().contains(sf::Vector2f(mousePos))) gameState = 1;
                    if (game2->getGlobalBounds().contains(sf::Vector2f(mousePos))) gameState = 2;
                    if (game3->getGlobalBounds().contains(sf::Vector2f(mousePos))) gameState = 3;
                    if (title2->getGlobalBounds().contains(sf::Vector2f(mousePos))) gameState = 4;
                    if (title3->getGlobalBounds().contains(sf::Vector2f(mousePos))) window.close();
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

        float offsetX = 8.f * sin(clock.getElapsedTime().asSeconds() * 2);
        mascot->setPosition({ 580.f + offsetX, 250.f });


        // Hover effects - button color change
        button1.setFillColor(button1.getGlobalBounds().contains(sf::Vector2f(mousePos)) ? darkPink : transparent);
        button2.setFillColor(button2.getGlobalBounds().contains(sf::Vector2f(mousePos)) ? darkPink : transparent);
        button3.setFillColor(button3.getGlobalBounds().contains(sf::Vector2f(mousePos)) ? darkPink : transparent);
        button4.setFillColor(button4.getGlobalBounds().contains(sf::Vector2f(mousePos)) ? darkPink : transparent);
        button5.setFillColor(button5.getGlobalBounds().contains(sf::Vector2f(mousePos)) ? darkPink : transparent);
        window.clear();

        if (gameState == 0)
        {
            window.draw(*bg);
            window.draw(*mascot);
            window.draw(overlay);
            window.draw(button1);
            window.draw(button2);
            window.draw(button3);
            window.draw(button4);
            window.draw(button5);
            window.draw(*titleShadow);
            window.draw(*title);
            window.draw(*game1);
            window.draw(*game2);
            window.draw(*game3);
            window.draw(*title2);
            window.draw(*title3);
            window.draw(*creditText);
            for (int i = 0; i < 10; i++)
                window.draw(particles[i]);
            if (music.getStatus() == sf::Music::Status::Playing)
                speakerSprite->setTexture(speakerOnTex);
            else
                speakerSprite->setTexture(speakerOffTex);
            window.draw(*speakerSprite);
        }
        else if (gameState == 1)
        {
            string p1 = getPlayerName("Player 1 - Enter your name:");
            string p2 = getPlayerName("Player 2 - Enter your name:");
            Pong pong(window, music, p1, p2); pong.run();
            string winner = pong.getWinner();
            if (winner != "")
                leaderboard.AddRecord(winner, "Pong", pong.getWinnerScore());
            gameState = 0;
        }

        else if (gameState == 3)
        {
            string p1 = getPlayerName("Enter your name:");
            BrickBreaker bb(window, music);  bb.run();
            if (bb.getFinalScore() > 0)
                leaderboard.AddRecord(p1, "BrickBreaker", bb.getFinalScore());
            gameState = 0;
        }

        else if (gameState == 2)
        {
            string p1 = getPlayerName("Enter your name:");
            Snake snake(window, music); snake.run();
            if (snake.getFinalScore() > 0)
                leaderboard.AddRecord(p1, "Snake", snake.getFinalScore());
            gameState = 0;
        
        }
        else if (gameState == 4)
        {
            window.clear();
            leaderboard.DisplayLeaderboard(window, lbFilter);
            window.display();
            continue; 
        }
        window.display();
    }
}