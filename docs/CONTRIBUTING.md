# Contributing to Littlev Graphics Library
It's glad to see that you are interested in Contributing to LittlevGL! 

In this guide you can learn how can you help to develop  LittlevGL. 

### Table Of Content
* [Who can contribute?](#who-can-contribute)
* [How to report an issue?](#how-to-report-a-bug)
* [How to suggest a feature?](#how-to-suggest-a-feature)
* [How to implement a feature?](#how-to-implement-a-feature)
* [Styling guide](#styling-guide)

## Who can contribute?
As graphical interfaces for embedded systems has an increasing relevance today you also might find important to work with a good graphics library. Now - independently from skills, programming level or any personal attributes - you can influence and help the development of Littlev Graphics Library with:
* Report an issue
* Suggest feature
* Fix an issue
* Implement a feature
* Help with testing bugfixes and new features

Please, take a look at [CODE_OF_CONDUCT](https://github.com/littlevgl/lvgl/blob/master/docs/CODE_OF_CONDUCT.md)

There are few **general rules**
* We use [GitHub's issue tracker](https://github.com/littlevgl/lvgl/issues)
* Be kind and respectful. Starting with "Hi" is always a good idea :)
* If somebody helped you give a feedback.
* One issue should be about one topic. If you have other questions please open a new issue.
* Always create an issue before creating a [Pull request](https://help.github.com/articles/about-pull-requests/) to discuss the idea first
* Create small, "digestible" Pull requests.  
* Tell your remarks in a structured way. Use paragraphs and the [Markdown](https://guides.github.com/features/mastering-markdown/) support of GitHub.
* Be sure you are using the latest version (from `master` branch)
* Keep in mind LittlevGL should be and should remain:
  - usable on small MCUs as well (think about memory footprint)
  - compilable with "non-standard" tools like Arduino (no gcc specific options) 
  - C compatible (no C++ specific code and features)
  - all configuration should be in `lv_conf.h`. (Instead of modifying the library)
  - the API clean and easiy to understand
  
## How to report a bug?
If you found a **simple and straightforward bug** like:
* misspelling (in comments function/variable names or grammatical issues in comments)
* not handled error cases (negative array index, overflow etc)
* anything else which can be fixed locally with a few lines of code
* or defective documentation

then tell:
* where you found the bug (which file/function/variable)
* how can it cause problem
* what is your suggested solution if you have 

If you faced with **something more complex** like:
* might be simple but you don't know its origin
* affects a whole file, module or even the architecture
* needs deeper discussion

then please
* tell what do you experience
* tell what do you expect to happen
* tell how to reproduce the issue 
* provide a simplified code example (better if can be tested with copy-paste)
* attache your lv_conf.h (if you feel it's important)
* logs and long codes should be attached in a file (instead of copying into a comment)

## How to suggest a feature?
If you have a good and useful idea open issue to tell it! Please note the followings on suggesting new features:
* What the new feature is about?
* Why/Where/In which case is it useful/helpful/relevant?
* Can you mention real life usecases/examples for the use this feature?
* Can you help in implementing it?

Your suggestion can have 4 possible outcomes:
1. This feature is already exists. In this case you will learn how to achieve your goal.
2. You can simply realize it with the current functionality.
3. Although it's a new feature but it would break LittlevGL's platform independent and/or resource minimalist nature.
4. It's really a new feature which would be good to be in LittlevGL. Hurray! In a discussion we figure out the technical details and implementation options. With the knowledge of how to do it somebody can implement the new feature.

Keep in mind if you wouldn't like to do the implementation there is no guarantee that it will be ready in the near future. 
However, if you would like to force it, take a look at this page: [Feature request service](http://www.gl.littlev.hu/services#feature)

## How to implement a feature?
In [docs/TODO_MINOR.md](https://github.com/littlevgl/lvgl/blob/master/docs/TODO_MINOR.md) and [docs/TODO_PATCH.md](https://github.com/littlevgl/lvgl/blob/master/docs/TODO_PATCH.md) you can see some ideas which are waiting for somebody to realize them! If want to deal with a feature from these files, please start an issue and discuss the details.

The new feature should be in a new branch.

## Styling guide

### File format
Use [lv_misc/lv_templ.c](https://github.com/littlevgl/lvgl/blob/master/lv_misc/lv_templ.c) and [lv_misc/lv_templ.h](https://github.com/littlevgl/lvgl/blob/master/lv_misc/lv_templ.h)

### Naming conventions
* Words are separated by '_'
* In variable and function names use only lower case letters (e.g. *height_tmp*)
* In enums and defines use only upper case letters (e.g. *e.g. MAX_LINE_NUM*)
* Global names (API):
  * starts with *lv*
  * followed by module name: *btn*, *label*, *style* etc.
  * followed by the action (for functions): *set*, *get*, *refr* etc.
  * closed with the subject: *name*, *size*, *state* etc. 
* Typedefs
  * prefer `typedef struct` and `typedef enum` instead of  `struct name` and `enum name`
  * always add a closing *..._t*
* Abbreviations:
  * Use abbreviations on public names only if they become longer than 32 characters 
  * Use only very straightforward (e.g. pos: position) or well-established (e.g. pr: press) abbreviations 

### Coding guide
* Functions:
  * Try to write function shorter than is 50 lines 
  * Always shorter than 100 lines (except very straightforwards) 
* Variables:
  * One line, one declaration (BAD: char x, y;)
  * Use `<stdint.h>` (*uint8_t*, *int32_t* etc)
  * Declare variables when needed (not all at function start)
  * Use the smallest required scope
  * Variables in a file (outside functions) are always *static*
  * Do not use global variables (use functions to set/get static variables)

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

In comments use \` \` when referring to a variable. E.g. ``/*Update the value of `x_act`*/``

### Formatting
Here is example to show bracket placing and using of white spaces:
```c
/**
 * Set a new text for a label. Memory will be allocated to store the text by the label.
 * @param label pointer to a label object
 * @param text '\0' terminated character string. NULL to refresh with the current text.
 */
void lv_label_set_text(lv_obj_t * label, const char * text)
{   /* Main brackets of functions in new line*/
    
    if(label == NULL) return; /*No bracket only if the command is inline with the if statement*/
    
    lv_obj_inv(label);
    
    lv_label_ext_t * ext = lv_obj_get_ext(label);

    /*Comment before a section */
    if(text == ext->txt || text == NULL) {  /*Bracket of statements start inline*/
        lv_label_refr_text(label);
        return;   
    }
    
    ...
}
```

Use 4 spaces indentation instead of tab.

You can use **astyle** to format the code. The required config flies are: `docs/astyle_c` and `docs/astyle_h`.
To format the source files: 
 `$ find . -type f -name "*.c" | xargs astyle --options=docs/astyle_c`
 
To format the header files:
 `$ find . -type f -name "*.h" | xargs astyle --options=docs/astyle_h`
 
Append `-n` to the end to skip creation of backup file OR use `$ find . -type f -name "*.bak"  -delete` (for source file's backups) and `find . -type f -name "*.orig" -delete` (for header file's backups)


