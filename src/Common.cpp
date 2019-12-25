#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "yspng.h"
#include "fssimplewindow.h"

#include "Common.h"

using namespace std;

vector<YsRawPngDecoder *> GetImages(const vector<string> filepaths) {
    vector<YsRawPngDecoder *> images;
    for (auto &filepath : filepaths) {
        images.push_back(GetImage(filepath));
    }
    return images;
}

YsRawPngDecoder *GetImage(const string filepath) {
    FsChangeToProgramDir();
    YsRawPngDecoder *image = new YsRawPngDecoder;
    if (!(image->Decode(filepath.c_str()))) {
        printf("Read File Error.\n");
    }
    image->Flip();
    return image;
}

void DrawPixel(const int x, const int y, const YsRawPngDecoder *image) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glRasterPos2i(x, y);
    glDrawPixels(image->wid, image->hei, GL_RGBA, GL_UNSIGNED_BYTE, image->rgba);
    glDisable(GL_BLEND);
}

/************************************* GameObject *************************************/
GameObject::GameObject(const int x, const int y, const bool walkable, const int type): x(x), y(y), walkable(walkable), type(type) {};

bool GameObject::IsWalkable() {
    return walkable;
}

void GameObject::Draw() const {
    printf("Not Implemented\n");
}

void GameObject::Draw(const YsRawPngDecoder *image) const {
    DrawPixel(GRID_SIZE * x, GRID_SIZE * (y + 1) - 1, image);
}

int GameObject::GetType() {
    return type;
}

/************************************* Role *************************************/
Role::Role(GameMap *const map, const int gridX, const int gridY, const Direction dir,
           const int speed, const int hp): map(map) {
    this->x = gridX * GRID_SIZE + LEFT_BOUND;
    this->y = (gridY + 1) * GRID_SIZE + UPPER_BOUND - 1;
    this->dir = dir;
    this->speed = speed;
    this->hp = hp;
};

int Role::GetGridX() const {
    return round(1.0 * (x - LEFT_BOUND) / GRID_SIZE);
}

int Role::GetGridY() const {
    return round(1.0 * (y + 1 - UPPER_BOUND) / GRID_SIZE - 1);
}

int Role::GetHp() const {
    return hp;
}

int Role::IsAlive() const {
    return hp > 0;
}

void Role::GetHurt() {
    if (hp > 0)
        hp--;
}

void Role::Draw() const {
    printf("Not Implemented");
};

void Role::Move() {
    switch (dir) {
        case UP:
            y -= speed;
            break;
        case DOWN:
            y += speed;
            break;
        case LEFT:
            x -= speed;
            break;
        case RIGHT:
            x += speed;
            break;
        default:
            break;
    }
};

void Role::NextGrid(int &nextGridX, int &nextGridY) {
    nextGridX = GetGridX();
    nextGridY = GetGridY();
    switch (dir) {
        case UP:
            nextGridY--;
            break;
        case DOWN:
            nextGridY++;
            break;
        case LEFT:
            nextGridX--;
            break;
        case RIGHT:
            nextGridX++;
            break;
        default:
            break;
    }
}

