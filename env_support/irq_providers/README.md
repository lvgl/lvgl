# IRQ providers

LVGL's GPU draw units wait for their hardware through
`lv_irq_attach_<source>()` / `lv_irq_detach_<source>()`
(`include/lvgl/irqal/lv_irq.h`). LVGL defines none of these functions: for each
source an *IRQ provider* does, and `LV_IRQ_<SOURCE>_PROVIDER` holds that
provider's ID (`LV_IRQ_PROVIDER_<NAME>`):

| Source        | Providers in LVGL's tree              |
|---------------|---------------------------------------|
| DMA2D         | CMSIS (NVIC), Zephyr (device tree)    |
| PXP           | CMSIS (NVIC), Zephyr (device tree)    |
| NemaGFX GPU2D | NemaGFX STM32 HAL                     |

With none the draw unit polls, or fails to build when it cannot.

## With Kconfig

Under "IRQ providers", "Default IRQ provider" serves every source it can, and
each source's own choice can override it: "Default provider" (the default
setting) follows it, "None" or a named provider overrides it. Kconfig lets only
one provider serve a source.

Each provider adds itself to these choices from its own `Kconfig`, which
`src/irqal/Kconfig` sources, together with its settings. No LVGL build compiles
the providers' `.c` files: add the ones that serve a source to your build.

## With lv_conf.h

Set each source directly, e.g.
`#define LV_IRQ_DMA2D_PROVIDER LV_IRQ_PROVIDER_CMSIS`.

## Providers outside LVGL

Set the environment variable `LV_IRQ_PROVIDERS_DIR` to a directory with a
`Kconfig.irq_providers` that sources them; their options then appear in the
same choices. `out_of_tree/` is an example: a Redox provider for DMA2D.

Such a provider has an ID between 128 and 255. It defines
`LV_IRQ_PROVIDER_<NAME>` both in its Kconfig and in a header of its own, since
LVGL's generated config does not know it.

## Writing your own

Write a provider as above, or, without Kconfig, set the source's
`LV_IRQ_<SOURCE>_PROVIDER` in `lv_conf.h` to an ID of your own. Define
the attach/detach pair: `attach` stores the callback and its `user_data` and
enables the interrupt; the interrupt handler calls the callback with that
`user_data` (for the NemaGFX GPU2D, with an `lv_irq_nema_gpu2d_event_t` that
carries it and the completed command list); `detach` disables the interrupt
before forgetting the callback. The callback must run where the draw unit can
signal from: today the draw units call `lv_thread_sync_signal_isr()`, which
not every OS supports outside an interrupt (the Redox example calls it from a
thread).
