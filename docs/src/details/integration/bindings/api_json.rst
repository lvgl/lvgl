.. _output_api_as_json_data:

=======================
Output API as JSON Data
=======================

As of 20-Jun-2024, LVGL comes packaged with a Python script
(``./scripts/gen_json/gen_json.py``) that reads the header files in LVGL and outputs
a more friendly JSON format for the API.  This is done so that bindings that generate
code automatically will have an easy way to collect the needed information without
having to reinvent the wheel.  JSON format was chosen because there are libraries for
reading JSON data in almost every programming language.



Requirements
************

- Python >= 3.10
- Pycparser >= 2.22: Python Library for reading C preprocessor output
- PyMSVC >= 0.4.0: Python library for using the MSVC Compiler
- A C compiler:  gcc for Linux, clang for OSX and MSVC for Windows
- Doxygen:  used to read Doxygen comments (the API documentation) from the header files.



Usage
*****

Command-Line Options
--------------------

- ``--output-path``:  output directory for JSON file.  If one is not supplied then it
  will be output to stdout.
- ``--lvgl-config``:  path to lv_conf.h (including file name).  If this is not set then
  a config file will be generated that has the most common LVGL options turned on.
- ``--develop``:  leaves the files generated in the temporary folder in place.

Examples
--------

Normal usage:

.. code-block:: shell

    python ./scripts/gen_json/gen_json.py --output-path=json/output/directory --lvgl-config=path/to/lv_conf.h

If you want to run a subprocess from inside of a generation script and read the output from stdout:

.. code-block:: shell

    python ./scripts/gen_json/gen_json.py --lvgl-config=path/to/lv_conf.h

Output Data
-----------

The contents of the output file is a large JSON object (``{...}``) with the following
key/value pairs (these are the keys):

.. parsed-literal::

    {
        "enums"            : [...],
        "functions"        : [...],
        "function_pointers": [...],
        "structures"       : [...],
        "unions"           : [...],
        "variables"        : [...],
        "typedefs"         : [...],
        "forward_decls"    : [...],
        "macros"           : [...]
    }

As you can see, the value of each of these elements is an array.  The elements in
each array are JSON objects, each with a structure unique to the type indicated by
the parent element name (e.g. "enums", "functions", etc.).

A key/value pair has been added to each object (key = "json_type") to make it possible
to pass an object to a generic function and have each object know its own type through
this field.  The possible "json_type" values are:

- ``"array"``: The array type is used to identify arrays.

  Fields:
    - ``"dim"``: number of items in array
    - ``"quals"``: array of qualifiers, IE "const"
    - ``"type"``: This may or may not be available.
    - ``"name"``: name of data type


- ``"field"``: This type is used to describe fields in structures and unions.
  It is used in the ``"fields"`` array of the ``"struct"`` and ``"union"`` types
  covered below.

  Fields:
    - ``"name"``: field name
    - ``"type"``: data type
      ``"json_type"`` carries object type (e.g. "enum", "function", etc.) identifying the top-level group it comes from
    - ``"bitsize"``: The number of bits for bit-fields, or ``null`` for normal field types.
    - ``"docstring"``: string containing Doxygen-extracted documentation


- ``"arg"``: Describes a function argument

  Fields:
    - ``"name"``: argument name
    - ``"type"``: data type
      ``"json_type"`` carries object type (e.g. "enum", "function", etc.) identifying the top-level group it comes from.
    - ``"docstring"``: string containing Doxygen-extracted documentation
    - ``"quals"``: array of any qualifiers present, e.g. "const"


- ``"forward_decl"``: Describes a forward declaration.  There are structures in
  LVGL that are considered to be private and that is what these desccribe.

  Fields:
    - ``"name"``: name of forward declaration
    - ``"type"``: data type
      ``"json_type"`` carries object type (e.g. "enum", "function", etc.) identifying the top-level group it comes from.
    - ``"docstring"``: string containing Doxygen-extracted documentation
    - ``"quals"``: array of any qualifiers present, e.g. "const"


- ``"function_pointer"``: Describes a function pointer.  These are used when
  registering callback functions in LVGL.

  Fields:
    - ``"name"``: name of function pointer
    - ``"type"``: function return type
    - ``"docstring"``: string containing Doxygen-extracted documentation
    - ``"args"``: array of ``"arg"`` objects described above
    - ``"quals"``: array of any qualifiers present, e.g. "const"


- ``"variable"``: Describes a global variable.

  Fields:
    - ``"name"``: variable name
    - ``"type"``: data type
      ``"json_type"`` carries object type (e.g. "enum", "function", etc.) identifying the top-level group it comes from.
    - ``"docstring"``: string containing Doxygen-extracted documentation
    - ``"quals"``: array of any qualifiers present, e.g. "const"
    - ``"storage"``: array of any storage-class specifiers present (e.g. "auto", "static", "extern", etc.)


