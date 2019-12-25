#include <iostream>
#include <string>
#include <list>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <math.h>
#include <time.h>
#include "yssimplesound.h"
#include "fssimplewindow.h"
#include "yspng.h"
#include "Common.h"



using namespace std;

const vector<string> filepath = {
    "images/objects/win_cup.png",
    "images/player/player_1_big.png",
    "images/player/player_2_big.png",
};
const vector<YsRawPngDecoder *> images = GetImages(filepath);

int main() {
    FsOpenWindow(0, 500, WINDOW_WIDTH, WINDOW_HEIGHT, true, "SugarBomb");
    srand(time(NULL));
    GameMap *map = new GameMap();
    
    Player *player1 = new Player(map, 1, GRID_VERTICAL_NUM - 2, 0);
    Player *player2 = new Player(map, GRID_HORIZONTAL_NUM - 2, GRID_VERTICAL_NUM - 2, 1);
    map->AddPlayer(player1);
    map->AddPlayer(player2);
    for (int i = 0; i < 6;) {
        int x = rand() % GRID_HORIZONTAL_NUM;
        int y = rand() % (GRID_VERTICAL_NUM / 2);
        if (map->Walkable(x, y)) {
            map->AddMonster(new Monster(map, x, y));
            i++;
        }
    }
    
    HPBoard hpboard(player1, player2);
    

    YsSoundPlayer player;
    YsSoundPlayer::SoundData wav;
    
    if(YSOK!=wav.LoadWav("music/Mr_Tea.wav"))
    {
        printf("Failed to read sound.wav\n");
        return 1;
    }
    
    player.Start();
    player.PlayBackground(wav);
    
    YsRawPngDecoder *winner = NULL;

    while (true) {
        
        FsPollDevice();
        if(FsInkey() == FSKEY_ESC)
            return 0;
        glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);
        map->periodCounter++;
        map->UpdateBombs();
        map->Draw();
        hpboard.Draw();
        
        if (!map->CheckDeadPlayer()) {
            map->UpdateRoles();
        } else {
            if (winner == NULL) {
                winner = player1->GetHp() ? images[1] : images[2];
            }
            DrawPixel(390, 450, winner);
            DrawPixel(520, 440, images[0]);
        }
        
        FsSwapBuffers();
        FsSleep(25);
    }

    return 0;
}
