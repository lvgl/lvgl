<h1 align="center"> LVGL - Biblioteca gráfica leve e versátil</h1>
<p align="center">
    <img src="https://lvgl.io/assets/images/lvgl_widgets_demo.gif">
</p>
<p align="center">
    O LVGL fornece tudo o que você precisa para criar uma GUI incorporada com elementos gráficos fáceis de usar, belos efeitos visuais e um baixo consumo de memória.
</p>
<h4 align="center">
    <a href="https://lvgl.io">Site</a> &middot;
    <a href="https://docs.lvgl.io/">Documentação</a> &middot;
    <a href="https://forum.lvgl.io">Fórum</a> &middot;
    <a href="https://lvgl.io/services">Serviços</a> &middot;
    <a href="https://docs.lvgl.io/master/examples.html">Exemplos interativos</a>
</h4>

[English](./README.md) | [中文](./README_zh.md) | **Português do Brasil**

---

### Tabela de conteúdo

- [Visão Geral](#visão-geral)
- [Iniciando](#iniciando)
- [Exemplos](#exemplos)
- [Serviços](#serviços)
- [Contribuindo](#contribuindo)

## Visão Geral

### Recursos
* Poderosos [widgets](https://docs.lvgl.io/master/widgets/index.html): botões, gráficos, listas, controles deslizantes (sliders), imagens, etc.
* Mecanismo gráfico avançado: animações, anti-aliasing, opacidade, rolagem suave, modos de mesclagem (blending modes), etc.
* Suporte à [vários dispositivos de entrada](https://docs.lvgl.io/master/overview/indev.html): tela sensível ao toque, mouse, teclado, codificador, botões, etc.
* Suporte à [vários monitores](https://docs.lvgl.io/master/overview/display.html)
* Pode ser usado com qualquer microcontrolador e display, independente do hardware
* Escalável para operar com pouca memória (64 kB Flash, 16 kB RAM)
* Suporte multilíngue com manipulação UTF-8, suporte ao alfabeto bidirecional, árabe e CJK (Chinês, Japonês e Coreano)
* Elementos gráficos totalmente personalizáveis por meio de [CSS](https://docs.lvgl.io/master/overview/style.html)
* Layouts poderosos inspirados em CSS: [Flexbox](https://docs.lvgl.io/master/layouts/flex.html) e [Grid](https://docs.lvgl.io/master/layouts/grid.html)
* SO, memória externa e GPU são suportados, mas não obrigatórios. (suporte integrado para STM32 DMA2D, SWM341 DMA2D e NXP PXP e VGLite)
* Renderização suave mesmo com um [buffer de quadro único](https://docs.lvgl.io/master/porting/display.html) (single frame buffer)
* Escrito em C e compatível com C++
* Uso do LittlevGL com Micropython simplificado com [LVGL API in Micropython](https://blog.lvgl.io/2019-02-20/micropython-bindings)
* [Simulador](https://docs.lvgl.io/master/get-started/platforms/pc-simulator.html) para desenvolver no PC sem hardware embutido
* Mais de 100 [exemplos simples](https://github.com/lvgl/lvgl/tree/master/examples)
* [Documentação](http://docs.lvgl.io/) e referências de API online e em PDF

### Requerimentos
Basicamente, todo controlador moderno (que é capaz de acionar um display) é adequado para executar LVGL. Os requisitos mínimos são:

<table>
  <tr>
    <td>
        <strong>Nome</strong>
    </td>
    <td>
        <strong>Minímo</strong>
    </td>
    <td>
        <strong>Recomendado</strong>
    </td>
  </tr>
  <tr>
    <td>
        <strong>Arquitetura</strong>
    </td>
    <td colspan="2">Microcontrolador ou processador de 16, 32 ou 64 bits</td>
  </tr>
  <tr>
    <td>
        <strong>Clock</strong>
    </td>
    <td>&gt; 16 MHz</td>
    <td>&gt; 48 MHz</td>
  </tr>
  <tr>
    <td>
        <strong>Flash/ROM</strong>
    </td>
    <td>&gt; 64 kB</td>
    <td>&gt; 180 kB</td>
  </tr>

  <tr>
    <td>
        <strong>RAM estática</strong>
    </td>
    <td>&gt; 16 kB</td>
    <td>&gt; 48 kB</td>
  </tr>

  <tr>
    <td>
        <strong>Draw buffer</strong>
    </td>
    <td>&gt; 1 &times; <em>hor. res.</em> pixels</td>
    <td>&gt; tamanho da tela de 1/10</td>
  </tr>

  <tr>
    <td>
        <strong>Compilador</strong>
    </td>
    <td colspan="2">Padrão C99 ou mais recente</td>
  </tr>
</table>

*Observe que o uso de memória pode variar dependendo da arquitetura, do compilador e das opções de compilação.*

### Plataformas suportadas
O LVGL é completamente independente de plataforma e pode ser usado com qualquer MCU que atenda aos requisitos.
Apenas para citar algumas plataformas:

- NXP: Kinetis, LPC, iMX, iMX RT
- STM32F1, STM32F3, STM32F4, STM32F7, STM32L4, STM32L5, STM32H7
- Microchip dsPIC33, PIC24, PIC32MX, PIC32MZ
- [Linux frame buffer](https://blog.lvgl.io/2018-01-03/linux_fb) (/dev/fb)
- [Raspberry Pi](http://www.vk3erw.com/index.php/16-software/63-raspberry-pi-official-7-touchscreen-and-littlevgl)
- [Espressif ESP32](https://github.com/lvgl/lv_port_esp32)
- [Infineon Aurix](https://github.com/lvgl/lv_port_aurix)
- Nordic NRF52 Bluetooth modules
- Quectel modems
- [SYNWIT SWM341](https://www.synwit.cn/)

LVGL também está disponível para:
- [Arduino library](https://docs.lvgl.io/master/get-started/platforms/arduino.html)
- [PlatformIO package](https://registry.platformio.org/libraries/lvgl/lvgl)
- [Zephyr library](https://docs.zephyrproject.org/latest/reference/kconfig/CONFIG_LVGL.html)
- [ESP32 component](https://docs.lvgl.io/master/get-started/platforms/espressif.html)
- [NXP MCUXpresso component](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
- [NuttX library](https://docs.lvgl.io/master/get-started/os/nuttx.html)
- [RT-Thread RTOS](https://docs.lvgl.io/master/get-started/os/rt-thread.html)

## Iniciando
Esta lista mostra a maneira recomendada de aprender sobre a biblioteca:

1. Confira as [demos on-line](https://lvgl.io/demos) para ver o LVGL em ação (3 minutos)
2. Leia a [introdução](https://docs.lvgl.io/master/intro/index.html) da documentação (5 minutos)
3. Familiarize-se com o básico da [Visão geral rápida](https://docs.lvgl.io/master/get-started/quick-overview.html) (15 minutos)
4. Configure um [simulador](https://docs.lvgl.io/master/get-started/platforms/pc-simulator.html) (10 minutos)
5. Experimente alguns [Exemplos](https://github.com/lvgl/lvgl/tree/master/examples)
6. Placa para porta LVGL. Veja o guia [porting](https://docs.lvgl.io/master/porting/index.html) ou verifique o pronto para usar [Projects](https://github.com/lvgl?q=lv_port_)
7. Leia a [visão geral](https://docs.lvgl.io/master/overview/index.html) para entender melhor a biblioteca (2-3 horas)
8. Verifique a documentação dos [widgets](https://docs.lvgl.io/master/widgets/index.html) para ver seus recursos e como utilizá-los
9. Se você tiver dúvidas, acesse o [fórum](http://forum.lvgl.io/)
10. Leia o guia de [contribuição](https://docs.lvgl.io/master/CONTRIBUTING.html) para ver como você pode ajudar a melhorar o LVGL (15 minutos)

## Exemplos
Para mais exemplos, veja a pasta [examples](https://github.com/lvgl/lvgl/tree/master/examples).

![Exemplo de botão LVGL com rótulo (label)](https://github.com/lvgl/lvgl/raw/master/docs/misc/btn_example.png)

### C

```c
lv_obj_t * button = lv_btn_create(lv_scr_act());                             /* Adiciona um botão à tela atual */
lv_obj_set_pos(button, 10, 10);                                              /* Define uma posição ao botão na tela */
lv_obj_set_size(button, 100, 50);                                            /* Define o tamanho */
lv_obj_add_event_cb(button, button_event_callback, LV_EVENT_CLICKED, NULL);  /* Atribui um retorno de chamada (callback) */

lv_obj_t * label = lv_label_create(button);                                  /* Adiciona um rótulo (label) */
lv_label_set_text(label, "Clique aqui");                                     /* Define o texto do rótulo (label) */
lv_obj_center(label);                                                        /* Alinha o texto ao centro */
...

void button_event_callback(lv_event_t * e)
{
  printf("Clicado\n");
}
```

### Micropython
Saiba mais em [Micropython](https://docs.lvgl.io/master/get-started/bindings/micropython.html)

```python
def button_event_callback(event):
  print("Clicado")

# Cria um botão e um rótulo (label)
button = lv.btn(lv.scr_act())
button.set_pos(10, 10)
button.set_size(100, 50)
button.add_event_cb(button_event_callback, lv.EVENT.CLICKED, None)

label = lv.label(button)
label.set_text("Cliquq aqui")
label.center()
```

## Serviços
O LVGL Kft foi estabelecido para fornecer uma base sólida para a biblioteca LVGL. Oferecemos vários tipos de serviços
para ajudá-lo no desenvolvimento da interface do usuário:

- Design gráfico
- Implementação de IU
- Consultoria/Suporte

Para mais informações, consulte [LVGL Serviços](https://lvgl.io/services). Sinta-se à vontade para entrar em contato
conosco se tiver alguma dúvida.

## Contribuindo
O LVGL é um projeto aberto e sua contribuição é muito bem-vinda. Há muitas maneiras de contribuir, desde simplesmente
falando sobre seu projeto, escrevendo exemplos, melhorando a documentação, corrigindo bugs até hospedar seu próprio
projeto sob a organização LVGL.

Para obter uma descrição detalhada das oportunidades de contribuição, visite a seção de [contribuição](https://docs.lvgl.io/master/CONTRIBUTING.html) da documentação.
