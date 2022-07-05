# Pinyin IME

Pinyin IME provides API to provide Chinese Pinyin input method (Chinese input) for keyboard object, which supports 26 key and 9 key input modes. You can think of `lv_ime_pinyin` as a Pinyin input method plug-in for keyboard objects.

Normally, an environment where [lv_keyboard](/widgets/extra/keyboard) can run can also run `lv_ime_pinyin`. There are two main influencing factors: the size of the font file and the size of the dictionary.

<details>
<summary>中文</summary>
<p>
            
`lv_ime_pinyin`为[键盘](/widgets/extra/keyboard)组件提供汉语拼音输入法（中文输入）的功能(后文简称为拼音输入法)，支持26键和9键输入模式。您可以将 `lv_ime_pinyin` 看成是键盘组件的汉语拼音输入法插件。
 
一般情况下，只要是[键盘](/widgets/extra/keyboard)组件能运行的环境 `lv_ime_pinyin` 也能运行。有两个影响因素：字库的大小和词库的大小。
            
</p>
</details>

## Usage

Enable `LV_USE_IME_PINYIN` in `lv_conf.h`.

First use `lv_ime_pinyin_create(lv_scr_act())` to create a Pinyin input method plug-in, then use `lv_ime_pinyin_set_keyboard(pinyin_ime, kb)` to add the `keyboard` you created to the Pinyin input method plug-in.
You can use `lv_ime_pinyin_set_dict(pinyin_ime, your_dict)` to use a custom dictionary (if you don't want to use the built-in dictionary at first, you can disable `LV_IME_PINYIN_USE_DEFAULT_DICT` in `lv_conf.h`, which can save a lot of memory space).

The built-in thesaurus is customized based on the **LV_FONT_SIMSUN_16_CJK** font library, which currently only has more than `1,000` most common CJK radicals, so it is recommended to use custom fonts and thesaurus.

In the process of using the Pinyin input method plug-in, you can change the keyboard and dictionary at any time.

<details>
<summary>中文</summary>
<p>
            
在 `lv_conf.h` 中打开 `LV_USE_IME_PINYIN`。
            
首先，使用 `lv_ime_pinyin_create(lv_scr_act())` 函数创建一个拼音输入法插件，
然后使用 `lv_ime_pinyin_set_keyboard(pinyin_ime, kb)` 函数将您创建的键盘组件添加到插件中。

内置的词库是基于 LVGL 的 **LV_FONT_SIMSUN_16_CJK** 字库定制，这个字库目前只有 `1000` 多个最常见的 CJK 部首，所以建议使用自定义字库和词库。

您可以使用 `lv_ime_pinyin_set_dict(pinyin_ime, your_dict)` 函数来设置使用自定义的词库，如果您一开始就不打算使用内置的词库，建议您在 `lv_conf.h` 中将 `LV_IME_PINYIN_USE_DEFAULT_DICT` 关闭，这可以节省一些内存空间。

</p>
</details>

## Custom dictionary

If you don't want to use the built-in Pinyin dictionary, you can use the custom dictionary.
Or if you think that the built-in phonetic dictionary consumes a lot of memory, you can also use a custom dictionary.

Customizing the dictionary is very simple.

First, set `LV_IME_PINYIN_USE_DEFAULT_DICT` to `0` in `lv_conf.h`

Then, write a dictionary in the following format.

<details>
<summary>中文</summary>
<p>
            
如果您不想使用内置的词库，可以通过下面的方法自定义词库。

自定义词典非常简单。
首先，在 `lv_conf.h` 将 `LV_IME_PINYIN_USE_DEFAULT_DICT` 设置为 0。
然后按照下面的格式编写词库。

</p>
</details>

### Dictionary format

The arrangement order of each pinyin syllable is very important. You need to customize your own thesaurus according to the Hanyu Pinyin syllable table. You can read [here](https://baike.baidu.com/item/%E6%B1%89%E8%AF%AD%E6%8B%BC%E9%9F%B3%E9%9F%B3%E8%8A%82/9167981) to learn about the Hanyu Pinyin syllables and the syllable table.

Then, write your own dictionary according to the following format:

<details>
<summary>中文</summary>
<p>

**注意**，各个拼音音节的排列顺序非常重要，您需要按照汉语拼音音节表定制自己的词库，可以阅读[这里](https://baike.baidu.com/item/%E6%B1%89%E8%AF%AD%E6%8B%BC%E9%9F%B3%E9%9F%B3%E8%8A%82/9167981)了解[汉语拼音音节](https://baike.baidu.com/item/%E6%B1%89%E8%AF%AD%E6%8B%BC%E9%9F%B3%E9%9F%B3%E8%8A%82/9167981)以及[音节表](https://baike.baidu.com/item/%E6%B1%89%E8%AF%AD%E6%8B%BC%E9%9F%B3%E9%9F%B3%E8%8A%82/9167981#1)。

然后，根据下面的格式编写自己的词库：

</p>
</details>

```c
lv_100ask_pinyin_dict_t your_pinyin_dict[] = {
            { "a", "啊阿呵吖" },
            { "ai", "埃挨哎唉哀皑蔼矮碍爱隘癌艾" },
            { "an", "按安暗岸俺案鞍氨胺厂广庵揞犴铵桉谙鹌埯黯" },
            { "ang", "昂肮盎仰" },
            { "ao", "凹敖熬翱袄傲奥懊澳" },
            { "ba", "芭捌叭吧笆八疤巴拔跋靶把坝霸罢爸扒耙" },
            { "bai", "白摆佰败拜柏百稗伯" },
            /* ...... */
            { "zuo", "昨左佐做作坐座撮琢柞"},
            {NULL, NULL}

```

**The last item** must end with `{null, null}` , or it will not work properly.

### Apply new dictionary

After writing a dictionary according to the above dictionary format, you only need to call this function to set up and use your dictionary:

<details>
<summary>中文</summary>
<p>

按照上面的词库格式编写好自己的词库之后，参考下面的用法，调用 `lv_100ask_pinyin_ime_set_dict(pinyin_ime, your_pinyin_dict)` 函数即可设置和使用新词库：

</p>
</details>

```c
    lv_obj_t * pinyin_ime = lv_100ask_pinyin_ime_create(lv_scr_act());
    lv_100ask_pinyin_ime_set_dict(pinyin_ime, your_pinyin_dict);
```

## Input modes

`lv_ime_pinyin` supports 26 key and 9 key input modes. The mode switching is very simple, just call the function `lv_ime_pinyin_set_mode`. If the second parameter of function `lv_ime_pinyin_set_mode` is' 1 ', switch to 26 key input mode; if it is' 0', switch to 9 key input mode, and the default is' 1 '.

<details>
<summary>中文</summary>
<p>

`lv_ime_pinyin` 支持26键和9键输入模式。模式的切换非常简单，只需调用函数 `lv_ime_pinyin_set_mode` 即可。如果函数 `lv_ime_pinyin_set_mode` 的第2个参数为 `1` 则切换到 26 键输入模式，如果为 `0` 则切换到 9 键输入法模式，默认为 `1` 。

</p>
</details>


## Example

```eval_rst

.. include:: ../../examples/others/ime/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_ime_pinyin.h
  :project: lvgl

```
