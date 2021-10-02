# SDL_Renderer Based Drawing Functions

In LVGL, drawing was performed by CPU. To improve drawing performance on platforms with GPU,
we should perform drawing operations on GPU if possible.

This implementation has moved most bitmap blending and drawing procedures to utilize SDL_Renderer,
which takes advantages of hardware acceleration APIs like DirectX or OpenGL.

This implementation can be also considered as a reference implementation, for contributors wants to
develop accelerated drawing functions with other APIs such as OpenGL/OpenGL ES.

## Caveats
`lv_draw_arc`, `lv_draw_line` is not enabled, due to incomplete implementation. So lines and arcs will
have significant impact to drawing performances.

Performance of this implementation still has room to improve. Or we should use more powerful APIs
such as OpenGL.

## Notices for files

### `lv_gpu_sdl_stack_blur.c`

Contains modified code from [android-stackblur](https://github.com/kikoso/android-stackblur) project.
Apache License 2.0

### `lv_gpu_sdl_lru.c`/`lv_gpu_sdl_lru.h`

Contains modified code from [C-LRU-Cache](https://github.com/willcannings/C-LRU-Cache) project. No license defined.