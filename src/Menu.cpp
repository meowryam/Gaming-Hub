#include "Menu.h"
#include "Pong.h"
#include "BrickBreaker.h"
#include "Snake.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
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

// ──────────────────────────────────────────────────
//  Layout constants
// ──────────────────────────────────────────────────
static const float WIN_W = 1000.f;
static const float WIN_H = 800.f;
static const float BTN_W = 520.f;   // wider pill body
static const float BTN_H = 60.f;
static const float BTN_R = BTN_H / 2.f;
static const float BTN_CX = WIN_W / 2.f;
static const float BTN_RECT_X = BTN_CX - BTN_W / 2.f;
static const float BTN_START_Y = 320.f;   // moved up
static const float BTN_GAP = 72.f;    // tighter gap

// ──────────────────────────────────────────────────
//  initPill
// ──────────────────────────────────────────────────
static void initPill(
    sf::RectangleShape& rect,
    sf::CircleShape& capL, sf::CircleShape& capR,
    sf::CircleShape& bdrL, sf::CircleShape& bdrR,
    sf::RectangleShape& bdr,
    float y,
    sf::Color fillCol, sf::Color borderCol)
{
    rect.setSize({ BTN_W, BTN_H });
    rect.setPosition({ BTN_RECT_X, y });
    rect.setFillColor(fillCol);
    rect.setOutlineThickness(0.f);

    bdr.setSize({ BTN_W + 5.f, BTN_H + 5.f });
    bdr.setPosition({ BTN_RECT_X - 2.5f, y - 2.5f });
    bdr.setFillColor(borderCol);
    bdr.setOutlineThickness(0.f);

    capL.setRadius(BTN_R);
    capL.setPosition({ BTN_RECT_X - BTN_R, y });
    capL.setFillColor(fillCol);
    capL.setPointCount(64);

    capR.setRadius(BTN_R);
    capR.setPosition({ BTN_RECT_X + BTN_W - BTN_R, y });
    capR.setFillColor(fillCol);
    capR.setPointCount(64);

    bdrL.setRadius(BTN_R + 2.5f);
    bdrL.setPosition({ BTN_RECT_X - BTN_R - 2.5f, y - 2.5f });
    bdrL.setFillColor(borderCol);
    bdrL.setPointCount(64);

    bdrR.setRadius(BTN_R + 2.5f);
    bdrR.setPosition({ BTN_RECT_X + BTN_W - BTN_R - 2.5f, y - 2.5f });
    bdrR.setFillColor(borderCol);
    bdrR.setPointCount(64);
}

// ──────────────────────────────────────────────────
//  Sprinkle shape helpers
// ──────────────────────────────────────────────────
static void makeDiamond(sf::ConvexShape& s, float r)
{
    s.setPointCount(4);
    s.setPoint(0, { 0.f, -r });
    s.setPoint(1, { r, 0.f });
    s.setPoint(2, { 0.f, r });
    s.setPoint(3, { -r, 0.f });
    s.setOrigin({ 0.f, 0.f });
}

static void makeDot(sf::ConvexShape& s, float r)
{
    const int N = 16;
    s.setPointCount(N);
    for (int p = 0; p < N; p++)
    {
        float a = (float)p / N * 6.2831853f;
        s.setPoint(p, { r * cos(a), r * sin(a) });
    }
    s.setOrigin({ 0.f, 0.f });
}

static void makeSparkle(sf::ConvexShape& s, float r)
{
    // 4-point sparkle/star: long spikes N/E/S/W, short spikes between
    float rIn = r * 0.32f;
    s.setPointCount(8);
    s.setPoint(0, { 0.f,  -r });
    s.setPoint(1, { rIn, -rIn });
    s.setPoint(2, { r,    0.f });
    s.setPoint(3, { rIn,  rIn });
    s.setPoint(4, { 0.f,  r });
    s.setPoint(5, { -rIn, rIn });
    s.setPoint(6, { -r,   0.f });
    s.setPoint(7, { -rIn, -rIn });
    s.setOrigin({ 0.f, 0.f });
}

