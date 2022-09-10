// stb_truetype.h - v1.24 - public domain
// authored from 2009-2020 by Sean Barrett / RAD Game Tools
//
// modified to use streams and callbacks by honey the codewitch
// the modifications are not general purpose but are for GFX
// integration. It may or may not be suitable for other things
//
// =======================================================================
//
//    NO SECURITY GUARANTEE -- DO NOT USE THIS ON UNTRUSTED FONT FILES
//
// This library does no range checking of the offsets found in the file,
// meaning an attacker can use it to read arbitrary memory.
//
// =======================================================================
//
//   This library processes TrueType files:
//        parse files
//        extract glyph metrics
//        extract glyph shapes
//        render glyphs to one-channel bitmaps with antialiasing (box filter)
//        render glyphs to one-channel SDF bitmaps (signed-distance field/function)
//
//   Todo:
//        non-MS cmaps
//        crashproof on bad data
//        hinting? (no longer patented)
//        cleartype-style AA?
//        optimize: use simple memory allocator for intermediates
//        optimize: build edge-list directly from curves
//        optimize: rasterize directly from curves?
//
// ADDITIONAL CONTRIBUTORS
//
//   Mikko Mononen: compound shape support, more cmap formats
//   Tor Andersson: kerning, subpixel rendering
//   Dougall Johnson: OpenType / Type 2 font handling
//   Daniel Ribeiro Maciel: basic GPOS-based kerning
//
//   Misc other:
//       Ryan Gordon
//       Simon Glass
//       github:IntellectualKitty
//       Imanol Celaya
//       Daniel Ribeiro Maciel
//
//   Bug/warning reports/fixes:
//       "Zer" on mollyrocket       Fabian "ryg" Giesen   github:NiLuJe
//       Cass Everitt               Martins Mozeiko       github:aloucks
//       stoiko (Haemimont Games)   Cap Petschulat        github:oyvindjam
//       Brian Hook                 Omar Cornut           github:vassvik
//       Walter van Niftrik         Ryan Griege
//       David Gow                  Peter LaValle
//       David Given                Sergey Popov
//       Ivan-Assen Ivanov          Giumo X. Clanjor
//       Anthony Pesch              Higor Euripedes
//       Johan Duparc               Thomas Fields
//       Hou Qiming                 Derek Vinyard
//       Rob Loach                  Cort Stratton
//       Kenney Phillis Jr.         Brian Costabile
//       Ken Voskuil (kaesve)
//
// VERSION HISTORY
//
//   1.24 (2020-02-05) fix warning
//   1.23 (2020-02-02) query SVG data for glyphs; query whole kerning table (but only kern not GPOS)
//   1.22 (2019-08-11) minimize missing-glyph duplication; fix kerning if both 'GPOS' and 'kern' are defined
//   1.21 (2019-02-25) fix warning
//   1.20 (2019-02-07) PackFontRange skips missing codepoints; GetScaleFontVMetrics()
//   1.19 (2018-02-11) GPOS kerning, STBTT_fmod
//   1.18 (2018-01-29) add missing function
//   1.17 (2017-07-23) make more arguments const; doc fix
//   1.16 (2017-07-12) SDF support
//   1.15 (2017-03-03) make more arguments const
//   1.14 (2017-01-16) num-fonts-in-TTC function
//   1.13 (2017-01-02) support OpenType fonts, certain Apple fonts
//   1.12 (2016-10-25) suppress warnings about casting away const with -Wcast-qual
//   1.11 (2016-04-02) fix unused-variable warning
//   1.10 (2016-04-02) user-defined fabs(); rare memory leak; remove duplicate typedef
//   1.09 (2016-01-16) warning fix; avoid crash on outofmem; use allocation userdata properly
//   1.08 (2015-09-13) document stbtt_Rasterize(); fixes for vertical & horizontal edges
//   1.07 (2015-08-01) allow PackFontRanges to accept arrays of sparse codepoints;
//                     variant PackFontRanges to pack and render in separate phases;
//                     fix stbtt_GetFontOFfsetForIndex (never worked for non-0 input?);
//                     fixed an assert() bug in the new rasterizer
//                     replace assert() with STBTT_assert() in new rasterizer
//
//   Full history can be found at the end of this file.
//
// LICENSE
//
//   See end of file for license information.
//
// USAGE
//
//   Include this file in whatever places need to refer to it. In ONE C/C++
//   file, write:
//      #define STB_TRUETYPE_IMPLEMENTATION
//   before the #include of this file. This expands out the actual
//   implementation into that C/C++ file.
//
//   To make the implementation private to the file that generates the implementation,
//      #define STBTT_STATIC
//
//   Simple 3D API (don't ship this, but it's fine for tools and quick start)
//           stbtt_BakeFontBitmap()               -- bake a font to a bitmap for use as texture
//           stbtt_GetBakedQuad()                 -- compute quad to draw for a given char
//
//   Improved 3D API (more shippable):
//           #include "stb_rect_pack.h"           -- optional, but you really want it
//           stbtt_PackBegin()
//           stbtt_PackSetOversampling()          -- for improved quality on small fonts
//           stbtt_PackFontRanges()               -- pack and renders
//           stbtt_PackEnd()
//           stbtt_GetPackedQuad()
//
//   "Load" a font file from a memory buffer (you have to keep the buffer loaded)
//           stbtt_InitFont()
//           stbtt_GetFontOffsetForIndex()        -- indexing for TTC font collections
//           stbtt_GetNumberOfFonts()             -- number of fonts for TTC font collections
//
//   Render a unicode codepoint to a bitmap
//           stbtt_GetCodepointBitmap()           -- allocates and returns a bitmap
//           stbtt_MakeCodepointBitmap()          -- renders into bitmap you provide
//           stbtt_GetCodepointBitmapBox()        -- how big the bitmap must be
//
//   Character advance/positioning
//           stbtt_GetCodepointHMetrics()
//           stbtt_GetFontVMetrics()
//           stbtt_GetFontVMetricsOS2()
//           stbtt_GetCodepointKernAdvance()
//
//   Starting with version 1.06, the rasterizer was replaced with a new,
//   faster and generally-more-precise rasterizer. The new rasterizer more
//   accurately measures pixel coverage for anti-aliasing, except in the case
//   where multiple shapes overlap, in which case it overestimates the AA pixel
//   coverage. Thus, anti-aliasing of intersecting shapes may look wrong. If
//   this turns out to be a problem, you can re-enable the old rasterizer with
//        #define STBTT_RASTERIZER_VERSION 1
//   which will incur about a 15% speed hit.
//
// ADDITIONAL DOCUMENTATION
//
//   Immediately after this block comment are a series of sample programs.
//
//   After the sample programs is the "header file" section. This section
//   includes documentation for each API function.
//
//   Some important concepts to understand to use this library:
//
//      Codepoint
//         Characters are defined by unicode codepoints, e.g. 65 is
//         uppercase A, 231 is lowercase c with a cedilla, 0x7e30 is
//         the hiragana for "ma".
//
//      Glyph
//         A visual character shape (every codepoint is rendered as
//         some glyph)
//
//      Glyph index
//         A font-specific integer ID representing a glyph
//
//      Baseline
//         Glyph shapes are defined relative to a baseline, which is the
//         bottom of uppercase characters. Characters extend both above
//         and below the baseline.
//
//      Current Point
//         As you draw text to the screen, you keep track of a "current point"
//         which is the origin of each character. The current point's vertical
//         position is the baseline. Even "baked fonts" use this model.
//
//      Vertical Font Metrics
//         The vertical qualities of the font, used to vertically position
//         and space the characters. See docs for stbtt_GetFontVMetrics.
//
//      Font Size in Pixels or Points
//         The preferred interface for specifying font sizes in stb_truetype
//         is to specify how tall the font's vertical extent should be in pixels.
//         If that sounds good enough, skip the next paragraph.
//
//         Most font APIs instead use "points", which are a common typographic
//         measurement for describing font size, defined as 72 points per inch.
//         stb_truetype provides a point API for compatibility. However, true
//         "per inch" conventions don't make much sense on computer displays
//         since different monitors have different number of pixels per
//         inch. For example, Windows traditionally uses a convention that
//         there are 96 pixels per inch, thus making 'inch' measurements have
//         nothing to do with inches, and thus effectively defining a point to
//         be 1.333 pixels. Additionally, the TrueType font data provides
//         an explicit scale factor to scale a given font's glyphs to points,
//         but the author has observed that this scale factor is often wrong
//         for non-commercial fonts, thus making fonts scaled in points
//         according to the TrueType spec incoherently sized in practice.
//
// DETAILED USAGE:
//
//  Scale:
//    Select how high you want the font to be, in points or pixels.
//    Call ScaleForPixelHeight or ScaleForMappingEmToPixels to compute
//    a scale factor SF that will be used by all other functions.
//
//  Baseline:
//    You need to select a y-coordinate that is the baseline of where
//    your text will appear. Call GetFontBoundingBox to get the baseline-relative
//    bounding box for all characters. SF*-y0 will be the distance in pixels
//    that the worst-case character could extend above the baseline, so if
//    you want the top edge of characters to appear at the top of the
//    screen where y=0, then you would set the baseline to SF*-y0.
//
//  Current point:
//    Set the current point where the first character will appear. The
//    first character could extend left of the current point; this is font
//    dependent. You can either choose a current point that is the leftmost
//    point and hope, or add some padding, or check the bounding box or
//    left-side-bearing of the first character to be displayed and set
//    the current point based on that.
//
//  Displaying a character:
//    Compute the bounding box of the character. It will contain signed values
//    relative to <current_point, baseline>. I.e. if it returns x0,y0,x1,y1,
//    then the character should be displayed in the rectangle from
//    <current_point+SF*x0, baseline+SF*y0> to <current_point+SF*x1,baseline+SF*y1).
//
//  Advancing for the next character:
//    Call GlyphHMetrics, and compute 'current_point += SF * advance'.
//
//
// ADVANCED USAGE
//
//   Quality:
//
//    - Use the functions with Subpixel at the end to allow your characters
//      to have subpixel positioning. Since the font is anti-aliased, not
//      hinted, this is very import for quality. (This is not possible with
//      baked fonts.)
//
//    - Kerning is now supported, and if you're supporting subpixel rendering
//      then kerning is worth using to give your text a polished look.
//
//   Performance:
//
//    - Convert Unicode codepoints to glyph indexes and operate on the glyphs;
//      if you don't do this, stb_truetype is forced to do the conversion on
//      every call.
//
//    - There are a lot of memory allocations. We should modify it to take
//      a temp buffer and allocate from the temp buffer (without freeing),
//      should help performance a lot.
//
// NOTES
//
//   The system uses the raw data found in the .ttf file without changing it
//   and without building auxiliary data structures. This is a bit inefficient
//   on little-endian systems (the data is big-endian), but assuming you're
//   caching the bitmaps or glyph shapes this shouldn't be a big deal.
//
//   It appears to be very hard to programmatically determine what font a
//   given file is in a general way. I provide an API for this, but I don't
//   recommend it.
//
//
// PERFORMANCE MEASUREMENTS FOR 1.06:
//
//                      32-bit     64-bit
//   Previous release:  8.83 s     7.68 s
//   Pool allocations:  7.72 s     6.34 s
//   Inline sort     :  6.54 s     5.65 s
//   New rasterizer  :  5.63 s     5.00 s

