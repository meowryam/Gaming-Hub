#pragma once
#include <SFML/Graphics.hpp>
#include <fstream>
#include <iostream>

using namespace std;

struct Record
{
    string playerName;   
    string gameName;     
    int score;       
};

class LeaderboardManager
{
private:
    int MAX_RECORDS;
    Record* records;
    int count;                    
    string filename;                      
    sf::Font  font;
    sf::Color darkPink;
    sf::Color textColor;
    sf::Color babyPink;
    sf::Texture bgTexture;
    std::optional<sf::Sprite> bg;
public:
    LeaderboardManager(const string& file = "leaderboard.txt");
    ~LeaderboardManager();
    void AddRecord(const string& playerName,const string& gameName,int score);
    void LoadFromFile();
    void SaveToFile();
    void SortRecords();
    void DisplayLeaderboard(sf::RenderWindow& window,const string& filter = "All");
    int GetHighScore(const string& gameName);
};