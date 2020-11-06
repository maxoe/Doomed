#include "core/app.h"

int main(int /*argc*/, char** /*argv*/)
{
    App app;

    const auto initError = app.initialize();
    if (initError != 0)
    {
        return initError;
    }

    return app.mainLoop();
}
