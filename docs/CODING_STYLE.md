
## File format
Use [lv_misc/lv_templ.c](https://github.com/littlevgl/lvgl/blob/master/lv_misc/lv_templ.c) and [lv_misc/lv_templ.h](https://github.com/littlevgl/lvgl/blob/master/lv_misc/lv_templ.h)

## Naming conventions
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
  * always end `typedef struct` and `typedef enum` type names with `_t`
* Abbreviations:
  * Use abbreviations on public names only if they become longer than 32 characters 
  * Use only very straightforward (e.g. pos: position) or well-established (e.g. pr: press) abbreviations 

## Coding guide
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

## Comments
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
