#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <time.h>

#include "yspng.h"
#include <math.h>
#include "Common.h"

using namespace std;

const vector<vector<string>> Player::filepaths = {
    {
        "images/player/player_1_left_0.png", "images/player/player_1_left_1.png",
        "images/player/player_1_up_0.png", "images/player/player_1_up_1.png",
        "images/player/player_1_right_0.png", "images/player/player_1_right_1.png",
        "images/player/player_1_down_0.png", "images/player/player_1_down_1.png",
        "images/player/player_1_still_0.png", "images/player/player_1_still_1.png"
    },
    {
        "images/player/player_2_left_0.png", "images/player/player_2_left_1.png",
        "images/player/player_2_up_0.png", "images/player/player_2_up_1.png",
        "images/player/player_2_right_0.png", "images/player/player_2_right_1.png",
        "images/player/player_2_down_0.png", "images/player/player_2_down_1.png",
        "images/player/player_2_still_0.png", "images/player/player_2_still_1.png"
    }
};

const vector<vector<YsRawPngDecoder *>> Player::images = {
    GetImages(Player::filepaths[0]),
    GetImages(Player::filepaths[1])
};

const vector<vector<int>> Player::keys = {
    {FSKEY_A, FSKEY_W, FSKEY_D, FSKEY_S, FSKEY_TAB},
    {FSKEY_LEFT, FSKEY_UP, FSKEY_RIGHT, FSKEY_DOWN, FSKEY_ENTER}
};

const int Player::recoverPeriod = 100;

Player::Player(GameMap * const map, const int gridX, const int gridY, const int id):
    Role(map, gridX, gridY, DOWN, 6, 5), id(id) {    
    power = 1;
    slipping = false;
    lastHurtPeriod = -1000;
    leftBombs = 5;
    lastBombPlacedPeriod = -1000;
};


void Player::Update() {
    int tmp = speed;
    if (slipping) {
        speed = 20;
    }
    
    if (x % GRID_SIZE != 0) {
        speed = min(speed, dir == LEFT ? x % GRID_SIZE : GRID_SIZE - x % GRID_SIZE);
        Move();
    } else if ((y + 1) % GRID_SIZE != 0) {
        speed = min(speed, dir == UP ? y % GRID_SIZE + 1 : GRID_SIZE - 1 - y % GRID_SIZE);
        Move();
    } else if (dir != STILL && slipping) {
        x -= x % speed;
        y -= (y + 1) % speed;
        int nextGridX, nextGridY;
        Role::NextGrid(nextGridX, nextGridY);
        if(map->Walkable(nextGridX, nextGridY)) {
            Move();
        } else {
            slipping = false;
        }
    } else {
        if(FsGetKeyState(keys[id][0])){
            dir = LEFT;
        } else if(FsGetKeyState(keys[id][1])){
            dir = UP;
        } else if(FsGetKeyState(keys[id][2])){
            dir  = RIGHT;
        } else if(FsGetKeyState(keys[id][3])){
            dir = DOWN;
        } else {
            dir = STILL;
        }
    
        int nextGridX, nextGridY;
        Role::NextGrid(nextGridX, nextGridY);
        if(map->Walkable(nextGridX, nextGridY)) {
            Move();
        }
    }
    speed = tmp;
    
    if (FsGetKeyState(keys[id][4]) && leftBombs > 0 && map->GetCurrentPeriod() - lastBombPlacedPeriod > 10){
        if (map->PlaceBomb(GetGridX(), GetGridY(), power, id)) {
            lastBombPlacedPeriod = map->GetCurrentPeriod();
            leftBombs--;
        }
    }
}

void Player::Draw() const {
    if (hp <= 0)
        return;
    int curPeriod = map->GetCurrentPeriod();
    if (curPeriod - lastHurtPeriod < recoverPeriod && curPeriod % 5 / 2 == 0)
        return;
    YsRawPngDecoder * image;
    if (dir == STILL)
        image = images[id][dir * 2 + curPeriod % 40 / 20];
    else {
        int period = GRID_SIZE / speed;
        image = images[id][dir * 2 + curPeriod % period / ((period + 1) / 2)];
    }
    DrawPixel(x, y, image);
}

void Player::AddBomb() {
    leftBombs++;
}

void Player:: AddBombPower(){
    power++;
};

void Player:: HitBanana(){
    slipping = true;
};

void Player:: Move() {
    Role::Move();
}

void Player:: SpeedUp() {
    if (speed < 15) {
        speed += 3;
    }
};

void Player:: SpeedDown(){
    if(speed > 3)
        speed -= 3;
};

void Player:: GetHurt(){
    int curPeriod = map->GetCurrentPeriod();
    if (curPeriod - lastHurtPeriod < recoverPeriod)
        return;
    Role::GetHurt();
    lastHurtPeriod = curPeriod;
};

int Player:: GetSpeed() const {
    return speed;
};

