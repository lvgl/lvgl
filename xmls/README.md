# XML Format

## Overview

### Goal

1. Describe widgets in a modular, reusable, and component-oriented way so that they can be embedded into each other.
2. Allow generating easily extendable, developer-friendly code.
3. Ensure that the XML files support describing the view without including the logic (similar to HTML).
4. Create a preview to show the UIs described in an XML file in real-time, without recompiling.
5. Provide an amazing user experience with autocompletion, tooltips, and help messages.
6. Allow compiling the custom widget with custom code into the preview to check them with all the logic in place.
7. Make the widgets testable and previewable in various configurations.
8. Prepare for adding docs/help from the beginning, even in multiple languages.

It's **not the goal** to create a no-code tool. It is assumed that the user is a developer and can write code.
The XML description should help with defining the view (and providing a preview) to allow fast iterations on the design.

It's **not the goal** to allow creating the whole UI with XML either. It's acceptable to create visually heavy parts here and use the widget's API to dynamically create the more complex cases from code. It's also acceptable to define callbacks in code.

### Design Considerations

1. Use XML in an HTML-like way because it's already proven that it works for both small and large UIs, and people are familiar with it.
2. The XML format should serve C, and not the other way around. That is, give freedom in the C API.
3. Focus on the view and do not develop a custom declarative language for the logic.
4. Prefer approaches for which a straightforward and standard XSD (XML Schema) can be created. This allows supporting autocompletion and type validation, even in external XML editors.

### Interacting with the XML

#### APIs

The XML files are used to create widgets with or without APIs.
Widgets can be nested, and for higher-level widgets, new (simpler) APIs can be defined.
Due to this, when custom widgets are embedded into another widget, they can be accessed via a target and simple API.

Eventually, most of the visual tasks can be realized directly in the XML files,
but the logic of the UI should be described from code.

What does the "logic" mean? For example:
- What should happen when a button is clicked? Move to a new screen? Create a widget? Increment a value?
- Read sensor data and show it on widgets.
- Complex logic, such as adding a new series to a chart or setting a cell's value in a table.

These tasks are where the child widgets are not used via their API, but require custom drawing and data management. These are outside of the scope of XML and preview and code should be written for these.

New widgets described in XMLs can be used in two ways:
1. Export code, add the content of the setter API function (if there is a new API),
compile the widget with the application and use its API from the application
2. Make LVGL load the XML file at runtime. (Only if there is no new API defined)

#### Exporting Code

For each widget, the following files are exported:
- `<widget_name>_gen.h`: Contains the generated API implementation of the widget.
- `<widget_name>_private_gen.h`: Contains private API and data for the widget.
- `<widget_name>_gen.c`: Contains the generated API and internals of the widget.
- `<widget_name>.h`: Includes `<widget_name>_gen.h` and allows the user to define custom APIs.
- `<widget_name>.c`: Contains hooks from `<widget_name>_gen.c` and allows the user to write custom code.
- `<widget_name>_xml_parser.c`: Processes the XML strings and calls the required functions according to the set attributes.

#### Workflow

The simpler workflow is when only the built-in widgets are used to describe how a screen or another widget looks without adding a specific API.
The workflow for this is the following:
1. Describe the `view` and `styles` of a widget or screen using the existing widgets.
It can be previewed instantly as the `view` is edited.
2. Use this component or screen in one of the following ways:
   1. Export C code for it, compile it as part of the application, and use it like`my_custom_widget_create(parent);`.
   2. Use the XML description directly and let LVGL process/create it at runtime (`lv_xml_load(parent, my_custom_widget_xml_data);`).

Using the built-in widgets of LVGL to create a new UI is important, but it's only part of the concept. The user should be able to add custom widgets to the editor as well. These custom widgets are very similar to LVGL's built-in widgets in terms of:
- They also have their own specific API to hide the internals.
- They are described as a class.
- Any number of instances can be created.
- They can be as complex as any built-in LVGL widget (e.g. chart, table, span, etc)

As an example, we will use a `sliderbox` widget which consists of a slider, a `+` button, a `-` button, and a title. The `sliderbox` has an API to set its `range`, `value`, and `title`. The workflow to add such a new built-in widget is the following:

