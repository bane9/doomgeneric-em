#include "doomgeneric.h"

void DG_Init()
{
}

void DG_DrawFrame()
{
}

void DG_SleepMs(uint32_t ms)
{
}

uint32_t DG_GetTicksMs()
{
    return 0;
}

int DG_GetKey(int *pressed, unsigned char *doomKey)
{
    return 0;
}

void DG_SetWindowTitle(const char *title)
{
}

int main(int argc, char **argv)
{
    doomgeneric_Create(argc, argv);

    while (1)
    {
        doomgeneric_Tick();
    }

    return 0;
}