//////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////
////   INTERFACE
////
////

#ifndef __STB_INCLUDE_STB_TRUETYPE_H__
#define __STB_INCLUDE_STB_TRUETYPE_H__
#include <lvgl.h>
#include "lv_ttf.h"
#ifdef STBTT_STATIC
#define STBTT_DEF static
#else
#define STBTT_DEF extern
#endif

#ifdef __cplusplus
namespace stbtt
{
#endif
// #define your own (u)stbtt_int8/16/32 before including to override this
#ifndef stbtt_uint8
typedef unsigned char stbtt_uint8;
typedef signed char stbtt_int8;
typedef unsigned short stbtt_uint16;
typedef signed short stbtt_int16;
typedef unsigned int stbtt_uint32;
typedef signed int stbtt_int32;
#endif

typedef char stbtt__check_size32[sizeof(stbtt_int32) == 4 ? 1 : -1];
typedef char stbtt__check_size16[sizeof(stbtt_int16) == 2 ? 1 : -1];


     typedef lv_ttf_buf_t stbtt__buf;

      //////////////////////////////////////////////////////////////////////////////
      //
      // TEXTURE BAKING API
      //
      // If you use this API, you only have to call two functions ever.
      //

      typedef struct
      {
         unsigned short x0, y0, x1, y1; // coordinates of bbox in bitmap
         float xoff, yoff, xadvance;
      } stbtt_bakedchar;