1. Describe the `view` of the `sliderbox` using the built-in widgets, i.e., add a slider, two buttons, and a label, arrange and style these widgets. It can be previewed instantly as the `view` is edited.
2. Describe the `api` as well (properties, enums, etc.).
3. Export C code for the `sliderbox` (a new class, setter functions based on the API, and a skeleton for parsing XML for the widget are created automatically).
4. Write the new widget's XML parser (helpers are available to simplify this) and the logic in the API functions (in XML parsing, call the corresponding setter functions of the `sliderbox` for an XML like `<sliderbox range="10 100" value="30" title="Hello"/>`).
5. Compile the C files of the `sliderbox` back into the editor. From this point, the editor will know about this new widget, process its API, and can handle it when it's embedded it into other widgets.
6. Write tests for the new widget to see how it behaves after different API calls.
7. Use the `sliderbox` in a new widget. Since it is already compiled into the editor, it will work the same way as any other built-in widget.

### Notions and Conventions

The standard XML format is followed: `<tag attribute="value"></tag>` or, if there are no children, `<tag attribute="value"/>`.

Tag and attribute names must start with a letter and cannot have spaces or special characters, except:
- `_`: Used to replace spaces.
- `-`: Cannot be used in user-defined names, but is used in automatically created compound names for separation, e.g., `my_chart-my_series`. This avoids naming conflicts with user-defined names.

These are valid names:
- `my_slider`
- `slider2`
- `SuperSlider2000`
- `value2_min`

The following special syntaxes are used:
- `!`: References constants. For example, `value="!const1"`.

### High-level example

The interfaces of a widget are described in an `<api>` tag.
The properties, elements (internal widgets), and enums of a given widget are defined here.

In the `<styles>` section, styles and their properties can be defined.

Widgets can be created, and their properties can be set, within a `<view>` tag.

An example of a custom `sliderbox` widget:

```xml
<!-- sliderbox.xml -->
<api>
	<enumdef name="mode">
		<enum name="normal" help="Normal operation"/>
		<enum name="inverted" help="Inverted operation"/>
	</enumdef>

	<prop name="range" help="The min and max range">
		<param name="min_range" type="int" help="The minimum value"/>
		<param name="max_range" type="int" help="The maximum value"/>
	</prop>
	<prop name="value" help="The current value">
		<param name="value" type="int" help="The current value to set as an integer"/>
	</prop>
	<prop name="title" help="The title">
		<param name="text" type="string" help="The title as a string"/>
	</prop>
</api>

<styles>
	<style name="main" bg_color="0xff0000" flex_flow="column"/>
	<style name="blue" bg_color="0x0000ff"/>
</styles>

<view extends="obj" width="300px" height="size_content" styles="main blue-pressed">
	<label text="Placeholder"/>
	<slider width="100%" range="-100 100"/>
	<button>
		<label text="Apply"/>
	</button>
</view>
```

This is how it looks when the slider box is used, for example, in a list:

```xml
<view>
	<sliderbox value="40" mode="normal" title="First"/>
	<sliderbox value="20" mode="inverted" title="Second"/>
</view>
```

## XML Overview

### Types
The following built-in types are supported:
- `int`: Normal number in the range of -2M to 2M by default.
- `px`: Simple pixel unit. The unit "px" can be omitted.
- `%`: Percentage. `%` must be appended as a unit.
- `content`: Means `LV_SIZE_CONTENT`.
- `subject`: A subject.
- `string`: Simple `\0` terminated string. When multiple strings are used in a property or string array `'`s should be used. E.g. `foo="'a' 'b'"`
- `color`: A color stored as 24-bit RGB.
- `opa`: Opacity value in the range of 0 to 255 or 0 to 100%.
- `grad`: A complex gradient descriptor.
- `obj`: Pointer to a widget.
- `event_cb`: An event callback
- `animation`: An animation descriptor.
- `font`: A font from the asset manager.
- `image_src`: An image from the asset manager.
- `style`: The name of a style.
- `bool`: `true` or `false`.
- `point`: A point with `x` and `y` values.
- `arglist`: Just list all the parameters as arguments. Supports only `int`s and `string`s. E.g. `foo="1 23 'hello' 'a'"`
- `type`: A type from this list.

Any type can be an array in four ways:
- `int[N]`: An integer array with `N` elements, and the count is passed as the next parameter.
- `string[...NULL]`: An array terminated with `NULL`. `NULL` can be replaced by any value, e.g `grid_template_last`
- `string[5]`: An array that must have 5 elements.
- `string[]`: No `NULL` termination and no count parameter either.

