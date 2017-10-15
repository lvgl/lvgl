# Contributing to Littlev Graphics Library
First of all thank you for reading these guide before contributing!

In this guide you can read how can you help in developing the Littlev Graphic Library. If you have a constructive idea just create pull request on this document!

### Table Of Content
* [Who can contribute?](#who-can-contribute)
* [How to report an issue?](#how-to-report-an-issue)
  * [Simple issue](#simple-issue)
  * [Complex issue](#complex-issue)
* [How to suggest a feature?](#how-to-suggest-a-feature)
* [How to implement a feature?](#how-to-implement-a-feature)
* [Styling guide](#styling-guide)
  * [File format](#file-format)
  * [Functions](#functions)
  * [Variables](#variables)
  * [Defines](#defines)
  * [Typedefs](#typedefs)
  * [Comments](#comments)
  * [Formatting](#formatting)


## Who can contribute?
As graphical interfaces for embedded systems has an increasing relevance today. You also might find important to work with a good graphics library. Now - independently from skills, programming level or any personal attributes - you can influence the development of the graphics library with:
* Report an issue
* Suggest feature
* Fix an issue
* Implement a feature

Please, take a look at [CODE_OF_CONDUCT](https://github.com/littlevgl/lvgl/blob/master/docs/CODE_OF_CONDUCT.md)

Continue reading to know how you can be part of the development! We are waiting for you!

## How to report an issue?

### Simple issue
If you find an issue which is very simple to fix, and you fixed it, please send a pull request against `beta` branch. 
A simple issue could be:
* Misspelled names
  * Comment: misspelling or grammatical error in comments
  * Variable: misspelled variable name (e.g. ***ojb**_next* instead of ***obj**_next*)
  * Define: only local defines in files because global defines affect API
  * Function: only static function name because global functions affect API
* Not handled error case:
  * A parameter can be NULL (during normal usage)
  * Negative index in array or over indexing
  * Overflow in variable
  * Anything which is local an can be fixed with a few lines of code  

### Complex issue
If you find a complex issue which:
* might be simple but you don't know its origin
* affects a whole file, module or even the architecture
* needs deeper discussion

please create a **new issue** and describe
* what you experience
* how to reproduce the issue (maybe with example code)
* version you are using (lvgl.h)
* misc library version (misc.h)

## How to suggest a feature?
If you have a good and useful idea you can use GitHub issues to suggest a new feature. Please note the followings on feature requests:
* What the new feature is about?
* Why/Where/In which case is it useful/helpful/relevant?
* Can you help in implementing it?

After a discussion we figure out the specification of the new feature and the technical details/implementation possibilities. 
With the knowledge of how to do it somebody can implement the new feature.

Keep in mind if you wouldn't like to do the implementation there is no guarantee that it will be ready in the new future. 
However if you would like to force it, take a look at this page: [Feature request service](http://www.gl.littlev.hu/services#feature)

## How to implement a feature?
In [docs/TODO_MINOR.md](https://github.com/littlevgl/lvgl/blob/master/docs/TODO_MINOR.md) and [docs/TODO_PATCH.md](https://github.com/littlevgl/lvgl/blob/master/docs/TODO_PATCH.md) you can see some ideas which are waiting for somebody to realize them! If want to deal with a feature from this files, please start an issue and discusse the details.

The new feature should be in a new branch.

## Styling guide

### File format
Use [misc/templ/templ.c](https://github.com/littlevgl/misc/blob/master/templ/templ.c) and [misc/templ/templ.h](https://github.com/littlevgl/misc/blob/master/templ/templ.h)

### Abbreviations
Please read this document to see the list of accepted abbreviations: [abbreviations-in-code](https://github.com/kisvegabor/abbreviations-in-code)

### Functions
* try to write function shorter then is 40 lines 
* always shorter then 100 lines (except very straightforwards) 
* in function names: 
  * words sparated by '_'
  * only lower case letters
  * only clear abbreviation (OK: *lv_xy_get_title_txt*, BAD: *lv_xy_get_ttxt*)

#### Global functions names (API)
An example: *lv_btn_set_state()*
* starts with *lv*
* followed by module name: *btn*, *label*, *style* etc.
* followed by the action: *set*, *get*, *refr* etc.
* closed with subject: *name*, *size*, *state* etc. 
  * optional like in *lv_obj_del()* it is missing
  * could contain more words: *long_mode*, *point_all* 

#### Static functions names
Names can be used freely.

### Variables
* words sparated by '_'
* always lower case
* one line, one declaration (BAD: char x, y;)
* use `<stdint.h>` (*uint8_t*, *int32_t* etc)
* declare variables when needed (not all at function start)
* use the smallest required scope
* variables in a file (outside functions) are always *static*
* do not use global variables (use functions to set/get static variables)

### Defines
* always upper case
* starts with *LV_*
* followed by the modul: *OBJ*, *BTN* etc.
* closed by the subject: *ANIM_TIME*, *VALUE_MIN*, *WIDTH_DEF*

### Typedefs
- prefer `typedef struct` instead of  `struct name`
- prefer `typedef enum` instead of  `enum name`
- types always lowercase speperated by '_' 
- first word for public typedefs is *lv_...*
- next word identifies the modul: *lv_obj_...*, *lv_btn_...*
- always add closing *..._t*
- Examples: *lv_obj_t*, *lv_cont_layout_t*

### Comments
Before every function have a comment like this:

```c
/**
 * Return with the screen of an object
 * @param obj pointer to an object
 * @return pointer to a screen
 */
lv_obj_t * lv_obj_get_scr(lv_obj_t * obj); 
```

Always use `/* Something */` format and NOT `//Something`

Write readable code to avoid descriptive comments like: 
`x++; /* Add 1 to x */`. 
The code should show clearly what you are doing.

You should write **why** have you done this: 
`x++; /*Because of closing '\0' of the string */`

Short "code summaries" of a few lines are accepted. E.g. `/*Calculate the new coordinates*/`

In comments use `' '` when refering to a variable. E.g. `/*Update the value of 'x_act'*/`

### Formatting
Here is example to show bracket placing and using of white spaces:
```c
/**
 * Set a new text for a label. Memory will be allocated to store the text by the label.
 * @param label pointer to a label object
 * @param text '\0' terminated character string. NULL to refresh with the current text.
 */
void lv_label_set_text(lv_obj_t * label, const char * text)
{   /*Main bracket in new line*/
    
    if(label == NULL) return; /*No bracket only if the command is inline with the if statement*/
    
    lv_obj_inv(label);
    
    lv_label_ext_t * ext = lv_obj_get_ext(label);

    /*Comment before a section */
    if(text == ext->txt || text == NULL) {  /*Bracket start inline*/
        lv_label_refr_text(label);
        return;   
    }
    
    .
    .
    .
}
```

Use 4 spaces indentation instead of tab.