      /*STBTT_DEF int stbtt_BakeFontBitmap(const unsigned char *data, int offset, // font location (use offset=0 for plain .ttf)
                                         float pixel_height,                    // height of font in pixels
                                         unsigned char *pixels, int pw, int ph, // bitmap to be filled in
                                         int first_char, int num_chars,         // characters to bake
                                         stbtt_bakedchar *chardata);            // you allocate this, it's num_chars long
      */
      // if return is positive, the first unused row of the bitmap
      // if return is negative, returns the negative of the number of characters that fit
      // if return is 0, no characters fit and no rows were used
      // This uses a very crappy packing.

      typedef struct
      {
         float x0, y0, s0, t0; // top-left
         float x1, y1, s1, t1; // bottom-right
      } stbtt_aligned_quad;

      /*STBTT_DEF void stbtt_GetBakedQuad(const stbtt_bakedchar *chardata, int pw, int ph, // same data as above
                                        int char_index,                                  // character to display
                                        float *xpos, float *ypos,                        // pointers to current position in screen pixel space
                                        stbtt_aligned_quad *q,                           // output: quad to draw
                                        int opengl_fillrule);                            // true if opengl fill rule; false if DX9 or earlier
      */
      // Call GetBakedQuad with char_index = 'character - first_char', and it
      // creates the quad you need to draw and advances the current position.
      //
      // The coordinate system used assumes y increases downwards.
      //
      // Characters will extend both above and below the current position;
      // see discussion of "BASELINE" above.
      //
      // It's inefficient; you might want to c&p it and optimize it.

      //STBTT_DEF void stbtt_GetScaledFontVMetrics(const unsigned char *fontdata, int index, float size, float *ascent, float *descent, float *lineGap);
      // Query the font vertical metrics without having to create a font first.

      //////////////////////////////////////////////////////////////////////////////
      //
      // NEW TEXTURE BAKING API
      //
      // This provides options for packing multiple fonts into one atlas, not
      // perfectly but better than nothing.

      typedef struct
      {
         unsigned short x0, y0, x1, y1; // coordinates of bbox in bitmap
         float xoff, yoff, xadvance;
         float xoff2, yoff2;
      } stbtt_packedchar;

      typedef struct stbtt_pack_context stbtt_pack_context;
      typedef struct lv_ttf stbtt_fontinfo;
#ifndef STB_RECT_PACK_VERSION
      typedef struct stbrp_rect stbrp_rect;
#endif

      //STBTT_DEF int stbtt_PackBegin(stbtt_pack_context *spc, unsigned char *pixels, int width, int height, int stride_in_bytes, int padding, void *alloc_context);
      // Initializes a packing context stored in the passed-in stbtt_pack_context.
      // Future calls using this context will pack characters into the bitmap passed
      // in here: a 1-channel bitmap that is width * height. stride_in_bytes is
      // the distance from one row to the next (or 0 to mean they are packed tightly
      // together). "padding" is the amount of padding to leave between each
      // character (normally you want '1' for bitmaps you'll use as textures with
      // bilinear filtering).
      //
      // Returns 0 on failure, 1 on success.

      //STBTT_DEF void stbtt_PackEnd(stbtt_pack_context *spc);
      // Cleans up the packing context and frees all memory.

#define STBTT_POINT_SIZE(x) (-(x))

     /* STBTT_DEF int stbtt_PackFontRange(stbtt_pack_context *spc, const unsigned char *fontdata, int font_index, float font_size,
                                        int first_unicode_char_in_range, int num_chars_in_range, stbtt_packedchar *chardata_for_range);*/
      // Creates character bitmaps from the font_index'th font found in fontdata (use
      // font_index=0 if you don't know what that is). It creates num_chars_in_range
      // bitmaps for characters with unicode values starting at first_unicode_char_in_range
      // and increasing. Data for how to render them is stored in chardata_for_range;
      // pass these to stbtt_GetPackedQuad to get back renderable quads.
      //
      // font_size is the full height of the character from ascender to descender,
      // as computed by stbtt_ScaleForPixelHeight. To use a point size as computed
      // by stbtt_ScaleForMappingEmToPixels, wrap the point size in STBTT_POINT_SIZE()
      // and pass that result as 'font_size':
      //       ...,                  20 , ... // font max minus min y is 20 pixels tall
      //       ..., STBTT_POINT_SIZE(20), ... // 'M' is 20 pixels tall

      typedef struct
      {
         float font_size;
         int first_unicode_codepoint_in_range; // if non-zero, then the chars are continuous, and this is the first codepoint
         int *array_of_unicode_codepoints;     // if non-zero, then this is an array of unicode codepoints
         int num_chars;
         stbtt_packedchar *chardata_for_range;     // output
         unsigned char h_oversample, v_oversample; // don't set these, they're used internally
      } stbtt_pack_range;

      //STBTT_DEF int stbtt_PackFontRanges(stbtt_pack_context *spc, const unsigned char *fontdata, int font_index, stbtt_pack_range *ranges, int num_ranges);
      // Creates character bitmaps from multiple ranges of characters stored in
      // ranges. This will usually create a better-packed bitmap than multiple
      // calls to stbtt_PackFontRange. Note that you can call this multiple
      // times within a single PackBegin/PackEnd.

