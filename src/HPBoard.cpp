#include <stdio.h>
#include <time.h>

#include "Common.h"
#include "fssimplewindow.h"
#include "ysglfontdata.h"
#include "yspng.h"

const vector<string> HPBoard::filepath = {
    "images/player/player_1_big.png",
    "images/player/player_2_big.png",
    "images/objects/hp.png",
    "images/objects/lightning.png",
};

const vector<YsRawPngDecoder *> HPBoard::images = GetImages(HPBoard::filepath);

HPBoard::HPBoard(const Player *player1, const Player *player2): player1(player1), player2(player2) {};

void HPBoard::Draw() const {
    int distance = 30;
    DrawPixel(840, 130, images[0]);
    DrawPixel(840, 660, images[1]);
    glColor3ub(0, 35, 58);
    
    glRasterPos2d(790,180);
    YsGlDrawFontBitmap16x20("HP");
    for (int i = 0; i < player1->GetHp(); i++) {
        DrawPixel(860 + i * distance, 180, images[2]);
    }
    
    glRasterPos2d(790,210);
    YsGlDrawFontBitmap16x20("SPD");
    for (int i = 0; i < player1->GetSpeed() / 3; i++) {
        DrawPixel(860 + i * distance, 210, images[3]);
    }
    
    glRasterPos2d(790,710);
    YsGlDrawFontBitmap16x20("HP");
    for (int i = 0; i < player2->GetHp(); i++) {
        DrawPixel(860 + i * distance, 710, images[2]);
    }
    
    glRasterPos2d(790,740);
    YsGlDrawFontBitmap16x20("SPD");
    for (int i = 0; i < player2->GetSpeed() / 3; i++) {
        DrawPixel(860 + i * distance, 740, images[3]);
    }
    return;
}
