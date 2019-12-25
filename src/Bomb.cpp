#include "Common.h"

using namespace std;

const std::vector<std::string> Bomb::filepaths = {
    "images/objects/bomb_1_0.png", "images/objects/bomb_1_1.png",
    "images/objects/bomb_2_0.png", "images/objects/bomb_2_1.png",
    "images/objects/expo_1_0.png", "images/objects/expo_1_1.png",
    "images/objects/expo_1_2.png", "images/objects/expo_1_3.png",
    "images/objects/expo_2_0.png", "images/objects/expo_2_1.png",
    "images/objects/expo_2_2.png", "images/objects/expo_2_3.png"
};
const std::vector<YsRawPngDecoder *> Bomb::images = GetImages(Bomb::filepaths);
const std::vector<std::pair<int, int>> Bomb::dirs = {{1,0}, {-1,0}, {0,1}, {0,-1}};

Bomb::Bomb(GameMap *map, int x, int y, int power, int owner, int placedPeriod) :
GameObject(x, y, false, BOMB), map(map), power(power), owner(owner), placedPeriod(placedPeriod) {}

void Bomb::Update() {
    if (map->GetCurrentPeriod() >= placedPeriod + lastPeriod) {
        state = 1;
    }
    if (map->GetCurrentPeriod() > placedPeriod + lastPeriod + explodeLastPeriod) {
        state = 2;
        // Delete block.
        DestroyBlock();
    }
}

void Bomb::DestroyBlock() {
    map->ExplodeGrid(x, y, true);
    for (auto dir : dirs) {
        for (int i = 1; i <= power; i++) {
            int affectX = x+i*dir.first;
            int affectY = y+i*dir.second;
            int type = map->gameObjects[affectY][affectX] == NULL ? -1 : map->gameObjects[affectY][affectX]->GetType();
            if (type == OBSTACLE) {
                map->ExplodeGrid(affectX, affectY, true);
                break;
            } else if (type == BLOCK)
                break;
        }
    }
}

void Bomb::DrawExplosion() const {
    YsRawPngDecoder *image = images[map->GetCurrentPeriod() % 4 + 4 + owner*4];
    DrawPixel(GRID_SIZE * x, GRID_SIZE * (y + 1) - 1, image);
    map->ExplodeGrid(x, y, false);
    for (auto dir : dirs) {
        for (int i = 1; i <= power; i++) {
            int affectX = x+i*dir.first;
            int affectY = y+i*dir.second;
            int type = map->gameObjects[affectY][affectX] == NULL ? -1 : map->gameObjects[affectY][affectX]->GetType();
            if (type == BOMB) {
                Bomb *bomb = static_cast<Bomb *>(map->gameObjects[affectY][affectX]);
                if (bomb->state == 0) {
                    bomb->placedPeriod = map->GetCurrentPeriod() - lastPeriod;
                    bomb->Update();
                }
            } else if(type != BLOCK) {
                map->ExplodeGrid(affectX, affectY, false);
                DrawPixel(GRID_SIZE * affectX, GRID_SIZE * (affectY + 1) - 1, image);
                if (type == OBSTACLE)
                    break;
            } else
                break;
        }
    }
}

void Bomb::Draw() const {
    int changeRate = 4 - (map->GetCurrentPeriod() - placedPeriod) / 40;
    if (changeRate < 1)
        changeRate = 1;
    YsRawPngDecoder *image = images[map->GetCurrentPeriod() % (10 * changeRate) / (5 * changeRate) + owner * 2];
    GameObject::Draw(image);
}

int Bomb::GetState() const {
    return state;
}