      //STBTT_DEF void stbtt_PackSetOversampling(stbtt_pack_context *spc, unsigned int h_oversample, unsigned int v_oversample);
      // Oversampling a font increases the quality by allowing higher-quality subpixel
      // positioning, and is especially valuable at smaller text sizes.
      //
      // This function sets the amount of oversampling for all following calls to
      // stbtt_PackFontRange(s) or stbtt_PackFontRangesGatherRects for a given
      // pack context. The default (no oversampling) is achieved by h_oversample=1
      // and v_oversample=1. The total number of pixels required is
      // h_oversample*v_oversample larger than the default; for example, 2x2
      // oversampling requires 4x the storage of 1x1. For best results, render
      // oversampled textures with bilinear filtering. Look at the readme in
      // stb/tests/oversample for information about oversampled fonts
      //
      // To use with PackFontRangesGather etc., you must set it before calls
      // call to PackFontRangesGatherRects.

      //STBTT_DEF void stbtt_PackSetSkipMissingCodepoints(stbtt_pack_context *spc, int skip);
      // If skip != 0, this tells stb_truetype to skip any codepoints for which
      // there is no corresponding glyph. If skip=0, which is the default, then
      // codepoints without a glyph recived the font's "missing character" glyph,
      // typically an empty box by convention.

      /*STBTT_DEF void stbtt_GetPackedQuad(const stbtt_packedchar *chardata, int pw, int ph, // same data as above
                                         int char_index,                                   // character to display
                                         float *xpos, float *ypos,                         // pointers to current position in screen pixel space
                                         stbtt_aligned_quad *q,                            // output: quad to draw
                                         int align_to_integer);*/

      //STBTT_DEF int stbtt_PackFontRangesGatherRects(stbtt_pack_context *spc, const stbtt_fontinfo *info, stbtt_pack_range *ranges, int num_ranges, stbrp_rect *rects);
      //STBTT_DEF void stbtt_PackFontRangesPackRects(stbtt_pack_context *spc, stbrp_rect *rects, int num_rects);
      //STBTT_DEF int stbtt_PackFontRangesRenderIntoRects(stbtt_pack_context *spc, const stbtt_fontinfo *info, stbtt_pack_range *ranges, int num_ranges, stbrp_rect *rects);
      // Calling these functions in sequence is roughly equivalent to calling
      // stbtt_PackFontRanges(). If you more control over the packing of multiple
      // fonts, or if you want to pack custom data into a font texture, take a look
      // at the source to of stbtt_PackFontRanges() and create a custom version
      // using these functions, e.g. call GatherRects multiple times,
      // building up a single array of rects, then call PackRects once,
      // then call RenderIntoRects repeatedly. This may result in a
      // better packing than calling PackFontRanges multiple times
      // (or it may not).

      // this is an opaque structure that you shouldn't mess with which holds
      // all the context needed from PackBegin to PackEnd.
      struct stbtt_pack_context
      {
         void *user_allocator_context;
         void *pack_info;
         int width;
         int height;
         int stride_in_bytes;
         int padding;
         int skip_missing;
         unsigned int h_oversample, v_oversample;
         unsigned char *pixels;
         void *nodes;
      };

      //////////////////////////////////////////////////////////////////////////////
      //
      // FONT LOADING
      //
      //

      //STBTT_DEF int stbtt_GetNumberOfFonts(const unsigned char *data);
      // This function will determine the number of fonts in a font file.  TrueType
      // collection (.ttc) files may contain multiple fonts, while TrueType font
      // (.ttf) files only contain one font. The number of fonts can be used for
      // indexing with the previous function where the index is between zero and one
      // less than the total fonts. If an error occurs, -1 is returned.

      //STBTT_DEF int stbtt_GetFontOffsetForIndex(const unsigned char *data, int index);
      // Each .ttf/.ttc file may have more than one font. Each font has a sequential
      // index number starting from 0. Call this function to get the font offset for
      // a given index; it returns -1 if the index is out of range. A regular .ttf
      // file will only define one font and it always be at offset 0, so it will
      // return '0' for index 0, and -1 for all other indices.

      // The following structure is defined publicly so you can declare one on
      // the stack or as a global or etc, but you should treat it as opaque.
      
      typedef int (*stbtt_render_callback)(int x, int y, int c, void *state);
      //STBTT_DEF int stbtt_InitFont(stbtt_fontinfo *info, io::stream *stream, const unsigned char *data, int offset);
      // Given an offset into the file that defines a font, this function builds
      // the necessary cached info for the rest of the system. You must allocate
      // the stbtt_fontinfo yourself, and stbtt_InitFont will fill it out. You don't
      // need to do anything special to free it, because the contents are pure
      // value data with no additional data structures. Returns 0 on failure.

      STBTT_DEF void stbtt_MakeGlyphBitmapSubpixel(const stbtt_fontinfo *info, stbtt_render_callback callback, void *callback_state, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int glyph);

      //////////////////////////////////////////////////////////////////////////////
      //
      // CHARACTER TO GLYPH-INDEX CONVERSIOn

      STBTT_DEF int stbtt_FindGlyphIndex(const stbtt_fontinfo *info, int unicode_codepoint);
      // If you're going to perform multiple operations on the same character
      // and you want a speed-up, call this function with the character you're
      // going to process, then use glyph-based functions instead of the
      // codepoint-based functions.
      // Returns 0 if the character codepoint is not defined in the font.

      //////////////////////////////////////////////////////////////////////////////
      //
      // CHARACTER PROPERTIES
      //

      STBTT_DEF float stbtt_ScaleForPixelHeight(const stbtt_fontinfo *info, float pixels);
      // computes a scale factor to produce a font whose "height" is 'pixels' tall.
      // Height is measured as the distance from the highest ascender to the lowest
      // descender; in other words, it's equivalent to calling stbtt_GetFontVMetrics
      // and computing:
      //       scale = pixels / (ascent - descent)
      // so if you prefer to measure height by the ascent only, use a similar calculation.

      //STBTT_DEF float stbtt_ScaleForMappingEmToPixels(const stbtt_fontinfo *info, float pixels);
      // computes a scale factor to produce a font whose EM size is mapped to
      // 'pixels' tall. This is probably what traditional APIs compute, but
      // I'm not positive.

