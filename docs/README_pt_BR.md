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

<h1 align="center">LVGL - Light and Versatile Graphics Library</h1>
<br/>
<h1 align="center">LVGL - Biblioteca gráfica leve e versátil</h1>

<br/>

<div align="center">
  <img src="https://raw.githubusercontent.com/kisvegabor/test/master/smartwatch_demo.gif">
  &nbsp;
  <img border="1px" src="https://lvgl.io/github-assets/widgets-demo.gif">
</div>

<br/>


<p align="center">
  <a href="https://lvgl.io" title="Homepage of LVGL">Site</a> |
  <a href="https://pro.lvgl.io" title="LVGL Pro XML based UI Editor">Editor LVGL Pro</a> |
  <a href="https://docs.lvgl.io/" title="Detailed documentation with 100+ examples">Documentação</a> |
  <a href="https://forum.lvgl.io" title="Get help and help others">Fórum</a> |
  <a href="https://lvgl.io/demos" title="Demos running in your browser">Demos</a> |
  <a href="https://lvgl.io/services" title="Graphics design, UI implementation and consulting">Serviços</a>
</p>

<br/>

## 📒 Visão geral

**LVGL** é uma biblioteca de UI gratuita e de código aberto que permite criar interfaces gráficas
para qualquer MCU e MPU, de qualquer fabricante, em qualquer plataforma.

**Requisitos**: o LVGL não possui dependências externas, o que facilita compilar para qualquer alvo moderno,
desde pequenos MCUs até MPUs Linux multicore com suporte a 3D. Para uma UI simples, você precisa de ~100 kB de RAM,
~200–300 kB de flash e um buffer de renderização com 1/10 do tamanho da tela.

**Para começar**, escolha um projeto pronto para uso no VSCode, Eclipse ou outro, e experimente o LVGL
no seu PC. O código de UI do LVGL é totalmente independente de plataforma, então você pode reutilizá-lo
nos seus alvos embarcados.

**LVGL Pro** é um kit completo para ajudar você a criar, testar, compartilhar e entregar UIs com mais rapidez.
Ele inclui um Editor XML onde você pode criar e testar componentes reutilizáveis,
exportar código C ou carregar os XMLs em tempo de execução. Saiba mais aqui.

## 💡 Recursos

