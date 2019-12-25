#ifndef Common_hpp
#define Common_hpp

#include <stdio.h>
#include <vector>
#include <set>
#include <string>

#include "yspng.h"
#include "fssimplewindow.h"

using namespace std;

class GameMap;
class GameObject;
class Obstacle;
class Block;
class Bomb;
class Prop;
class Role;
class Player;
class Monster;
class Banana;
class SpeedBooster;
class Weight;
class Powder;

vector<YsRawPngDecoder *> GetImages(const vector<string> filepaths);

YsRawPngDecoder *GetImage(const string filepath);

void DrawPixel(const int x, const int y, const YsRawPngDecoder *image);

const int WINDOW_WIDTH = 1020;
const int WINDOW_HEIGHT = 780;
const int LEFT_BOUND = 0;
const int UPPER_BOUND = 0;
const int GRID_SIZE = 60;
const int GRID_HORIZONTAL_NUM = 13;
const int GRID_VERTICAL_NUM = 13;
const int PROP_NUM = 4;
const int OBSTACLE_NUM = 40;

enum GameObjectType {
    BLOCK = 0, OBSTACLE = 1, BOMB = 2, PROP = 3
};

enum Direction {
    LEFT = 0, UP = 1, RIGHT = 2, DOWN = 3, STILL = 4
};

class GameObject {
protected:
    const int x, y;
    void Draw(const YsRawPngDecoder *image) const;
public:
    bool walkable;
    int type;
    GameObject(const int x, const int y, const bool walkable, const int type);
    bool IsWalkable();
    int GetType();
    virtual void Draw() const;
};

class Block : public GameObject {
private:
    const static string filepath;
    const static YsRawPngDecoder *image;
public:
    Block(const int x, const int y);
    void Draw() const override;
};

class Obstacle : public GameObject {
private:
    const static string filepath;
    const static YsRawPngDecoder *image;
public:
    Obstacle(const int x, const int y);
    void Draw() const;
};

class Bomb : public GameObject {
private:
    const static int lastPeriod = 160; // Bomb last for 4s.
    const static int explodeLastPeriod = 20; // Bomb explosion last for 0.5s.
    const static std::vector<YsRawPngDecoder *> images;
    const static std::vector<std::string> filepaths;
    const static std::vector<std::pair<int, int>> dirs;
    GameMap *const map;
    const int power;
    int state = 0;
    int placedPeriod;
    void DestroyBlock();
    int FigureOutInitTime(int placedPeriod);
public:
    const int owner;
    Bomb(GameMap *map, int x, int y, int power, int owner, int placedPeriod);
    void Draw() const override;
    void DrawExplosion() const;
    int GetState() const;
    void Update();
};

class GameMap {
private:
    void ExplodeObstacle(int x, int y);
    void ExplodeProp(int x, int y);
    void ExplodePlayer(int x, int y);
    void ExplodeMonster(int x, int y);
    vector<Bomb *> explodingBomb;
    vector<Player*> players;
    vector<Monster*> monsters;

public:
    int periodCounter = 0;
    GameObject *gameObjects[GRID_VERTICAL_NUM][GRID_HORIZONTAL_NUM];

    GameMap();
    void Draw() const;
    bool Walkable(int x, int y) const;
    bool CheckDeadPlayer() const;
    int GetCurrentPeriod() const;
    void UpdateRoles();
    void UpdateBombs();
    bool PlaceBomb(int x, int y, int power, int owner);
    void ExplodeGrid(int x, int y, bool isFinal);
    void AddPlayer(Player *player);
    void AddMonster(Monster *monster);
};

class Role {
protected:
    GameMap *const map;
    int x, y;
    Direction dir;
    int speed;
    int hp;
    Role(GameMap *const map, const int gridX, const int gridY, const Direction dir, const int speed, const int hp);
    void NextGrid(int &nextGridX, int &nextGridY);
    virtual void Draw() const;
    virtual void Move();

public:
    int GetGridX() const;
    int GetGridY() const;
    int GetHp() const;
    int IsAlive() const;
    virtual void GetHurt();
};

class Monster : public Role {
private:
    const static Direction initDir;
    const static int initHp;
    const static int initSpeed;
    const static int dirChangeProb;
    const static vector<string> filepaths;
    const static vector<YsRawPngDecoder *> images;
    int counter = 0;
public:
    Monster(GameMap *const map, const int gridX, const int gridY);
    void Draw() const override;
    void Move() override;
    void HurtPlayers(vector<Player *> &players) const;
};

class Player : public Role {
private:
    const static vector<vector<int>> keys;
    const static vector<vector<string>> filepaths;
    const static vector<vector<YsRawPngDecoder *>> images;
    const static int recoverPeriod;
    int power;
    bool slipping;
    int leftBombs;
    int lastHurtPeriod;
    int lastBombPlacedPeriod;
    void Move() override;

public:
    const int id;
    Player(GameMap * const map, const int gridX, const int gridY, const int id);
    void Draw() const override;
    int GetSpeed() const;
    void Update();
    void AddBomb();
    void AddBombPower();
    void HitBanana();
    void SpeedUp();
    void SpeedDown();
    void GetHurt() override;
};
class Prop : public GameObject {
protected:
    const GameMap *map;
public:
    virtual ~Prop();
    virtual void Draw() const = 0;
    virtual void Action(Player *player) = 0;
    Prop(const GameMap *map, const int x, const int y);
};

class Banana : public Prop  {
private:
    const static vector<string> filepath;
    const static vector<YsRawPngDecoder *> images;
public:
    void Draw() const override;
    void Action(Player *player) override;
    Banana(const GameMap *map, const int x, const int y);
};

class Weight : public Prop {
private:
    const static vector<string> filepath;
    const static vector<YsRawPngDecoder *> images;
public:
    void Draw() const override;
    void Action(Player *player) override;
    Weight(const GameMap *map, const int gridX, const int gridY);
};

class Powder : public Prop {
private:
    const static vector<std::string> filepath;
    const static vector<YsRawPngDecoder *> images;
public:
    void Draw() const override;
    void Action(Player *player) override;
    Powder(const GameMap *map, const int gridX, const int gridY);
};

class SpeedBooster : public Prop {
private:
    const static vector<std::string> filepath;
    const static vector<YsRawPngDecoder *> images;
public:
    void Draw() const override;
    void Action(Player *player) override;
    SpeedBooster(const GameMap *map, const int gridX, const int gridY);
};

class HPBoard {
private:
    const Player* player1;
    const Player* player2;
    const static vector<string> filepath;
    const static vector<YsRawPngDecoder *> images;
    
public:
    void Draw() const;
    HPBoard(const Player* player1, const Player* player2);
};

#endif