      STBTT_DEF void stbtt_GetFontVMetrics(const stbtt_fontinfo *info, int *ascent, int *descent, int *lineGap);
      // ascent is the coordinate above the baseline the font extends; descent
      // is the coordinate below the baseline the font extends (i.e. it is typically negative)
      // lineGap is the spacing between one row's descent and the next row's ascent...
      // so you should advance the vertical position by "*ascent - *descent + *lineGap"
      //   these are expressed in unscaled coordinates, so you must multiply by
      //   the scale factor for a given size

      //STBTT_DEF int stbtt_GetFontVMetricsOS2(const stbtt_fontinfo *info, int *typoAscent, int *typoDescent, int *typoLineGap);
      // analogous to GetFontVMetrics, but returns the "typographic" values from the OS/2
      // table (specific to MS/Windows TTF files).
      //
      // Returns 1 on success (table present), 0 on failure.

      STBTT_DEF void stbtt_GetFontBoundingBox(const stbtt_fontinfo *info, int *x0, int *y0, int *x1, int *y1);
      // the bounding box around all possible characters

      STBTT_DEF void stbtt_GetCodepointHMetrics(const stbtt_fontinfo *info, int codepoint, int *advanceWidth, int *leftSideBearing);
      // leftSideBearing is the offset from the current horizontal position to the left edge of the character
      // advanceWidth is the offset from the current horizontal position to the next horizontal position
      //   these are expressed in unscaled coordinates

      STBTT_DEF int stbtt_GetCodepointKernAdvance(const stbtt_fontinfo *info, int ch1, int ch2);
      // an additional amount to add to the 'advance' value between ch1 and ch2

      //STBTT_DEF int stbtt_GetCodepointBox(const stbtt_fontinfo *info, int codepoint, int *x0, int *y0, int *x1, int *y1);
      // Gets the bounding box of the visible part of the glyph, in unscaled coordinates

      STBTT_DEF void stbtt_GetGlyphHMetrics(const stbtt_fontinfo *info, int glyph_index, int *advanceWidth, int *leftSideBearing);
      STBTT_DEF int stbtt_GetGlyphKernAdvance(const stbtt_fontinfo *info, int glyph1, int glyph2);
      STBTT_DEF int stbtt_GetGlyphBox(const stbtt_fontinfo *info, int glyph_index, int *x0, int *y0, int *x1, int *y1);
      // as above, but takes one or more glyph indices for greater efficiency

      typedef struct stbtt_kerningentry
      {
         int glyph1; // use stbtt_FindGlyphIndex
         int glyph2;
         int advance;
      } stbtt_kerningentry;

      //STBTT_DEF int stbtt_GetKerningTableLength(const stbtt_fontinfo *info);
      //STBTT_DEF int stbtt_GetKerningTable(const stbtt_fontinfo *info, stbtt_kerningentry *table, int table_length);
      // Retrieves a complete list of all of the kerning pairs provided by the font
      // stbtt_GetKerningTable never writes more than table_length entries and returns how many entries it did write.
      // The table will be sorted by (a.glyph1 == b.glyph1)?(a.glyph2 < b.glyph2):(a.glyph1 < b.glyph1)

      //////////////////////////////////////////////////////////////////////////////
      //
      // GLYPH SHAPES (you probably don't need these, but they have to go before
      // the bitmaps for C declaration-order reasons)
      //

#ifndef STBTT_vmove // you can predefine these to use different values (but why?)
      enum
      {
         STBTT_vmove = 1,
         STBTT_vline,
         STBTT_vcurve,
         STBTT_vcubic
      };
#endif

#ifndef stbtt_vertex            // you can predefine this to use different values 
                                // (we share this with other code at RAD)
#define stbtt_vertex_type short // can't use stbtt_int16 because that's not visible in the header file
      typedef struct
      {
         stbtt_vertex_type x, y, cx, cy, cx1, cy1;
         unsigned char type, padding;
      } stbtt_vertex;
#endif

      //STBTT_DEF int stbtt_IsGlyphEmpty(const stbtt_fontinfo *info, int glyph_index);
      // returns non-zero if nothing is drawn for this glyph

      //STBTT_DEF int stbtt_GetCodepointShape(const stbtt_fontinfo *info, int unicode_codepoint, stbtt_vertex **vertices);
      STBTT_DEF int stbtt_GetGlyphShape(const stbtt_fontinfo *info, int glyph_index, stbtt_vertex **vertices);
      // returns # of vertices and fills *vertices with the pointer to them
      //   these are expressed in "unscaled" coordinates
      //
      // The shape is a series of contours. Each one starts with
      // a STBTT_moveto, then consists of a series of mixed
      // STBTT_lineto and STBTT_curveto segments. A lineto
      // draws a line from previous endpoint to its x,y; a curveto
      // draws a quadratic bezier from previous endpoint to
      // its x,y, using cx,cy as the bezier control point.

      //STBTT_DEF void stbtt_FreeShape(const stbtt_fontinfo *info, stbtt_vertex *vertices);
      // frees the data allocated above

      //STBTT_DEF int stbtt_GetCodepointSVG(const stbtt_fontinfo *info, int unicode_codepoint, char *buffer, int buffer_length);
      //STBTT_DEF int stbtt_GetGlyphSVG(const stbtt_fontinfo *info, int gl, char *buffer, int buffer_length);
      // fills svg with the character's SVG data.
      // returns data size or 0 if SVG not found.

      //////////////////////////////////////////////////////////////////////////////
      //
      // BITMAP RENDERING
      //

      //STBTT_DEF void stbtt_FreeBitmap(unsigned char *bitmap, void *userdata);
      // frees the bitmap allocated below

      //STBTT_DEF unsigned char *stbtt_GetCodepointBitmap(const stbtt_fontinfo *info, float scale_x, float scale_y, int codepoint, int *width, int *height, int *xoff, int *yoff);
      // allocates a large-enough single-channel 8bpp bitmap and renders the
      // specified character/glyph at the specified scale into it, with
      // antialiasing. 0 is no coverage (transparent), 255 is fully covered (opaque).
      // *width & *height are filled out with the width & height of the bitmap,
      // which is stored left-to-right, top-to-bottom.
      //
      // xoff/yoff are the offset it pixel space from the glyph origin to the top-left of the bitmap

