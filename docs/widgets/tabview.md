```eval_rst
.. include:: /header.rst 
:github_url: |github_link_base|/widgets/tabview.md
```

# Tabview (lv_tabview)

## Overview

The Tab view object can be used to organize content in tabs.

## Parts and Styles

The Tab view object has several parts. The main is `LV_TABVIEW_PART_BG`. It a rectangle-like container which holds the other parts of the Tab view.

On the background 2 important real parts are created:

- `LV_TABVIEW_PART_BG_SCRL`: it's the scrollable part of [Page](/widgets/page). It holds the content of the tabs next to each other. The background of the Page is always transparent and can't be accessed externally.
- `LV_TABVIEW_PART_TAB_BG`: The tab buttons which is a [Button matrix](/widgets/btnmatrix).
Clicking on a button will scroll `LV_TABVIEW_PART_BG_SCRL` to the related tab's content. The tab buttons can be accessed via `LV_TABVIEW_PART_TAB_BTN`. When tabs are selected, the buttons are in the checked state, and can be styled using `LV_STATE_CHECKED`.
The height of the tab's button matrix is calculated from the font height plus padding of the background's and the button's style. 

All the listed parts supports the typical background style properties and padding.

`LV_TABVIEW_PART_TAB_BG` has an additional real part, an indicator, called `LV_TABVIEW_PART_INDIC`.
It's a thin rectangle-like object under the currently selected tab. When the tab view is animated to an other tab the indicator will be animated too.
It can be styles using the typical background style properties. The *size* style property will set the its thickness.

When a new tab is added a [Page](/widgets/page) is create for them on `LV_TABVIEW_PART_BG_SCRL` and a new button is added to `LV_TABVIEW_PART_TAB_BG` Button matrix.
The created Pages can be used as normal Pages and they have the usual Page parts.

## Usage

### Adding tab

New tabs can be added with `lv_tabview_add_tab(tabview, "Tab name")`. It will return with a pointer to a [Page](/widgets/page) object where the tab's content can be created.

### Change tab

To select a new tab you can:

- Click on it on the Button matrix part
- Slide
- Use `lv_tabview_set_tab_act(tabview, id, LV_ANIM_ON/OFF)` function

### Change tab's name

To change the name (shown text of the underlying button matrix) of tab `id` during runtime the function `lv_tabview_set_tab_name(tabview, id, name)` can be used.

### Tab button's position

By default, the tab selector buttons are placed on the top of the Tab view. It can be changed with `lv_tabview_set_btns_pos(tabview, LV_TABVIEW_TAB_POS_TOP/BOTTOM/LEFT/RIGHT/NONE)`

`LV_TABVIEW_TAB_POS_NONE` will hide the tabs.

Note that, you can't change the tab position from top or bottom to left or right when tabs are already added.

### Animation time

The animation time is adjusted by `lv_tabview_set_anim_time(tabview, anim_time_ms)`. It is used when the new tab is loaded.

### Scroll propagation

As the tabs' content object is a Page it can receive scroll propagation from an other Page-like object.
For example, if a text area is created on the tab's content and that Text area is scrolled but it reached the end the scroll can be propagated to the content Page.
It can be enabled with `lv_page/textarea_set_scroll_propagation(obj, true)`.

By default the tab's content Pages have enabled scroll propagation, therefore when they are scrolled horizontally the scroll is propagated to `LV_TABVIEW_PART_BG_SCRL` and this way the Pages will be scrolled.

The manual sliding can be disabled with `lv_page_set_scroll_propagation(tab_page, false)`.

## Events

Besides the [Generic events](../overview/event.html#generic-events) the following [Special events](../overview/event.html#special-events) are sent by the Slider:

- **LV_EVENT_VALUE_CHANGED** Sent when a new tab is selected by sliding or clicking the tab button

Learn more about [Events](/overview/event).

## Keys

The following *Keys* are processed by the Tabview:

- **LV_KEY_RIGHT/LEFT** Select a tab
- **LV_KEY_ENTER** Change to the selected tab

Learn more about [Keys](/overview/indev).

## Example

```eval_rst

.. include:: /lv_examples/src/lv_ex_widgets/lv_ex_tabview/index.rst

```

## API

```eval_rst

.. doxygenfile:: lv_tabview.h
  :project: lvgl

```