Types can be compound, meaning multiple options/types are possible. For example, for width: `type="px|%|content"`.

`<enumdef>` can be used to create custom enums.
When used as a type, a `+` suffix means multiple values can be selected and ORed. For example: `type="axis+"`.
It's also possible to limit the possible options the user can select from an enum. For example: `type="dir(top bottom)"`

### API and View

The following elements can be defined in the `<api>` section and used later in the `<view>`.

#### `<enumdef>`

Used to define new enum types for the given widget.

It should contain `<enum>` fields to define the possible options.

Example:
```xml
<!-- my_widget.xml -->
<api>
	<enumdef name="mode" help="Possible modes" help-zh="Chinese help">
		<enum name="normal" help="Normal mode" help-zh="Normal mode in Chinese" value="0x10"/>
		<enum name="inverted" help="Inverted mode"/>
	</enumdef>

	<prop name="prop1" help="help">
		<param name="operation_mode" type="mode" help="help"/>
	</prop>
</api>
```

The following code can be generated from the definition above:
```c
/**
  * Possible modes
  *
  * Chinese help
  */
typedef enum {
	/**
	  * Normal mode.
	  *
	  * Normal mode in Chinese.
	  */
	LV_MY_WIDGET_MODE_NORMAL = 0x10,

	/**
	  * Inverted mode.
	  */
	LV_MY_WIDGET_MODE_INVERTED
} lv_my_widget_mode_t;
```

This is how it can be used in a <view>:
```xml
<!-- complex_widget.xml -->
<view>
	<my_widget mode="normal"/>
</view>
```

#### `<arg>` and `<prop>`

Right inside the `<api>` tag `<arg>` tags can be defined.
Bedides the parent widget these will be passed as arguments to the create function.

Inside `<prop>`s, `<param>` elements must be defined to describe the arguments.
All properties are optional. If a property is not set, simply it won't be applied and the created widget's default value will be applied (e.g. `"text"` for label text) or the defined default value will be set as described below..

All `<arg>`, `<prop>`, and `<param>` can have default values, however, their meanings differ for each tag:
- `<arg default="10">`: If the argument is not defined, the default value will be passed as a parameter. If the default attribute is not added, the argument is mandatory.
- `<prop default="10">`: When the widget is created, the related setter function will be called with the given default value. If the default attribute is omitted, no specific default value will be applied.
- `<param default="10">`: The last parameters can be omitted if the default value is set. This is not useful if there is only a single parameter for a property, but it can be helpful for compound properties.

For example:
```xml
<api>
	<arg name="main_color" type="color" help="Sets the color of the slider." default="0xff0"/>
	<prop name="range" default="0 100" help="Set the range." help-zh="Chinese set range.">
		<param name="range_min" type="int" help="Sets the minimum value." help-zh="Translation for range_min."/>
		<param name="range_max" type="int" default="100" help="Sets the maximum value."/>
	</prop>
</api>
```

When a property is used, all parameters are set as a single attribute value. For example:

```xml
<slider range="-100 100"/>
```

Each property will correspond to a setter function.
By default, `name` in `<prop>` is used to build the name of the setter function like this:
```c
<widget_name>_set_<prop_name>(lv_obj_t * obj, <param1_type> <param1_name>, <param2_type> <param2_name>, ...);
```

For the sake of flexibility, an `alias` attribute can be defined in `<prop>` to explicitly set the name of the setter function. For example: <prop alias="custom_range_set">.

`<prop>`s have an optional `<postponed>` `bool` attribute.
By default it's `false` but if it's set to `true` the give property will be applied after all the children are created.
A practical example is that for example the current tab of the tabview can not be set before the tabs are created.

The following code can be generated in a header file:
```h
/**
 * Create my_widget
 * @param parent  	 	The parent of my_widget
 * @param main_color  	Sets the color of the slider.
 * @return 				Pointer to the created widget
 */
void lv_my_widget_set_range(lv_obj_t * obj, lv_color_t main_color);

/**
 * Set the range.
 *
 * Chinese set range.
 *
 * @param range_min  Sets the minimum value.
 *                   Translation for range_min.
 * @param range_max  Sets the maximum value.
 */
void lv_my_widget_set_range(lv_obj_t * obj, int32_t range_min, int32_t range_max);
```