      //STBTT_DEF unsigned char *stbtt_GetCodepointBitmapSubpixel(const stbtt_fontinfo *info, float scale_x, float scale_y, float shift_x, float shift_y, int codepoint, int *width, int *height, int *xoff, int *yoff);
      // the same as stbtt_GetCodepoitnBitmap, but you can specify a subpixel
      // shift for the character

      //STBTT_DEF void stbtt_MakeCodepointBitmap(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, int codepoint);
      // the same as stbtt_GetCodepointBitmap, but you pass in storage for the bitmap
      // in the form of 'output', with row spacing of 'out_stride' bytes. the bitmap
      // is clipped to out_w/out_h bytes. Call stbtt_GetCodepointBitmapBox to get the
      // width and height and positioning info for it first.

      //STBTT_DEF void stbtt_MakeCodepointBitmapSubpixel(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int codepoint);
      // same as stbtt_MakeCodepointBitmap, but you can specify a subpixel
      // shift for the character

      //STBTT_DEF void stbtt_MakeCodepointBitmapSubpixelPrefilter(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int oversample_x, int oversample_y, float *sub_x, float *sub_y, int codepoint);
      // same as stbtt_MakeCodepointBitmapSubpixel, but prefiltering
      // is performed (see stbtt_PackSetOversampling)

      //STBTT_DEF void stbtt_GetCodepointBitmapBox(const stbtt_fontinfo *font, int codepoint, float scale_x, float scale_y, int *ix0, int *iy0, int *ix1, int *iy1);
      // get the bbox of the bitmap centered around the glyph origin; so the
      // bitmap width is ix1-ix0, height is iy1-iy0, and location to place
      // the bitmap top left is (leftSideBearing*scale,iy0).
      // (Note that the bitmap uses y-increases-down, but the shape uses
      // y-increases-up, so CodepointBitmapBox and CodepointBox are inverted.)

      STBTT_DEF void stbtt_GetCodepointBitmapBoxSubpixel(const stbtt_fontinfo *font, int codepoint, float scale_x, float scale_y, float shift_x, float shift_y, int *ix0, int *iy0, int *ix1, int *iy1);
      // same as stbtt_GetCodepointBitmapBox, but you can specify a subpixel
      // shift for the character

      // the following functions are equivalent to the above functions, but operate
      // on glyph indices instead of Unicode codepoints (for efficiency)
      //STBTT_DEF unsigned char *stbtt_GetGlyphBitmap(const stbtt_fontinfo *info, float scale_x, float scale_y, int glyph, int *width, int *height, int *xoff, int *yoff);
      //STBTT_DEF unsigned char *stbtt_GetGlyphBitmapSubpixel(const stbtt_fontinfo *info, float scale_x, float scale_y, float shift_x, float shift_y, int glyph, int *width, int *height, int *xoff, int *yoff);
      //STBTT_DEF void stbtt_MakeGlyphBitmap(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, int glyph);
      //STBTT_DEF void stbtt_MakeGlyphBitmapSubpixel(const stbtt_fontinfo *info, stbtt_render_callback callback, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int glyph);
      //STBTT_DEF void stbtt_MakeGlyphBitmapSubpixelPrefilter(const stbtt_fontinfo *info, unsigned char *output, int out_w, int out_h, int out_stride, float scale_x, float scale_y, float shift_x, float shift_y, int oversample_x, int oversample_y, float *sub_x, float *sub_y, int glyph);
      //STBTT_DEF void stbtt_GetGlyphBitmapBox(const stbtt_fontinfo *font, int glyph, float scale_x, float scale_y, int *ix0, int *iy0, int *ix1, int *iy1);
      STBTT_DEF void stbtt_GetGlyphBitmapBoxSubpixel(const stbtt_fontinfo *font, int glyph, float scale_x, float scale_y, float shift_x, float shift_y, int *ix0, int *iy0, int *ix1, int *iy1);

      // @TODO: don't expose this structure
      typedef struct
      {
         int w, h, stride;
         unsigned char *pixels;
      } stbtt__bitmap;

      // rasterize a shape with quadratic beziers into a bitmap
      STBTT_DEF void stbtt_Rasterize(stbtt_render_callback callback, // 1-channel bitmap to draw into
                                     void *callback_state,
                                     int target_width,
                                     int target_height,
                                     float flatness_in_pixels,     // allowable error of curve in pixels
                                     stbtt_vertex *vertices,       // array of vertices defining shape
                                     int num_verts,                // number of vertices in above array
                                     float scale_x, float scale_y, // scale applied to input vertices
                                     float shift_x, float shift_y, // translation applied to input vertices
                                     int x_off, int y_off,         // another translation applied to input
                                     int invert,                   // if non-zero, vertically flip shape
                                     void *userdata);              // context for to STBTT_MALLOC

      //////////////////////////////////////////////////////////////////////////////
      //
      // Signed Distance Function (or Field) rendering

      //STBTT_DEF void stbtt_FreeSDF(unsigned char *bitmap, void *userdata);
      // frees the SDF bitmap allocated below