**Gratuito e portátil**
  - Biblioteca totalmente em C (compatível com C++) sem dependências externas.
  - Pode ser compilada para qualquer MCU ou MPU, com qualquer (RT)OS. Make, CMake e globbing simples são suportados.
  - Suporta displays monocromáticos, ePaper, OLED, TFT e até monitores. [Displays](https://docs.lvgl.io/master/details/main-modules/display/index.html)
  - Licença MIT, o que facilita o uso em projetos comerciais.
  - Precisa de apenas 32 kB de RAM, 128 kB de Flash, um frame buffer e pelo menos um buffer de 1/10 da tela para renderização.
  - Sistema operacional, memória externa e GPU são suportados, mas não obrigatórios.

**Widgets, estilos, layouts e mais**
  - 30+ [Widgets](https://docs.lvgl.io/master/details/widgets/index.html) embutidos: Button, Label, Slider, Chart, Keyboard, Meter, Arc, Table e muitos outros.
  - [Sistema de estilos](https://docs.lvgl.io/master/details/common-widget-features/styles/index.html) flexível com ~100 propriedades para personalizar qualquer parte dos widgets em qualquer estado.
  - Motores de layout tipo [Flexbox](https://docs.lvgl.io/master/details/common-widget-features/layouts/flex.html) e [Grid](https://docs.lvgl.io/master/details/common-widget-features/layouts/grid.html) para dimensionar e posicionar widgets de forma responsiva.
  - Texto renderizado com codificação UTF-8, com suporte a CJK, tailandês, híndi, árabe e persa.
  - [Data bindings](https://docs.lvgl.io/master/details/auxiliary-modules/observer/index.html) para conectar facilmente a UI ao aplicativo.
  - Motor de renderização com suporte a animações, antialiasing, opacidade, rolagem suave, sombras, transformação de imagens e mais.
  - [Poderoso motor de renderização 3D](https://docs.lvgl.io/master/details/libs/gltf.html) para exibir [modelos glTF](https://sketchfab.com/) com OpenGL.
  - Suporte a mouse, touchpad, keypad, teclado, botões externos e encoder. [Dispositivos de entrada](https://docs.lvgl.io/master/details/main-modules/indev.html)
  - Suporte a [múltiplos displays](https://docs.lvgl.io/master/details/main-modules/display/overview.html#how-many-displays-can-lvgl-use).

## 📦️ Plataformas compatíveis

O LVGL não possui dependências externas, por isso é fácil de compilar para qualquer dispositivo e também está disponível em vários gerenciadores de pacotes e RTOS:

- [Biblioteca Arduino](https://docs.lvgl.io/master/details/integration/framework/arduino.html)
- [Pacote PlatformIO](https://registry.platformio.org/libraries/lvgl/lvgl)
- [Biblioteca Zephyr](https://docs.lvgl.io/master/details/integration/os/zephyr.html)
- [Componente ESP-IDF (ESP32)](https://components.espressif.com/components/lvgl/lvgl)
- [Componente NXP MCUXpresso](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
- [Biblioteca NuttX](https://docs.lvgl.io/master/details/integration/os/nuttx.html)
- [RT-Thread RTOS](https://docs.lvgl.io/master/details/integration/os/rt-thread.html)
- CMSIS-Pack
- [Pacote RIOT OS](https://doc.riot-os.org/group__pkg__lvgl.html#details)

## 🚀 Editor LVGL Pro

O LVGL Pro é um conjunto completo para criar, testar, compartilhar e entregar UIs embarcadas com eficiência.

Ele é composto por quatro ferramentas integradas:

1. **Editor XML**: o coração do LVGL Pro. Um app de desktop para criar componentes e telas em XML, gerenciar data bindings, traduções, animações, testes e mais. Saiba mais sobre o [formato XML](https://docs.lvgl.io/master/details/xml/xml/index.html) e o [Editor](https://docs.lvgl.io/master/details/xml/editor/index.html).
2. **Visualizador online**: execute o Editor no navegador, abra projetos do GitHub e compartilhe facilmente sem configurar um ambiente de desenvolvimento. Visite [https://viewer.lvgl.io](https://viewer.lvgl.io).
3. **Ferramenta CLI**: gere código C e rode testes no CI/CD. Veja os detalhes [aqui](https://docs.lvgl.io/master/details/xml/tools/cli.html).
4. **Plugin Figma**: sincronize e extraia estilos diretamente do Figma. Veja como funciona [aqui](https://docs.lvgl.io/master/details/xml/tools/figma.html).

Juntas, essas ferramentas permitem criar UIs com eficiência, testá-las com confiabilidade e colaborar com colegas e clientes.

Saiba mais em https://pro.lvgl.io

## 🤝 Serviços comerciais

A LVGL LLC oferece vários tipos de serviços comerciais para apoiar o seu desenvolvimento de UI. Com mais de 15 anos de experiência em interfaces de usuário e gráficos, ajudamos você a levar sua UI para o próximo nível.

- **Design gráfico**: nossos designers internos são especialistas em criar visuais modernos e bonitos que combinam com seu produto e com as capacidades do hardware.
- **Implementação de UI**: implementamos sua UI com base no design criado por você ou por nós. Vamos tirar o máximo do seu hardware e do LVGL. Se faltar algum recurso ou widget no LVGL, nós implementamos para você.
- **Consultoria e suporte**: oferecemos consultoria para evitar erros caros e demorados durante o desenvolvimento da UI.
- **Certificação de placas**: para empresas que oferecem placas de desenvolvimento ou kits prontos para produção, fornecemos certificação demonstrando como a placa roda o LVGL.

Veja nossos [Demos](https://lvgl.io/demos) como referência. Para mais informações, acesse a [página de Serviços](https://lvgl.io/services).

[Entre em contato](https://lvgl.io/#contact) e conte como podemos ajudar.

## 🧑‍💻 Integração do LVGL

Integrar o LVGL é simples. Basta adicioná-lo ao projeto e compilar como você compila outros arquivos.
Para configurar, copie `lv_conf_template.h` para `lv_conf.h`, habilite o primeiro `#if 0` e ajuste as configs conforme necessário.
(A configuração padrão geralmente é suficiente.) Quando disponível, o LVGL também pode ser usado com Kconfig.

Depois de incluído no projeto, você pode inicializar o LVGL e criar dispositivos de display e de entrada assim:

```c
#include "lvgl/lvgl.h" /*Defina LV_LVGL_H_INCLUDE_SIMPLE para incluir como "lvgl.h"*/

#define TFT_HOR_RES 320
#define TFT_VER_RES 240

static uint32_t my_tick_cb(void)
{
    return my_get_millisec();
}

static void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    /*Escreva px_map na área (x1..x2, y1..y2) do frame buffer ou do controlador externo*/
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

    /*Inicialize o LVGL*/
    lv_init();

    /*Defina a fonte de tick em milissegundos para o LVGL controlar o tempo*/
    lv_tick_set_cb(my_tick_cb);

    /*Crie um display onde telas e widgets serão adicionados*/
    lv_display_t * display = lv_display_create(TFT_HOR_RES, TFT_VER_RES);

    /*Adicione buffers de renderização
     *Aqui adicionamos um buffer parcial menor assumindo 16 bits (formato RGB565)*/
    static uint8_t buf[TFT_HOR_RES * TFT_VER_RES / 10 * 2]; /* x2 por causa de 16 bits por pixel */
    lv_display_set_buffers(display, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    /*Adicione o callback que faz o flush do conteúdo de `buf` quando renderizado*/
    lv_display_set_flush_cb(display, my_flush_cb);

    /*Crie um dispositivo de entrada para toque*/
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touch_read_cb);

    /*Drivers prontos, agora crie a UI*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_center(label);

    /*Execute as tarefas do LVGL em loop*/
    while(1) {
        lv_timer_handler();
        my_sleep_ms(5);         /*Espere um pouco para o sistema respirar*/
    }
}
```

## 🤖 Exemplos

Você pode conferir mais de 100 exemplos em https://docs.lvgl.io/master/examples.html

O Visualizador Online também contém tutoriais para aprender XML com facilidade: https://viewer.lvgl.io/


### Botão Hello World com evento

<img width="311" height="232" alt="image" src="https://github.com/user-attachments/assets/5948b485-e3f7-4a63-bb21-984381417c4a" />

<details>
  <summary>C code</summary>

  ```c
static void button_clicked_cb(lv_event_t * e)
{
	printf("Clicked\n");
}

[...]

  lv_obj_t * button = lv_button_create(lv_screen_active());
  lv_obj_center(button);
  lv_obj_add_event_cb(button, button_clicked_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t * label = lv_label_create(button);
  lv_label_set_text(label, "Hello from LVGL!");
```
</details>

<details>
  <summary>Em XML com o LVGL Pro</summary>

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

</details>

### Slider estilizado com data binding

<img width="314" height="233" alt="image" src="https://github.com/user-attachments/assets/268db1a0-946c-42e2-aee4-9550bdf5f4f9" />

<details>
  <summary>C code</summary>

```c
static void my_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
	printf("Slider value: %d\n", lv_subject_get_int(subject));
}

[...]

static lv_subject_t subject_value;
lv_subject_init_int(&subject_value, 35);
lv_subject_add_observer(&subject_value, my_observer_cb, NULL);

lv_style_t style_base;
lv_style_init(&style_base);
lv_style_set_bg_color(&style_base, lv_color_hex(0xff8800));
lv_style_set_bg_opa(&style_base, 255);
lv_style_set_radius(&style_base, 4);

lv_obj_t * slider = lv_slider_create(lv_screen_active());
lv_obj_center(slider);
lv_obj_set_size(slider, lv_pct(80), 16);
lv_obj_add_style(slider, &style_base, LV_PART_INDICATOR);
lv_obj_add_style(slider, &style_base, LV_PART_KNOB);
lv_obj_add_style(slider, &style_base, 0);
lv_obj_set_style_bg_opa(slider, LV_OPA_50, 0);
lv_obj_set_style_border_width(slider, 3, LV_PART_KNOB);
lv_obj_set_style_border_color(slider, lv_color_hex3(0xfff), LV_PART_KNOB);
lv_slider_bind_value(slider, &subject_value);

lv_obj_t * label = lv_label_create(lv_screen_active());
lv_obj_align(label, LV_ALIGN_CENTER, 0, -30);
lv_label_bind_text(label, &subject_value, "Temperature: %d °C");
```

</details>

<details>
  <summary>Em XML com o LVGL Pro</summary>

```xml
<screen>
	<styles>
		<style name="style_base" bg_opa="100%" bg_color="0xff8800" radius="4" />
		<style name="style_border" border_color="0xfff" border_width="3" />
	</styles>

	<view>
		<lv_label bind_text="value" bind_text-fmt="Temperature: %d °C" align="center" y="-30" />
		<lv_slider align="center" bind_value="value" style_bg_opa="30%">
			<style name="style_base" />
			<style name="style_base" selector="knob" />
			<style name="style_base" selector="indicator" />
			<style name="style_border" selector="knob" />
		</lv_slider>
	</view>
</screen>
```

</details>

### Checkboxes em um layout

<img width="311" height="231" alt="image" src="https://github.com/user-attachments/assets/ba9af647-2ea1-4bc8-b53d-c7b43ce24b6e" />

<details>
  <summary>C code</summary>

  ```c
/*Crie uma nova tela e carregue-a*/
lv_obj_t * scr = lv_obj_create(NULL);
lv_screen_load(scr);

/*Defina um layout em coluna*/
lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, /*Alinhamento vertical*/
						   LV_FLEX_ALIGN_START,	       /*Alinhamento horizontal na trilha*/
						   LV_FLEX_ALIGN_CENTER);      /*Alinhamento da trilha*/

/*Crie 5 checkboxes*/
const char * texts[5] = {"Input 1", "Input 2", "Input 3", "Output 1", "Output 2"};
for(int i = 0; i < 5; i++) {
	lv_obj_t * cb = lv_checkbox_create(scr);
	lv_checkbox_set_text(cb, texts[i]);
}

/*Altere alguns estados*/
lv_obj_add_state(lv_obj_get_child(scr, 1), LV_STATE_CHECKED);
lv_obj_add_state(lv_obj_get_child(scr, 3), LV_STATE_DISABLED);
```

</details>

<details>
  <summary>Em XML com o LVGL Pro</summary>

```xml
<screen>
	<view
		flex_flow="column"
		style_flex_main_place="space_evenly"
		style_flex_cross_place="start"
		style_flex_track_place="center"
	>
		<lv_checkbox text="Input 1"/>
		<lv_checkbox text="Input 2"/>
		<lv_checkbox text="Input 3" checked="true"/>
		<lv_checkbox text="Output 1"/>
		<lv_checkbox text="Output 2" disabled="true"/>
   </view>
</screen>
```

</details>


## 🌟 Como contribuir

O LVGL é um projeto aberto, e contribuições são muito bem-vindas. Há muitas formas de contribuir, desde falar sobre o seu projeto, escrever exemplos, melhorar a documentação, corrigir bugs, até hospedar seu próprio projeto na organização LVGL.

Para uma descrição detalhada das oportunidades de contribuição, visite a seção [Contributing](https://docs.lvgl.io/master/contributing/index.html)
da documentação.

Mais de 600 pessoas já deixaram sua marca no LVGL. Junte-se a nós. Até breve 🙂

<a href="https://github.com/lvgl/lvgl/graphs/contributors"> <img src="https://contrib.rocks/image?repo=lvgl/lvgl&max=48" /> </a>

... e muitas outras.

