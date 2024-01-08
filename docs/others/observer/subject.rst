Subject
*******

Subject initialization
----------------------

Subjects have to be static or global :cpp:type:`lv_subject_t` type variables.

To initialize a subject use :cpp:expr:`lv_subject_init_<type>(&subject, params, init_value)`.
The following initializations exist for types:

- **Integer**: :cpp:expr:`lv_subject_init_int(lv_subject_t * subject, int32_t value)`
- **String**: :cpp:expr:`lv_subject_init_string(lv_subject_t * subject, char * buf, char * prev_buf, size_t size, const char * value)`
- **Pointer**:  :cpp:expr:`lv_subject_init_pointer(lv_subject_t * subject, void * value)`
- **Color**: :cpp:expr:`lv_subject_init_color(lv_subject_t * subject, lv_color_t color)`
- **Group**: :cpp:expr:`lv_subject_init_group(lv_subject_t * subject, lv_subject_t * list[], uint32_t list_len)`


Set subject value
-----------------

The following functions can be used to set a subject's value:

- **Integer**: :cpp:expr:`void lv_subject_set_int(lv_subject_t * subject, int32_t value)`
- **String**: :cpp:expr:`void lv_subject_copy_string(lv_subject_t * subject, char * buf)`
- **Pointer**:  :cpp:expr:`void lv_subject_set_pointer(lv_subject_t * subject, void * ptr)`
- **Color**: :cpp:expr:`void lv_subject_set_color(lv_subject_t * subject, lv_color_t color)`


Get subject's value
-------------------

The following functions can be used to get a subject's value:


- **Integer**: :cpp:expr:`int32_t lv_subject_get_int(lv_subject_t * subject)`
- **String**: :cpp:expr:`const char * lv_subject_get_string(lv_subject_t * subject)`
- **Pointer**:  :cpp:expr:`const void * lv_subject_get_pointer(lv_subject_t * subject)`
- **Color**: :cpp:expr:`lv_color_t lv_subject_get_color(lv_subject_t * subject)`


Get subject's previous value
----------------------------

The following functions can be used to get a subject's previous value:


- **Integer**: :cpp:expr:`int32_t lv_subject_get_previous_int(lv_subject_t * subject)`
- **String**: :cpp:expr:`const char * lv_subject_get_previous_string(lv_subject_t * subject)`
- **Pointer**: :cpp:expr:`const void * lv_subject_get_previous_pointer(lv_subject_t * subject)`
- **Color**: :cpp:expr:`lv_color_t lv_subject_get_previous_color(lv_subject_t * subject)`