static void makeHeart(sf::ConvexShape& s, float r)
{
    // Simple symmetric heart traced with 10 points, scaled by r
    float pts[10][2] = {
        { 0.00f,  0.65f}, {-0.55f, 0.15f}, {-0.85f, -0.25f}, {-0.55f, -0.65f}, {-0.18f, -0.55f},
        { 0.00f, -0.30f},
        { 0.18f, -0.55f}, { 0.55f, -0.65f}, { 0.85f, -0.25f}, { 0.55f,  0.15f}
    };
    s.setPointCount(10);
    for (int p = 0; p < 10; p++)
        s.setPoint(p, { pts[p][0] * r, pts[p][1] * r });
    s.setOrigin({ 0.f, 0.f });
}

// ──────────────────────────────────────────────────
//  Constructor
// ──────────────────────────────────────────────────
Menu::Menu()
    : window(sf::VideoMode({ (unsigned)WIN_W, (unsigned)WIN_H }), "Pookie Gaming Hub"),
    babyPink(255, 182, 193),
    lightPink(255, 215, 230),
    darkPink(220, 75, 145),
    textColor(255, 240, 245),
    transparent(0, 0, 0, 0),
    stuble(255, 182, 193, 150),
    pyaraclr(255, 182, 193, 80),
    creamWhite(255, 248, 252),
    softPink(250, 220, 235),
    pillBorder(225, 150, 185),
    button1({ BTN_W, BTN_H }),
    button2({ BTN_W, BTN_H }),
    button3({ BTN_W, BTN_H }),
    button4({ BTN_W, BTN_H }),
    button5({ BTN_W, BTN_H }),
    overlay({ BTN_W, 1.f }),
    lbFilter("All"),
    gameState(0)
{
    srand((unsigned int)time(nullptr));

    if (!font1.openFromFile("assets/fonts/CherryBomb.ttf"))
        std::cout << "Font1 failed to load\n";

    // ── Title ────────────────────────────────────────
    title.emplace(font1, "Pookie", 95);
    title->setFillColor(darkPink);
    title->setOutlineColor(sf::Color(255, 255, 255, 200));
    title->setOutlineThickness(5.f);
    title->setPosition({ WIN_W / 2.f - title->getGlobalBounds().size.x / 2.f, 10.f });

    titleShadow.emplace(font1, "Pookie", 95);
    titleShadow->setFillColor(sf::Color(190, 60, 120, 90));
    titleShadow->setOutlineThickness(0.f);
    titleShadow->setPosition({ WIN_W / 2.f - titleShadow->getGlobalBounds().size.x / 2.f + 5.f, 16.f });

    // ── Subtitle ─────────────────────────────────────
    subtitle.emplace(font1, "GAMING  HUB", 28);
    subtitle->setFillColor(sf::Color(195, 90, 140));
    subtitle->setOutlineColor(sf::Color(255, 255, 255, 100));
    subtitle->setOutlineThickness(2.f);
    subtitle->setLetterSpacing(2.f);
    subtitle->setPosition({ WIN_W / 2.f - subtitle->getGlobalBounds().size.x / 2.f, 128.f });

    // ── Button labels ────────────────────────────────
    sf::Color labelCol(190, 80, 130);

    game1.emplace(font1, "Ping Pong", 34);
    game2.emplace(font1, "Feed The Snake", 34);
    game3.emplace(font1, "Brick Breaker", 34);
    title2.emplace(font1, "Leaderboard", 34);
    title3.emplace(font1, "Exit", 34);

    auto centerLabel = [&](std::optional<sf::Text>& t, float y) {
        t->setFillColor(labelCol);
        t->setOutlineThickness(0.f);
        float tx = WIN_W / 2.f - t->getGlobalBounds().size.x / 2.f;
        float ty = y + BTN_H / 2.f - t->getGlobalBounds().size.y / 2.f - 3.f;
        t->setPosition({ tx, ty });
        };

    for (int i = 0; i < 5; i++)
    {
        float y = BTN_START_Y + i * BTN_GAP;
        std::optional<sf::Text>* labels[5] = { &game1, &game2, &game3, &title2, &title3 };
        centerLabel(*labels[i], y);
    }

    // ── Pill buttons ──────────────────────────────────
    sf::Color fillCol = creamWhite;
    sf::Color bdrCol = pillBorder;
    for (int i = 0; i < 5; i++)
    {
        float y = BTN_START_Y + i * BTN_GAP;
        sf::RectangleShape* rects[5] = { &button1,&button2,&button3,&button4,&button5 };
        initPill(*rects[i], pillCapL[i], pillCapR[i],
            pillCapLBorder[i], pillCapRBorder[i],
            buttonBorder[i], y, fillCol, bdrCol);
    }

    overlay.setFillColor(transparent);

    // ── Background ───────────────────────────────────
    if (!bgTexture.loadFromFile("assets/images/background.jpeg"))
        std::cout << "Background failed to load\n";
    bg.emplace(bgTexture);
    bg->setScale({ WIN_W / bgTexture.getSize().x, WIN_H / bgTexture.getSize().y });

    // ── Mascot ───────────────────────────────────────
    if (!mascotTexture.loadFromFile("assets/images/food_heart.png"))
        std::cout << "Mascot failed to load\n";
    mascot.emplace(mascotTexture);
    {
        float mScale = 150.f / mascotTexture.getSize().x;
        mascot->setScale({ mScale, mScale });
        float mw = mascotTexture.getSize().x * mScale;
        mascot->setPosition({ WIN_W / 2.f - mw / 2.f, 148.f });
        mascot->setColor(sf::Color(255, 255, 255, 240));
    }

    // ── Speaker ──────────────────────────────────────
    if (!speakerOnTex.loadFromFile("assets/images/sound_on.png"))
        std::cout << "Speaker on texture failed!\n";
    if (!speakerOffTex.loadFromFile("assets/images/sound_off.png"))
        std::cout << "Speaker off texture failed!\n";
    speakerSprite.emplace(speakerOnTex);
    speakerSprite->setColor(darkPink);
    speakerSprite->setScale({ 0.07f, 0.07f });
    speakerSprite->setPosition({ 950.f, 10.f });

    // ── Music ────────────────────────────────────────
    if (!music.openFromFile("assets/sounds/Meow.mp3"))
        std::cout << "Music failed to load\n";
    else { music.setLooping(true); music.setVolume(10); music.play(); }

    // ── Credit ───────────────────────────────────────
    creditText.emplace(font1, "meow meow meow", 16);
    creditText->setFillColor(sf::Color(210, 140, 175, 140));
    creditText->setPosition({ WIN_W / 2.f - creditText->getGlobalBounds().size.x / 2.f, WIN_H - 26.f });

    // ── Soft glow blobs (behind title area) ──────────
    // Large semi-transparent circles give a dreamy cloud vibe
    float blobData[5][4] = {
        // cx,   cy,   r,    alpha
        { 500.f, 100.f, 200.f, 30.f },
        { 200.f, 400.f, 140.f, 22.f },
        { 800.f, 350.f, 160.f, 22.f },
        { 100.f, 650.f, 120.f, 18.f },
        { 880.f, 680.f, 130.f, 18.f },
    };
    for (int i = 0; i < NUM_BLOBS; i++)
    {
        blobs[i].setRadius(blobData[i][2]);
        blobs[i].setPointCount(64);
        blobs[i].setFillColor(sf::Color(255, 200, 220, (uint8_t)blobData[i][3]));
        blobs[i].setPosition({ blobData[i][0] - blobData[i][2], blobData[i][1] - blobData[i][2] });
    }

    // ── Sprinkles ─────────────────────────────────────
    // Mix of: diamonds, dots, hearts, sparkles — scattered across the whole window
    // We store positions and use setRotation each frame for a slow spin
    for (int i = 0; i < NUM_SPRINKLES; i++)
    {
        sprinkleX[i] = 20.f + (rand() % (int)(WIN_W - 40));
        sprinkleY[i] = 20.f + (rand() % (int)(WIN_H - 40));
        sprinkleAngle[i] = (float)(rand() % 360);
        sprinkleSpeed[i] = 8.f + (rand() % 30) * 0.5f;   // deg/sec

        // Cycle through: diamond, dot, heart, heart, sparkle
        int kind = i % 5;
        sprinkleKind[i] = kind;
        float r;

        if (kind == 0)
        {
            // 4-point star (diamond)
            r = 4.f + (rand() % 5) * 1.5f;
            makeDiamond(sprinkles[i], r);
            sprinkles[i].setFillColor(sf::Color(200, 30, 110, 140 + rand() % 80));
        }
        else if (kind == 1)
        {
            // Tiny circle dot
            r = 2.f + (rand() % 3);
            makeDot(sprinkles[i], r);
            sprinkles[i].setFillColor(sf::Color(255, 250, 235, 150 + rand() % 80));
        }
        else if (kind == 2)
        {
            // Small heart
            r = 7.f + (rand() % 4) * 1.5f;
            makeHeart(sprinkles[i], r);
            sprinkles[i].setFillColor(sf::Color(225, 50, 110, 140 + rand() % 70));
        }
        else if (kind == 3)
        {
            // Small heart (deeper pink variant)
            r = 7.f + (rand() % 4) * 1.5f;
            makeHeart(sprinkles[i], r);
            sprinkles[i].setFillColor(sf::Color(170, 20, 90, 130 + rand() % 70));
        }
        else
        {
            // Sparkle (4-point star with concave sides)
            r = 6.f + (rand() % 5) * 1.5f;
            makeSparkle(sprinkles[i], r);
            sprinkles[i].setFillColor(sf::Color(255, 245, 225, 150 + rand() % 80));
        }

        sprinkles[i].setPosition({ sprinkleX[i], sprinkleY[i] });
    }
}

