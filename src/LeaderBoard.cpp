#include "LeaderBoard.h"


static string intToStr(int n) //to_string
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

static int strLen(const string& s) //length
{
    int len = 0;
    while (s[len] != '\0') len++;
    return len;
}

static bool strEmpty(const string& s) //empty
{
    return strLen(s) == 0;
}

static string strSub(const string& s, int start, int length)
{
    string result = "";
    for (int i = start; i < start + length; i++)
        result += s[i];
    return result;
}


LeaderboardManager::LeaderboardManager(const string& file)
    : count(0),
    MAX_RECORDS(200),
    records(new Record[200]),  
    filename(file),
    darkPink(255, 105, 180),
    textColor(120, 30, 70),      
    babyPink(200, 80, 130)       
{
    if (!font.openFromFile("assets/fonts/CherryBomb.ttf"))
        cout << "Leaderboard font failed to load!\n";

    if (!bgTexture.loadFromFile("assets/images/background.jpeg"))
        cout << "Leaderboard background failed!\n";
    bg.emplace(bgTexture);
    LoadFromFile();
}


LeaderboardManager::~LeaderboardManager()
{
    delete[] records;
}


void LeaderboardManager::AddRecord(const string& playerName,  const string& gameName,  int score)
{
    if (count >= MAX_RECORDS)
    {
        cout << "Leaderboard full, oldest record will be dropped.\n";
        for (int i = 0; i < MAX_RECORDS - 1; i++)
            records[i] = records[i + 1];
        count = MAX_RECORDS - 1;
    }

    // Fill the new record at the end
    records[count].playerName = playerName;
    records[count].gameName = gameName;
    records[count].score = score;
    count++;
    SortRecords();
    SaveToFile();
    cout << "Record added: " << playerName << " | " << gameName << " | " << score << "\n";}


void LeaderboardManager::LoadFromFile()
{
    count = 0;   
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "No leaderboard file found. starting fresh.\n";
        return;
    }

    char buffer[300] = {}; //getline
    while (file.getline(buffer, 300) && count < MAX_RECORDS)
    {
        string line = "";
        for (int i = 0; buffer[i] != '\0' && buffer[i] != '\r'; i++)
            line += buffer[i];
   
            if (strEmpty(line)) continue;

        int firstComma = -1;
        for (int i = 0; i < strLen(line); i++)
        {
            if (line[i] == ',') { firstComma = i; break; }
        }

        int secondComma = -1;
        for (int i = firstComma + 1; i < strLen(line); i++)
        {
            if (line[i] == ',') { secondComma = i; break; }
        }

        if (firstComma == -1 || secondComma == -1)
        {
            cout << "Skipping corrupt line: " << line << "\n";
            continue;
        }

        records[count].playerName = strSub(line, 0, firstComma);
        records[count].gameName = strSub(line, firstComma + 1, secondComma - firstComma - 1);
       

        string scoreStr = strSub(line, secondComma + 1, strLen(line) - secondComma - 1);
        int score = 0;
        bool negative = false;
        int start = 0;
        if (!strEmpty(scoreStr) && scoreStr[0] == '-')
        {
            negative = true;
            start = 1;
        }

        for (int i = start; i < strLen(scoreStr); i++)
        {
            char c = scoreStr[i];
            if (c >= '0' && c <= '9')
                score = score * 10 + (c - '0');  //digit-by-digit
            else
                break;   //stop on any non-digit 
        }

        if (negative) score = -score;
        records[count].score = score;

        count++;
    }

    file.close();
    SortRecords();
    cout << "Loaded " << count << " records from " << filename << "\n";
}

void LeaderboardManager::SaveToFile()
{
    ofstream file(filename, ios::trunc);

    if (!file.is_open()) { cout << "ERROR: Could not open " << filename << " for writing!\n";  return;   }

    for (int i = 0; i < count; i++)
    {
        file << records[i].playerName << ","  << records[i].gameName << "," << records[i].score << "\n";
    }

    file.close();
    cout << "Leaderboard saved (" << count << " records).\n";
}


void LeaderboardManager::SortRecords()
{
    for (int i = 0; i < count - 1; i++)
    {
        int maxIdx = i;
        for (int j = i + 1; j < count; j++)
        {
            if (records[j].score > records[maxIdx].score)
                maxIdx = j;
        }
        // Swap entire Record structs
        if (maxIdx != i)
        {
            Record temp = records[i];
            records[i] = records[maxIdx];
            records[maxIdx] = temp;
        }
    }
}