- ``"special_type"``:  Currently only used to describe an ellipsis argument of a function.

  Fields:
    - ``"name"``: always "ellipsis"


- ``"primitive_type"``: Data type that does not begin with ``"lv_"`` and end with
  ``"_t"``.  Compare to ``"lvgl_type"``  This includes struct, union, integral types
  (e.g. int, unsigned int), etc..

  Fields:
    - ``"name"``: name of primitive type


- ``"enum"``: C enumerations

  Fields:
    - ``"name"``: If enumeration is the result of a ``typedef``, this field carries
      the type name defined.  Example:  ``lv_align_t``.  (Not always available.)
    - ``"type"``: type of enumerators (always "int")
    - ``"docstring"``: string containing Doxygen-extracted documentation
    - ``"members"``: array of ``"enum_member"`` objects


- ``"enum_member"``: enumerator (enumeration value).  This "json_type" is only found
  in the ``"members"`` array of an ``"enum"`` object

  Fields:
    - ``"name"``: enumerator name
    - ``"type"``: If enumeration is the result of a ``typedef``, this field carries
      the type name defined.  Example:  ``lv_align_t``.
    - ``"docstring"``: string containing Doxygen-extracted documentation
    - ``"value"``: enumerator value


- ``"lvgl_type"``: Data type defined in LVGL (begins with ``"lv_"`` and ends with ``"_t"``.

  Fields:
    - ``"name"``: type name
    - ``"quals"``: array of any qualifiers present, e.g. "const"


- ``"struct"``: C struct

  Fields:
    - ``"name"``: struct name (data type if defined by ``typedef``)
    - ``"type"``: a "primitive_type" object {"name": "struct", "json_type": "primitive_type"}.  (See definition above.)
    - ``"docstring"``: string containing Doxygen-extracted documentation
    - ``"fields"``: array of ``"field"`` objects (See definition above.)
    - ``"quals"``: array of any qualifiers present, e.g. "const"


- ``"union"``: C union

  Fields:
    - ``"name"``: union name (data type if defined by ``typedef``)
    - ``"type"``: a "primitive_type" object {"name": "union", "json_type": "primitive_type"}.  (See definition above.)
    - ``"docstring"``: string containing Doxygen-extracted documentation
    - ``"fields"``: array of ``"field"`` elements.
    - ``"quals"``: array of any qualifiers present, e.g. "const"


- ``"macro"``: C macro.  There is limited information that can be
  collected about macros and in most cases a binding will need to have these
  statically added to a binding.  It is more for collecting the docstrings than
  anything else.

  Fields:
    - ``"name"``: macro name
    - ``"docstring"``: string containing Doxygen-extracted documentation


- ``"ret_type"``: return type from a function. This is only going to be seen in the ``"type"``
  element of a ``"function"`` type.

  Fields:
    - ``"type"``: data type
      ``"json_type"`` carries object type (e.g. "enum", "function", etc.) identifying the top-level group it comes from.
    - ``"docstring"``: string containing Doxygen-extracted documentation


- ``"function"``: C function

  Fields:
    - ``"name"``: function name
    - ``"type"``: A "ret_type" object.  (See definition above.)
    - ``"docstring"``: string containing Doxygen-extracted documentation
    - ``"args"``: array of ``"arg"`` json types.  (See definition above.)


- ``"stdlib_type"``:  C integral type (int, unsigned int, float, etc.)

  Fields:
    - ``"name"``: type name
    - ``"quals"``: array of any qualifiers present, e.g. "const"


- ``"unknown_type"``: This should not be seen. If it is then there needs to be
  an adjustment made to the script. Please open an issue and let us know if you see this type.

  Fields:
    - ``"name"``: type name
    - ``"quals"``: array of any qualifiers present, e.g. "const"


- ``"pointer"``: C pointer

  Fields:
    - ``"type"``: pointer type
      ``"json_type"`` carries object type (e.g. "enum", "function", etc.) identifying the top-level group it comes from.
    - ``"quals"``: array of any qualifiers present, e.g. "const"


- ``"typedef"``: C type definition

  Fields:
    - ``"name"``: type name (e.g. ``lv_part_t``)
    - ``"type"``: a "primitive_type" object {"name": "uint32_t", "json_type": "stdlib_type"}.  (See definition above.)
      ``"json_type"`` carries object type (e.g. "enum", "function", etc.) identifying the top-level group it comes from.
    - ``"docstring"``: string containing Doxygen-extracted documentation
    - ``"quals"``: array of any qualifiers present, e.g. "const"



Here is a shortened example of what the output looks like.

.. code-block:: json

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
                "name":"_lv_grad_cache_t",
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
