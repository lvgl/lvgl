import argparse
import gdb


class DumpCache(gdb.Command):
    """dump cache info for specified cache"""

    def __init__(self):
        super(DumpCache, self).__init__(
            "dump cache", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description="Dump lvgl cache info.")
        parser.add_argument(
            "cache",
            type=str,
            choices=["image", "image_header"],
            default="image",
            help="cache to dump.",
        )

        from lvglgdb import curr_inst

        try:
            args = parser.parse_args(gdb.string_to_argv(args))
        except SystemExit:
            return

        cache = None
        if args.cache == "image":
            cache = curr_inst().image_cache()
        elif args.cache == "image_header":
            cache = curr_inst().image_header_cache()

        if not cache:
            print("Invalid cache: ", args.cache)
            return

        cache.print_entries()
