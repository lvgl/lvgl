# File Explorer

`lv_file_explorer` provides an API to browse the contents of the file system. `lv_file_explorer` only provides the file browsing function, but does not provide the actual file operation function. In other words, you can't click a picture file to open and view the picture like a PC. `lv_file_explorer` will tell you the full path and name of the currently clicked file. The file operation function needs to be implemented by the user.


The file list in `lv_file_explorer` is based on [lv_table](/widgets/table), and the quick access bar is based on [lv_list](/widgets/list). Therefore, care should be taken to ensure that [lv_table](/widgets/table) and [lv_list](/widgets/list) are enabled.

<details>
<summary>中文</summary>
<p>

`lv_file_explorer` 提供API让我们可以浏览文件系统中的内容。`lv_file_explorer` 只提供了文件浏览功能，并不提供实际的文件操作功能，也就是说，不能像PC那样点击一个图片文件就可以打开查看该图片。`lv_file_explorer` 会告诉您当前点击的文件的完整路径和名称，文件操作功能需要用户自己实现。

`lv_file_explorer` 中的文件列表基于 [lv_table](/widgets/table) 实现，快速访问栏基于 [lv_list](/widgets/list) 实现。因此，要注意确保使能了 `lv_table` 和 `lv_list`。


</p>
</details>

## Usage

Enable `LV_USE_FILE_EXPLORER` in `lv_conf.h`.

First use `lv_file_explorer_create(lv_scr_act())` to create a file explorer, The default size is the screen size. After that, you can customize the style like widget.

<details>
<summary>中文</summary>
<p>

在 `lv_conf.h` 中打开 `LV_USE_FILE_EXPLORER`。

首先，使用 `lv_file_explorer_create(lv_scr_act())` 函数创建一个文件浏览器，默认大小为屏幕大小，之后可以像组件那样自定义样式。

</p>
</details>


### Quick access

The quick access bar is optional. You can turn off `LV_FILE_EXPLORER_QUICK_ACCESS` in `lv_conf.h` so that the quick access bar will not be created. This can save some memory, but not much. After the quick access bar is created, it can be hidden by clicking the button at the top left corner of the browsing area, which is very useful for small screen devices.

You can use `lv_file_explorer_set_quick_access_path(file_explorer, LV_FILE_EXPLORER_QA_XX, "path")` to set the path of the quick access bar. The items of the quick access bar are fixed. Currently, there are the following items:

- `LV_FILE_EXPLORER_QA_HOME`
- `LV_FILE_EXPLORER_QA_MUSIC`
- `LV_FILE_EXPLORER_QA_PICTURES`
- `LV_FILE_EXPLORER_QA_VIDEO`
- `LV_FILE_EXPLORER_QA_DOCS`
- `LV_FILE_EXPLORER_QA_MNT`
- `LV_FILE_EXPLORER_QA_FS`

<details>
<summary>中文</summary>
<p>

快速访问栏是可选的，您可以在 `lv_conf.h` 中关闭 `LV_FILE_EXPLORER_QUICK_ACCESS`，这样快速访问栏就不会被创建出来，这能节省一些内存，但并不是很多。快速访问栏被创建出来之后，可以通过点击浏览区域顶部左上角的按钮隐藏起来，这对于小屏幕设备非常有用。

可以通过 `lv_file_explorer_set_quick_access_path(file_explorer, LV_FILE_EXPLORER_QA_XX, "path")` 设置快速访问栏的路径，快速访问栏的项目是固定的，目前有以下项目：

- `LV_FILE_EXPLORER_QA_HOME`
- `LV_FILE_EXPLORER_QA_MUSIC`
- `LV_FILE_EXPLORER_QA_PICTURES`
- `LV_FILE_EXPLORER_QA_VIDEO`
- `LV_FILE_EXPLORER_QA_DOCS`
- `LV_FILE_EXPLORER_QA_MNT`
- `LV_FILE_EXPLORER_QA_FS`

</p>
</details>

### Sort

You can use `lv_file_explorer_set_sort(file_explorer, LV_EXPLORER_SORT_XX)` to set sorting method. There are the following sorting methods:

- `LV_EXPLORER_SORT_NONE`
- `LV_EXPLORER_SORT_KIND`

You can customize the sorting. Before custom sort, please set the default sorting to `LV_EXPLORER_SORT_NONE`. The default is `LV_EXPLORER_SORT_NONE`.

<details>
<summary>中文</summary>
<p>

可以通过 `lv_file_explorer_set_sort(file_explorer, LV_EXPLORER_SORT_XX)` 设置排序方式，有以下排序方式：

- `LV_EXPLORER_SORT_NONE`
- `LV_EXPLORER_SORT_KIND`

您可以自定义排序规则，在这之前请先将排序规则设置为 `LV_EXPLORER_SORT_NONE` 然后在 `LV_EVENT_READY` 事件中处理。默认的排序规则是 `LV_EXPLORER_SORT_NONE`

</p>
</details>

## Event

- `LV_EVENT_READY` sent shen a directory is opened. You can customize the sort.

- `LV_EVENT_VALUE_CHANGED` sent when an item(file) in the file list is clicked.

You can use `lv_file_explorer_get_cur_path` to get the current path and `lv_file_explorer_get_sel_fn` to get the name of the currently selected file in the event processing function. For example:

```c
static void file_explorer_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        char * cur_path =  lv_file_explorer_get_cur_path(obj);
        char * sel_fn = lv_file_explorer_get_sel_fn(obj);
        LV_LOG_USER("%s%s", cur_path, sel_fn);
    }
}
```

You can also save the obtained **path** and **file** name into an array through functions such as *strcpy* and *strcat* for later use.

<details>
<summary>中文</summary>
<p>

- 当打开一个目录后会发送 `LV_EVENT_READY` 事件。您可以在这里自定义排序规则。
- 当文件列表中的项目（文件）被点击时会发送 `LV_EVENT_VALUE_CHANGED` 事件。

可以在事件处理函数中通过 `lv_file_explorer_get_cur_path` 获取当前所在的路径，通过 `lv_file_explorer_get_sel_fn` 获取当前选中的文件的名称。比如：

```c
static void file_explorer_event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    if(code == LV_EVENT_VALUE_CHANGED) {
        char * cur_path =  lv_file_explorer_get_cur_path(obj);
        char * sel_fn = lv_file_explorer_get_sel_fn(obj);
        LV_LOG_USER("%s%s", cur_path, sel_fn);
    }
}
```

您还可以将获取到的 **路径** 和 **文件名称** 通过例如 strcpy 和 strcat 函数保存到一个数组中，方便后续使用。

</p>
</details>

## Example

```eval_rst

.. include:: ../../examples/others/file_explorer/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_file_explorer.h
  :project: lvgl

```
