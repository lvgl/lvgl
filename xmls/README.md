# XML Format

## Overview

### Goal

Create a UI editor for LVGL to support developers in speeding up the visual part of the UI while allowing code writing where it's more effective.

To achieve this:
- Think in components.
- Describe the widgets in a declarative way.
- Allow nesting of widgets.
- Generate developer-friendly code.
- Load widgets at runtime.
- Provide a real-time preview of the components.
- Support testing of the new components.

It's **not the goal** to create a no-code tool. The assumption is that the user is a developer and can write code. The XML description should help with defining the view (and providing a preview), allowing for fast design iterations.

Because of this, it is **not the goal** to allow creating the entire UI with XML. It is acceptable to create visually heavy parts here and use the widget's API to dynamically create more complex cases from code. It is also acceptable to define callbacks in code.

### Success Criteria

If the following can be fulfilled, the tool is considered successful:

1. **Component Library (No Handwritten Code)**:
    - Create a component library consisting of styled buttons, sliders, and tabviews.
    - Load these components at runtime from XML.
    - Support nested components.

2. **Access Handwritten Components and Widgets (No Editor Code)**:
    - Create custom components or widgets manually (without the editor).
    - Describe only their interface for the editor.
    - Allow users to use and preview these as part of other components.

3. **Complex Widgets in the Editor**:
    - Create complex widgets in the editor, completed with custom code.
    - Make them available in the editor and allow using them in custom components or other widgets.

4. **Runtime Loading**:
    - Load nested components at runtime.
    - Use all three options above.

5. **Figma Import**:
    - Allow loading styles and nested elements from Figma in a simple way.

6. **Testing**:
    - Detect regression in components and widgets with CI.

7. **Collaboration**:
    - Let a designer preview the components and screens in a browser and allow commenting on them.

8. **Other Features**:
    - Handle images and fonts.
    - Allow creating variants for fonts, images, and styles.
    - Support subjects.
    - Support translations.

### Workflows

The main purpose of the editor is to create and test:

- **Widgets**: Similar to built-in widgets (built from classes, with a large API). They can have custom C code to describe complex logic. Custom widgets are compiled into the editor's preview to allow running their custom code.
- **Components**: Simple wrappers for some widgets or other components. They only have a simple `create` function as their API and cannot have custom C code. Therefore, components can be loaded at runtime from XML.

#### Widgets