When this widget is used in a <view> as a child, it looks like this:
```xml
<!-- complex_widget.xml -->
<view>
	<my_widget name="obj1" range="100 200">
		<label text="Label text"/>
		... other children here as needed ...
	</my_widget>
</view>
```

#### `<element>`

Elements are used to describe sub-widgets or internal parts of widgets. Examples include the list of a dropdown, the tabs of a tabview, or the series of a chart.

Elements can also have `<arg>` and `<prop>` definitions. The default values work the same way as they do for normal widget APIs.

An element in a `<view>` can be referenced like this: `<widget_name-element_name>`. The widget name and the element name are separated by a `-`, so `-` is not allowed in widget and element names (only `_` can be used).

Example:
```xml
<my_chart-super_series color="0xff0000"/>
```
An important attribute of elements is `access`. The possible values are:

##### `access="add"`

The element is created explicitly with an `add` function, e.g., `lv_tabview_add_tab(obj, "Title");`.
The `add` function can return a custom ata type if its type if `type="custom:my_data"`. In these case no children can be added.

`<arg>` elements defined directly inside the `<element>` are passed to the add function as arguments.

It can be used like this:
```xml
<!-- my_widget.xml -->
<api>
	<element name="indicator" type="obj" help="The indicator of my_widget" access="add">
		<arg name="color" type="color" help="Help for color"/>
		<arg name="max_value" type="int" help="Help for max_value"/>
		<prop name="value" help="Set a new value for the indicator">
			<param name="value" type="int" help="Help for value"/>
		</prop>
	</element>
</api>

<view>

</view>

<view element="indicator" parent="name_from_the_normal_view">
	...describe the element's view
</view>

```
This is the generated header file:
```c
/**
 * Create the indicator of my_widget.
 * @param parent     Pointer to a my_widget object the indicator should be added to
 * @param color      Help for color
 * @param max_value  Help for max_value
 * @return           Pointer to the created indicator
 */
lv_obj_t * lv_my_widget_add_indicator(lv_obj_t * parent, lv_color_t color, int32_t max_value);

/**
 * Set a new value for the indicator.
 * @param value      Help for value
 */
void lv_my_widget_set_indicator_value(lv_obj_t * obj, int32_t value);
```
In a view:

```xml
<!-- complex_widget.xml -->
<view>
	<my_widget width="100px">
		<my_widget-indicator name="indic1" color="0xff0000" max_value="120" value="30"/>
	</my_widget>
</view>
```
And this is the related C file where the indicator is created:
```c
lv_obj_t * indic1 = lv_my_widget_add_indicator(parent, color, max_value);
lv_my_widget_set_indicator_value(indic1, value);
```

##### `access="get"`

The element is created automatically and can be retrieved with a function like `lv_dropdown_get_list(obj);`.

`<arg>` elements defined directly inside the `<element>` are passed to the `get` function as arguments.

As these `get` functions can be any complex and can return a widget or custom data in any custom ways
it's not possible to describe them in the view. They require custom code.

For custom widgets the properties on these elements will be applied only if the widget is compiled with the user's custom code
and it's part of the preview.


It can be used like this in the API:
```xml
<!-- my_widget.xml -->
<api>
	<element name="control_button" type="obj" help="A control button of my_widget" access="get">
		<arg name="index" type="int" help="The zero-based index of the control button"/>
		<prop name="title" help="Set the title of a control button">
			<param name="text" type="string" help="The text for the title"/>
		</prop>
	</element>
</api>
```
This is the generated header file:
```c
/**
 * Get a control button of my_widget.
 * @param parent     Pointer to a my_widget object from where the control_button should be retrieved.
 * @param index      The zero-based index of the control button
 * @return           Pointer to the control button
 */
lv_obj_t * lv_my_widget_get_control_button(lv_obj_t * parent, int32_t index);

/**
 * Set the title of a control button.
 * @param text       The text for the title
 */
void lv_my_widget_set_control_button_title(lv_obj_t * obj, const char * text);
```

In a view:

```xml
<!-- complex_widget.xml -->
<view>
	<my_widget width="100px">
		<my_widget-control_button name="btn1" index="3" title="Hello"/>
	</my_widget>
</view>
```
And this is the related C file where the control button is retrieved:

```c
lv_obj_t * btn1 = lv_my_widget_get_control_button(parent, index);
lv_my_widget_set_control_button_title(btn1, text);
```

