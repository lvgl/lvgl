from lvglgdb.value import Value


def dump_style_info(style: Value):
    prop = int(style.prop)
    value = style.value
    print(f"{prop} = {value}")
