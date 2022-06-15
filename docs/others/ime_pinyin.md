# Pinyin IME

Pinyin IME provides API to provide Pinyin input method (Chinese input) for keyboard object. You can think of `lv_ime_pinyin` as a Pinyin input method plug-in for keyboard objects.

Normally, an environment where [lv_keyboard](/widgets/extra/keyboard) can run can also run `lv_ime_pinyin`. There are two main influencing factors: the size of the font file and the size of the dictionary.

## Usage

Enable `LV_USE_IME_PINYIN` in `lv_conf.h`.

First use `lv_ime_pinyin_create(lv_scr_act())` to create a Pinyin input method plug-in, then use `lv_ime_pinyin_set_keyboard(pinyin_ime, kb)` to add the `keyboard` you created to the Pinyin input method plug-in.
You can use `lv_ime_pinyin_set_dict(pinyin_ime, your_dict)` to use a custom dictionary (if you don't want to use the built-in dictionary at first, you can disable `LV_IME_PINYIN_USE_DEFAULT_DICT` in `lv_conf.h`, which can save a lot of memory space).

In the process of using the Pinyin input method plug-in, you can change the keyboard and dictionary at any time.

## Custom dictionary

If you don't want to use the built-in Pinyin dictionary, you can use the custom dictionary.
Or if you think that the built-in phonetic dictionary consumes a lot of memory, you can also use a custom dictionary.

Customizing the dictionary is very simple.

First, set `LV_IME_PINYIN_USE_DEFAULT_DICT` to `0` in `lv_lib_100ask_conf.h`

Then, write a dictionary in the following format.

### Dictionary format

Write your own dictionary according to the following format:

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

```c
    lv_obj_t * pinyin_ime = lv_100ask_pinyin_ime_create(lv_scr_act());
    lv_100ask_pinyin_ime_set_dict(pinyin_ime, your_pinyin_dict);
```

## Example

```eval_rst

.. include:: ../../examples/others/ime/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_ime_pinyin.h
  :project: lvgl

```