##### `access="set"`

Used when elements are created automatically but need to be selected in API calls. For example, selecting a row and column to set a cell's value: `lv_table_set_cell_value(table, row, col, "text");`.

`<arg>` elements defined directly inside the `<element>` are passed to the `set` function as arguments.

As these `set` functions can be any complex with custom logic inside it's not possible to describe them in the view. They require custom code.
For custom widgets the properties on these elements will be applied only if the widget is compiled with the user's custom code
and it's part of the preview.

It can be used like this in the API:
```xml
<!-- my_widget.xml -->
<api>
	<element name="item" type="obj" help="An item on my_widget" access="set">
		<arg name="index" type="int" help="The zero-based index of the item"/>
		<prop name="icon" help="Set the icon of an item">
			<param name="icon_src" type="img_src" help="The image to set as an icon."/>
		</prop>
		<prop name="color" help="Set the color">
			<param name="color" type="color" help="The color to set for the item."/>
		</prop>
	</element>
</api>
```

This is the generated header file:
```c
/**
 * Set the icon of an item.
 * @param parent     Pointer to a my_widget object where the item should be adjusted.
 * @param index      The zero-based index of the item
 * @param icon_src   The image to set as an icon.
 */
void lv_my_widget_set_item_icon(lv_obj_t * parent, int32_t index, const void * icon_src);

/**
 * Set the color of an item.
 * @param parent     Pointer to a my_widget object where the item should be adjusted.
 * @param index      The zero-based index of the item
 * @param color      The color to set for the item.
 */
void lv_my_widget_set_item_color(lv_obj_t * parent, int32_t index, lv_color_t color);

```

In a view:
```xml
<!-- complex_widget.xml -->
<view>
	<my_widget width="100px">
		<my_widget-item index="3" icon_src="image1" color="0xff0000"/>
	</my_widget>
</view>

```

And this is the related C file where the item properties are set:
```c
lv_my_widget_set_item_icon(parent, index, image1);
lv_my_widget_set_item_color(parent, index, color);
```

#### `<view>`

The `<view>` tag was mostly introduced above through examples hence only a few points are added here.


#### Store references

As widgets are described a `store="true"` property can added to any widgets or elements to
store its pointer in the widgets class data. For example:
```xml
<view>
	<slider name="slider1" store="true" help="Some help"/>
</view>
```

will store the slider like:
```c
typedef struct {
	lv_obj_t obj;
	lv_obj_t * slider1;  /**< Some help */
} lv_my_widget_t
```

#### Extend

The `view` element can have an `extend` attribute. It tells that the new widget is
inherited from an other widget. For example
```xml
<view extend="slider" width="200px"/>
```

The default value of `extend` is `obj`.

If the `<api>` element is missing the properties of the `extend`ed widget will be used.
This way simple new widgets can be created by using the built-in widgets and not writing any logic.
These simple widget can be loaded at runtime from XML too.

### Styles

In the `<styles>` section, styles and their properties can be defined like this:

```xml
<style name="red"
       help="What is this style about?"
       border_width="2px"
       border_color="0xff0000"/>
```

Styles can be referenced like this in the view:

```xml
<view>
	<slider styles="main red:indicator small_shadow:knob:focused"/>
</view>
```

As shown in the example, parts and states are appended after a `-` to the style's name.

The `<style>` tag can have `<static_variant>` and `<dynamic_variant>` child tags.

```xml
<styles>
	<style name="big_button" bg_color="0xf00" border_width="1px" pad_left="10px">
		<static_variant name="size" case="small" pad_left="4px" text_font="arial_10"/>
		<static_variant name="size" case="medium" pad_left="8px" text_font="arial_20"/>
		<static_variant name="size" case="large" pad_left="12px" text_font="arial_30"/>
		<dynamic_variant name="color" case="red" bg_color="0xf00"/>
		<dynamic_variant name="color" case="green" bg_color="0x0f0"/>
		<dynamic_variant name="color" case="blue" bg_color="0x00f"/>
	</style>
</styles>
```

For `<static_variant>`s, an `#if` directive is exported like this:

```c
#if MY_COMPONENT_LIBRARY_SIZE == MY_COMPONENT_LIBRARY_SMALL
	lv_style_set_pad_left(&style_big_button, 4);
	lv_style_set_text_font(&style_big_button, arial_10);
#elif MY_COMPONENT_LIBRARY_SIZE == MY_COMPONENT_LIBRARY_MEDIUM
	lv_style_set_pad_left(&style_big_button, 8);
	lv_style_set_text_font(&style_big_button, arial_20);
#elif MY_COMPONENT_LIBRARY_SIZE == MY_COMPONENT_LIBRARY_LARGE
	lv_style_set_pad_left(&style_big_button, 12);
	lv_style_set_text_font(&style_big_button, arial_30);
#else
	lv_style_set_pad_left(&style_big_button, 10);
#endif
```

`<dynamic_variant>`s are more complex, and they can alter styles at runtime.
The `variant_<name>` subjects of the widget library is used for each setting, and an observer callback is generated with all the style properties valid for that variant.
The observer callback first resets the style and sets all the properties afterward.

### Constants

Constants can be defined to replace any value with a selected type or to be used as special values.

The supported types are:
- color
- px
- percentage
- string
- opa
- bool

```xml
<consts>
	<color name="color1" value="0xff0000" help="Primary color"/>
	<px name="pad_xs" value="8" help="Small padding"/>
</consts>
```

The following is an example of the exported code:

```c
/**
 * Primary color
 */
#define MY_WIDGET_COLOR1 lv_color_hex(0xff0000)

/**
 * Small padding
 */
#define MY_WIDGET_PAD_XS 8
```

Constants support a `variant` attribute to change the constants at compile time. For example:

```xml
<consts>
	<px name="pad" value="8" help="General padding">
		<static_variant name="size" case="small" value="4"/>
		<static_variant name="size" case="large" value="12"/>
	</px>
</consts>
```

Ignoring the comments, the following code can be exported:

```c
#if MY_COMPONENT_LIBRARY_SIZE == MY_COMPONENT_LIBRARY_SMALL
	#define MY_WIDGET_PAD 4
#elif MY_COMPONENT_LIBRARY_SIZE == MY_COMPONENT_LIBRARY_LARGE
	#define MY_WIDGET_PAD 12
#else
	#define MY_WIDGET_PAD 8
#endif
```

### Fonts

A `<fonts>` section can be added to a widget locally or in any `global_*.xml` files.

```xml
<fonts>
	<data name="medium" src_path="file.ttf" range="0x20-0x7f" symbols="°" size="24" />
	<data name="big" src_path="file.ttf" range="0x20-0x7f" symbols="auto" size="48"/>
	<freetype name="chinese" src_path="file.ttf" size="48" custom_freetype_attribute="abc"/>
</fonts>
```

For `<data>`, fonts are converted to a C array. In the case of `<freetype>`, the fonts are created with `lv_freetype`.
`symbols="auto"` means collecting all the symbols automatically from widgets where this font is used.

The fonts can have `<static_variant>`s as shown below:

```xml
<fonts>
	<data name="medium" src_path="file.ttf" range="0x20-0x7f" symbols="°" size="24">
		<static_variant name="size" case="small" size="12"/>
		<static_variant name="size" case="large" size="36"/>
	</data>
</fonts>
```

For the example above, two fonts will be generated, each guarded with:

```c
#if MY_COMPONENT_LIBRARY_SIZE == MY_COMPONENT_LIBRARY_SMALL // or LARGE
```

In the `<static_variant>`, any properties can be overridden (e.g., size, range, symbol, src_path) except the name.

### Images

An `<images>` section can be added to a widget locally or in any `global.xml` files.

```xml
<images>
	<data name="logo" src_path="logo1.png" scale="50%" color_format="RGB565A8" memory="RAM2"/>
	<data name="wallpaper" src_path="wp.jpg" scale="1000 800" color_format="RGB565"/>
	<data name="apply" src_path="apply_icon.jpg" scale="16" color_format="ARGB8888"/>
	<data name="settings_icon" src_path="setting_icon.svg" scale="16" color_format="raw"/>
	<file name="avatar" src_path="avatar1.svg" scale="30" file_format="png" color_format="ARGB8888"/>
</images>

<images scale="20" color_format="ARGB8888" memory="flash_2">
	<data name="icon1" src_path="icon1.png"/>
</images>
```

Similarly to fonts, images can also have static variants like this:

```xml
<images>
	<data name="logo" src_path="logo1.png" color_format="RGB565A8" memory="RAM2">
		<static_variant name="size" case="small" scale="12"/>
		<static_variant name="size" case="large" scale="36"/>
	</data>
</images>
```

For the example above, two images will be generated, each guarded with:

```c
#if MY_COMPONENT_LIBRARY_SIZE == MY_COMPONENT_LIBRARY_SMALL // or LARGE
```

In the `<static_variant>`, any properties can be overridden (e.g., scale, color_format, src_path, memory) except the name.

### Subjects

To connect values of the widget internally or to external data, subjects can be used. For example, an internally connected value could be a slider's value mapped to a label. Externally connected data could be the current number of users shown on a label.

To handle internal connections, local subjects can be created like this:
```xml
<subjects>
	<int name="a" value="20"/>
	<string name="b" value="Hello"/>
	<group name="a_and_b" value="a b"/>
</subjects>
```

These subjects can be used in widget APIs like:
```xml
<view>
	<label bind_text="a 'Progress: %d'"/>
</view>
```

When generating code, the subjects are saved in the widget's data and are used like this:
```c
lv_subject_init_int(&my_widget->subject_a, 20);
lv_subject_init_string(&my_widget->subject_b, "Hello");

my_widget->subject_a_and_b_list = lv_malloc(sizeof(lv_subject_t *) * 2);
my_widget->subject_a_and_b_list[0] = &my_widget->subject_a;
my_widget->subject_a_and_b_list[1] = &my_widget->subject_b;
lv_subject_init_group(&my_widget->subject_a_and_b, my_widget->subject_a_and_b_list);
```

If the connection is more complex and not supported out of the box, it can be handled from code.

External subjects are defined in the API of the widget:
```xml
<api>
	<prop name="bind_value" help="">
		<param name="subject" type="subject" help=""/>
		<param name="max_value" type="int" help="Just another parameter, e.g., to limit the value"/>
	</prop>
</api>
```

### Animations

Any widget and style property can be animated.

Animations are always interpreted as an animation timeline. For example:

```xml
<animations>
	<animation name="anim_in">
		<widget name="slider1">
			<opa start="0" end="255" delay="0" duration="100"/>
			<y start="-30" end="0" delay="100" duration="100"/>
		</widget>
		<widget name="button1">
			<opa start="0" end="255" delay="0" duration="100" delay="200"/>
			<y start="30" end="0" delay="100" duration="300"/>
		</widget>
		<style name="main">
			<bg_color start="current" end="0xeee" delay="0" duration="100" delay="200"/>
		</style>
	</animation>
</animations>
```

When the code is exported, an LVGL animation timeline is created, which will be available to animate the widget later from code.

### Events

Event handlers can be specified for widgets like this:

```xml
<button width="100px">
	<events trigger="clicked">
		<call_function name="some_callback"/>
		<play_animation name="anim1"/>
		<set_subject name="subject1" value="Hello!"/>
	</events>
	<label text="Apply"/>
</button>
```

For `call_function`s the user needs to implement the function with the given name.

### Translations

`type="string"` widget property values can be translated.

To indicate that a string is selected for translation, add an `_` prefix. For example: `text="_hello"`.
The strings to be translated will be exported as `_("hello")` in C, making the `_` prefix consistent.

A `translations.yml` file can be created in any folder, and all translations will be collected from subfolders where there is no `translations.yml`.

The typical content is like:

```xml
<translations generated="true">
    <translation tag="dog" en="dog" de="Hund"/>
</translations_gen>

<translations>
    <translation tag="cat" en="cat" de="Katze"/>
</translations_user>
```

The `generated="true"` attribute is used for automatically collected translations.


## Tests

Test cases can be created for the widgets by using the `<tests>` tag in any files. The test cases should be rendered similarly to the view of the widgets, allowing the user to preview them.

Inside `<tests>`, `<test>` tags should be created, where the user can define views, subjects, consts, fonts, images, etc., except an `<api>`.

To use the tests, the widget needs to be compiled; otherwise, its API and logic won't be available.

For a sliderbox widget, the test file can look like this:

```xml
<!-- test_sliderbox.xml -->

<tests name="Some name" help="Description about the batch of tests below.">
	<test name="basic test" help="Some description">
		<consts>
			<int name="value" value="30"/>
		</consts>

		<view>
			<sliderbox value="!value" range="20 80"/>
		</view>
	</test>

	<test name="test subjects" help="Some description">
		<subjects>
			<subject name="value" value="30"/>
		</subjects>
		<view>
			<sliderbox bind_value="value" range="20 80"/>
		</view>
	</test>

	<test name="test in list with different sizes" help="Some description">
		<view width="300px" height="600px" style_flex_flow="column">
			<sliderbox value="30" range="20 80"/>
			<sliderbox value="40" range="20 80" width="100%"/>
			<sliderbox value="50" range="20 80" width="200px"/>
			<sliderbox value="60" range="20 80" width="100px"/>
		</view>
	</test>

	<test name="interactive test" help="Some description">
		<view>
			<sliderbox value="30" range="20 80"/>
		</view>
		<steps>
			<click pos="20 30"/>
			<wait time="100"/>
			<screenshot ref="ref_img1"/>
		</steps>
	</test>
</tests>
```


## Folder Structure

It's up to the user to define the folder structure for widgets, tests, and assets.

An example:
```
- my_widget_library
	- globals.xml
	- widget_1
		- widget_1.xml
		- assets
		- tests
			- test_1.xml
			- test_2.xml
	- widget_2
		- widget_2.xml
		- tests
			- test_1.xml
```

Another example:
```
- my_widget_library
	- globals.xml
	- assets
	- widgets
		- widget_1.xml
		- widget_2.xml
	- tests
		- test_widget_1.xml
		- test_widget_2.xml
```


## Global Data

A `globals.xml` file can be created in each widget library. These definitions do not belong to any specific widget but are available throughout the entire UI, widgets, and all XMLs. The valid tags are:

- `<config>`: Can specify `name` and `help`.
- `<api>`: Only for `<enumdefs>`.
- `<subjects>`
- `<consts>`
- `<styles>`
- `<fonts>`
- `<images>`
- `<static_variants>` See below
- `<dynamic_variants>` See below

`globals.xml` files cannot be nested, meaning that there cannot be another `globals.xml` file in a subfolder.

From each `globals.xml` file, an `<info.name>.h` file is generated, which is included in all generated header files—not only in the subfolders where `globals.xml` is created, but in all exported C and H files. This ensures that constants, fonts, and other global data are available for all widgets and new widgets.

### Variants

`<static_variant>`s can be used by styles, images, fonts, and constants to create variants that can be selected at compile time.

`<dynamic_variant>`s can be used by styles only, to modify the styles at runtime.
The select the current dynamic variant a `<config.name>_variant` integer subject created which can be sued to alter the styles if needed.

In `globals.xml`, the possible variants should be described. Here is an example:

```xml
<static_variant name="size" help="Select the size">
	<case name="small" help="Assets for 320x240 screen"/>
	<case name="large" help="Assets for 1280x768 screen"/>
</static_variants>

<dynamic_variant name="color" help="Select the color of the UI">
	<case name="red" help="Select a red theme"/>
	<case name="blue" help="Select a blue theme"/>
</dynamic_variants>
```

### Namespacing

There is no built-in concept for name spacing but it's the user's responsibility to avoide naming conflicts
among the widget libraries. It can be done by prefixing widget names and global data.

For example:
```xml
<view>
	<lv_button styles="ebike_panel ebike_shadow ebike_red" style_text_font="my_font2"/>
	<ebike_button/>
</view>
```

## Project Data

A single `project.xml` file should be created where the following content is specified:

```xml
<folders>
	<folder path="../widget_lib1"/>
	<folder path="/home/user/work/ui_libs/modern"/>
</folders>

<hardware>
	<renesas>
		<board name="Renesas RA8D1-EK"/>
		<custom>
			...
		</custom>
	</renesas>

	<simulator>
		<display width="320" height="240" color_format="RGB565"/>
		<display width="800" height="480" color_format="ARGB8888"/>
	</simulator>
</hardware>
```

- `<folders>` specifies where the widgets should be searched and loaded from. LVGL's base widgets are always loaded with an `lv` prefix.
- `<hardware>` describes various hardware configurations, allowing the software to check if the UI is out of resources.


## Property Update

It should be possible to update properties without regenerating the entire UI.

The exact mechanism is still to be defined, but some special tags could support property updates, such as:

```xml
<update_widget name="slider1" prop="range" param="10 200"/>
<update_subject name="temp1" value="13"/>
```