int LeaderboardManager::GetHighScore(const string& gameName)
{
    int best = 0;
    for (int i = 0; i < count; i++)
    {
        if (records[i].gameName == gameName && records[i].score > best)
            best = records[i].score;
    }
    return best;
}

void LeaderboardManager::DisplayLeaderboard(sf::RenderWindow& window, const string& filter)
{
    float scaleX = (float)window.getSize().x / bgTexture.getSize().x;
    float scaleY = (float)window.getSize().y / bgTexture.getSize().y;
    bg->setScale({ scaleX, scaleY });
    window.draw(*bg);

    sf::Text title(font, "LEADERBOARD", 60);
    title.setFillColor(darkPink);
    title.setOutlineColor(sf::Color(255, 240, 245));
    title.setOutlineThickness(3.f);
    title.setPosition({ window.getSize().x / 2.f - title.getGlobalBounds().size.x / 2.f, 20.f });
    window.draw(title);

    sf::Text filterText(font, "Showing: " + filter, 25);
    filterText.setFillColor(babyPink);
    filterText.setPosition({ 30.f, 95.f });
    window.draw(filterText);

    
    sf::Text rankHeader(font, "Rank", 22); rankHeader.setFillColor(textColor);
    sf::Text nameHeader(font, "Player", 22); nameHeader.setFillColor(textColor);
    sf::Text gameHeader(font, "Game", 22); gameHeader.setFillColor(textColor);
    sf::Text scoreHeader(font, "Score", 22); scoreHeader.setFillColor(textColor);

    rankHeader.setPosition({ 30.f,  130.f });
    nameHeader.setPosition({ 130.f, 130.f });
    gameHeader.setPosition({ 380.f, 130.f });
    scoreHeader.setPosition({ 650.f, 130.f });

    window.draw(rankHeader);
    window.draw(nameHeader);
    window.draw(gameHeader);
    window.draw(scoreHeader);

    sf::RectangleShape divider({ (float)window.getSize().x - 60.f, 2.f });
    divider.setPosition({ 30.f, 160.f });
    divider.setFillColor(sf::Color(180, 50, 100));
    window.draw(divider);

    float rowY = 170.f;
    float rowH = 38.f;
    int   rank = 1;
    int   shown = 0;
    int   maxRows = 15;  

    for (int i = 0; i < count && shown < maxRows; i++)
    {
      
        if (filter != "All" && records[i].gameName != filter)
            continue;

        if (shown % 2 == 0)
        {
            sf::RectangleShape rowBg({ (float)window.getSize().x - 60.f, rowH });
            rowBg.setPosition({ 30.f, rowY });
            rowBg.setFillColor(sf::Color(255, 105, 180, 60));  
            window.draw(rowBg);
        }
        sf::Text rankText(font, intToStr(rank), 22);
        sf::Text nameText(font, records[i].playerName, 22);
        sf::Text gameText(font, records[i].gameName, 22);
        sf::Text scoreText(font, intToStr(records[i].score), 22);

     
        sf::Color rowColor = babyPink;
        if (rank == 1) rowColor = sf::Color(180, 130, 0);    // dark gold
        if (rank == 2) rowColor = sf::Color(100, 100, 100);  // dark silver
        if (rank == 3) rowColor = sf::Color(140, 80, 20);    // dark bronze

        rankText.setFillColor(rowColor);
        nameText.setFillColor(rowColor);
        gameText.setFillColor(rowColor);
        scoreText.setFillColor(rowColor);

        rankText.setPosition({ 30.f,  rowY + 6.f });
        nameText.setPosition({ 130.f, rowY + 6.f });
        gameText.setPosition({ 380.f, rowY + 6.f });
        scoreText.setPosition({ 650.f, rowY + 6.f });

        window.draw(rankText);
        window.draw(nameText);
        window.draw(gameText);
        window.draw(scoreText);
        rowY += rowH;
        rank++;
        shown++;
    }

    if (shown == 0)
    {
        sf::Text empty(font, "No records yet. Play a game!", 35);
        empty.setFillColor(babyPink);
        empty.setPosition({window.getSize().x / 2.f - empty.getGlobalBounds().size.x / 2.f, window.getSize().y / 2.f });
        window.draw(empty);
    }

    sf::Text footer(font, "1=All  2=Snake  3=BrickBreaker  4=Ping Pong  |  Escape=Menu", 20);
    footer.setFillColor(sf::Color(180, 50, 100));
    footer.setPosition({ window.getSize().x / 2.f - footer.getGlobalBounds().size.x / 2.f, window.getSize().y - 40.f});
    window.draw(footer);
}