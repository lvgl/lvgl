#pragma once

#if LV_USE_LOVYAN_GFX

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX : public lgfx::LGFX_Device
{
};

#endif