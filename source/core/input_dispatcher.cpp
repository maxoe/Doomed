#include "core/input_dispatcher.h"

#include <core/app.h>
#include <world/maze.h>

InputDispatcher::InputDispatcher()
    : firstMouse(true)
    , mouseSensitivity(0.2f)
    , movementSpeed(5.0f)
    , lastMouseX(0.0f)
    , lastMouseY(0.0f)
    , lastMovementFrame(glfwGetTime())
    , lastMouseFrame(glfwGetTime())
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
    float currentMouseFrame = glfwGetTime();
    float currentMouseDelta = currentMouseFrame - lastMouseFrame;
    lastMouseFrame = currentMouseFrame;

    if (firstMouse)
    {
        lastMouseX = xPos;
        lastMouseY = yPos;
        firstMouse = false;
    }

    float xMouseOffset = xPos - lastMouseX;
    float yMouseOffset = lastMouseY - yPos;
    xMouseOffset *= mouseSensitivity * currentMouseDelta;
    yMouseOffset *= mouseSensitivity * currentMouseDelta;

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