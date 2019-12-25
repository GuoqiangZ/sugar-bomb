#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>
#include "yspng.h"
#include "Common.h"
#include <stdexcept>
#include <math.h>

using namespace std;

/************************************* Prop *************************************/
Prop::Prop(const GameMap *map, const int x, const int y): GameObject(x, y, true, PROP), map(map) {};
Prop::~Prop() {};

/************************************* Weight *************************************/
const vector<string> Weight::filepath = {
    "images/objects/glue_0.png", "images/objects/glue_1.png"
};

const vector<YsRawPngDecoder *> Weight::images = GetImages(Weight::filepath);

Weight::Weight(const GameMap *map, const int x, const int y): Prop(map, x, y) {};

void Weight::Draw() const {
    YsRawPngDecoder *image = images[map->GetCurrentPeriod() % 40 / 20];
    GameObject::Draw(image);
}

void Weight::Action(Player *player){
    player -> SpeedDown();
}

/************************************* Banana *************************************/
const vector<string> Banana::filepath = {
    "images/objects/banana_0.png", "images/objects/banana_1.png"
};

const vector<YsRawPngDecoder *> Banana::images = GetImages(Banana::filepath);

Banana::Banana(const GameMap *map, const int x, const int y): Prop(map, x, y) {};

void Banana::Draw() const {
    YsRawPngDecoder *image = images[map->GetCurrentPeriod() % 40 / 20];
    GameObject::Draw(image);
}

void Banana::Action(Player *player){
    player -> HitBanana();
}

/************************************* Powder *************************************/
const vector<string> Powder::filepath = {
    "images/objects/bomb_pot_0.png", "images/objects/bomb_pot_1.png"
};

const vector<YsRawPngDecoder *> Powder::images = GetImages(Powder::filepath);

Powder::Powder(const GameMap *map, const int x, const int y): Prop(map, x, y) {};

void Powder::Draw() const {
    YsRawPngDecoder *image = images[map->GetCurrentPeriod() % 40 / 20];
    GameObject::Draw(image);
}

void Powder::Action(Player *player){
    player -> AddBombPower();
}

/************************************* SpeedBooster *************************************/
const vector<string> SpeedBooster::filepath = {
    "images/objects/speed_pot_0.png", "images/objects/speed_pot_1.png"
};

const vector<YsRawPngDecoder *> SpeedBooster::images = GetImages(SpeedBooster::filepath);

SpeedBooster::SpeedBooster(const GameMap *map, const int x, const int y): Prop(map, x, y) {};

void SpeedBooster::Draw() const {
    YsRawPngDecoder *image = images[map->GetCurrentPeriod() % 40 / 20];
    GameObject::Draw(image);
}

void SpeedBooster::Action(Player *player){
    player -> SpeedUp();
}
