# Embedded friendly Doomgeneric port

This repo has QOL fixes that make embedded ports easier.

It features:
- Isolation of (hopefully) all syscalls into doomgeneric_syscalls.c file
- Easy support for serialization of .wad files into C arrays
- Support for arbitrary resolutions and aspect ratios
- RGB565 support (along with RGB888)
- Can set MAXPLAYERS 1-4 (currently set to 2)
- The large arrays inside info.c can dynamically allocated if needed
- Switched to CMake
- Bare file available for porting
- Sprint always enabled by default
- A lot of unnecessary code removed
    - Saved around 64 KiB of RAM and 80 KiB of ROM compared to base doomgeneric
    - Sound code is completely removed which changes how ticking works. This breaks demos but gameplay itself works fine.

And probably more that I forgot.

## Prepocessor defines for porting
I won't give detailed guide into port friendly defines, instead check out the following inside the code:
- DOOMGENERC_IWAD_MEMMAPPED
- DOOMGENERIC_RESX
- DOOMGENERIC_RESY
- DOOMGENERIC_FB_565
- DOOMGENERIC_ALWAYS_RUN
- DOOMGENERIC_INFO_DYN
- MAXPLAYERS

## Serializing IWAD files

To serialize an IWAD file, first compile `bin_to_c`:
```bash
gcc -O3 bin_to_c.c -o bin_to_c.o
```
Then, serialize your iwad file via:
```bash
./bin_to_c.o doom.iwad
```
Make sure both `bin_to_c.o` and your iwad file are at the root of this git directory.

At the end, make sure to globally define `DOOMGENERC_IWAD_MEMMAPPED`
_____
# doomgeneric
The purpose of doomgeneric is to make porting Doom easier.
Of course Doom is already portable but with doomgeneric it is possible with just a few functions.

To try it you will need a WAD file (game data). If you don't own the game, shareware version is freely available (doom1.wad).

# porting
Create a file named doomgeneric_yourplatform.c and just implement these functions to suit your platform.
* DG_Init
* DG_DrawFrame
* DG_SleepMs
* DG_GetTicksMs
* DG_GetKey

|Functions            |Description|
|---------------------|-----------|
|DG_Init              |Initialize your platfrom (create window, framebuffer, etc...).
|DG_DrawFrame         |Frame is ready in DG_ScreenBuffer. Copy it to your platform's screen.
|DG_SleepMs           |Sleep in milliseconds.
|DG_GetTicksMs        |The ticks passed since launch in milliseconds.
|DG_GetKey            |Provide keyboard events.
|DG_SetWindowTitle    |Not required. This is for setting the window title as Doom sets this from WAD file.

### main loop
At start, call doomgeneric_Create().

In a loop, call doomgeneric_Tick().

In simplest form:
```
int main(int argc, char **argv)
{
    doomgeneric_Create(argc, argv);

    while (1)
    {
        doomgeneric_Tick();
    }
    
    return 0;
}
```

# platforms
Ported platforms include Windows, X11, SDL, emscripten. Just look at (doomgeneric_win.c, doomgeneric_xlib.c, doomgeneric_sdl.c).
Makefiles provided for each platform.

## emscripten
You can try it directly here:
https://ozkl.github.io/doomgeneric/

