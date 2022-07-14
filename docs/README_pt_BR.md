**NOTA IMPORTANTE** A próxima versão principal (v9.0.0) está sendo desenvolvida na branch master.
A última versão estável está disponível na branch [release/v8.3](https://github.com/lvgl/lvgl/tree/release/v8.3).

---

<p align="right"><b>English</b> | <a href="./README_zh.md">中文</a> | <a href="/README_pt_BR.md">Português do Brasil</a></p>

<p align="center">
  <img src="https://lvgl.io/assets/images/logo_lvgl.png">
</p>

  <h1 align="center">LVGL - Biblioteca gráfica leve e versátil</h1>
  <br>
<div align="center">
  <img src="https://github.com/kisvegabor/test/raw/master/smartwatch_demo.gif">
  &nbsp;
  <img border="1px" src="https://lvgl.io/assets/images/lvgl_widgets_demo.gif">
</div>
<br>
<p align="center">
  <a href="https://lvgl.io" title="Página inicial do LVGL">Site</a> |
  <a href="https://docs.lvgl.io/" title="Documentação detalhada com +100 exemplos">Documentação</a> |
  <a href="https://forum.lvgl.io" title="Obtenha ajuda e ajude outras pessoas">Fórum</a> |
  <a href="https://lvgl.io/services" title="Design gráfico, implementações e consultoria de serviços">Serviços</a> |
  <a href="https:/lvgl.io/demos" title="Execute demonstrações no seu navegador">Demonstrações</a> |
  <a href="https://squareline.io/" title="Editor web para LVGL">Editor SquareLine Studio</a>
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
  - Suporta monitores monocromáticos, ePaper, OLED ou TFT.
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
  
**Suporte de vinculação e compilação de arquivos**

  - Expondo a API do LVGL com o [Micropython](https://blog.lvgl.io/2019-02-20/micropython-bindings)
  - Nenhum sistema de compilação personalizado é usado. Você pode construir o LVGL enquanto constrói os outros arquivos do seu projeto.
  - O suporte para Make e CMake já vem incluído.
  - Desenvolva no PC e use o mesmo código de interface do usuário em hardwares incorporados (embedded hardware).
  - Converta o código C para um arquivo HTML com o [Emscripten port](https://github.com/lvgl/lv_web_emscripten).

**Documentação, ferramentas e serviços**

  - Documentação detalhada com [+100 exemplos simples](https://docs.lvgl.io/master/index.html)
  - [SquareLine Studio](https://squareline.io) - Um software editor UI profissional e fácil de usar, para acelerar e simplificar o desenvolvimento da interface do usuário.
  - [Serviços](https://lvgl.io/services) como design de UI, implementação e consultoria para tornar o desenvolvimento de UI mais simples e rápido.

## :arrow_forward: Começando
Esta lista irá guiá-lo para começar com o LVGL passo a passo.

**Familiarize-se com o LVGL**

  1. Confira as [demos on-line](https://lvgl.io/demos) para ver o LVGL em ação (~3 minutos)
  2. Leia a página de [introdução](https://docs.lvgl.io/master/intro/index.html) da documentação (~5 minutos)
  3. Familiarize-se com o básico na página de [visão geral rápida](https://docs.lvgl.io/master/get-started/quick-overview.html) (~15 minutos)

**Começando a usar o LVGL**
  
  4. Configure um [simulador](https://docs.lvgl.io/master/get-started/platforms/pc-simulator.html) (~10 minutos)
  5. Experimente alguns [exemplos](https://github.com/lvgl/lvgl/tree/master/examples)
  6. Porte o LVGL para uma placa. Veja o guia [portando o LVGL](https://docs.lvgl.io/master/porting/index.html) ou veja um projeto pronto para usar em [projetos](https://github.com/lvgl?q=lv_port_)

**Torne-se um profissional**

  7. Leia a página [visão geral](https://docs.lvgl.io/master/overview/index.html) para entender melhor a biblioteca (~2-3 horas)
  8. Verifique a documentação dos [widgets](https://docs.lvgl.io/master/widgets/index.html) para ver seus recursos e usabilidade

**Obtenha ajuda e ajude outras pessoas**

  9. Se você tiver dúvidas, acesse o [Fórum](http://forum.lvgl.io)
  10. Leia o guia de [contribuição](https://docs.lvgl.io/master/CONTRIBUTING.html) para ver como você pode ajudar a melhorar o LVGL (~15 minutos)

**E mais**

  11. Baixe e experimente o editor [SquareLine Studio](https://squareline.io).
  12. Entre em contato conosco para [serviços e consultoria](https://lvgl.io/services).

## :package: Pacotes 

LVGL está disponível para:

- [Arduino library](https://docs.lvgl.io/master/get-started/platforms/arduino.html)
- [PlatformIO package](https://registry.platformio.org/libraries/lvgl/lvgl)
- [Zephyr library](https://docs.zephyrproject.org/latest/reference/kconfig/CONFIG_LVGL.html)
- [ESP32 component](https://docs.lvgl.io/master/get-started/platforms/espressif.html)
- [NXP MCUXpresso component](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
- [NuttX library](https://docs.lvgl.io/master/get-started/os/nuttx.html)
- [RT-Thread RTOS](https://docs.lvgl.io/master/get-started/os/rt-thread.html)
- CMSIS-Pack

## :man_technologist: Exemplos

Veja como criar um botão com um evento de clique em C e MicroPython. Para mais exemplos, veja a pasta [examples](https://github.com/lvgl/lvgl/tree/master/examples).

![LVGL button with label example](https://github.com/lvgl/lvgl/raw/master/docs/misc/btn_example.png)

### C
```c
lv_obj_t * btn = lv_btn_create(lv_scr_act());                   /* Adiciona um botão à tela atual */
lv_obj_set_pos(btn, 10, 10);                                    /* Define a posição do botão */
lv_obj_set_size(btn, 100, 50);                                  /* Define o  tamanho do botão */
lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL); /* Atribui um retorno de chamada (callback) ao botão */

lv_obj_t * label = lv_label_create(btn);                        /* Adiciona um rótulo (label) ao botão */
lv_label_set_text(label, "Botão");                              /* Defina o texto do rótulo (label) */
lv_obj_center(label);                                           /* Alinha o texto no centro do botão */
...

void btn_event_cb(lv_event_t * e)
{
  printf("Clicado\n");
}
```
### Micropython
```python
def btn_event_cb(e):
  print("Clicado")

# Cria o botão e o rótulo (label)
btn = lv.btn(lv.scr_act())
btn.set_pos(10, 10)
btn.set_size(100, 50)
btn.add_event_cb(btn_event_cb, lv.EVENT.CLICKED, None)

label = lv.label(btn)
label.set_text("Botão")
label.center()
```

Aprenda mais sobre [Micropython](https://docs.lvgl.io/master/get-started/bindings/micropython.html).

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

Para obter uma descrição detalhada das oportunidades de contribuição, visite a página de [contribuição](https://docs.lvgl.io/master/CONTRIBUTING.html) da documentação.
