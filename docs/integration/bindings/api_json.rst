Output API as JSON data
=======================

We have written a script that will read the header files in LVGL and outputs a
more friendly JSON format for the API. This is done so that bindings that generate
code automatically will have an easy way to collect the needed information without
having to reinvent the wheel. The JSON data format has already made libraries for
reading the format for just about every programming language out there.

The script in order to run does have some requirements.

  - Python >= 3.10
  - Pycparser >= 2.21: Python Library for reading the preprocessor ouotput from the C compiler
  - PyMSVC >= 0.4.0: Python library is using MSVC Compiler
  - C compiler, gcc for Linux, clang for OSX and MSVC for Windows
  - Doxygen: used to read the docstrings from the header files.

There are several options when running the script. They are as follows

  - `--output-path`: output directory for JSON file. If one is not supplied
    then it will be output stdout
  - `--lvgl-config`: path to lv_conf.h (including file name), if this is not
    set then a config file will be generated that has most common things turned on
  - `--develop`: leaves the temporary folder in place.


to use the script

.. code:: shell

    python /scripts/gen_json/gen_json.py --output-path=json/output/directory --lvgl-config=path/to/lv_conf.h


or if you want to run a subprocess from inside of a generation script and read the output from stdout

.. code:: shell

    python /scripts/gen_json/gen_json.py --lvgl-config=path/to/lv_conf.h




The JSON data is broken apart into a couple of main categories.

  - enums
  - functions
  - function_pointers
  - structures
  - unions
  - variables
  - typedefs
  - forward_decls
  - macros

Those categories are the element names undert the root of the JSON data.
The value for each categry is an array of JSON elements. There is a bit of
nesting with the elements in the arrays and I have created "json_types" that
will allow you to identify exactly what you are dealing with.