Main characteristics:
- Similar to LVGL's built-in widgets.
- Built from classes.
- Have a large API.
- Support internal widgets (e.g., tabview's tabs).
- Can have custom C code.
- Compiled into the editor's preview.

Widgets are wrapped in a `<widget>` element.

The interfaces of a widget are described in an `<api>` tag. The properties, elements (internal widgets), and enums of a given widget are defined here.

In the `<styles>` section, styles and their properties can be defined.

Widgets can be created, and their properties can be set, within a `<view>` tag.

##### New Widget in the Editor

An example of a custom `sliderbox` widget:

```xml
<!-- sliderbox.xml -->
<widget>
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
</widget>
```

This is how it looks when the slider box is used, for example, in a list:

```xml
<view style_flex_flow="column">
	<sliderbox value="40" mode="normal" title="First"/>
	<sliderbox value="20" mode="inverted" title="Second"/>
</view>
```

For each property in `<api>`, a setter function will also be exported where the user can define any custom logic.

An XML parser skeleton file will also be exported where the user can process the attributes and call the related setter functions. There are many helper functions to make it simple, so it should look like this:
```c
if(lv_streq(attr, "color")) my_widget_set_color(obj, lv_xml_str_to_color(value));
if(lv_streq(attr, "limit")) my_widget_set_limit(obj, atoi(value));
```

Once both the setters and XML parser are implemented, the editor's core can be recompiled with this new widget, so that it will be part of the editor and the preview.

In summary, these files will be generated for widgets:
- `<widget_name>_gen.h`: Contains the generated API implementation of the widget.
- `<widget_name>_private_gen.h`: Contains private API and the data for the widget.
- `<widget_name>_gen.c`: Contains the internals of the widget.
- `<widget_name>.h`: Includes `<widget_name>_gen.h` and allows the user to define custom APIs.
- `<widget_name>.c`: Contains hooks from `<widget_name>_gen.c` and allows the user to write custom code.
- `<widget_name>_xml_parser.c`: Processes the XML strings and calls the required functions according to the set attributes.

##### New Widget from Code

It's also possible to create the entire widget from code. In this case, only an `<api>` element needs to be specified in the XML file. An XML parser also needs to be implemented to call the required API functions of the widget according to the parameters.

#### Components

Main characteristics:
- Built from widgets or other components.
- They have only a single `create` function with some parameters.
- Can be used for styling widgets or to create widgets/components together.
- Cannot have custom C code.
- Can be loaded from XML at runtime.

##### New Component in the Editor

The example below shows the XML of a `my_button` component.

```xml
<!-- my_button.xml -->
<component>
	<params>
		<string name="text"/>
		<int name="radius" default="0"/>
	</params>

	<consts>
		<px name="width" value="100"/>
	</consts>

	<styles>
		<style name="red" bg_color="0xff0000" width="#{width}"/>
		<style name="blue" bg_color="0x0000ff"/>
	</styles>

	<view extends="button" style_radius="${radius}" styles="red blue:pressed">
		<label text="${text}" align="center"/>
	</view>
</component>
```

`my_button` can be used like this in a view:
```xml
<view>
	<my_button text="Settings" click_cb="open_settings"/>
</view>
```

From the XML file above, a C function can be generated like this:
```c
lv_obj_t * my_button_create(lv_obj_t * parent, const char * title, int32_t radius);
```

In C, XML components can be loaded like this:
```c
/* Save the XML string, create the styles, save the constants, params, register a generic XML parser, etc. */
lv_xml_component_t * my_button = lv_xml_load_component("path/to/my_button.xml");
```

Once a component is loaded, instances can be created in two ways:
- By calling C functions, like:

```c
/* Create an instance according to some attributes. (e.g., const char * attrs[] = {"width", "100", "radius", "20", NULL, NULL};) */
lv_obj_t * button1 = lv_xml_create_component(my_button, parent, attrs);
```

- From XML, by using the `my_button` component in the view of another XML:
```xml
<!-- my_card.xml -->
...
<view extends="obj" flex_flow="column">
	<label text="Card1"/>
	<my_button text="Apply"/>
</view>
```

##### New Component from Code

If the component is created from code, only its `<params>` need to be described to define how to use the component.

A generic XML parser will read the attributes and pass them as parameters to the component's `create` function.

An additional feature could be to later use LVGL's DOM API to retrieve the structure and properties of the widgets and generate the view automatically. This could also help import UIs not created in the editor.

## XML details

### Notions and Conventions

The standard XML format is followed: `<tag attribute="value"></tag>` or, if there are no children, `<tag attribute="value"/>`.

Tag and attribute names must start with a letter and cannot have spaces or special characters, except:
- `_`: Used to replace spaces.
- `-`: Cannot be used in user-defined names, but is used in automatically created compound names for separation, e.g., `my_chart-my_series`. This avoids naming conflicts with user-defined names.
- `:`: Used for namespacing.

These are valid names:
- `my_slider`
- `slider2`
- `SuperSlider2000`
- `value2_min`

The following special syntaxes are used:
- `#{}`: References constants. For example, `value="#{const1}"`.
- `${}`: References arguments. For example, `value="${num}"`.


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

`<enumdef>` can be used to create custom enums for widgets.
When used as a type, a `+` suffix means multiple values can be selected and ORed. For example: `type="axis+"`.
It's also possible to limit the possible options the user can select from an enum. For example: `type="dir(top bottom)"`

### `<params>`

Used only in the case of `<component>`s to describe the parameters of the component.

Here is a detailed example:
```xml
<params>
	<param name="title" type="string" help="The title"/>
	<param name="limit" type="int" help="The max value" default="0"/>
	<param name="mode" type="lv_slider:mode" help="The slider mode"/>
</params>
```

There are a few interesting things to notice:
- If a `default` value is added, the parameter is optional. If it's omitted, the default value will be used. If there is no default value, the element is mandatory.
- As `type`, any other widget's enums can be used in the following format: `<widget_name>:<enum_name>`

In the view, parameters can be used like this:
```xml
<view>
	<my_label title="Hello"/>
</view>
```


### `<api>`

The `<api>` tag can be used only in `<widget>`s to define the API of the widget.
`<api>` is more complex than `<params>`. It supports the following elements and features.

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

This is how it can be used in a `<view>`:
```xml
<!-- complex_widget.xml -->
<view>
	<my_widget mode="normal"/>
</view>
```

#### `<prop>`

Inside `<prop>`s, `<param>` elements must be defined to describe the arguments.
All properties are optional. If a property is not set, it simply won't be applied, and the created widget's default value will be applied (e.g., `"text"` for label text) or the defined default value will be set as described below.

For example:
```xml
<api>
	<prop name="range" default="0 100" help="Set the range." help-zh="Chinese set range.">
		<param name="range_min" type="int" help="Sets the minimum value." help-zh="Translation for range_min."/>
		<param name="range_max" type="int" help="Sets the maximum value."/>
	</prop>
</api>
```

When a property is used, all parameters are set as a single attribute value. For example:

```xml
<slider range="-100 100"/>
```

Each property will correspond to a setter function. `name` in `<prop>` is used to build the name of the setter function like this:
```c
<widget_name>_set_<prop_name>(lv_obj_t * obj, <param1_type> <param1_name>, <param2_type> <param2_name>, ...);
```

`<prop>`s have an optional `<postponed>` `bool` attribute.
By default, it's `false`, but if it's set to `true`, the given property will be applied after all the children are created.
A practical example is that, for example, the current tab of the tabview cannot be set before the tabs are created.

The following code can be generated in a header file:
```c
/**
 * Create my_widget
 * @param parent  		The parent of my_widget
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

When this widget is used in a `<view>` as a child, it looks like this:
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

<view extends="obj">
	<button name="btn1"/>
</view>

<view-element name="indicator" parent="btn1">
	...describe the element's view. (The indicator will be added to btn1)
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

### `<view>`

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

It works only if there is `<api>` the pointers are stored in the widgets' class data.
Without `<api>` element just a simple create function is exported.

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

### `<screen>`

In the XML normally components are described.
By using a widget in the `<view>` if an XML file an instance of the widget will be created.

However, besides components screens are also important to create a well structured UI.

Screens can be created by using a `<screens>` element with `<screen>` children. For example:

```xml
<screens>
	<screen name="main_screen" dynamic="true">
		<styles .../>
		<consts .../>
		<subjects .../>
		<images .../>
		<fonts .../>
		<view>
			<header/>
			<menu_selector/>
			<footer/>
		</view>
	</screen>

	<screen name="settings">
		<view>
			<list>
				<checkbox text="Option1" bind="subject1"/>
				<checkbox text="Option1" bind="subject2"/>
			</list>
		</view>
	</screen>
</screens>
```

Each `<screen>` elements will be exported into a separate file.
As the example shows internal styles, constants, subjects, images and fonts also can be defined for each screen.

If `dynamic="true"` is set the screen will be automatically created and deleted when it's loaded/unloaded.
Otherwise the screen is created once when the widget library is initialized.

When loading a screen from XML `lv_xml_load(NULL, xml_data, xml_data_len);` can be used.
The first parameter (parent) is `NULL` as the screens has no parent.

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

And they can be used like:
```
<styles>
	<style name="style1" bg_color=#{color1}/>
</styles>
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

If the XML file of a widget loaded at run-time the fonts should be mapped to their names like this:

```c
lv_xml_map_font("font1", some_font); /* lv_font_t * some_font */
```


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


If the XML file of a widget loaded at run-time the images should be mapped to their names like this:

```c
lv_xml_map_image("logo1", &some_image);
lv_xml_map_image("logo2", "A:path/to/file.png");
```


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

If the XML file loaded at run-time the the `call_function` names should be mapped to C functions like this:

```c
lv_xml_map_event("some_callback", my_callback);
```

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
		<display name="small" width="320" height="240" color_format="RGB565"/>
		<display name="large" width="800" height="480" color_format="ARGB8888"/>
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