// ──────────────────────────────────────────────────
//  drawPillButton
// ──────────────────────────────────────────────────
void Menu::drawPillButton(int idx, bool hovered)
{
    sf::RectangleShape* rects[5] = { &button1,&button2,&button3,&button4,&button5 };
    sf::RectangleShape* borders[5] = { &buttonBorder[0],&buttonBorder[1],
                                       &buttonBorder[2],&buttonBorder[3],&buttonBorder[4] };
    sf::Text* labels[5] = {
        game1 ? &*game1 : nullptr,
        game2 ? &*game2 : nullptr,
        game3 ? &*game3 : nullptr,
        title2 ? &*title2 : nullptr,
        title3 ? &*title3 : nullptr
    };

    sf::Color fillCol = hovered ? darkPink : creamWhite;
    sf::Color bdrCol = hovered ? sf::Color(185, 45, 115) : pillBorder;
    sf::Color labelCol = hovered ? sf::Color(255, 255, 255) : sf::Color(190, 80, 130);

    // Subtle inner glow on hover (slightly lighter rect inside)
    if (hovered)
    {
        // Draw a slightly smaller lighter rect for a glossy top-third
        sf::RectangleShape gloss;
        gloss.setSize({ BTN_W - 40.f, BTN_H * 0.35f });
        gloss.setPosition({ BTN_RECT_X + 20.f, rects[idx]->getPosition().y + 5.f });
        gloss.setFillColor(sf::Color(255, 255, 255, 35));
        // Draw after main fill — we'll handle order below
        // (just prepare it here, draw after fill)

        borders[idx]->setFillColor(bdrCol);
        pillCapLBorder[idx].setFillColor(bdrCol);
        pillCapRBorder[idx].setFillColor(bdrCol);
        window.draw(*borders[idx]);
        window.draw(pillCapLBorder[idx]);
        window.draw(pillCapRBorder[idx]);

        rects[idx]->setFillColor(fillCol);
        pillCapL[idx].setFillColor(fillCol);
        pillCapR[idx].setFillColor(fillCol);
        window.draw(*rects[idx]);
        window.draw(pillCapL[idx]);
        window.draw(pillCapR[idx]);
        window.draw(gloss);
    }
    else
    {
        borders[idx]->setFillColor(bdrCol);
        pillCapLBorder[idx].setFillColor(bdrCol);
        pillCapRBorder[idx].setFillColor(bdrCol);
        window.draw(*borders[idx]);
        window.draw(pillCapLBorder[idx]);
        window.draw(pillCapRBorder[idx]);

        rects[idx]->setFillColor(fillCol);
        pillCapL[idx].setFillColor(fillCol);
        pillCapR[idx].setFillColor(fillCol);
        window.draw(*rects[idx]);
        window.draw(pillCapL[idx]);
        window.draw(pillCapR[idx]);
    }

    if (labels[idx])
    {
        labels[idx]->setFillColor(labelCol);
        window.draw(*labels[idx]);
    }

    // Hover sparkle crosses on both sides
    if (hovered)
    {
        float y = rects[idx]->getPosition().y + BTN_H / 2.f;
        float lx = BTN_RECT_X - BTN_R - 28.f;
        float rx = BTN_RECT_X + BTN_W + BTN_R + 10.f;
        float t = clock.getElapsedTime().asSeconds();
        float p = 0.65f + 0.35f * sin(t * 7.f);
        uint8_t a = (uint8_t)(int)(220 * p);

        // Draw a small ✦ cross on each side
        for (int side = 0; side < 2; side++)
        {
            float sx = (side == 0) ? lx : rx;
            // horizontal bar
            sf::RectangleShape h({ 16.f, 3.f });
            h.setFillColor(sf::Color(255, 255, 255, a));
            h.setOrigin({ 8.f, 1.5f });
            h.setPosition({ sx + 8.f, y });
            window.draw(h);
            // vertical bar
            sf::RectangleShape v({ 3.f, 16.f });
            v.setFillColor(sf::Color(255, 255, 255, a));
            v.setOrigin({ 1.5f, 8.f });
            v.setPosition({ sx + 8.f, y });
            window.draw(v);
            // diagonal 1
            sf::RectangleShape d1({ 10.f, 2.f });
            d1.setFillColor(sf::Color(255, 255, 255, (uint8_t)(a / 2)));
            d1.setOrigin({ 5.f, 1.f });
            d1.setPosition({ sx + 8.f, y });
            d1.setRotation(sf::degrees(45.f));
            window.draw(d1);
            // diagonal 2
            sf::RectangleShape d2({ 10.f, 2.f });
            d2.setFillColor(sf::Color(255, 255, 255, (uint8_t)(a / 2)));
            d2.setOrigin({ 5.f, 1.f });
            d2.setPosition({ sx + 8.f, y });
            d2.setRotation(sf::degrees(-45.f));
            window.draw(d2);
        }
    }
}