The different "json_types" are as follows:

  - ``"array"``: The array type is used to identify arrays.

    Available JSON fields:
      - ``"dim"``: number of items in the array
      - ``"quals"``: array of qualifiers, IE "const"
      - ``"type"``: This may or may not be available.
      - ``"name"``: the name of the data type


  - ``"field"``: This type is used to describe fields in structures and unions.
    It is used in the ``"fields"`` array of the ``"struct"`` and ``"union"`` JSON types.

    Available JSON fields:
      - ``"name"``: The name of the field.
      - ``"type"``: This contains the type information for the field. Check the
        ``"json_type"`` to know what type you are dealing with.
      - ``"bitsize"``: The number of bits the field has or ``null``
        if there is no bit size defined
      - ``"docstring"``: you should know what this is.


  - ``"arg"``: Used to describe an argument/parameter in a function or a function pointer.

    Available JSON fields:
      - ``"name"``: The name of the argument/parameter.
      - ``"type"``: This contains the type information for the field. Check the
        ``"json_type"`` to know what type you are dealing with.
      - ``"docstring"``: you should know what this is.
      - ``"quals"``: array of qualifiers, IE "const"


  - ``"forward_decl"``: Describes a forward declaration.There are structures in
    LVGL that are considered to be private and that is what these desccribe.

    Available JSON fields:
      - ``"name"``: The name of the formard declaration.
      - ``"type"``: This contains the type information for the field. Check the
        ``"json_type"`` to know what type you are dealing with.
      - ``"docstring"``: you should know what this is.
      - ``"quals"``: array of qualifiers, IE "const"


  - ``"function_pointer"``: Describes a function pointer. These are used when
    registering callback functions in LVGL.

    Available JSON fields:
      - ``"name"``: The name of the function pointer.
      - ``"type"``: This contains the return type information for the function pointer.
      - ``"docstring"``: you should know what this is.
      - ``"args"``: array of ``"arg"`` objects. This describes the fuction arguments/parameters.
      - ``"quals"``: array of qualifiers, IE "const"


  - ``"variable"``: Describes a global variable.

    Available JSON fields:
      - ``"name"``: The name of the variable.
      - ``"type"``: This contains the type information for the field. Check the
        ``"json_type"`` to know what type you are dealing with.
      - ``"docstring"``: you should know what this is.
      - ``"quals"``: array of qualifiers, IE "const"
      - ``"storage"``: array of storage classifiers, IE "extern"


  - ``"special_type"``:  Currently only used to describe an ellipsis argument
    for a function.

    Available JSON fields:
      - ``"name"``: will always be "ellipsis".


  - ``"primitive_type"``: This is a base type. There or no other types beneith this.
    This tells you that the type is a basic or primitive C type.
    IE: struct, union, int, unsigned int, etc...

    Available JSON fields:
      - ``"name"``: The name of the primitive type.


  - ``"enum"``: Describes a grouping of enumeration items/members.

    Available JSON fields:
      - ``"name"``: The name of the enumeration group/type.
      - ``"type"``: This contains the type information for the enumeration group.
        This is always going to be an "int" type. Make sure you do not use this
        type as the type for the members of this enumeration group. Check the
        enumeration members type to get the correct type.
      - ``"docstring"``: you should know what this is.
      - ``"members"``: array of ``"enum_member"`` objects


  - ``"enum_member"``: Describes an enumeration item/member. Only found under
    the ``"members"`` field of an ``"enum"`` JSON type

    Available JSON fields:
      - ``"name"``: The name of the enumeration.
      - ``"type"``: This contains the type information for the enum member.
        This gets a bit tricky because the type specified in here is not always
        going to be an "int". It will usually point to an lvgl type and the type
        of the lvgl type can be found in the ``"typedefs"`` section.
      - ``"docstring"``: you should know what this is.
      - ``"value"``: the enumeration member/item's value


  - ``"lvgl_type"``: This is a base type. There or no other types beneith this.
    This tells you that the type is an LVGL data type.

    Available JSON fields:
      - ``"name"``: The name of the type.
      - ``"quals"``: array of qualifiers, IE "const


  - ``"struct"``: Describes a structure

    Available JSON fields:
      - ``"name"``: The name of the structure.
      - ``"type"``: This contains the primitive type information for the structure.
      - ``"docstring"``: you should know what this is.
      - ``"fields"``: array of ``"field"`` elements.
      - ``"quals"``: array of qualifiers, IE "const"


  - ``"union"``: Describes a union

    Available JSON fields:
      - ``"name"``: The name of the union.
      - ``"type"``: This contains the primitive type information for the union.
      - ``"docstring"``: you should know what this is.
      - ``"fields"``: array of ``"field"`` elements.
      - ``"quals"``: array of qualifiers, IE "const"


  - ``"macro"``: describes a macro. There is limited information that can be
    collected about macros and in most cases a binding will need to have these
    statically added to a binding. It is more for collecting the docstrings than
    anything else.

    Available JSON fields:
      - ``"name"``: The name of the macro.
      - ``"docstring"``: you should know what this is.


  - ``"ret_type"``: return type from a function. This is only going to be seen in the ``"type"``
    element of a ``"function"`` type.

    Available JSON fields:
      - ``"type"``: This contains the type information for the field. Check the
        ``"json_type"`` to know what type you are dealing with.
      - ``"docstring"``: you should know what this is.


  - ``"function"``: Describes a function.

    Available JSON fields:
      - ``"name"``: The name of the function.
      - ``"type"``: This contains the type information for the return value.
      - ``"docstring"``: you should know what this is.
      - ``"args"``: array of ``"arg"`` json types. This describes the fuction arguments/parameters.


  - ``"stdlib_type"``: This is a base type, meaning that there are no more
    type levels beneith this. This tells us that the type is from the C stdlib.

    Available JSON fields:
      - ``"name"``: The name of the type.
      - ``"quals"``: array of qualifiers, IE "const


  - ``"unknown_type"``: This should not be seen. If it is then there needs to be
    an adjustment made to the script. Please open an issue and let us know if you see this type.

    Available JSON fields:
      - ``"name"``: The name of the type.
      - ``"quals"``: array of qualifiers, IE "const


  - ``"pointer"``: This is a wrapper object to let you know that the type you
    are dealing with is a pointer

    Available JSON fields:
      - ``"type"``: This contains the type information for the pointer. Check the
        ``"json_type"`` to know what type you are dealing with.
      - ``"quals"``: array of qualifiers, IE "const", may or may not be available.


  - ``"typedef"``: type definitions. I will explain more on this below.

    Available JSON fields:
      - ``"name"``: The name of the typedef.
      - ``"type"``: This contains the type information for the field. Check the
        ``"json_type"`` to know what type you are dealing with.
      - ``"docstring"``: you should know what this is.
      - ``"quals"``: array of qualifiers, IE "const"



Here is an example of what the output will look like.

