/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2017 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "../../SDL_internal.h"

#if SDL_VIDEO_DRIVER_ORBITAL

/* Dummy SDL video driver implementation; this is just enough to make an
 *  SDL-based application THINK it's got a working video driver, for
 *  applications that call SDL_Init(SDL_INIT_VIDEO) when they don't need it,
 *  and also for use as a collection of stubs when porting SDL to a new
 *  platform for which you haven't yet written a valid video driver.
 *
 * This is also a great way to determine bottlenecks: if you think that SDL
 *  is a performance problem for a given platform, enable this driver, and
 *  then see if your application runs faster without video overhead.
 *
 * Initial work by Ryan C. Gordon (icculus@icculus.org). A good portion
 *  of this was cut-and-pasted from Stephane Peter's work in the AAlib
 *  SDL video driver.  Renamed to "DUMMY" by Sam Lantinga.
 */

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"

#include "SDL_orbitalvideo.h"
#include "SDL_orbitalevents_c.h"
#include "SDL_orbitalframebuffer_c.h"

#include <fcntl.h>
#include <unistd.h>

#define ORBITALVID_DRIVER_NAME "orbital"

/* Initialization/Query functions */
static int ORBITAL_VideoInit(_THIS);
static int ORBITAL_SetDisplayMode(_THIS, SDL_VideoDisplay * display, SDL_DisplayMode * mode);
static void ORBITAL_VideoQuit(_THIS);

/* ORBITAL driver bootstrap functions */

static int
ORBITAL_Available(void)
{
    printf("ORBITAL_Available\n");
    return (1);
}

static void
ORBITAL_DeleteDevice(SDL_VideoDevice * device)
{
    SDL_free(device);
}

static SDL_VideoDevice *
ORBITAL_CreateDevice(int devindex)
{
    SDL_VideoDevice *device;

    printf("ORBITAL_CreateDevice\n");

    /* Initialize all variables that we clean on shutdown */
    device = (SDL_VideoDevice *) SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (!device) {
        SDL_OutOfMemory();
        return (0);
    }
    device->is_dummy = SDL_FALSE;

    /* Set the function pointers */
    device->VideoInit = ORBITAL_VideoInit;
    device->VideoQuit = ORBITAL_VideoQuit;
    device->SetDisplayMode = ORBITAL_SetDisplayMode;
    device->PumpEvents = ORBITAL_PumpEvents;
    device->CreateWindowFramebuffer = SDL_ORBITAL_CreateWindowFramebuffer;
    device->UpdateWindowFramebuffer = SDL_ORBITAL_UpdateWindowFramebuffer;
    device->DestroyWindowFramebuffer = SDL_ORBITAL_DestroyWindowFramebuffer;

    device->free = ORBITAL_DeleteDevice;

    return device;
}

VideoBootStrap ORBITAL_bootstrap = {
    ORBITALVID_DRIVER_NAME, "SDL orbital video driver",
    ORBITAL_Available, ORBITAL_CreateDevice
};

int
ORBITAL_VideoInit(_THIS)
{
    SDL_DisplayMode mode;
    SDL_OrbitalData * data;
    char path[4096];

    printf("ORBITAL_VideoInit\n");

    /* Use a fake 32-bpp desktop mode */
    mode.format = SDL_PIXELFORMAT_RGB888;
    mode.w = 1024;
    mode.h = 768;
    mode.refresh_rate = 0;
    mode.driverdata = NULL;
    if (SDL_AddBasicVideoDisplay(&mode) < 0) {
        return -1;
    }

    data = (SDL_OrbitalData *)SDL_calloc(sizeof(SDL_OrbitalData), 1);
    if(!data){
        SDL_OutOfMemory();
        return -1;
    }
    _this->driverdata = data;

    snprintf(path, 4096, "orbital:/0/0/%d/%d/", mode.w, mode.h);
    data->fd = open(path, O_RDWR);
    if(data->fd < 0){
        return -1;
    }

    SDL_zero(mode);
    SDL_AddDisplayMode(&_this->displays[0], &mode);

    /* We're done! */
    return 0;
}

static int
ORBITAL_SetDisplayMode(_THIS, SDL_VideoDisplay * display, SDL_DisplayMode * mode)
{
    printf("ORBITAL_SetDisplayMode\n");

    return 0;
}

void
ORBITAL_VideoQuit(_THIS)
{
}

#endif /* SDL_VIDEO_DRIVER_ORBITAL */
