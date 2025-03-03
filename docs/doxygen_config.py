"""doxygen_config.py
Python Interface to Doxygen Config Files (Doxyfiles)

Author   :  "Victor Wheeler"
Copyright:  "Copyright (C) 2025 WGA Crystal Research, Inc."
License  :  "MIT"
Version  :  "1.0"

This work was inspired by the `doxygen-python-interface` project at
https://github.com/TraceSoftwareInternational/doxygen-python-interface.
On 27-Feb-2025 I was engaged in a production project wherein I wanted
to find a Python module that I could re-use to reliably work with
Doxygen configuration files (Doxyfiles).  The best one I found was
`doxygen-python-interface`.  Unfortunately, the ``configParser`` from
that project could not be used because it both had important bugs and
design flaws in it (conflicts with legal Doxygen config syntax), and
it appears to have been abandoned after 26-Apr-2018, preventing these
things from being remedied.

So a brand-new module has been created herewith based on sound O-O design
principles and a design that actually works in alignment with Doxygen
configuration syntax.

Usage:

    import doxygen_config
    ...
    # 1. Load configuration from Doxyfile.
    cfg = doxygen_config.DoxygenConfig()
    cfg.load(doxyfile_src_file)

    # 2. Get a list of Doxygen option names.
    opt_list = cfg.options()
    ok_to_proceed = cfg.is_valid_option('PREDEFINED') \
        and cfg.is_valid_option('INPUT')

    # 3. Update it.
    if ok_to_proceed:
        temp = cfg.value('PREDEFINED')
        temp = temp.replace('<<LV_CONF_PATH>>', lv_conf_file)
        cfg.set('PREDEFINED', temp)

        temp = cfg.value('INPUT')
        temp = temp.replace('<<SRC>>', f'"{lvgl_src_dir}"')
        cfg.set('INPUT', temp)

    # 4. Save it.
    # The original comments and order of config options are preserved.
    # The ``bare`` argument discards comments from the output.
    cfg.save(cfg_dict, doxyfile_dst_file, bare=True)

Design Differences from `doxygen-python-interface`:

    - The DoxygenConfig class represents the actual Doxygen configuration,
      in alignment with O-O theory --- it is not just a place to store a
      set of functions that never needed to be a class.

    - If the user does a default ``save()`` (not requesting a "bare"
      version of the Doxygen configuration), the saved Doxyfile
      should be a binary match to the original Doxyfile loaded.

      Exceptions:

      1.  Any trailing whitespace in original Doxyfile after the ``=``
          on empty options is not preserved.

      2.  Multi-line lists that had unaligned backslashes after them like this:

            EXCLUDE_PATTERNS       = */libs/barcode/code* \
                                     */libs/freetype/ft*  \
                                     */libs/gif/gif*      \
                                     */libs/lodepng/lode* \
                                     */libs/qrcode/qr* \
                                     */libs/thorvg/*  \
                                     */libs/tiny_ttf/stb* \
                                     */libs/tjpgd/tjp* \
                                     */others/vg_lite_tvg/vg*

          will be saved like this:

            EXCLUDE_PATTERNS       = */libs/barcode/code*      \
                                     */libs/freetype/ft*       \
                                     */libs/gif/gif*           \
                                     */libs/lodepng/lode*      \
                                     */libs/qrcode/qr*         \
                                     */libs/thorvg/*           \
                                     */libs/tiny_ttf/stb*      \
                                     */libs/tjpgd/tjp*         \
                                     */others/vg_lite_tvg/vg*

      ``doxygen-python-interface`` did not save the comments so an
      "edit in place" of a Doxyfile could be catastrophic if the
      comments were needed as they often are in production scenarios.

    - The ``save()`` method has an optional ``bare`` argument (default False)
      that can be used to save a "bare" version of the Doxyfile options,
      discarding the comments from the currently-loaded Doxyfile.

    - Input values are preserved exactly as they were found.  The
      `doxygen-python-interface`'s ``configParser`` class removed
      quotation marks from incoming values and added quotation marks
      to values containing spaces before storing them again.  While
      this "sounds nice", it was incompatible with Doxygen for every
      type of item that could have a "list" as a value, such as the
      PREDEFINED and ABBREVIATE_BRIEF options.

      Examples:

        PREDEFINED             = USE_LIST USE_TABLE USE_CHART

        PREDEFINED             = DOXYGEN LV_CONF_PATH="/path with spaces/to/lv_conf.h"

        PREDEFINED             = DOXYGEN \
                                 LV_CONF_PATH="/path with spaces/to/lv_conf.h"

      These are all valid values for the PREDEFINED option and
      MUST NOT have quotes around any of them!

      Thus, it is up to the user to know when values he is changing
      have space(s) AND ALSO need quotes and take appropriate measures
      by adding quotes when needed and not otherwise.

    - The storage of the list of Doxygen options is encapsulated
      in the instance of the DoxygenConfig class instead of being
      returned as a dictionary from the ``load...()`` function.
      Its values are readable and writeable via methods.  The
      end user is not able to add options that were not part
      of the original input Doxyfile, nor remove options that were
      part of the original input Doxyfile.  This gives some level of
      control on retaining valid Doxygen options.

      It is an error to attempt to set a value with an option name
      that does not exist in the configuration.  A NameError
      exception is raised if it is attempted.

      While Doxygen options change from time to time, it is up to the
      end user to use ``doxygen -u Doxyfile`` to keep his input
      Doxyfile(s) up to date.

Storage:

    The actual configuration values are represented in an internal
    dictionary not intended to be accessed directly by the typical end
    user.  The keys are the Doxygen option names and the values are:

    - str :  single values with possibly embedded spaces
    - list:  multi-line values with possibly embedded spaces

    Quotation marks are neither removed nor added, so it is up to the
    user to set values compatible with Doxygen configuration syntax.
    This also makes it okay for multi-line values to have more than one
    value per line:  if it is okay by Doxygen, then it is okay by
    the DoxygenConfig class.

    If the user sets an option value passing a list, those values
    will be represented as a multi-line value in the saved Doxyfile.

The Philosophy of Removing Quotation Marks Is Not Workable for Doxygen:

    When one asks, "Is it appropriate to remove the quotation marks?"
    What if a value looked like this (2 quoted items in one line),
    removing quotation marks would be an error:

        "abc def" "ghi jkl"

    The ABBREVIATE_BRIEF list could indeed appear like this.

    If it were argued that all multi-value items should be formatted as
    multi-line lists, then quotation marks theory works, as the
    ABBREVIATE_BRIEF option does not require quotation marks around
    every value.

    However, since Doxygen does not require this, there is still a
    strong argument for not tampering with quotation marks at all
    when importing values.  The strongest reasons are:

    -   Doxygen can and does accept values like this where the value
        of an option can be a list:

            "abc def" "ghi jkl"

    -   If the end user is going to set values with spaces in them,
        it could be made the user's responsibility to know when
        there are spaces and thus include quotes when needed.

    In the end, the "do not tamper with quotation marks" argument wins
    for sake of reliability.  So the policy is:  quotation marks are
    neither removed nor added.  It is up to the user to know when they
    are needed and add them himself.
"""
import logging
import os
import re


