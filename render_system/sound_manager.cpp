//
// Created by Матвей on 15.12.2017.
//
#include <c++/iostream>
#include <cmath>
#include "sound_manager.h"

bool SoundManager::playSound (float x, float y, std::string filename) {
    if(!soundBuffer.loadFromFile(filename)) {
        return false;
    }

    sound.setBuffer(soundBuffer);

    int obstacle_count = 0;

    for(int i = 0; i < obstacles.size(); i++) {
        if(checkLine(obstacles[i], x, y))
            obstacle_count++;

        if(obstacle_count == 2)
            break;
    }

    if(obstacle_count != 2) {
        sound.setVolume(sound_const * __max(0, 1 - distanceToListener(x, y) / sound_const));
    } else {
        sound.setVolume(0);
    }

    std::cout << sound.getVolume();

    sound.play();

    return true;
}

void SoundManager::setListenerPosition(float x, float y) {
    listener_x = x;
    listener_y = y;
}

void SoundManager::setObstaclePosition(float centerX, float centerY, float sizeX, float sizeY) {
    obstacles.push_back(ObstacleData(centerX, centerY, sizeX, sizeY));
}

float SoundManager::distanceToListener(float x, float y) {
    return sqrt(pow(listener_x - x, 2) + pow(listener_y - y, 2));
}

bool SoundManager::checkLine(ObstacleData obstacle, float x, float y) {
    float half_x = obstacle.sizeX/2, half_y = obstacle.sizeY/2;

    return lineIntersect(obstacle.centerX - half_x, obstacle.centerY - half_y, obstacle.centerX + half_x, obstacle.centerY - half_y, x, y, listener_x, listener_y) ||
           lineIntersect(obstacle.centerX - half_x, obstacle.centerY - half_y, obstacle.centerX - half_x, obstacle.centerY + half_y, x, y, listener_x, listener_y) ||
           lineIntersect(obstacle.centerX + half_x, obstacle.centerY - half_y, obstacle.centerX + half_x, obstacle.centerY + half_y, x, y, listener_x, listener_y) ||
           lineIntersect(obstacle.centerX - half_x, obstacle.centerY + half_y, obstacle.centerX + half_x, obstacle.centerY + half_y, x, y, listener_x, listener_y);
}

bool SoundManager::lineIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) {
    return intersect(x1, x2, x3, x4) &&
           intersect(y1, y2, y3, y4) &&
           (areaSign(x1, y1, x2, y2, x3, y3) ^ areaSign(x1, y1, x2, y2, x4, y4)) &&
           (areaSign(x3, y3, x4, y4, x1, y1) ^ areaSign(x3, y3, x4, y4, x2, y2));
}

bool SoundManager::areaSign(float x1, float y1, float x2, float y2, float x3, float y3) {
    return std::signbit((x2 - x1) * (y3 - y1) - (y2 - y1) * (x3 - x1)) == 0;
}

bool SoundManager::intersect(float a, float b, float c, float d) {
    if (a > b)
        std::swap (a, b);

    if (c > d)
        std::swap (c, d);

    return __max(a,c) <= __min(b,d);
}