      //STBTT_DEF unsigned char *stbtt_GetGlyphSDF(const stbtt_fontinfo *info, float scale, int glyph, int padding, unsigned char onedge_value, float pixel_dist_scale, int *width, int *height, int *xoff, int *yoff);
      //STBTT_DEF unsigned char *stbtt_GetCodepointSDF(const stbtt_fontinfo *info, float scale, int codepoint, int padding, unsigned char onedge_value, float pixel_dist_scale, int *width, int *height, int *xoff, int *yoff);
      // These functions compute a discretized SDF field for a single character, suitable for storing
      // in a single-channel texture, sampling with bilinear filtering, and testing against
      // larger than some threshold to produce scalable fonts.
      //        info              --  the font
      //        scale             --  controls the size of the resulting SDF bitmap, same as it would be creating a regular bitmap
      //        glyph/codepoint   --  the character to generate the SDF for
      //        padding           --  extra "pixels" around the character which are filled with the distance to the character (not 0),
      //                                 which allows effects like bit outlines
      //        onedge_value      --  value 0-255 to test the SDF against to reconstruct the character (i.e. the isocontour of the character)
      //        pixel_dist_scale  --  what value the SDF should increase by when moving one SDF "pixel" away from the edge (on the 0..255 scale)
      //                                 if positive, > onedge_value is inside; if negative, < onedge_value is inside
      //        width,height      --  output height & width of the SDF bitmap (including padding)
      //        xoff,yoff         --  output origin of the character
      //        return value      --  a 2D array of bytes 0..255, width*height in size
      //
      // pixel_dist_scale & onedge_value are a scale & bias that allows you to make
      // optimal use of the limited 0..255 for your application, trading off precision
      // and special effects. SDF values outside the range 0..255 are clamped to 0..255.
      //
      // Example:
      //      scale = stbtt_ScaleForPixelHeight(22)
      //      padding = 5
      //      onedge_value = 180
      //      pixel_dist_scale = 180/5.0 = 36.0
      //
      //      This will create an SDF bitmap in which the character is about 22 pixels
      //      high but the whole bitmap is about 22+5+5=32 pixels high. To produce a filled
      //      shape, sample the SDF at each pixel and fill the pixel if the SDF value
      //      is greater than or equal to 180/255. (You'll actually want to antialias,
      //      which is beyond the scope of this example.) Additionally, you can compute
      //      offset outlines (e.g. to stroke the character border inside & outside,
      //      or only outside). For example, to fill outside the character up to 3 SDF
      //      pixels, you would compare against (180-36.0*3)/255 = 72/255. The above
      //      choice of variables maps a range from 5 pixels outside the shape to
      //      2 pixels inside the shape to 0..255; this is intended primarily for apply
      //      outside effects only (the interior range is needed to allow proper
      //      antialiasing of the font at *smaller* sizes)
      //
      // The function computes the SDF analytically at each SDF pixel, not by e.g.
      // building a higher-res bitmap and approximating it. In theory the quality
      // should be as high as possible for an SDF of this size & representation, but
      // unclear if this is true in practice (perhaps building a higher-res bitmap
      // and computing from that can allow drop-out prevention).
      //
      // The algorithm has not been optimized at all, so expect it to be slow
      // if computing lots of characters or very large sizes.

      //////////////////////////////////////////////////////////////////////////////
      //
      // Finding the right font...
      //
      // You should really just solve this offline, keep your own tables
      // of what font is what, and don't try to get it out of the .ttf file.
      // That's because getting it out of the .ttf file is really hard, because
      // the names in the file can appear in many possible encodings, in many
      // possible languages, and e.g. if you need a case-insensitive comparison,
      // the details of that depend on the encoding & language in a complex way
      // (actually underspecified in truetype, but also gigantic).
      //
      // But you can use the provided functions in two possible ways:
      //     stbtt_FindMatchingFont() will use *case-sensitive* comparisons on
      //             unicode-encoded names to try to find the font you want;
      //             you can run this before calling stbtt_InitFont()
      //
      //     stbtt_GetFontNameString() lets you get any of the various strings
      //             from the file yourself and do your own comparisons on them.
      //             You have to have called stbtt_InitFont() first.

      //STBTT_DEF int stbtt_FindMatchingFont(const unsigned char *fontdata, const char *name, int flags);
// returns the offset (not index) of the font that matches, or -1 if none
//   if you use STBTT_MACSTYLE_DONTCARE, use a font name like "Arial Bold".
//   if you use any other flag, use a font name like "Arial"; this checks
//     the 'macStyle' header field; i don't know if fonts set this consistently
#define STBTT_MACSTYLE_DONTCARE 0
#define STBTT_MACSTYLE_BOLD 1
#define STBTT_MACSTYLE_ITALIC 2
#define STBTT_MACSTYLE_UNDERSCORE 4
#define STBTT_MACSTYLE_NONE 8 // <= not same as 0, this makes us check the bitfield is 0

      //STBTT_DEF int stbtt_CompareUTF8toUTF16_bigendian(const char *s1, int len1, const char *s2, int len2);
      // returns 1/0 whether the first string interpreted as utf8 is identical to
      // the second string interpreted as big-endian utf16... useful for strings from next func

      //STBTT_DEF const char *stbtt_GetFontNameString(const stbtt_fontinfo *font, int *length, int platformID, int encodingID, int languageID, int nameID);
      // returns the string (which may be big-endian double byte, e.g. for unicode)
      // and puts the length in bytes in *length.
      //
      // some of the values for the IDs are below; for more see the truetype spec:
      //     http://developer.apple.com/textfonts/TTRefMan/RM06/Chap6name.html
      //     http://www.microsoft.com/typography/otspec/name.htm

      enum
      { // platformID
         STBTT_PLATFORM_ID_UNICODE = 0,
         STBTT_PLATFORM_ID_MAC = 1,
         STBTT_PLATFORM_ID_ISO = 2,
         STBTT_PLATFORM_ID_MICROSOFT = 3
      };

      enum
      { // encodingID for STBTT_PLATFORM_ID_UNICODE
         STBTT_UNICODE_EID_UNICODE_1_0 = 0,
         STBTT_UNICODE_EID_UNICODE_1_1 = 1,
         STBTT_UNICODE_EID_ISO_10646 = 2,
         STBTT_UNICODE_EID_UNICODE_2_0_BMP = 3,
         STBTT_UNICODE_EID_UNICODE_2_0_FULL = 4
      };

      enum
      { // encodingID for STBTT_PLATFORM_ID_MICROSOFT
         STBTT_MS_EID_SYMBOL = 0,
         STBTT_MS_EID_UNICODE_BMP = 1,
         STBTT_MS_EID_SHIFTJIS = 2,
         STBTT_MS_EID_UNICODE_FULL = 10
      };

      enum
      { // encodingID for STBTT_PLATFORM_ID_MAC; same as Script Manager codes
         STBTT_MAC_EID_ROMAN = 0,
         STBTT_MAC_EID_ARABIC = 4,
         STBTT_MAC_EID_JAPANESE = 1,
         STBTT_MAC_EID_HEBREW = 5,
         STBTT_MAC_EID_CHINESE_TRAD = 2,
         STBTT_MAC_EID_GREEK = 6,
         STBTT_MAC_EID_KOREAN = 3,
         STBTT_MAC_EID_RUSSIAN = 7
      };

