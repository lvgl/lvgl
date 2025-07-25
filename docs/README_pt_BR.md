**NOTA IMPORTANTE** A próxima versão principal (v9.0.0) está sendo desenvolvida na branch master.
A última versão estável está disponível na branch [release/v8.3](https://github.com/lvgl/lvgl/tree/release/v8.3).

---

<a href="https://github.com/sponsors/lvgl" target="_blank"><img align="left" src="https://lvgl.io/github-assets/sponsor.png" height="32px"></a>

<p align="right">
  <a href="../README.md">English</a> |
  <a href="./README_zh.md">中文</a> |
  <b>Português do Brasil</b> |
  <a href="./README_jp.md">日本語</a>
</p>
<p align="center">
  <img src="https://lvgl.io/github-assets/logo-colored.png" width=300px>
</p>




  <h1 align="center">LVGL - Biblioteca gráfica leve e versátil</h1>
  <br>
<div align="center">
  <img src="https://raw.githubusercontent.com/kisvegabor/test/master/smartwatch_demo.gif">
  &nbsp;
  <img border="1px" src="https://lvgl.io/github-assets/widgets-demo.gif">
</div>

<br>

<p align="center">
  <a href="https://lvgl.io" title="Página inicial do LVGL">Site</a> |
  <a href="https://docs.lvgl.io/" title="Documentação detalhada com +100 exemplos">Documentação</a> |
  <a href="https://forum.lvgl.io" title="Obtenha ajuda e ajude outras pessoas">Fórum</a> |
  <a href="https://lvgl.io/services" title="Design gráfico, implementações e consultoria de serviços">Serviços</a> |
  <a href="https://lvgl.io/demos" title="Execute demonstrações no seu navegador">Demonstrações</a>
</p>

<br>

## :monocle_face: Visão Geral

**Maduro e popular**

LVGL é a biblioteca gráfica incorporada gratuita e de código aberto mais popular para criar belas interfaces de usuário para qualquer display do tipo MCU, MPU. Ela é suportada por fornecedores e projetos líderes do setor, como ARM, STM32, NXP, Espressif, Nuvoton, Arduino, RT-Thread, Zephyr, NuttX, Adafruit e muitos outros.

**Rico em recursos**

Ela tem todos os recursos para a criação de GUIs modernas e bonitas: mais de 30 widgets integrados, um sistema de design poderoso, gerenciadores de layout inspirados na web e um sistema de tipografia com suporte para vários idiomas. Para integrar o LVGL em sua plataforma, tudo que você precisa é de pelo menos 32kB de RAM e 128kB de Flash, um compilador C, um frame buffer e pelo menos uma tela de tamanho 1/10 para renderização.

**Editor UI profissional**

SquareLine Studio é um editor de interface do usuário de (arrasta e solta) profissional para LVGL. Ele roda em Windows, Linux e MacOS também e você pode experimentá-lo sem se registrar no site.

**Serviços**

Nossa equipe está pronta para ajudá-lo com design gráfico, implementação de UI e serviços de consultoria. Entre em contato conosco se precisar de algum suporte durante o desenvolvimento de seu próximo projeto de GUI.

## :rocket: Recursos

**Gratuito e portátil**

  - Uma biblioteca C totalmente portátil (compatível com C++) sem dependências externas.
  - Pode ser compilado para qualquer display MCU ou MPU, e qualquer sistema operacional de tempo real (RT-OS).
  - Suporta monitores monocromáticos, ePaper, OLED ou TFT. [Guia de portabilidade](https://docs.lvgl.io/master/details/integration/adding-lvgl-to-your-project/index.html)
  - Distribuído sob a licença do MIT, para que você também possa usá-lo facilmente em projetos comerciais.
  - Precisa de apenas 32 kB de RAM e 128 kB de Flash, um frame buffer e pelo menos uma tela de tamanho 1/10 para renderização.
  - Sistemas operacionais, memória externa e GPU são suportados, mas não obrigatórios.

**Widgets, designs, layouts e muito mais**

  - Mais de 30 widgets integrados: botão, etiqueta (label), controle deslizante (slider), gráfico (chart), teclado, medidor (meter), tabelas e muito mais.
  - Sistema de design flexível com pelo menos 100 propriedades de estilo para personalizar qualquer parte dos widgets.
  - Mecanismos de layouts Flexbox e Grid para dimensionar e posicionar automaticamente os widgets de maneira responsiva.
  - Os textos são renderizados com codificação UTF-8, suportando sistemas de escrita CJK (chinês, japonês e coreano), tailandês, hindi, árabe e persa.
  - Quebra de palavras (word wrapping), espaçamento entre letras (kerning), rolagem de texto (scrolling), renderização subpixel, entrada em chinês Pinyin-IME e emojis.
  - Mecanismo de renderização que suporta animações, anti-aliasing, opacidade, rolagem suave (smooth scroll), sombras, transformação de imagens, etc.
  - Suporta mouse, touchpad, teclado, botões externos, dispositivos de entrada codificadores (encoders).
  - Suporta vários monitores.

**Suporte de vinculação (binding) e compilação de arquivos**

  - Exposição da API do LVGL com o [Micropython](https://blog.lvgl.io/2019-02-20/micropython-bindings)
  - Nenhum sistema de compilação personalizado é usado. Você pode construir o LVGL enquanto constrói os outros arquivos do seu projeto.
  - O suporte para Make e [CMake](https://docs.lvgl.io/master/details/integration/building/cmake.html) já vem incluído.
  - [Desenvolva no PC](https://docs.lvgl.io/master/details/integration/ide/pc-simulator.html) e use o mesmo código de interface do usuário em hardwares incorporados (embedded hardware).
  - Converta o código C para um arquivo HTML com o [Emscripten port](https://github.com/lvgl/lv_web_emscripten).

**Documentação, ferramentas e serviços**

  - Documentação detalhada com [+100 exemplos simples](https://docs.lvgl.io/master/examples.html)
  - [SquareLine Studio](https://squareline.io) - Um software editor UI profissional e fácil de usar, para acelerar e simplificar o desenvolvimento da interface do usuário.
  - [Serviços](https://lvgl.io/services) como design de UI, implementação e consultoria para tornar o desenvolvimento de UI mais simples e rápido.

## :heart: Patrocinador

Se o LVGL economizou muito tempo e dinheiro ou você apenas se divertiu ao usá-lo, considere Apoiar o desenvolvimento.

**Como e com o que utilizamos os recursos doados?**
Nosso objetivo é fornecer compensação financeira para as pessoas que mais fazem pelo LVGL. Isso significa que não apenas os mantenedores, mas qualquer pessoa que implemente um ótimo recurso deve receber um pagamento com o dinheiro acumulado. Usamos as doações para cobrir nossos custos operacionais, como servidores e serviços relacionados.

**Como doar?**
Usamos o [Open Collective](https://opencollective.com/lvgl), onde você pode enviar facilmente doações únicas ou recorrentes. Você também pode ver todas as nossas despesas de forma transparente.

**Como receber o pagamento de sua contribuição?**
Se alguém implementar ou corrigir um problema rotulado como [Patrocinado](https://github.com/lvgl/lvgl/labels/Sponsored), essa pessoa receberá um pagamento por esse trabalho. Estimamos o tempo necessário, a complexidade e a importância da questão e definimos um preço de acordo. Para entrar, apenas comente sobre um problema patrocinado dizendo "Olá, gostaria de lidar com isso. É assim que estou planejando corrigi-lo/implementá-lo...". Um trabalho é considerado pronto quando é aprovado e mesclado por um mantenedor. Depois disso, você pode enviar uma "despesa" (expense) pela plataforma [opencollective.com](https://opencollective.com/lvgl) e então receberá o pagamento em alguns dias.

**Organizações que apoiam o projeto LVGL**<br>
[![Patrocinadores do LVGL](https://opencollective.com/lvgl/organizations.svg?width=600)](https://opencollective.com/lvgl)

**Pessoas que apoiam o projeto LVGL**<br>
[![Backers of LVGL](https://contrib.rocks/image?repo=lvgl/lvgl&max=48)](https://opencollective.com/lvgl)

## :package: Pacotes

LVGL está disponível para:

- [Arduino library](https://docs.lvgl.io/master/details/integration/framework/arduino.html)
- [PlatformIO package](https://registry.platformio.org/libraries/lvgl/lvgl)
- [Zephyr library](https://docs.zephyrproject.org/latest/kconfig.html#CONFIG_LVGL)
- [ESP32 component](https://docs.lvgl.io/master/details/integration/chip/espressif.html)
- [NXP MCUXpresso component](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
- [NuttX library](https://docs.lvgl.io/master/details/integration/os/nuttx.html)
- [RT-Thread RTOS](https://docs.lvgl.io/master/details/integration/os/rt-thread.html)
- NXP MCUXpresso library
- CMSIS-Pack

## :man_technologist: Exemplos

Veja como criar um botão com um evento de clique em C e MicroPython. Para mais exemplos, veja a pasta [examples](https://github.com/lvgl/lvgl/tree/master/examples).

### Botão com evento de clique

![Botão LVGL com exemplo de rótulo (label)](https://github.com/kisvegabor/test/raw/master/readme_example_2.gif)

<details>
  <summary>Código C</summary>

```c
lv_obj_t * btn = lv_button_create(lv_screen_active());          /* Adiciona o botão a tela atual */
lv_obj_center(btn);                                             /* Define a posição do botão */
lv_obj_set_size(btn, 100, 50);                                  /* Define o tamanho do botão */
lv_obj_add_event(btn, btn_event_cb, LV_EVENT_CLICKED, NULL);    /* Atribui um retorno de chamada (callback) ao botão */

lv_obj_t * label = lv_label_create(btn);                        /* Adiciona um rótulo (label) */
lv_label_set_text(label, "Botão");                              /* Define um texto para o rótulo (label) */
lv_obj_center(label);                                           /* Alinha o texto no centro do botão */
...

void btn_event_cb(lv_event_t * e)
{
  printf("Clicado\n");
}
```
</details>

<details>
  <summary>Código MicroPython | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=926bde43ec7af0146c486de470c53f11f167491e" target="_blank">Simulador online</a></summary>

```python
def btn_event_cb(e):
  print("Clicado")

# Cria um botão e um rótulo (label)
btn = lv.btn(lv.screen_active())
btn.center()
btn.set_size(100, 50)
btn.add_event(btn_event_cb, lv.EVENT.CLICKED, None)

label = lv.label(btn)
label.set_text("Botão")
label.center()
```
</details>
<br>

### Caixas de seleção (chackboxes) com layout
![Caixas de seleção (chackboxes) com layout no LVGL](https://github.com/kisvegabor/test/raw/master/readme_example_3.gif)

<details>
  <summary>Código em C</summary>

```c

lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN);
lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

lv_obj_t * cb;
cb = lv_checkbox_create(lv_screen_active());
lv_checkbox_set_text(cb, "Maça");
lv_obj_add_event(cb, event_handler, LV_EVENT_ALL, NULL);

cb = lv_checkbox_create(lv_screen_active());
lv_checkbox_set_text(cb, "Banana");
lv_obj_add_state(cb, LV_STATE_CHECKED);
lv_obj_add_event(cb, event_handler, LV_EVENT_ALL, NULL);

cb = lv_checkbox_create(lv_screen_active());
lv_checkbox_set_text(cb, "Limão");
lv_obj_add_state(cb, LV_STATE_DISABLED);
lv_obj_add_event(cb, event_handler, LV_EVENT_ALL, NULL);

cb = lv_checkbox_create(lv_screen_active());
lv_obj_add_state(cb, LV_STATE_CHECKED | LV_STATE_DISABLED);
lv_checkbox_set_text(cb, "Melão\ne uma nova linha");
lv_obj_add_event(cb, event_handler, LV_EVENT_ALL, NULL);
```
</details>

<details>
  <summary>Código MicroPython | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=311d37e5f70daf1cb0d2cad24c7f72751b5f1792" target="_blank">Online Simulator</a></summary>

```python
def event_handler(e):
    code = e.get_code()
    obj = e.get_target_obj()
    if code == lv.EVENT.VALUE_CHANGED:
        txt = obj.get_text()
        if obj.get_state() & lv.STATE.CHECKED:
            state = "Marcador"
        else:
            state = "Desmarcado"
        print(txt + ":" + state)


lv.scr_act().set_flex_flow(lv.FLEX_FLOW.COLUMN)
lv.scr_act().set_flex_align(lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.START, lv.FLEX_ALIGN.CENTER)

cb = lv.checkbox(lv.screen_active())
cb.set_text("Maça")
cb.add_event(event_handler, lv.EVENT.ALL, None)

cb = lv.checkbox(lv.screen_active())
cb.set_text("Banana")
cb.add_state(lv.STATE.CHECKED)
cb.add_event(event_handler, lv.EVENT.ALL, None)

cb = lv.checkbox(lv.screen_active())
cb.set_text("Limão")
cb.add_state(lv.STATE.DISABLED)
cb.add_event(event_handler, lv.EVENT.ALL, None)

cb = lv.checkbox(lv.screen_active())
cb.add_state(lv.STATE.CHECKED | lv.STATE.DISABLED)
cb.set_text("Melão")
cb.add_event(event_handler, lv.EVENT.ALL, None)
```

</details>
<br>

### Estilizando um controle deslizante (slider)
![Estilizando um controle deslizante (slider) com LVGL](https://github.com/kisvegabor/test/raw/master/readme_example_4.gif)


<details>
  <summary>Código C</summary>

```c
lv_obj_t * slider = lv_slider_create(lv_screen_active());
lv_slider_set_value(slider, 70, LV_ANIM_OFF);
lv_obj_set_size(slider, 300, 20);
lv_obj_center(slider);

/* Adiciona estilos locais à parte MAIN (retângulo de fundo) */
lv_obj_set_style_bg_color(slider, lv_color_hex(0x0F1215), LV_PART_MAIN);
lv_obj_set_style_bg_opa(slider, 255, LV_PART_MAIN);
lv_obj_set_style_border_color(slider, lv_color_hex(0x333943), LV_PART_MAIN);
lv_obj_set_style_border_width(slider, 5, LV_PART_MAIN);
lv_obj_set_style_pad_all(slider, 5, LV_PART_MAIN);

/* Crie uma folha de estilo reutilizável para a parte do (INDICADOR) */
static lv_style_t style_indicator;
lv_style_init(&style_indicator);
lv_style_set_bg_color(&style_indicator, lv_color_hex(0x37B9F5));
lv_style_set_bg_grad_color(&style_indicator, lv_color_hex(0x1464F0));
lv_style_set_bg_grad_dir(&style_indicator, LV_GRAD_DIR_HOR);
lv_style_set_shadow_color(&style_indicator, lv_color_hex(0x37B9F5));
lv_style_set_shadow_width(&style_indicator, 15);
lv_style_set_shadow_spread(&style_indicator, 5);

/* Adicione a folha de estilo à parte do INDICATOR do controle deslizante (slider) */
lv_obj_add_style(slider, &style_indicator, LV_PART_INDICATOR);

/* Adicione o mesmo estilo à parte do KNOB e sobrescreva localmente algumas propriedades */
lv_obj_add_style(slider, &style_indicator, LV_PART_KNOB);

lv_obj_set_style_outline_color(slider, lv_color_hex(0x0096FF), LV_PART_KNOB);
lv_obj_set_style_outline_width(slider, 3, LV_PART_KNOB);
lv_obj_set_style_outline_pad(slider, -5, LV_PART_KNOB);
lv_obj_set_style_shadow_spread(slider, 2, LV_PART_KNOB);
```

</details>

<details>
  <summary>Código MicroPython |
<a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=c431c7b4dfd2cc0dd9c392b74365d5af6ea986f0" target="_blank">Simulador online</a>
</summary>


```python
# Crie um controle deslizante (slider) e adicione o estilo
slider = lv.slider(lv.screen_active())
slider.set_value(70, lv.ANIM.OFF)
slider.set_size(300, 20)
slider.center()

# Adicione estilos locais à parte MAIN (retângulo de fundo)
slider.set_style_bg_color(lv.color_hex(0x0F1215), lv.PART.MAIN)
slider.set_style_bg_opa(255, lv.PART.MAIN)
slider.set_style_border_color(lv.color_hex(0x333943), lv.PART.MAIN)
slider.set_style_border_width(5, lv.PART.MAIN)
slider.set_style_pad_all(5, lv.PART.MAIN)

# Crie uma folha de estilo reutilizável para a parte do INDICATOR
style_indicator = lv.style_t()
style_indicator.init()
style_indicator.set_bg_color(lv.color_hex(0x37B9F5))
style_indicator.set_bg_grad_color(lv.color_hex(0x1464F0))
style_indicator.set_bg_grad_dir(lv.GRAD_DIR.HOR)
style_indicator.set_shadow_color(lv.color_hex(0x37B9F5))
style_indicator.set_shadow_width(15)
style_indicator.set_shadow_spread(5)

# Adicione a folha de estilo à parte do INDICATOR do controle deslizante (slider)
slider.add_style(style_indicator, lv.PART.INDICATOR)
slider.add_style(style_indicator, lv.PART.KNOB)

# Adicione o mesmo estilo à parte do KNOB e sobrescreva localmente algumas propriedades
slider.set_style_outline_color(lv.color_hex(0x0096FF), lv.PART.KNOB)
slider.set_style_outline_width(3, lv.PART.KNOB)
slider.set_style_outline_pad(-5, lv.PART.KNOB)
slider.set_style_shadow_spread(2, lv.PART.KNOB)
```

</details>
<br>

### Textos em inglês, hebraico (LRT-RTL misto) e chinês

![Textos em inglês, hebraico (LRT-RTL misto) e chinês com LVGL](https://raw.githubusercontent.com/kisvegabor/test/master/readme_example_5.png)

<details>
  <summary>Código C</summary>

```c
lv_obj_t * ltr_label = lv_label_create(lv_screen_active());
lv_label_set_text(ltr_label, "In modern terminology, a microcontroller is similar to a system on a chip (SoC).");
lv_obj_set_style_text_font(ltr_label, &lv_font_montserrat_16, 0);
lv_obj_set_width(ltr_label, 310);
lv_obj_align(ltr_label, LV_ALIGN_TOP_LEFT, 5, 5);

lv_obj_t * rtl_label = lv_label_create(lv_screen_active());
lv_label_set_text(rtl_label,"מעבד, או בשמו המלא יחידת עיבוד מרכזית (באנגלית: CPU - Central Processing Unit).");
lv_obj_set_style_base_dir(rtl_label, LV_BASE_DIR_RTL, 0);
lv_obj_set_style_text_font(rtl_label, &lv_font_dejavu_16_persian_hebrew, 0);
lv_obj_set_width(rtl_label, 310);
lv_obj_align(rtl_label, LV_ALIGN_LEFT_MID, 5, 0);

lv_obj_t * cz_label = lv_label_create(lv_screen_active());
lv_label_set_text(cz_label,
                  "嵌入式系统（Embedded System），\n是一种嵌入机械或电气系统内部、具有专一功能和实时计算性能的计算机系统。");
lv_obj_set_style_text_font(cz_label, &lv_font_source_han_sans_sc_16_cjk, 0);
lv_obj_set_width(cz_label, 310);
lv_obj_align(cz_label, LV_ALIGN_BOTTOM_LEFT, 5, -5);
```

</details>

<details>
  <summary>Código MicroPython | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=18bb38200a64e10ead1aa17a65c977fc18131842" target="_blank">Simulador online</a></summary>

```python
ltr_label = lv.label(lv.screen_active())
ltr_label.set_text("In modern terminology, a microcontroller is similar to a system on a chip (SoC).")
ltr_label.set_style_text_font(lv.font_montserrat_16, 0);

ltr_label.set_width(310)
ltr_label.align(lv.ALIGN.TOP_LEFT, 5, 5)

rtl_label = lv.label(lv.screen_active())
rtl_label.set_text("מעבד, או בשמו המלא יחידת עיבוד מרכזית (באנגלית: CPU - Central Processing Unit).")
rtl_label.set_style_base_dir(lv.BASE_DIR.RTL, 0)
rtl_label.set_style_text_font(lv.font_dejavu_16_persian_hebrew, 0)
rtl_label.set_width(310)
rtl_label.align(lv.ALIGN.LEFT_MID, 5, 0)

font_han_sans_16_cjk = lv.font_load("S:../../assets/font/lv_font_source_han_sans_sc_16_cjk.fnt")

cz_label = lv.label(lv.screen_active())
cz_label.set_style_text_font(font_han_sans_16_cjk, 0)
cz_label.set_text("嵌入式系统（Embedded System），\n是一种嵌入机械或电气系统内部、具有专一功能和实时计算性能的计算机系统。")
cz_label.set_width(310)
cz_label.align(lv.ALIGN.BOTTOM_LEFT, 5, -5)

```
</details>

## :arrow_forward: Começando
Esta lista irá guiá-lo para começar com o LVGL passo a passo.

**Familiarize-se com o LVGL**

  1. Confira as [demos on-line](https://lvgl.io/demos) para ver o LVGL em ação (~3 minutos)
  2. Leia a página de [introdução](https://docs.lvgl.io/master/intro/introduction/index.html) da documentação (~5 minutos)
  3. Familiarize-se com o básico na página de [visão geral rápida](https://docs.lvgl.io/master/intro/getting_started/learn_the_basics.html) (~15 minutos)

**Começando a usar o LVGL**

  4. Configure um [simulador](https://docs.lvgl.io/master/details/integration/ide/pc-simulator.html) (~10 minutos)
  5. Experimente alguns [exemplos](https://github.com/lvgl/lvgl/tree/master/examples)
  6. Porte o LVGL para uma placa. Veja o guia [portando o LVGL](https://docs.lvgl.io/master/details/integration/adding-lvgl-to-your-project/index.html) ou veja um projeto pronto para usar em [projetos](https://github.com/lvgl?q=lv_port_)

**Torne-se um profissional**

  7. Leia a página [visão geral](https://docs.lvgl.io/master/intro/getting_started.html) para entender melhor a biblioteca (~2-3 horas)
  8. Verifique a documentação dos [widgets](https://docs.lvgl.io/master/details/widgets/index.html) para ver seus recursos e usabilidade

**Obtenha ajuda e ajude outras pessoas**

  9. Se você tiver dúvidas, acesse o [Fórum](http://forum.lvgl.io)
  10. Leia o guia de [contribuição](https://docs.lvgl.io/master/contributing/index.html) para ver como você pode ajudar a melhorar o LVGL (~15 minutos)

**E mais**

  11. Baixe e experimente o editor [SquareLine Studio](https://squareline.io).
  12. Entre em contato conosco para [serviços e consultoria](https://lvgl.io/services).

## :handshake: Serviços
A LVGL LLC foi criada para fornecer uma base sólida para a biblioteca LVGL e oferecer vários tipos de serviços para ajudá-lo no desenvolvimento da sua interface do usuário. Com mais de 15 anos de experiência na indústria gráfica e de interface do usuário, podemos ajudá-lo a levar sua interface do usuário para o próximo nível.

- **Design gráfico**: Nossos designers gráficos internos são especialistas em criar belos designs modernos que se adaptam ao seu produto e aos recursos do seu hardware.
- **Implementação da interface do usuário**: Também podemos implementar sua interface do usuário com base no design que você ou nós criamos. Você pode ter certeza de que tiraremos o máximo proveito de seu hardware e do LVGL. Se um recurso ou widget estiver faltando no LVGL, não se preocupe, nós o implementaremos para você.
- **Consultoria e Suporte**: Também podemos apoiá-lo com consultoria para evitar erros que podem te custar caros durante o desenvolvimento da sua interface do usuário.
- **Certificação**: Para empresas que oferecem placas para desenvolvimento ou kits prontos para produção, fazemos certificação que mostram como uma placa pode executar o LVGL.

Confira nossas [demonstrações](https://lvgl.io/demos) como referência. Para obter mais informações, consulte a [página de serviços](https://lvgl.io/services).

[Fale conosco](https://lvgl.io/#contact) e conte como podemos ajudar.

## :star2: Contribuindo
O LVGL é um projeto aberto e sua contribuição é muito bem-vinda. Há muitas maneiras de contribuir, desde simplesmente falando sobre seu projeto, escrevendo exemplos, melhorando a documentação, corrigindo bugs até hospedar seu próprio projeto sob a organização LVGL.

Para obter uma descrição detalhada das oportunidades de contribuição, visite a página de [contribuição](https://docs.lvgl.io/master/contributing/index.html) da documentação.

Mais de 300 pessoas já deixaram sua impressão digital no LVGL. Seja um deles! Veja o seu aqui! :slightly_smiling_face:

<a href="https://github.com/lvgl/lvgl/graphs/contributors">
   <img src="https://contrib.rocks/image?repo=lvgl/lvgl&max=48" />
</a>

... e muitos outros.
