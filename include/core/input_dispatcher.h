#pragma once

#include "core/app_camera.h"

class InputDispatcher
{
public:
    InputDispatcher();
    InputDispatcher(float width, float height);

    enum class Movement
    {
        NONE = 0,
        FRONT = 1,
        BACK = 2,
        LEFT = 4,
        RIGHT = 8
    };

    inline friend Movement operator|(Movement a, Movement b)
    {
        return static_cast<Movement>(static_cast<int>(a) | static_cast<int>(b));
    }

    inline friend Movement operator&(Movement a, Movement b)
    {
        return static_cast<Movement>(static_cast<int>(a) & static_cast<int>(b));
    }

    inline friend Movement& operator|=(Movement& a, Movement b)
    {
        return a = a | b;
    }

    void onMouseOut();
    void addMovement(Movement dir);
    void processMouseMovement(AppCamera* camera, double xPos, double yPos);
    void processKeyInput(Movement dir, AppCamera* camera);

private:
    float lastMouseX;
    float lastMouseY;
    bool firstMouse;
    float mouseSensitivity;
    float movementSpeed;

    float lastMovementFrame;
};