      enum
      { // languageID for STBTT_PLATFORM_ID_MICROSOFT; same as LCID...
         // problematic because there are e.g. 16 english LCIDs and 16 arabic LCIDs
         STBTT_MS_LANG_ENGLISH = 0x0409,
         STBTT_MS_LANG_ITALIAN = 0x0410,
         STBTT_MS_LANG_CHINESE = 0x0804,
         STBTT_MS_LANG_JAPANESE = 0x0411,
         STBTT_MS_LANG_DUTCH = 0x0413,
         STBTT_MS_LANG_KOREAN = 0x0412,
         STBTT_MS_LANG_FRENCH = 0x040c,
         STBTT_MS_LANG_RUSSIAN = 0x0419,
         STBTT_MS_LANG_GERMAN = 0x0407,
         STBTT_MS_LANG_SPANISH = 0x0409,
         STBTT_MS_LANG_HEBREW = 0x040d,
         STBTT_MS_LANG_SWEDISH = 0x041D
      };

      enum
      { // languageID for STBTT_PLATFORM_ID_MAC
         STBTT_MAC_LANG_ENGLISH = 0,
         STBTT_MAC_LANG_JAPANESE = 11,
         STBTT_MAC_LANG_ARABIC = 12,
         STBTT_MAC_LANG_KOREAN = 23,
         STBTT_MAC_LANG_DUTCH = 4,
         STBTT_MAC_LANG_RUSSIAN = 32,
         STBTT_MAC_LANG_FRENCH = 1,
         STBTT_MAC_LANG_SPANISH = 6,
         STBTT_MAC_LANG_GERMAN = 2,
         STBTT_MAC_LANG_SWEDISH = 5,
         STBTT_MAC_LANG_HEBREW = 10,
         STBTT_MAC_LANG_CHINESE_SIMPLIFIED = 33,
         STBTT_MAC_LANG_ITALIAN = 3,
         STBTT_MAC_LANG_CHINESE_TRAD = 19
      };

#ifdef __cplusplus
   }
#endif

#endif // __STB_INCLUDE_STB_TRUETYPE_H__

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   ////
   ////   IMPLEMENTATION
   ////
   ////

// FULL VERSION HISTORY
//
//   1.19 (2018-02-11) OpenType GPOS kerning (horizontal only), STBTT_fmod
//   1.18 (2018-01-29) add missing function
//   1.17 (2017-07-23) make more arguments const; doc fix
//   1.16 (2017-07-12) SDF support
//   1.15 (2017-03-03) make more arguments const
//   1.14 (2017-01-16) num-fonts-in-TTC function
//   1.13 (2017-01-02) support OpenType fonts, certain Apple fonts
//   1.12 (2016-10-25) suppress warnings about casting away const with -Wcast-qual
//   1.11 (2016-04-02) fix unused-variable warning
//   1.10 (2016-04-02) allow user-defined fabs() replacement
//                     fix memory leak if fontsize=0.0
//                     fix warning from duplicate typedef
//   1.09 (2016-01-16) warning fix; avoid crash on outofmem; use alloc userdata for PackFontRanges
//   1.08 (2015-09-13) document stbtt_Rasterize(); fixes for vertical & horizontal edges
//   1.07 (2015-08-01) allow PackFontRanges to accept arrays of sparse codepoints;
//                     allow PackFontRanges to pack and render in separate phases;
//                     fix stbtt_GetFontOFfsetForIndex (never worked for non-0 input?);
//                     fixed an assert() bug in the new rasterizer
//                     replace assert() with STBTT_assert() in new rasterizer
//   1.06 (2015-07-14) performance improvements (~35% faster on x86 and x64 on test machine)
//                     also more precise AA rasterizer, except if shapes overlap
//                     remove need for STBTT_sort
//   1.05 (2015-04-15) fix misplaced definitions for STBTT_STATIC
//   1.04 (2015-04-15) typo in example
//   1.03 (2015-04-12) STBTT_STATIC, fix memory leak in new packing, various fixes
//   1.02 (2014-12-10) fix various warnings & compile issues w/ stb_rect_pack, C++
//   1.01 (2014-12-08) fix subpixel position when oversampling to exactly match
//                        non-oversampled; STBTT_POINT_SIZE for packed case only
//   1.00 (2014-12-06) add new PackBegin etc. API, w/ support for oversampling
//   0.99 (2014-09-18) fix multiple bugs with subpixel rendering (ryg)
//   0.9  (2014-08-07) support certain mac/iOS fonts without an MS platformID
//   0.8b (2014-07-07) fix a warning
//   0.8  (2014-05-25) fix a few more warnings
//   0.7  (2013-09-25) bugfix: subpixel glyph bug fixed in 0.5 had come back
//   0.6c (2012-07-24) improve documentation
//   0.6b (2012-07-20) fix a few more warnings
//   0.6  (2012-07-17) fix warnings; added stbtt_ScaleForMappingEmToPixels,
//                        stbtt_GetFontBoundingBox, stbtt_IsGlyphEmpty
//   0.5  (2011-12-09) bugfixes:
//                        subpixel glyph renderer computed wrong bounding box
//                        first vertex of shape can be off-curve (FreeSans)
//   0.4b (2011-12-03) fixed an error in the font baking example
//   0.4  (2011-12-01) kerning, subpixel rendering (tor)
//                    bugfixes for:
//                        codepoint-to-glyph conversion using table fmt=12
//                        codepoint-to-glyph conversion using table fmt=4
//                        stbtt_GetBakedQuad with non-square texture (Zer)
//                    updated Hello World! sample to use kerning and subpixel
//                    fixed some warnings
//   0.3  (2009-06-24) cmap fmt=12, compound shapes (MM)
//                    userdata, malloc-from-userdata, non-zero fill (stb)
//   0.2  (2009-03-11) Fix unsigned/signed char warnings
//   0.1  (2009-03-09) First public release
//

/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this
software, either in source code form or as a compiled binary, for any purpose,
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this
software dedicate any and all copyright interest in the software to the public
domain. We make this dedication for the benefit of the public at large and to
the detriment of our heirs and successors. We intend this dedication to be an
overt act of relinquishment in perpetuity of all present and future rights to
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/