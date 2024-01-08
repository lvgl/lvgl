
Subject groups
**************

There are cases when a subject changes and the value of some other subjects are also required by the observer.
As a practical example imagine an instrument which measures either voltage or current.
To display the measured value on a label 3 things are required:

1. What do we measure (current or voltage)?
2. What is the measured value?
3. What is the range or unit (mV, V, mA, A)?

When any of these 3 parameters changes the label needs to be updated,
and it needs to know all 3 parameters to compose its text.

To handle this you can create an array from some existing subjects and pass
this array as a parameter when you initialize a subject with group type.

.. code:: c

    static lv_subject_t * subject_list[3] = {&subject_1, &subject_2, &subject_3};
    lv_subject_init_group(&subject_all, subject_list, 3);  /*The last parameter is the number of elements*/

You can add observers to subject groups in the regular way.
The trick is that when any element of the group is notified the subject group will be notified too.

The above Voltage/Current measurement example looks like this in the practice:

.. code:: c

    lv_obj_t * label = lv_label_create(lv_screen_active());

    lv_subject_t subject_mode;  //Voltage or Current
    lv_subject_t subject_value; //Measured value
    lv_subject_t subject_unit;  //The unit
    lv_subject_t subject_all;   //It will be the subject group
    lv_subject_t * subject_list[3] = {&subject_mode, &subject_value, &subject_unit};  //The elements of the group

    lv_subject_init_int(&subject_mode, 0); //Let's say 0 is Voltage, 1 is Current
    lv_subject_init_int(&subject_value, 0);
    lv_subject_init_pointer(&subject_unit, "V");
    lv_subject_init_group(&subject_all, subject_list, 3);

    lv_subject_add_observer_obj(&subject_all, all_observer_cb, label, NULL);

    ...

    static void all_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
    {
        lv_obj_t * label = lv_observer_get_target(observer);
        lv_subject_t * subject_mode = lv_subject_get_group_element(subject, 0);
        lv_subject_t * subject_value = lv_subject_get_group_element(subject, 1);
        lv_subject_t * subject_unit = lv_subject_get_group_element(subject, 2);

        int32_t mode = lv_subject_get_int(subject_mode);
        int32_t value = lv_subject_get_int(subject_value);
        const char * unit = lv_subject_get_pointer(subject_unit);

        lv_label_set_text_fmt(label, "%s: %d %s", mode ? "Current" : "Voltage", value, unit);
    }

