#!/user/bin/env python

'''
Small utility that converts 1bpp BDF fonts to a font compatible with littlevgl
'''

# General imports
import argparse
import math


class Glyph:
    def __init__(self, props ):
        for k, v in props.items():
            setattr(self, k, v)

    def __lt__(self, other):
        return self.encoding < other.encoding

    def get_width(self):
        return self.dwidth[0]

    def get_byte_width(self):
        # Compute how many bytes wide the glyph will be
        return math.ceil(self.dwidth[0] / 8.0)

    def get_height(self):
        return len(self.bitmap)

    def get_encoding(self):
        return self.encoding

    def write_bitmap(self, f):
        f.write('''/*Unicode: U+%04x ( %s ) , Width: %d */\n''' %
                (self.encoding, self.name, self.dwidth[0]) )
        for line in self.bitmap:
            for i in range(0, len(line), 2):
                f.write("0x%s," % line[i:i+2])
                if(i > 0):
                    f.write(" ")
            f.write("\n")
        f.write("\n\n")

def parse_bdf(fn):
    # Converts bdf file to a list of Glyphs
    # Read in BDF File
    with open(fn, "r") as f:
        bdf = f.readlines()
    bdf = [x.rstrip("\n") for x in bdf]

    # Iterate through BDF Glyphs
    glyphs = []
    i = -1
    while i < len(bdf):
        props = {}

        i += 1
        tokens = bdf[i].split(' ')
        if tokens[0] == 'ENDFONT':
            break;
        if tokens[0] != "STARTCHAR":
            continue
        props['name'] = tokens[1]

        i += 1
        tokens = bdf[i].split(' ')
        if tokens[0] != "ENCODING":
            continue
        encoding = int(tokens[-1])
        if encoding < 0: # skip glyphs with a negative encoding value
            continue
        props['encoding'] = encoding

        i += 1
        tokens = bdf[i].split(' ')
        if tokens[0] != "SWIDTH":
            continue
        props['swidth'] = (int(tokens[1]), int(tokens[2]))

        i += 1
        tokens = bdf[i].split(' ')
        if tokens[0] != "DWIDTH":
            continue
        props['dwidth'] = (int(tokens[1]), int(tokens[2]))

        i += 1
        tokens = bdf[i].split(' ')
        if tokens[0] != "BBX":
            continue
        props['dwidth'] = (int(tokens[1]), int(tokens[2]),
                int(tokens[3]), int(tokens[4]))

        i += 1
        tokens = bdf[i].split(' ')
        if tokens[0] != "BITMAP":
            continue

        props['bitmap'] = []
        while True:
            i += 1
            tokens = bdf[i].split(' ')
            if tokens[0] == 'ENDCHAR':
                break
            props['bitmap'].append(tokens[0])
        glyphs.append(Glyph(props))
    return glyphs

def parse_args():
    ''' Parse CLI arguments into an object and a dictionary '''
    parser = argparse.ArgumentParser()
    parser.add_argument('bdf_fn', type=str, default='imgs',
            help='BDF Filename')
    parser.add_argument('font_name', type=str, default='font_name',
            help='Name of the font to be generated')
    args = parser.parse_args()
    dargs = vars(args)
    return (args, dargs)

def main():
    args, dargs = parse_args()

    glyphs = parse_bdf(args.bdf_fn)
    glyphs.sort() # Sorts by encoding (utf8) value

    ################
    # WRITE HEADER #
    ################
    out = open(args.font_name + '.c', "w")
    out.write('''
#include "../lv_misc/lv_font.h"
''')

    #################
    # WRITE BITMAPS #
    #################
    out.write(
'''
static const uint8_t %s_glyph_bitmap[] =
{
''' % args.font_name)

    for glyph in glyphs:
        glyph.write_bitmap(out)

    out.write('''
};
''')

    ######################
    # WRITE DESCRIPTIONS #
    ######################
    out.write(
'''
/*Store the glyph descriptions*/
static const lv_font_glyph_dsc_t %s_glyph_dsc[] =
{
''' % args.font_name)
    glyph_index = 0
    for glyph in glyphs:
        out.write('''
{.w_px = %d, .glyph_index = %d}, /*Unicode: U+%04x ( )*/
''' % (glyph.get_width(), glyph_index, glyph.get_encoding()) )
        glyph_index += glyph.get_byte_width() * glyph.get_height()

    out.write('''
};
''')

    #####################
    # WRITE FONT STRUCT #
    #####################
    out.write(
'''
lv_font_t %s =
{
    .unicode_first = %d,	/*First Unicode letter in this font*/
    .unicode_last = %d,	/*Last Unicode letter in this font*/
    .h_px = %d,				/*Font height in pixels*/
    .glyph_bitmap = %s_glyph_bitmap,	/*Bitmap of glyphs*/
    .glyph_dsc = %s_glyph_dsc,		/*Description of glyphs*/
    .glyph_cnt = %d,			/*Number of glyphs in the font*/
    .unicode_list = NULL,	/*Every character in the font from 'unicode_first' to 'unicode_last'*/
    .get_bitmap = lv_font_get_bitmap_continuous,	/*Function pointer to get glyph's bitmap*/
    .get_width = lv_font_get_width_continuous,	/*Function pointer to get glyph's width*/
    .bpp = 1,				/*Bit per pixel*/
    .monospace = 0,				/*Fix width (0: if not used)*/
    .next_page = NULL,		/*Pointer to a font extension*/
};
''' % (args.font_name,
    glyphs[0].get_encoding(),
    glyphs[-1].get_encoding(),
    glyphs[0].get_height(),
    args.font_name,
    args.font_name,
    len(glyphs),
    ) )

    out.close()

if __name__=='__main__':
    main()
