#ifndef DOOM_GENERIC
#define DOOM_GENERIC

#include <stdlib.h>
#include <stdint.h>

#ifndef DOOMGENERIC_RESX
#define DOOMGENERIC_RESX 800
#endif  // DOOMGENERIC_RESX

#ifndef DOOMGENERIC_RESY
#define DOOMGENERIC_RESY 600
#endif  // DOOMGENERIC_RESY


#ifdef DOOMGENERIC_FB_565
typedef uint16_t pixel_t;
#else
typedef uint32_t pixel_t;
#endif

#ifndef DOOMGENERIC_ALWAYS_RUN
#define DOOMGENERIC_ALWAYS_RUN 1
#endif

extern pixel_t* DG_ScreenBuffer;

void doomgeneric_Create(int argc, char **argv);
void doomgeneric_Tick();


//Implement below functions for your platform
void DG_Init();
void DG_DrawFrame();
void DG_SleepMs(uint32_t ms);
uint32_t DG_GetTicksMs();
int DG_GetKey(int* pressed, unsigned char* key);
void DG_SetWindowTitle(const char * title);

#endif //DOOM_GENERIC
