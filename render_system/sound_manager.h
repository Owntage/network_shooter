//
// Created by Матвей on 15.12.2017.
//

#ifndef NETWORK_SHOOTER_PROJECT_SOUND_MANAGER_H
#define NETWORK_SHOOTER_PROJECT_SOUND_MANAGER_H

#include <SFML/Audio.hpp>

struct SoundManager {
    bool playSound (float x, float y, std::string filename);
    void setListenerPosition(float x, float y);
    void setObstaclePosition(float centerX, float centerY, float sizeX, float sizeY);

private:
    struct ObstacleData {
        ObstacleData(float cX, float cY, float sX, float sY) :
                centerX(cX),
                centerY(cY),
                sizeX(sX),
                sizeY(sY) {}

        float centerX, centerY, sizeX, sizeY;
    };

    float distanceToListener(float x, float y);
    float listener_x, listener_y;

    const float sound_const = 100;

    std::vector<ObstacleData> obstacles;

    sf::SoundBuffer soundBuffer;
    sf::Sound sound;

    bool checkLine(ObstacleData obstacle, float x, float y);
    bool lineIntersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4); // AB - A(x1, y1), B(x2, y2), same for CD
    bool areaSign(float x1, float y1, float x2, float y2, float x3, float y3); // true if area positive, else false
    bool intersect(float a, float b, float c, float d);
};


#endif //NETWORK_SHOOTER_PROJECT_SOUND_MANAGER_H