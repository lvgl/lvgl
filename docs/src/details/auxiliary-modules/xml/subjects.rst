.. _xml_subjects:

========
Subjects
========

To connect values of the widget internally or to external data, subjects can be used. For example, an internally connected value could be a slider's value mapped to a label. Externally connected data could be the current number of users shown on a label.

To handle internal connections, local subjects can be created like this:

<subjects>
	<int name="a" value="20"/>
	<string name="b" value="Hello"/>
	<group name="a_and_b" value="a b"/>
</subjects>

These subjects can be used in widget APIs like:

<view>
	<label bind_text="a 'Progress: %d'"/>
</view>
When generating code, the subjects are saved in the widget's data and are used like this:

lv_subject_init_int(&my_widget->subject_a, 20);
lv_subject_init_string(&my_widget->subject_b, "Hello");

my_widget->subject_a_and_b_list = lv_malloc(sizeof(lv_subject_t *) * 2);
my_widget->subject_a_and_b_list[0] = &my_widget->subject_a;
my_widget->subject_a_and_b_list[1] = &my_widget->subject_b;
lv_subject_init_group(&my_widget->subject_a_and_b, my_widget->subject_a_and_b_list);
If the connection is more complex and not supported out of the box, it can be handled from code.

External subjects are defined in the API of the widget:

<api>
	<prop name="bind_value" help="">
		<param name="subject" type="subject" help=""/>
		<param name="max_value" type="int" help="Just another parameter, e.g., to limit the value"/>
	</prop>
</api>
