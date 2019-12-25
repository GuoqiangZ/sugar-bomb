#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include "Common.h"
#include "fssimplewindow.h"
#include "yspng.h"

using namespace std;

/************************************* GameMap *************************************/
int CheckValid(int coor_x, int coor_y)
{
    if ((coor_x == 1 && coor_y == 11) || (coor_x == 1 && coor_y == 10) || (coor_x == 2 && coor_y == 11)) {
        return -1;
    }
    
    if ((coor_x == 11 && coor_y == 11) || (coor_x == 10 && coor_y == 11) || (coor_x == 11 && coor_y == 10)) {
        return -1;
    }
    
    return 0;
}

GameMap::GameMap() {
    for (int i = 0; i < GRID_VERTICAL_NUM; i++)
        for (int j = 0; j < GRID_HORIZONTAL_NUM; j++)
            gameObjects[i][j] = NULL;
    
    for (int i = 0; i < GRID_HORIZONTAL_NUM; i++) {
        gameObjects[0][i] = new Block(i, 0);
        gameObjects[GRID_VERTICAL_NUM - 1][i] = new Block(i, GRID_VERTICAL_NUM - 1);
    }
    
    for (int i = 1; i < GRID_VERTICAL_NUM - 1; i++) {
        gameObjects[i][0] = new Block(0, i);
        gameObjects[i][GRID_HORIZONTAL_NUM - 1] = new Block(GRID_HORIZONTAL_NUM - 1, i);
    }
    
    for (int i = 2; i < GRID_VERTICAL_NUM - 1; i++) {
        for (int j = 2; j < GRID_HORIZONTAL_NUM - 1; j++) {
            if (i % 2 == 0 && j % 2 == 0) {
                gameObjects[i][j] = new Block(j, i);
            }
        }
    }
    
    for (int i = 0; i < OBSTACLE_NUM; i++) {
        int coor_x = rand() % GRID_HORIZONTAL_NUM;
        int coor_y = rand() % GRID_VERTICAL_NUM;
        while (gameObjects[coor_y][coor_x] != NULL || CheckValid(coor_x, coor_y) == -1)
        {
            coor_x = rand() % GRID_HORIZONTAL_NUM;
            coor_y = rand() % GRID_VERTICAL_NUM;
        }
        gameObjects[coor_y][coor_x] = new Obstacle(coor_x, coor_y);
    }
}


int GameMap::GetCurrentPeriod() const {
    return periodCounter;
}

void GameMap::Draw() const {
    glClearColor(0.4, 0.7, 1, 0);
    
    static YsRawPngDecoder *background = GetImage("images/gamemap/background.png");
    DrawPixel(GRID_SIZE, WINDOW_HEIGHT - GRID_SIZE - 1, background);
    
    for (int i = 0; i < GRID_VERTICAL_NUM; i++)
    {
        for (int j = 0; j < GRID_HORIZONTAL_NUM; j++)
        {
            if (gameObjects[i][j] != NULL)
            {
                gameObjects[i][j]->Draw();
            }
        }
    }
    
    for (auto ite = monsters.begin(); ite != monsters.end(); ite++) {
        (*ite)->Draw();;
    }
    for (auto ite = players.begin(); ite != players.end(); ite++) {
        (*ite)->Draw();;
    }
    
    for (Bomb *bomb : explodingBomb) {
        bomb->DrawExplosion();
    }
}

bool GameMap::Walkable(int x, int y) const {
    return gameObjects[y][x] == NULL || gameObjects[y][x]->IsWalkable();
}

bool GameMap::PlaceBomb(int x, int y, int power, int owner) {
    if (gameObjects[y][x] != NULL)
        return false;
    gameObjects[y][x] = new Bomb(this, x, y, power, owner, periodCounter);
    return true;
}

void GameMap::ExplodeGrid(int x, int y, bool isFinal) {
    if (gameObjects[y][x] != NULL) {
        switch (gameObjects[y][x]->type) {
            case OBSTACLE:
                if (isFinal)
                    ExplodeObstacle(x, y);
                break;
            case PROP:
                ExplodeProp(x, y);
                break;
            default:
                break;
        }
    }
    ExplodePlayer(x, y);
    ExplodeMonster(x, y);
}