// ──────────────────────────────────────────────────
//  drawStar  (unused but satisfies declaration)
// ──────────────────────────────────────────────────
void Menu::drawStar(float cx, float cy, float r, float angle, sf::Color col)
{
    sf::CircleShape s(r, 4);
    s.setOrigin({ r, r });
    s.setPosition({ cx, cy });
    s.setRotation(sf::degrees(angle));
    s.setFillColor(col);
    window.draw(s);
}

// ──────────────────────────────────────────────────
//  getPlayerName
// ──────────────────────────────────────────────────
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
        nameDisplay.setString(name + "_");
        window.draw(nameDisplay);
        window.display();
    }
    return name;
}

// ──────────────────────────────────────────────────
//  run
// ──────────────────────────────────────────────────
void Menu::run()
{
    while (window.isOpen())
    {
        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        sf::Vector2f mp((float)mousePos.x, (float)mousePos.y);

        auto pillHit = [&](int idx) -> bool {
            float y = BTN_START_Y + idx * BTN_GAP;
            float left = BTN_RECT_X - BTN_R;
            float right = BTN_RECT_X + BTN_W + BTN_R;
            return mp.x >= left && mp.x <= right && mp.y >= y && mp.y <= y + BTN_H;
            };

        bool h0 = pillHit(0), h1 = pillHit(1), h2 = pillHit(2),
            h3 = pillHit(3), h4 = pillHit(4);

        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto* kp = event->getIf<sf::Event::KeyPressed>())
            {
                if (kp->code == sf::Keyboard::Key::M)
                {
                    if (music.getStatus() == sf::Music::Status::Playing) music.pause();
                    else music.play();
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
                    if (h0) gameState = 1;
                    if (h1) gameState = 2;
                    if (h2) gameState = 3;
                    if (h3) gameState = 4;
                    if (h4) window.close();
                    if (speakerSprite->getGlobalBounds().contains(mp))
                    {
                        if (music.getStatus() == sf::Music::Status::Playing) music.pause();
                        else music.play();
                    }
                }
            }
        }

        float t = clock.getElapsedTime().asSeconds();

        // ── Mascot float ─────────────────────────────
        {
            float mScale = 150.f / mascotTexture.getSize().x;
            float mw = mascotTexture.getSize().x * mScale;
            float bobY = 5.f * sin(t * 2.0f);
            mascot->setPosition({ WIN_W / 2.f - mw / 2.f, 148.f + bobY });
        }

        // ── Sprinkle animation: slow rotation + alpha pulse ───────────
        for (int i = 0; i < NUM_SPRINKLES; i++)
        {
            sprinkleAngle[i] += sprinkleSpeed[i] * (1.f / 60.f);
            if (sprinkleAngle[i] > 360.f) sprinkleAngle[i] -= 360.f;
            sprinkles[i].setRotation(sf::degrees(sprinkleAngle[i]));
            // Alpha pulse — each sprinkle at its own phase
            float phase = t * 1.2f + i * 0.47f;
            int   baseA = (int)sprinkles[i].getFillColor().a;
            // Clamp alpha around a base with ±30 pulse
            int pa = 40 + (int)(50 * sin(phase));
            if (pa < 10) pa = 10;
            if (pa > 200) pa = 200;
            sf::Color col = sprinkles[i].getFillColor();
            col.a = (uint8_t)pa;
            sprinkles[i].setFillColor(col);
        }

        // ── Blob pulse ────────────────────────────────
        for (int i = 0; i < NUM_BLOBS; i++)
        {
            float s = 1.f + 0.04f * sin(t * 0.6f + i * 1.2f);
            // just pulse alpha gently
            sf::Color c = blobs[i].getFillColor();
            c.a = (uint8_t)(18 + (int)(10 * sin(t * 0.5f + i)));
            blobs[i].setFillColor(c);
        }

        // ── Render ───────────────────────────────────
        window.clear(sf::Color(252, 218, 232));

        if (gameState == 0)
        {
            window.draw(*bg);

            // glow blobs
            for (int i = 0; i < NUM_BLOBS; i++)
                window.draw(blobs[i]);

            // sprinkles (behind title and buttons)
            for (int i = 0; i < NUM_SPRINKLES; i++)
                window.draw(sprinkles[i]);

            // mascot
            window.draw(*mascot);

            // title
            window.draw(*titleShadow);
            window.draw(*title);
            window.draw(*subtitle);

            // buttons
            drawPillButton(0, h0);
            drawPillButton(1, h1);
            drawPillButton(2, h2);
            drawPillButton(3, h3);
            drawPillButton(4, h4);

            window.draw(*creditText);

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
            BrickBreaker bb(window, music); bb.run();
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