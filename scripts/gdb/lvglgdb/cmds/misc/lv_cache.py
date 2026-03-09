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

        from lvglgdb.lvgl.formatter import print_info, print_spec_table

        # Print cache-level info
        snap = cache.snapshot()
        print_info(snap)

        # Print cache entries
        snaps = cache.snapshots()
        extra_fields = getattr(cache, "_last_extra_fields", [])

        col_align = {"src": "l", "type": "c"}

        extra_columns = ["entry"] + extra_fields if extra_fields else ["entry"]

        def _extra_row(d):
            extras = d.get("extra_fields", {})
            extra_vals = [extras.get(f, "") for f in extra_fields]
            return [d["entry_addr"]] + extra_vals

        print_spec_table(snaps,
                         align="r", numbered=False, col_align=col_align,
                         extra_columns=extra_columns,
                         extra_row_fn=_extra_row)


class CheckPrefix(gdb.Command):
    """prefix command for check subcommands"""

    def __init__(self):
        super(CheckPrefix, self).__init__(
            "check", gdb.COMMAND_USER, gdb.COMPLETE_NONE, True
        )

    def invoke(self, args, from_tty):
        gdb.execute("help check")


class CheckCache(gdb.Command):
    """run sanity check on specified cache"""

    def __init__(self):
        super(CheckCache, self).__init__(
            "check cache", gdb.COMMAND_USER, gdb.COMPLETE_EXPRESSION
        )

    def invoke(self, args, from_tty):
        parser = argparse.ArgumentParser(description="Run cache sanity check.")
        parser.add_argument(
            "cache",
            type=str,
            choices=["image", "image_header"],
            default="image",
            help="cache to check.",
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

        cache.sanity_check()