.. code:: json

    {
        "enums":[
            {
                "name":"_lv_result_t",
                "type":{
                    "name":"int",
                    "json_type":"primitive_type"
                },
                "json_type":"enum",
                "docstring":"LVGL error codes. ",
                "members":[
                    {
                        "name":"LV_RESULT_INVALID",
                        "type":{
                            "name":"_lv_result_t",
                            "json_type":"lvgl_type"
                        },
                        "json_type":"enum_member",
                        "docstring":"",
                        "value":"0x0"
                    },
                    {
                        "name":"LV_RESULT_OK",
                        "type":{
                            "name":"_lv_result_t",
                            "json_type":"lvgl_type"
                        },
                        "json_type":"enum_member",
                        "docstring":"",
                        "value":"0x1"
                    }
                ]
            }
        ],
        "functions":[
            {
                "name":"lv_version_info",
                "type":{
                    "type":{
                        "type":{
                            "name":"char",
                            "json_type":"primitive_type",
                            "quals":[
                                "const"
                            ]
                        },
                        "json_type":"pointer",
                        "quals":[]
                    },
                    "json_type":"ret_type",
                    "docstring":""
                },
                "json_type":"function",
                "docstring":"",
                "args":[
                    {
                        "name":null,
                        "type":{
                            "name":"void",
                            "json_type":"primitive_type",
                            "quals":[]
                        },
                        "json_type":"arg",
                        "docstring":"",
                        "quals":[]
                    }
                ]
            }
        ],
        "function_pointers":[
            {
                "name":"lv_tlsf_walker",
                "type":{
                    "type":{
                        "name":"void",
                        "json_type":"primitive_type",
                        "quals":[]
                    },
                    "json_type":"ret_type",
                    "docstring":""
                },
                "json_type":"function_pointer",
                "docstring":"",
                "args":[
                    {
                        "name":"ptr",
                        "type":{
                            "type":{
                                "name":"void",
                                "json_type":"primitive_type",
                                "quals":[]
                            },
                            "json_type":"pointer",
                            "quals":[]
                        },
                        "json_type":"arg",
                        "docstring":""
                    },
                    {
                        "name":"size",
                        "type":{
                            "name":"size_t",
                            "json_type":"stdlib_type",
                            "quals":[]
                        },
                        "json_type":"arg",
                        "docstring":""
                    },
                    {
                        "name":"used",
                        "type":{
                            "name":"int",
                            "json_type":"primitive_type",
                            "quals":[]
                        },
                        "json_type":"arg",
                        "docstring":""
                    },
                    {
                        "name":"user",
                        "type":{
                            "type":{
                                "name":"void",
                                "json_type":"primitive_type",
                                "quals":[]
                            },
                            "json_type":"pointer",
                            "quals":[]
                        },
                        "json_type":"arg",
                        "docstring":""
                    }
                ],
                "quals":[]
            }
        ],
        "structures":[
            {
                "name":"_lv_gradient_cache_t",
                "type":{
                    "name":"struct",
                    "json_type":"primitive_type"
                },
                "json_type":"struct",
                "docstring":null,
                "fields":[
                    {
                        "name":"color_map",
                        "type":{
                            "type":{
                                "name":"lv_color_t",
                                "json_type":"lvgl_type",
                                "quals":[]
                            },
                            "json_type":"pointer",
                            "quals":[]
                        },
                        "json_type":"field",
                        "bitsize":null,
                        "docstring":""
                    },
                    {
                        "name":"opa_map",
                        "type":{
                            "type":{
                                "name":"lv_opa_t",
                                "json_type":"lvgl_type",
                                "quals":[]
                            },
                            "json_type":"pointer",
                            "quals":[]
                        },
                        "json_type":"field",
                        "bitsize":null,
                        "docstring":""
                    },
                    {
                        "name":"size",
                        "type":{
                            "name":"uint32_t",
                            "json_type":"stdlib_type",
                            "quals":[]
                        },
                        "json_type":"field",
                        "bitsize":null,
                        "docstring":""
                    }
                ]
            }
        ],
        "unions":[],
        "variables":[
            {
                "name":"lv_global",
                "type":{
                    "name":"lv_global_t",
                    "json_type":"lvgl_type",
                    "quals":[]
                },
                "json_type":"variable",
                "docstring":"",
                "quals":[],
                "storage":[
                    "extern"
                ]
            }
        ],
        "typedefs":[
            {
                "name":"lv_pool_t",
                "type":{
                    "type":{
                        "name":"void",
                        "json_type":"primitive_type",
                        "quals":[]
                    },
                    "json_type":"pointer"
                },
                "json_type":"typedef",
                "docstring":"",
                "quals":[]
            }
        ],
        "forward_decls":[
            {
                "name":"lv_fragment_managed_states_t",
                "type":{
                    "name":"struct",
                    "json_type":"primitive_type"
                },
                "json_type":"forward_decl",
                "docstring":"",
                "quals":[]
            }
        ],
        "macros":[
            {
                "name":"ZERO_MEM_SENTINEL",
                "json_type":"macro",
                "docstring":""
            }
        ]
    }
