import os

path = os.path.abspath(os.path.dirname(__file__))
output_path = ''


def iter_path(p):
    res = []
    for file in os.listdir(p):
        src = os.path.join(p, file)
        dst = src.replace(path, output_path)
        if os.path.isdir(src):
            res.extend(iter_path(src))
        elif src.endswith('.rst') and 'index.rst' not in src:
            res.append((src, dst))
    if res:
        dp = p.replace(path, output_path)
        if not os.path.exists(dp):
            os.makedirs(dp)

    return res


heading = '==='  # above and below
title = '###'  # above and below
chapter = '***'  # above and below
section = '==='  # only below
sub_section1 = '---'  # only below
sub_section2 = '***'  # below
subsub_section1 = '^^^'  # only below
subsub_section2 = '~~~'  # only below
paragraph = '"""'  # only below


def check_line(lne):
    for item in (
        heading,
        title,
        chapter,
        sub_section1,
        subsub_section1,
        subsub_section2,
        paragraph
    ):

        if lne.startswith(item):
            return True

    return False


def run(out_path, print_references):
    global output_path

    output_path = out_path

    files = iter_path(path)
    used = []

    for src_file, dst_file in files:
        with open(src_file, 'rb') as f:
            data = f.read().decode('utf-8')

        ref_file = src_file.replace(path, '')[:-4].lower().replace('\\', '/')

        data = data.split('\n')

        output = []

        level = 0
        section_count = 0
        level_count = 0

        found_above = None
        text = None
        found_below = None

        for line in data:
            if check_line(line):
                found_below = line
            else:
                if found_below and (text is None or not text.strip()):
                    if text is not None:
                        output.append(text)

                    text = found_below
                    found_below = None

                elif found_below and text is not None and text.strip():
                    if found_above is not None and found_above == found_below:
                        if found_above.startswith(heading):
                            if level < 1:
                                level_count += 1
                            elif level > 1:
                                level_count -= 1
                                section_count += 1

                            level = 1
                        elif found_above.startswith(title):
                            if level < 2:
                                level_count += 1
                            elif level > 2:
                                level_count -= 1
                                section_count += 1

                            level = 2
                        elif found_above.startswith(chapter):
                            if level < 3:
                                level_count += 1
                            elif level > 3:
                                level_count -= 1
                                section_count += 1

                            level = 3
                        else:
                            print(src_file)
                            raise RuntimeError()

                        ref = f'.. _{ref_file[1:]}/{text.lower().strip().replace(":", "")} [{section_count}-{level}]:'

                        directive = f':ref:`{ref_file[1:]}/{text.lower().strip().replace(":", "")} [{section_count}-{level_count}]`'

                        if ref in used:
                            raise RuntimeError(ref)

                        used.append(ref)
                        output.append(ref)
                        output.append('')
                        output.append(found_above)
                        found_above = text
                        text = found_below
                    else:
                        if found_below.startswith(section):
                            if level < 4:
                                level_count += 1
                            elif level > 4:
                                level_count -= 1
                                section_count += 1

                            level = 4
                        elif found_below.startswith(sub_section1):
                            if level < 5:
                                level_count += 1
                            elif level > 5:
                                level_count -= 1
                                section_count += 1

                            level = 5
                        elif found_below.startswith(sub_section2):
                            if level < 5:
                                level_count += 1
                            elif level > 5:
                                level_count -= 1
                                section_count += 1

                            level = 5
                        elif found_below.startswith(subsub_section1):
                            if level < 6:
                                level_count += 1
                            elif level > 6:
                                level_count -= 1
                                section_count += 1

                            level = 6
                        elif found_below.startswith(subsub_section2):
                            if level < 6:
                                level_count += 1
                            elif level > 6:
                                level_count -= 1
                                section_count += 1

                            level = 6
                        elif found_below.startswith(paragraph):
                            if level < 7:
                                level_count += 1

                            level = 7

                        else:
                            print('ERROR:', src_file)
                            print(found_below)
                            raise RuntimeError()

                        ref = f'.. _{ref_file[1:]}/{text.lower().strip().replace(":", "")} [{section_count}-{level_count}]:'
                        directive = f':ref:`{ref_file[1:]}/{text.lower().strip().replace(":", "")} [{section_count}-{level_count}]`'

                        if ref in used:
                            raise RuntimeError(ref)

                        used.append(ref)

                        if found_above is None:
                            output.append(ref)

                        else:
                            output.append(found_above)
                            output.append('')
                            output.append(ref)

                        output.append('')
                        found_above = text
                        text = found_below

                    found_below = None

                    if print_references:
                        print(ref)
                        print(directive)
                        print()

                if found_above is not None:
                    output.append(found_above)

                found_above = text
                text = line

        if not print_references:
            output = '\n'.join(output)

            with open(dst_file, 'wb') as f:
                f.write(output.encode('utf-8'))

