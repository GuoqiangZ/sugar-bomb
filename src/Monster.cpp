#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <math.h>

#include "yspng.h"
#include "Common.h"

using namespace std;

const Direction Monster::initDir = DOWN;
const int Monster::initHp = 1;
const int Monster::initSpeed = 3;
const int Monster::dirChangeProb = 1;
const vector<string> Monster::filepaths = {
    "images/monster/left_0.png", "images/monster/left_1.png",
    "images/monster/up_0.png", "images/monster/up_1.png",
    "images/monster/right_0.png", "images/monster/right_1.png",
    "images/monster/down_0.png", "images/monster/down_1.png",
    "images/monster/still_0.png", "images/monster/still_1.png"
};
const vector<YsRawPngDecoder *> Monster::images = GetImages(Monster::filepaths);

Monster::Monster(GameMap * const map, const int gridX, const int gridY):
    Role(map, gridX, gridY, initDir, initSpeed, initHp) {};

void Monster::Move() {
    if ((x - LEFT_BOUND) % GRID_SIZE == 0
        && (y + 1 - UPPER_BOUND) % GRID_SIZE == 0) {
        int nextGridX, nextGridY;
        Role::NextGrid(nextGridX, nextGridY);
        if (!map->Walkable(nextGridX, nextGridY) || rand() % dirChangeProb == 0) {
            int step = rand() % 2 == 0 ? 1 : 3;
            dir = static_cast<Direction>((dir + 3 * step) % 4);
            int i = 0;
            for (i = 0; i < 4; i++) {
                Role::NextGrid(nextGridX, nextGridY);
                if (map->Walkable(nextGridX, nextGridY))
                    break;
                dir = static_cast<Direction>((dir + step) % 4);
            }
            if (i == 4)
                dir = STILL;
        }
    }
    if (rand() % 50 > 0)
        Role::Move();
}

void Monster::Draw() const {
    YsRawPngDecoder * image;
    image = images[dir * 2 + map->GetCurrentPeriod() % 40 / 20];
    DrawPixel(x, y, image);
    return;
}

void Monster::HurtPlayers(vector<Player *> &players) const {
    int curGridX = GetGridX(), curGridY = GetGridY();
    for (auto player : players) {
        if (player->GetGridX() == curGridX && player->GetGridY() == curGridY) {
            player->GetHurt();
        }
    }
}















