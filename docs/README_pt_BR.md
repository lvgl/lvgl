<p align="center">
  <a href="https://lvgl.io/docs" title="Documentação">Docs</a> •
  <a href="https://forum.lvgl.io" title="Fórum da comunidade">Fórum</a> •
  <a href="https://blog.lvgl.io" title="Notícias e artigos">Blog</a> •
  <a href="https://lvgl.io/services" title="Serviços profissionais">Serviços</a>
  <img src="https://upload.wikimedia.org/wikipedia/commons/c/ca/1x1.png" alt="spacer" width="250px" height="1px">
  <a href="../README.md">EN</a> •
  <a href="./README_zh.md">中文</a> •
  <a href="./README_ja.md">日本語</a> •
  <a href="./README_ko.md">한국어</a> •
  <b>PT</b> •
  <a href="./README_he.md">עברית</a>
</p>

<p align="center">
  <a href="https://lvgl.io"><img src="https://lvgl.io/github-assets/logo-colored.png" height=50px/></a>
  <h1 align="center">Light and Versatile Graphics Library</h1>
</p>

<br/>
<br/>

<div align="center">
  <img src="https://github.com/user-attachments/assets/965e8b8b-d240-45ed-9744-bdd81785967d" height="220" alt="Demo de Ebike com gráficos vetoriais">
  &nbsp;
  <img src="https://github.com/user-attachments/assets/d83820ed-5448-494e-94c8-3ca1b4ddceb0" height="220" alt="Demo de ECG com modelo 3D animado">
</div>

<br/>
<br/>
<br/>

<p align="center">
  <a href="#overview" title="O que é o LVGL">Visão geral</a> •
  <a href="#features" title="O que o LVGL pode fazer">Recursos</a> •
  <a href="#lvgl-pro" title="A cadeia de ferramentas profissional">LVGL Pro</a> •
  <a href="#examples" title="Exemplos em C e XML">Exemplos</a> •
  <a href="#integration" title="Como adicionar o LVGL ao seu projeto">Integração</a> •
  <a href="#contributing" title="Como participar">Contribuindo</a> •
  <a href="#license" title="Termos de licenciamento">Licença</a>
</p>

<a id="overview"></a>
## Visão geral

O **LVGL** é uma biblioteca de UI livre e de código aberto que permite criar interfaces gráficas de usuário
para quaisquer MCUs e MPUs, de qualquer fabricante, em qualquer plataforma.

**Requisitos**: o LVGL não possui dependências externas, o que facilita sua compilação para qualquer alvo moderno,
de pequenos MCUs a MPUs multi-core baseados em Linux com suporte a 3D. Para uma UI *típica*, você precisa de apenas ~100kB de RAM,
~200–300kB de flash e um buffer com 1/10 do tamanho da tela para renderização.

**Ampla adoção**: fabricantes de chips (como NXP, Espressif, Renesas e assim por diante), projetos de RTOS (Zephyr, NuttX, etc.)
e fabricantes de placas (Riverdi, Seeed Studio, VIEWE, Elecrow, etc.) já integraram o LVGL. Se uma placa de desenvolvimento tem um display, é muito provável
que o fabricante também ofereça suporte ao LVGL.

