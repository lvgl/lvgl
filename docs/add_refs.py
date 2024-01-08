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
        elif src.endswith('.rst'):
            res.append((src, dst))
    if res:
        dp = p.replace(path, output_path)
        if not os.path.exists(dp):
            os.makedirs(dp)

    return res


heading = '==='  # above and below  1
title = '###'  # above and below  2
chapter = '***'  # above and below  3
section = '==='  # only below  4
sub_section1 = '---'  # only below  5
sub_section2 = '***'  # below  5
subsub_section1 = '^^^'  # only below  6
subsub_section2 = '~~~'  # only below  6
paragraph = '"""'  # only below  7


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

        found_above = None
        text = None
        found_below = None

        def _eval():
            if found_above is not None and found_above == found_below:
                if found_above.startswith(heading):
                    pass
                elif found_above.startswith(title):
                    pass
                elif found_above.startswith(chapter):
                    pass
                else:
                    print(src_file)
                    raise RuntimeError()

                ref = f'.. _{ref_file[1:]}/{text.lower().strip().replace(":", "")}:'
                directive = f':ref:`{ref_file[1:]}/{text.lower().strip().replace(":", "")}`'

                ref_count = 1
                while ref in used:
                    ref = f'.. _{ref_file[1:]}/{text.lower().strip().replace(":", "")}{ref_count}:'
                    directive = f':ref:`{ref_file[1:]}/{text.lower().strip().replace(":", "")}{ref_count}`'
                    ref_count += 1

                used.append(ref)
                output.append(ref)
                output.append('')
                output.append(found_above)
            else:
                if found_below.startswith(section):
                    pass
                elif found_below.startswith(sub_section1):
                    pass
                elif found_below.startswith(sub_section2):
                    pass
                elif found_below.startswith(subsub_section1):
                    pass
                elif found_below.startswith(subsub_section2):
                    pass
                elif found_below.startswith(paragraph):
                    pass
                else:
                    print('ERROR:', src_file)
                    raise RuntimeError(found_below)

                ref = f'.. _{ref_file[1:]}/{text.lower().strip().replace(":", "")}:'
                directive = f':ref:`{ref_file[1:]}/{text.lower().strip().replace(":", "")}`'

                ref_count = 1
                while ref in used:
                    ref = f'.. _{ref_file[1:]}/{text.lower().strip().replace(":", "")}{ref_count}:'
                    directive = f':ref:`{ref_file[1:]}/{text.lower().strip().replace(":", "")}{ref_count}`'
                    ref_count += 1

                used.append(ref)

                if found_above is None:
                    output.append(ref)
                else:
                    output.append(found_above)
                    output.append('')
                    output.append(ref)

                output.append('')

            return ref, directive

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
                    r, d = _eval()

                    found_above = text
                    text = found_below
                    found_below = None

                    if print_references:
                        print(r)
                        print(d)
                        print()

                if found_above is not None:
                    output.append(found_above)

                found_above = text
                text = line

        if found_below and (text is None or not text.strip()):
            if text is not None:
                output.append(text)

            text = found_below
            found_below = None

        elif found_below and text is not None and text.strip():
            r, d = _eval()

            found_above = text
            text = found_below
            found_below = None

            if print_references:
                print(r)
                print(d)
                print()

        if found_above is not None:
            output.append(found_above)

        if text is not None:
            output.append(text)

        if found_below is not None:
            output.append(found_below)

        if not print_references:
            output = '\n'.join(output)

            with open(dst_file, 'wb') as f:
                f.write(output.encode('utf-8'))




