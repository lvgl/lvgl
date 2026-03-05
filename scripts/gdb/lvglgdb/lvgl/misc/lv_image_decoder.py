from prettytable import PrettyTable

from lvglgdb.value import Value, ValueInput


class LVImageDecoder(Value):
    """LVGL image decoder wrapper"""

    def __init__(self, decoder: ValueInput):
        super().__init__(Value.normalize(decoder, "lv_image_decoder_t"))

    @property
    def name(self) -> str:
        n = self.super_value("name")
        return n.string() if int(n) else "(unnamed)"

    @property
    def info_cb(self) -> Value:
        return self.super_value("info_cb")

    @property
    def open_cb(self) -> Value:
        return self.super_value("open_cb")

    @property
    def close_cb(self) -> Value:
        return self.super_value("close_cb")

    @property
    def get_area_cb(self) -> Value:
        return self.super_value("get_area_cb")

    @property
    def custom_draw_cb(self) -> Value:
        return self.super_value("custom_draw_cb")

    @property
    def user_data(self) -> Value:
        return self.super_value("user_data")

    @staticmethod
    def print_entries(decoders):
        """Print image decoders as a PrettyTable."""
        table = PrettyTable()
        table.field_names = ["#", "name", "info_cb", "open_cb", "close_cb"]
        table.align = "l"

        for i, dec in enumerate(decoders):
            table.add_row(
                [
                    i,
                    dec.name,
                    dec.info_cb.format_string(symbols=True),
                    dec.open_cb.format_string(symbols=True),
                    dec.close_cb.format_string(symbols=True),
                ]
            )

        if not table.rows:
            print("No registered image decoders.")
        else:
            print(table)