**Ferramentas profissionais**: em vez de escrever código C, você pode acelerar e simplificar enormemente o desenvolvimento de UI usando o [LVGL Pro](#lvgl-pro), um kit de ferramentas completo com Editor, integração com Figma, Online Viewer e CLI. Ele exporta código C puro do LVGL a partir de XML, sem runtime extra ou mágica oculta. É gratuito para uso não comercial e avaliação.

**Para começar**, navegue pelos [Exemplos](#examples), inicie um [projeto de Simulador](https://lvgl.io/docs/open/integration/pc), explore o [Online Viewer](https://viewer.lvgl.io/) do LVGL Pro, compile e execute uma UI em uma janela no [LVGL Pro](https://lvgl.io/docs/pro/integration/simulator) com um clique, ou mergulhe na nossa [documentação](https://lvgl.io/docs/open) pronta para IA. Basta clicar em [**Ask AI**](https://lvgl.io/docs/open)
e perguntar qualquer coisa!

<a id="features"></a>
## Recursos

**Livre, portátil e escalável**
  - Uma biblioteca C totalmente portátil (compatível com C++) sem dependências externas.
  - Pode ser compilada para qualquer MCU ou MPU, com qualquer (RT)OS. Make, CMake e globbing simples são todos suportados.
  - Suporta displays monocromáticos, ePaper, OLED ou TFT, ou até monitores. [Displays](https://lvgl.io/docs/open/main-modules/display)
  - Distribuída sob a licença MIT, então você pode usá-la facilmente em projetos comerciais também.
  - No mínimo absoluto, precisa apenas de 32kB de RAM e 128kB de Flash, um frame buffer e pelo menos um buffer com 1/10 do tamanho da tela para renderização.
  - OS, memória externa e GPU são suportados, mas não obrigatórios.

**Widgets, estilos, layouts e muito mais**
  - Mais de 30 [Widgets](https://lvgl.io/docs/open/widgets) integrados: Button, Label, Slider, Chart, Keyboard, Meter, Arc, Table e muitos outros.
  - [Sistema de estilos](https://lvgl.io/docs/open/common-widget-features/styles) flexível com mais de 100 propriedades de estilo para personalizar qualquer parte dos widgets em qualquer estado.
  - Motores de layout semelhantes a [Flexbox](https://lvgl.io/docs/open/common-widget-features/layouts/flex) e [Grid](https://lvgl.io/docs/open/common-widget-features/layouts/grid) para dimensionar e posicionar os widgets de forma responsiva automaticamente.
  - [Data bindings](https://lvgl.io/docs/open/main-modules/observer) para conectar facilmente a UI à aplicação.
  - Suporta Mouse, Touchpad, Keypad, Keyboard, botões externos e Encoder [dispositivos de entrada](https://lvgl.io/docs/open/main-modules/indev).
  - Suporte a [múltiplos displays](https://lvgl.io/docs/open/main-modules/display/overview#how-many-displays-can-lvgl-use).

**Renderização**
  - Motor de renderização 2D integrado com suporte a formas básicas, gradientes, anti-aliasing, opacidade, rolagem suave, sombras de caixa e projetadas, transformação de imagens, etc.
  - [Motor de renderização 3D poderoso](https://lvgl.io/docs/open/libs/gltf) para exibir [modelos glTF](https://sketchfab.com/) com OpenGL.
  - Suporte a gráficos vetoriais, SVG e Lottie.
  - O texto é renderizado com codificação UTF-8, com suporte aos sistemas de escrita CJK, tailandês, hindi, árabe e persa.
  - Suporte integrado a GPUs como VG-Lite, Dave2D, NeoChrome, OpenGL, etc.

<a id="lvgl-pro"></a>
## LVGL Pro

Construir UIs em C funciona bem, mas se torna lento para iterar e mais difícil de manter consistente à medida que um projeto cresce.
O [LVGL Pro](https://lvgl.io/pro) permite construir componentes e telas reutilizáveis visualmente, visualizar mudanças
instantaneamente e gerenciar data bindings, traduções, animações e testes em um só lugar.

O Pro exporta código C puro do LVGL: o mesmo LVGL que você já usa, sem runtime extra ou dependência. Ele se encaixa
em um projeto existente sem mudar a forma como você compila ou entrega.

<p align="center">
  <img height="500" alt="Construindo uma UI no editor LVGL Pro com pré-visualização ao vivo" src="https://github.com/user-attachments/assets/8cef0f05-0ff1-4766-8dfd-1d15e47f181a" />
</p>

Você pode experimentá-lo no navegador em [viewer.lvgl.io](https://viewer.lvgl.io) sem instalar nada, ou
[baixar o Editor](https://lvgl.io/pro#download) e usá-lo sob a licença gratuita Community.

O LVGL Pro consiste em quatro ferramentas fortemente relacionadas:

1. **Editor**: o coração do LVGL Pro. Um aplicativo desktop para construir componentes e telas em XML, gerenciar data bindings, traduções, animações, testes e muito mais. Saiba mais sobre o [Formato XML](https://lvgl.io/docs/pro/syntax) e os [Widgets](https://lvgl.io/docs/pro/built_in_widgets).
2. **Online Viewer**: execute o Editor no seu navegador, abra projetos do GitHub e compartilhe facilmente sem configurar um ambiente de desenvolvimento. Visite [https://viewer.lvgl.io](https://viewer.lvgl.io).
3. **Plugin do Figma**: mova um design do Figma para o LVGL Pro com um clique. Veja como funciona [aqui](https://lvgl.io/docs/pro/figma).
4. **Ferramenta CLI**: gere código C e execute testes em CI/CD. Veja os detalhes [aqui](https://lvgl.io/docs/pro/cli).

As camadas Community e Evaluation são gratuitas para uso; consulte o [licenciamento](#license) para uso comercial.

<a id="examples"></a>
## Exemplos

Você pode conferir mais de 100 exemplos em C e XML em https://lvgl.io/docs/open/examples

O [Online Viewer](https://viewer.lvgl.io/) do LVGL Pro também contém muitos tutoriais e os mesmos exemplos, com os quais você pode interagir de forma interativa.

Como prévia, aqui está a mesma UI simples escrita em C e em XML:

<p align="center">
  <img width="311" height="232" alt="Um botão centralizado que imprime quando clicado" src="https://github.com/user-attachments/assets/5948b485-e3f7-4a63-bb21-984381417c4a" />
</p>

<!-- GitHub renders fenced code blocks inside table cells, so this shows C and XML side by side. -->
<table>
<tr>
<td width="50%">

```c
static void button_clicked_cb(lv_event_t * e)
{
    printf("Clicked\n");
}

/* ... */

lv_obj_t * button = lv_button_create(lv_screen_active());
lv_obj_center(button);
lv_obj_add_event_cb(button, button_clicked_cb,
                    LV_EVENT_CLICKED, NULL);

lv_obj_t * label = lv_label_create(button);
lv_label_set_text(label, "Hello from LVGL!");
```

</td>
<td width="50%">

```xml
<screen>
  <view>
    <lv_button align="center">
      <event_cb callback="button_clicked_cb" />
      <lv_label text="Hello from LVGL!" />
    </lv_button>
  </view>
</screen>
```

</td>
</tr>
</table>

O XML acima é com o que o [LVGL Pro](#lvgl-pro) trabalha: você constrói a tela visualmente e ele gera o C para você.

<a id="integration"></a>
## Integração

O LVGL não possui dependências externas, então pode ser facilmente compilado para qualquer dispositivo. Ele vem com drivers integrados, está disponível em
muitos gerenciadores de pacotes e RTOSes, e também é fácil de portar para qualquer novo dispositivo.

### Pré-integrado

- **Fabricantes de chips**: [ESP32](https://components.espressif.com/components/lvgl/lvgl), [componente NXP MCUXpresso](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY), [Renesas FSP](https://lvgl.io/docs/open/integration/chip/renesas), [STM32](https://lvgl.io/docs/open/integration/chip/stm32)

- **RTOSes**: [Zephyr](https://lvgl.io/docs/open/integration/os/zephyr), [NuttX](https://lvgl.io/docs/open/integration/os/nuttx), [RT-Thread](https://lvgl.io/docs/open/integration/os/rt-thread)

- **Frameworks**: [Arduino](https://lvgl.io/docs/open/integration/framework/arduino), [PlatformIO](https://registry.platformio.org/libraries/lvgl/lvgl), [CMSIS-Pack](https://lvgl.io/docs/open/integration/framework/cmsis-pack)

- **Fabricantes de placas**: [Seeed Studio](https://www.seeedstudio.com), [Elecrow](https://www.elecrow.com/display/esp-hmi-display.html), [Riverdi](https://lvgl.io/docs/open/integration/boards/manufacturers/riverdi), [VIEWE](https://lvgl.io/docs/open/integration/boards/manufacturers/viewe), e [muitos outros](https://lvgl.io/boards)

### Drivers integrados
O LVGL vem com drivers prontos para uso, então em plataformas comuns você não precisa escrever o tratamento de display e entrada por conta própria:

- **Simulador / desktop**: janelas [SDL](https://lvgl.io/docs/open/integration/pc/sdl), X11 e [Wayland](https://lvgl.io/docs/open/integration/embedded_linux/drivers/wayland) para desenvolver e pré-visualizar sua UI em um PC.
- **Controladores de display**: LCDs MIPI-DBI/SPI genéricos ([ILI9341](https://lvgl.io/docs/open/integration/external_display_controllers/ili9341), ST7789 e similares), além de controladores de fabricantes como o [LTDC](https://lvgl.io/docs/open/integration/chip_vendors/stm32/ltdc) da ST e o [eLCDIF](https://lvgl.io/docs/open/integration/chip_vendors/nxp/elcdif) da NXP.
- **Linux embarcado**: framebuffer (fbdev), DRM/KMS, Wayland e `libinput`/`evdev` para entrada de toque e ponteiro. Saiba mais [aqui](https://lvgl.io/docs/open/integration/embedded_linux).
- **GPU / aceleradores**: VG-Lite, NXP PXP, ThinkSilicon NemaGFX, Arm-2D  e [OpenGL ES](https://lvgl.io/docs/open/integration/embedded_linux/drivers/opengl_driver)

Veja a lista completa e os guias de configuração na [documentação de Integração](https://lvgl.io/docs/open/integration).

### No LVGL Pro

No LVGL Pro você pode criar projetos prontos para uso somente de UI, VSCode, Zephyr e Linux com um único clique.
<img width="600" alt="image" src="https://github.com/user-attachments/assets/5aadb850-6b40-49d1-ba96-2296041c7e27" />

### Portando manualmente

Integrar o LVGL é muito simples. Basta colocá-lo em qualquer projeto e compilá-lo como você compilaria outros arquivos.
Para configurar o LVGL, copie `lv_conf_template.h` como `lv_conf.h`, habilite o primeiro `#if 0` e ajuste as configurações conforme necessário.
(A configuração padrão geralmente é suficiente.) Se disponível, o LVGL também pode ser usado com Kconfig.

Uma vez no projeto, você pode inicializar o LVGL e criar os dispositivos de display e entrada da seguinte forma:

```c
#include "lvgl/lvgl.h" /*Define LV_LVGL_H_INCLUDE_SIMPLE to include as "lvgl.h"*/

#define TFT_HOR_RES 320
#define TFT_VER_RES 240

static uint32_t my_tick_cb(void)
{
    return my_get_millisec();
}

static void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    /*Write px_map to the area->x1, area->x2, area->y1, area->y2 area of the
     *frame buffer or external display controller. */

    /* signal LVGL that we're done */
    lv_display_flush_ready(disp);
}

static void my_touch_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
   if(my_touch_is_pressed()) {
       data->point.x = touchpad_x;
       data->point.y = touchpad_y;
       data->state = LV_INDEV_STATE_PRESSED;
   } else {
       data->state = LV_INDEV_STATE_RELEASED;
   }
}

void main(void)
{
    my_hardware_init();

    /*Initialize LVGL*/
    lv_init();

    /*Set millisecond-based tick source for LVGL so that it can track time.*/
    lv_tick_set_cb(my_tick_cb);

    /*Create a display where screens and widgets can be added*/
    lv_display_t * display = lv_display_create(TFT_HOR_RES, TFT_VER_RES);

    /*Add rendering buffers to the screen.
     *Here adding a smaller partial buffer assuming 16-bit (RGB565 color format)*/
    static uint8_t buf[TFT_HOR_RES * TFT_VER_RES / 10 * 2]; /* x2 because of 16-bit color depth */
    lv_display_set_buffers(display, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    /*Add a callback that can flush the content from `buf` when it has been rendered*/
    lv_display_set_flush_cb(display, my_flush_cb);

    /*Create an input device for touch handling*/
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touch_read_cb);

    /*The drivers are in place; now we can create the UI*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_center(label);

    /*Execute the LVGL-related tasks in a loop*/
    while(1) {
        lv_timer_handler();
        my_sleep_ms(5);         /*Wait a little to let the system breathe*/
    }
}
```


<a id="contributing"></a>
## Contribuindo

O LVGL é um projeto aberto, e as contribuições são muito bem-vindas. Existem muitas maneiras de contribuir, desde simplesmente falar sobre o seu projeto, escrever exemplos, melhorar a documentação, corrigir bugs, ou até mesmo hospedar o seu próprio projeto sob a organização LVGL.

Para uma descrição detalhada das oportunidades de contribuição, visite a seção [Contribuindo](https://lvgl.io/docs/open/contributing)
da documentação.

Centenas de pessoas já deixaram sua marca no LVGL. Seja uma delas! Vejo você por aqui! 🙂


<a id="license"></a>
## Licença

A biblioteca LVGL é distribuída sob a **licença MIT**, então você pode usá-la livremente tanto em
produtos de código aberto quanto comerciais sem royalties. Veja [`LICENCE.txt`](../LICENCE.txt).

Todas as bibliotecas de terceiros incluídas no LVGL também são lançadas sob licenças compatíveis com a MIT,
então você pode usar o LVGL e suas dependências com confiança.

O **[LVGL Pro](https://lvgl.io/pro)** possui licenciamento separado:
- As camadas **Community** e **Evaluation** são **gratuitas para uso não comercial**, perfeitas para
  aprendizado, projetos de hobby e avaliação das ferramentas.
- O uso comercial do LVGL Pro requer uma licença paga. Veja os [Preços e Detalhes](https://lvgl.io/pro#pricing).