void GameMap::ExplodeObstacle(int x, int y) {
    if (gameObjects[y][x] == NULL || gameObjects[y][x]->type != OBSTACLE)
        return;
    
    int propType = rand() % PROP_NUM;
    switch (propType) {
        case 0:
            gameObjects[y][x] = new Banana(this, x, y);
            break;
        case 1:
            gameObjects[y][x] = new SpeedBooster(this, x, y);
            break;
        case 2:
            gameObjects[y][x] = new Weight(this, x, y);
            break;
        case 3:
            gameObjects[y][x] = new Powder(this, x, y);
            break;
        default:
            break;
    }
}

void GameMap::ExplodeProp(int x, int y)
{
    if (gameObjects[y][x] == NULL || gameObjects[y][x]->type != PROP)
        return;
    delete gameObjects[y][x];
    gameObjects[y][x] = NULL;
}

void GameMap::ExplodePlayer(int x, int y) {
    for (int i = 0; i < players.size(); i++) {
        Player *player = players.at(i);
        if (player->GetGridX() == x && player->GetGridY() == y)
            player->GetHurt();
    }
}

void GameMap::ExplodeMonster(int x, int y) {
    for (int i = 0; i < monsters.size(); i++) {
        Monster *monster = monsters.at(i);
        if (monster->GetGridX() == x && monster->GetGridY() == y)
            monster->GetHurt();
    }
}

bool GameMap::CheckDeadPlayer() const {
    return !players[0]->IsAlive() || !players[1]->IsAlive();
}

void GameMap::AddPlayer(Player *player) {
    players.push_back(player);
}

void GameMap::AddMonster(Monster *monster) {
    monsters.push_back(monster);
}

void GameMap::UpdateBombs() {
    for (auto ite = explodingBomb.begin(); ite != explodingBomb.end(); ) {
        Bomb *bomb = *ite;
        bomb->Update();
        if (bomb->GetState() == 2) {
            explodingBomb.erase(ite);
            players[bomb->owner]->AddBomb();
            delete bomb;
        } else {
            ite++;
        }
    }
    for (int i = 0; i < 13; i++) {
        for (int j = 0; j < 13; j++) {
            if (gameObjects[j][i] && gameObjects[j][i]->GetType() == BOMB) {
                Bomb *bomb = dynamic_cast<Bomb *>(gameObjects[j][i]);
                bomb->Update();
                if (bomb->GetState() == 1) {
                    gameObjects[j][i] = NULL;
                    explodingBomb.push_back(bomb);
                }
            }
        }
    }
}

void GameMap::UpdateRoles() {
    for (auto ite = monsters.begin(); ite != monsters.end();) {
        Monster *monster = *ite;
        if (!monster->IsAlive()) {
            monsters.erase(ite);
            delete monster;
            continue;
        }
        monster->Move();
        monster->HurtPlayers(players);
        ite++;
    }
    
    for (auto ite = players.begin(); ite != players.end();) {
        Player *player = *ite;
        player->Update();
        int x = player->GetGridX(), y = player->GetGridY();
        if (gameObjects[y][x] != NULL && gameObjects[y][x]->type == PROP) {
            Prop *prop = dynamic_cast<Prop *>(gameObjects[y][x]);
            prop->Action(player);
            delete prop;
            gameObjects[y][x] = NULL;
        }
        ite++;
    }
}

/************************************* Block *************************************/
const string Block::filepath = {"images/gamemap/block.png"};

const YsRawPngDecoder* Block::image = GetImage(Block::filepath);

Block::Block(const int x, const int y): GameObject(x, y, false, BLOCK) {};

void Block::Draw() const {
    GameObject::Draw(image);
}

/************************************* Obstacle *************************************/
const string Obstacle::filepath = "images/gamemap/obstacle.png";
const YsRawPngDecoder* Obstacle::image = GetImage(Obstacle::filepath);

Obstacle::Obstacle(const int x, const int y): GameObject(x, y, false, OBSTACLE) {};

void Obstacle::Draw() const {
    GameObject::Draw(image);
}