__author__ = "Victor Wheeler"
__copyright__ = "Copyright (C) 2025 WGA Crystal Research, Inc."
__license__ = "MIT"
__version__ = "1.0"


class ParseException(Exception):
    """Exception thrown upon unexpected parsing errors."""
    pass


class DoxygenConfig:
    """Doxygen Configurations (from/to Doxyfiles)"""

    def __init__(self):
        """Prepare instantiated DoxygenConfig for use."""
        # Regexes used during Doxyfile parsing
        self._re_single_line_option = re.compile(r'^\s*(\w+)\s*=\s*([^\\]*)\s*$')
        self._re_top_of_multiline_option = re.compile(r'^\s*(\w+)\s*=\s*(|.*\S)\s*\\$')
        # Doxygen cfg items by option name
        self._cfg_items_dict = {}
        # Comments by name of option below it.
        # Comments at end of file have key 'self._end_key'.
        self._cfg_comments_dict = {}
        # Key used for comments found after last option in Doxyfile
        self._end_key = 'END'
        # Configuration to match Doxygen -g output (template Doxyfile)
        self._char_count_before_equals = 23

    def load(self, doxyfile: str):
        """Load options and comments from `doxyfile`

        :param    doxyfile: Path to doxyfile

        :raise FileNotFoundError: When doxyfile not found
        :raise ParseException:    When there is a parsing error
        """

        if not os.path.exists(doxyfile):
            logging.error(f'Doxyfile not found {doxyfile}.')
            raise FileNotFoundError(doxyfile)

        self._cfg_items_dict.clear()
        self._cfg_comments_dict.clear()

        # Default encoding:  UTF-8.
        with open(doxyfile, 'r') as file:
            in_multiline_opt = False
            multiline_opt_name_bep = None   # "bep" = "being processed"
            accumulated_other_lines = []

            for line in file.readlines():
                line = line.strip()

                if in_multiline_opt:
                    # There are 2 ways this list can end:
                    # 1.  the normal way when last item has no trailing `\`, or
                    # 2.  the last item has a trailing `\` and there is a blank-
                    #     or comment-line after it, which should NOT be added
                    #     to the list, but instead signal end-of-list.
                    if not line.endswith('\\'):
                        in_multiline_opt = False

                    val = line.rstrip('\\').strip()

                    if self._bool_comment_or_blank_line(val):
                        accumulated_other_lines.append(line)
                        in_multiline_opt = False
                    else:
                        self._cfg_items_dict[multiline_opt_name_bep].append(val)

                elif self._bool_comment_or_blank_line(line):
                    accumulated_other_lines.append(line)

                elif self._bool_top_of_multiline_option(line):
                    multiline_opt_name_bep, val = self._parse_multiline_option(line)
                    self._cfg_items_dict[multiline_opt_name_bep] = [val]
                    self._cfg_comments_dict[multiline_opt_name_bep] = accumulated_other_lines
                    accumulated_other_lines = []
                    in_multiline_opt = True

                elif self._bool_single_line_option(line):
                    option_name, val = self._parse_single_line_option(line)
                    self._cfg_items_dict[option_name] = val
                    self._cfg_comments_dict[option_name] = accumulated_other_lines
                    accumulated_other_lines = []

            # Any comments or blank lines found after last Doxygen option
            # are represented in _cfg_comments_dict with key `self._end_key`.
            if accumulated_other_lines:
                self._cfg_comments_dict[self._end_key] = accumulated_other_lines
                accumulated_other_lines.clear()

    def save(self, doxyfile: str, bare=False):
        """Save configuration to `doxyfile`.

        :param doxyfile:  Output path where Doxygen configuration will be
                            written. Overwrites file if it exists.
        :param bare:      Do not preserve comments from loaded file.
        """

        lines = []

        for option_name, val in self._cfg_items_dict.items():
            if not bare:
                lines.extend(self._cfg_comments_dict[option_name])

            if type(val) is list:
                # We will be aligning the backslashes after the
                # items in the list, so we need to know the longest.
                # First value in list:
                multi_line_indent = ' ' * (self._char_count_before_equals + 2)
                longest_len = len(max(val, key=len))
                val_w_len = val[0].ljust(longest_len)
                lines.append(f'{option_name:<23}= {val_w_len}  \\')

                # Next n-2 values in list:
                if len(val) > 2:
                    for temp in val[1:-1]:
                        val_w_len = temp.ljust(longest_len)
                        lines.append(f'{multi_line_indent}{val_w_len}  \\')

                # Last value in list:
                lines.append(f'{multi_line_indent}{val[-1]}')
            elif type(val) is str:
                val_w_len = option_name.ljust(self._char_count_before_equals)
                if len(val) == 0:
                    lines.append(f'{val_w_len}=')
                else:
                    lines.append(f'{val_w_len}= {val}')

        if self._end_key in self._cfg_comments_dict:
            if not bare:
                lines.extend(self._cfg_comments_dict[self._end_key])

        # Ensure there is exactly 1 newline at end of file.
        lines.append('')

        with open(doxyfile, 'w') as file:
            file.write('\n'.join(lines))

        logging.debug(f'Saved configuration to [{doxyfile}].')

    def option_names(self):
        """List of contained Doxygen option names"""
        return self._cfg_items_dict.keys()

    def is_valid_option(self, option_name: str) -> bool:
        """Is `option_name` a valid option name?"""
        return option_name in self._cfg_items_dict

    def set(self, option_name: str, val: str or list):
        """Set value of specified option

        :param option_name:  Name of Doxygen option whose value to fetch
        :param val:          Value to set
                               - str  = single-line value;
                               - list = multi-line value.

        :raises NameError:   When ``name`` is not found.
        """
        if option_name in self._cfg_items_dict:
            self._cfg_items_dict[option_name] = val
            if type(val) is list:
                logging.debug(f'Item [{option_name}] set to list.')
            else:
                logging.debug(f'Item [{option_name}] set to [{val}].')
        else:
            logging.error(f'Doxyfile option {option_name} not found.')
            raise NameError(f'Doxygen option {option_name} not found.')

    def value(self, option_name: str) -> str or list:
        """Value of specified option

        :param option_name:  Name of Doxygen option whose value to fetch

        :returns string:  single-line value
        :returns list:    multi-line value
        :returns None:    When ``option_name`` is not found.
        """
        if option_name in self._cfg_items_dict:
            result = self._cfg_items_dict[option_name]
            logging.debug(f'Item [{option_name}] fetched.')
        else:
            result = None
            logging.debug(f'Item [{option_name}] not found.')

        return result

    def _parse_multiline_option(self, line) -> (str, str):
        """Extract option name and first line of value of multi-line option.

        :param    line: line to parse
        :return:  name and first line of multi-line option
        :raise    ParseException: When process fail to extract data
        """

        matches = self._re_top_of_multiline_option.search(line)
        if matches is None or len(matches.groups()) != 2:
            logging.error(f'Error extracting first value in multi-line option from [{line}].')
            raise ParseException(f'Error extracting first value in multi-line option from [{line}].')

        return matches.group(1), matches.group(2)

    def _parse_single_line_option(self, line) -> (str, str):
        """Extract option name and value of single line option.

        :param line:  line to parse
        :return:      option name and value
        :raise        ParseException: When process fail to extract data
        """

        matches = self._re_single_line_option.search(line)

        if matches is None or len(matches.groups()) != 2:
            logging.error(f'Error extracting option name and value from [{line}].')
            raise ParseException(f'Error extracting option name and value from [{line}].')

        return matches.group(1), matches.group(2)

    def _bool_single_line_option(self, line: str) -> bool:
        return self._re_single_line_option.match(line) is not None

    def _bool_comment_or_blank_line(self, line: str) -> bool:  # NoQA
        return line.startswith("#") or (len(line) == 0)

    def _bool_top_of_multiline_option(self, line) -> bool:
        return self._re_top_of_multiline_option.match(line) is not None
