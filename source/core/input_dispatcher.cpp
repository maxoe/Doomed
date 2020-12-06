#include "core/input_dispatcher.h"

#include <core/app.h>
#include <world/maze.h>

InputDispatcher::InputDispatcher()
    : firstMouse(true)
    , mouseSensitivity(0.05f)
    , lastMouseX(0.0f)
    , lastMouseY(0.0f)
    , movementSpeed(0.7f)
    , lastMovementFrame(glfwGetTime())
{
}

InputDispatcher::InputDispatcher(float width, float height)
    : InputDispatcher()
{
    lastMouseX = width / 2.0f;
    lastMouseY = height / 2.0f;
}

void InputDispatcher::onMouseOut()
{
    firstMouse = true;
}

void InputDispatcher::processMouseMovement(Camera* camera, double xPos, double yPos)
{
    if (firstMouse)
    {
        lastMouseX = xPos;
        lastMouseY = yPos;
        firstMouse = false;
    }

    float xMouseOffset = xPos - lastMouseX;
    float yMouseOffset = lastMouseY - yPos;
    xMouseOffset *= mouseSensitivity;
    yMouseOffset *= mouseSensitivity;

    lastMouseX = xPos;
    lastMouseY = yPos;

    camera->addYaw(xMouseOffset);
    camera->addPitch(yMouseOffset);
}

void InputDispatcher::processKeyInput(Movement dir, Camera* camera)
{
    float currentMovementFrame = glfwGetTime();
    float currentMovementDelta = currentMovementFrame - lastMovementFrame;
    lastMovementFrame = currentMovementFrame;

    glm::vec2 deltaPos(0.0f);

    if ((dir & Movement::FRONT) != Movement::NONE)
    {
        deltaPos.x += movementSpeed;
    }
    if ((dir & Movement::BACK) != Movement::NONE)
    {
        deltaPos.x -= movementSpeed;
    }
    if ((dir & Movement::LEFT) != Movement::NONE)
    {
        deltaPos.y -= movementSpeed;
    }
    if ((dir & Movement::RIGHT) != Movement::NONE)
    {
        deltaPos.y += movementSpeed;
    }

    camera->move(deltaPos * currentMovementDelta);
}