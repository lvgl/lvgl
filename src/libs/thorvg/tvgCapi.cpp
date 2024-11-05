/*
 * Copyright (c) 2020 - 2024 the ThorVG project. All rights reserved.

 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:

 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.

 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "../../lv_conf_internal.h"
#if LV_USE_THORVG_INTERNAL

#include "config.h"
#include <string>
#include "thorvg.h"
#include "thorvg_capi.h"
#ifdef THORVG_LOTTIE_LOADER_SUPPORT
#include "thorvg_lottie.h"
#endif

using namespace std;
using namespace tvg;

#ifdef __cplusplus
extern "C" {
#endif


/************************************************************************/
/* Engine API                                                           */
/************************************************************************/

TVG_API Tvg_Result tvg_engine_init(Tvg_Engine engine_method, unsigned threads)
{
    return (Tvg_Result) Initializer::init(CanvasEngine(engine_method), threads);
}


TVG_API Tvg_Result tvg_engine_term(Tvg_Engine engine_method)
{
    return (Tvg_Result) Initializer::term(CanvasEngine(engine_method));
}


TVG_API Tvg_Result tvg_engine_version(uint32_t* major, uint32_t* minor, uint32_t* micro, const char** version)
{
    if (version) *version = Initializer::version(major, minor, micro);
    return TVG_RESULT_SUCCESS;
}

/************************************************************************/
/* Canvas API                                                           */
/************************************************************************/

TVG_API Tvg_Canvas* tvg_swcanvas_create()
{
    return (Tvg_Canvas*) SwCanvas::gen().release();
}


TVG_API Tvg_Result tvg_canvas_destroy(Tvg_Canvas* canvas)
{
    if (!canvas) return TVG_RESULT_INVALID_ARGUMENT;
    delete(reinterpret_cast<Canvas*>(canvas));
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_swcanvas_set_mempool(Tvg_Canvas* canvas, Tvg_Mempool_Policy policy)
{
    if (!canvas) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<SwCanvas*>(canvas)->mempool(static_cast<SwCanvas::MempoolPolicy>(policy));
}


TVG_API Tvg_Result tvg_swcanvas_set_target(Tvg_Canvas* canvas, uint32_t* buffer, uint32_t stride, uint32_t w, uint32_t h, Tvg_Colorspace cs)
{
    if (!canvas) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<SwCanvas*>(canvas)->target(buffer, stride, w, h, static_cast<SwCanvas::Colorspace>(cs));
}


TVG_API Tvg_Result tvg_canvas_push(Tvg_Canvas* canvas, Tvg_Paint* paint)
{
    if (!canvas || !paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Canvas*>(canvas)->push(unique_ptr<Paint>((Paint*)paint));
}


TVG_API Tvg_Result tvg_canvas_reserve(Tvg_Canvas* canvas, uint32_t n)
{
    return TVG_RESULT_NOT_SUPPORTED;
}


TVG_API Tvg_Result tvg_canvas_clear(Tvg_Canvas* canvas, bool free)
{
    if (!canvas) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Canvas*>(canvas)->clear(free);
}


TVG_API Tvg_Result tvg_canvas_update(Tvg_Canvas* canvas)
{
    if (!canvas) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Canvas*>(canvas)->update(nullptr);
}


TVG_API Tvg_Result tvg_canvas_update_paint(Tvg_Canvas* canvas, Tvg_Paint* paint)
{
    if (!canvas || !paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Canvas*>(canvas)->update((Paint*) paint);
}


TVG_API Tvg_Result tvg_canvas_draw(Tvg_Canvas* canvas)
{
    if (!canvas) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Canvas*>(canvas)->draw();
}


TVG_API Tvg_Result tvg_canvas_sync(Tvg_Canvas* canvas)
{
    if (!canvas) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Canvas*>(canvas)->sync();
}


TVG_API Tvg_Result tvg_canvas_set_viewport(Tvg_Canvas* canvas, int32_t x, int32_t y, int32_t w, int32_t h)
{
    if (!canvas) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Canvas*>(canvas)->viewport(x, y, w, h);
}


/************************************************************************/
/* Paint API                                                            */
/************************************************************************/

TVG_API Tvg_Result tvg_paint_del(Tvg_Paint* paint)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    delete(reinterpret_cast<Paint*>(paint));
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_paint_scale(Tvg_Paint* paint, float factor)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Paint*>(paint)->scale(factor);
}


TVG_API Tvg_Result tvg_paint_rotate(Tvg_Paint* paint, float degree)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Paint*>(paint)->rotate(degree);
}


TVG_API Tvg_Result tvg_paint_translate(Tvg_Paint* paint, float x, float y)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Paint*>(paint)->translate(x, y);
}


TVG_API Tvg_Result tvg_paint_set_transform(Tvg_Paint* paint, const Tvg_Matrix* m)
{
    if (!paint || !m) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Paint*>(paint)->transform(*(reinterpret_cast<const Matrix*>(m)));
}


TVG_API Tvg_Result tvg_paint_get_transform(Tvg_Paint* paint, Tvg_Matrix* m)
{
    if (!paint || !m) return TVG_RESULT_INVALID_ARGUMENT;
    *reinterpret_cast<Matrix*>(m) = reinterpret_cast<Paint*>(paint)->transform();
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Paint* tvg_paint_duplicate(Tvg_Paint* paint)
{
    if (!paint) return nullptr;
    return (Tvg_Paint*) reinterpret_cast<Paint*>(paint)->duplicate();
}


TVG_API Tvg_Result tvg_paint_set_opacity(Tvg_Paint* paint, uint8_t opacity)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Paint*>(paint)->opacity(opacity);
}


TVG_API Tvg_Result tvg_paint_get_opacity(const Tvg_Paint* paint, uint8_t* opacity)
{
    if (!paint || !opacity)  return TVG_RESULT_INVALID_ARGUMENT;
    *opacity = reinterpret_cast<const Paint*>(paint)->opacity();
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_paint_get_bounds(const Tvg_Paint* paint, float* x, float* y, float* w, float* h, bool transformed)
{
   if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
   return (Tvg_Result) reinterpret_cast<const Paint*>(paint)->bounds(x, y, w, h, transformed);
}


TVG_API Tvg_Result tvg_paint_set_composite_method(Tvg_Paint* paint, Tvg_Paint* target, Tvg_Composite_Method method)
{
   if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
   return (Tvg_Result) reinterpret_cast<Paint*>(paint)->composite(unique_ptr<Paint>((Paint*)(target)), (CompositeMethod)method);
}


TVG_API Tvg_Result tvg_paint_get_composite_method(const Tvg_Paint* paint, const Tvg_Paint** target, Tvg_Composite_Method* method)
{
   if (!paint || !target || !method) return TVG_RESULT_INVALID_ARGUMENT;
   *reinterpret_cast<CompositeMethod*>(method) = reinterpret_cast<const Paint*>(paint)->composite(reinterpret_cast<const Paint**>(target));
   return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_paint_set_blend_method(Tvg_Paint* paint, Tvg_Blend_Method method)
{
   if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
   return (Tvg_Result) reinterpret_cast<Paint*>(paint)->blend((BlendMethod)method);
}


TVG_API Tvg_Result tvg_paint_get_type(const Tvg_Paint* paint, Tvg_Type* type)
{
    if (!paint || !type) return TVG_RESULT_INVALID_ARGUMENT;
    *type = static_cast<Tvg_Type>(reinterpret_cast<const Paint*>(paint)->type());
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_paint_set_clip(Tvg_Paint* paint, Tvg_Paint* clipper)
{
   if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
   return (Tvg_Result) reinterpret_cast<Paint*>(paint)->clip(unique_ptr<Paint>((Paint*)(clipper)));
}


TVG_DEPRECATED TVG_API Tvg_Result tvg_paint_get_identifier(const Tvg_Paint* paint, Tvg_Identifier* identifier)
{
    return tvg_paint_get_type(paint, (Tvg_Type*) identifier);
}

/************************************************************************/
/* Shape API                                                            */
/************************************************************************/

TVG_API Tvg_Paint* tvg_shape_new()
{
    return (Tvg_Paint*) Shape::gen().release();
}


TVG_API Tvg_Result tvg_shape_reset(Tvg_Paint* paint)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->reset();
}


TVG_API Tvg_Result tvg_shape_move_to(Tvg_Paint* paint, float x, float y)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->moveTo(x, y);
}


TVG_API Tvg_Result tvg_shape_line_to(Tvg_Paint* paint, float x, float y)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->lineTo(x, y);
}


TVG_API Tvg_Result tvg_shape_cubic_to(Tvg_Paint* paint, float cx1, float cy1, float cx2, float cy2, float x, float y)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->cubicTo(cx1, cy1, cx2, cy2, x, y);
}


TVG_API Tvg_Result tvg_shape_close(Tvg_Paint* paint)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->close();
}


TVG_API Tvg_Result tvg_shape_append_rect(Tvg_Paint* paint, float x, float y, float w, float h, float rx, float ry)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->appendRect(x, y, w, h, rx, ry);
}


TVG_API Tvg_Result tvg_shape_append_arc(Tvg_Paint* paint, float cx, float cy, float radius, float startAngle, float sweep, uint8_t pie)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->appendArc(cx, cy, radius, startAngle, sweep, pie);
}


TVG_API Tvg_Result tvg_shape_append_circle(Tvg_Paint* paint, float cx, float cy, float rx, float ry)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->appendCircle(cx, cy, rx, ry);
}


TVG_API Tvg_Result tvg_shape_append_path(Tvg_Paint* paint, const Tvg_Path_Command* cmds, uint32_t cmdCnt, const Tvg_Point* pts, uint32_t ptsCnt)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->appendPath((const PathCommand*)cmds, cmdCnt, (const Point*)pts, ptsCnt);
}


TVG_API Tvg_Result tvg_shape_get_path_coords(const Tvg_Paint* paint, const Tvg_Point** pts, uint32_t* cnt)
{
    if (!paint || !pts || !cnt) return TVG_RESULT_INVALID_ARGUMENT;
    *cnt = reinterpret_cast<const Shape*>(paint)->pathCoords((const Point**)pts);
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_shape_get_path_commands(const Tvg_Paint* paint, const Tvg_Path_Command** cmds, uint32_t* cnt)
{
    if (!paint || !cmds || !cnt) return TVG_RESULT_INVALID_ARGUMENT;
    *cnt = reinterpret_cast<const Shape*>(paint)->pathCommands((const PathCommand**)cmds);
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_shape_set_stroke_width(Tvg_Paint* paint, float width)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->stroke(width);
}


TVG_API Tvg_Result tvg_shape_get_stroke_width(const Tvg_Paint* paint, float* width)
{
    if (!paint || !width) return TVG_RESULT_INVALID_ARGUMENT;
    *width = reinterpret_cast<const Shape*>(paint)->strokeWidth();
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_shape_set_stroke_color(Tvg_Paint* paint, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->stroke(r, g, b, a);
}


TVG_API Tvg_Result tvg_shape_get_stroke_color(const Tvg_Paint* paint, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<const Shape*>(paint)->strokeColor(r, g, b, a);
}


TVG_API Tvg_Result tvg_shape_set_stroke_linear_gradient(Tvg_Paint* paint, Tvg_Gradient* gradient)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->stroke(unique_ptr<LinearGradient>((LinearGradient*)(gradient)));
}


TVG_API Tvg_Result tvg_shape_set_stroke_radial_gradient(Tvg_Paint* paint, Tvg_Gradient* gradient)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->stroke(unique_ptr<RadialGradient>((RadialGradient*)(gradient)));
}


TVG_API Tvg_Result tvg_shape_get_stroke_gradient(const Tvg_Paint* paint, Tvg_Gradient** gradient)
{
   if (!paint || !gradient) return TVG_RESULT_INVALID_ARGUMENT;
   *gradient = (Tvg_Gradient*)(reinterpret_cast<const Shape*>(paint)->strokeFill());
   return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_shape_set_stroke_dash(Tvg_Paint* paint, const float* dashPattern, uint32_t cnt)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->stroke(dashPattern, cnt);
}


TVG_API Tvg_Result tvg_shape_get_stroke_dash(const Tvg_Paint* paint, const float** dashPattern, uint32_t* cnt)
{
    if (!paint || !cnt || !dashPattern) return TVG_RESULT_INVALID_ARGUMENT;
    *cnt = reinterpret_cast<const Shape*>(paint)->strokeDash(dashPattern);
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_shape_set_stroke_cap(Tvg_Paint* paint, Tvg_Stroke_Cap cap)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->stroke((StrokeCap)cap);
}


TVG_API Tvg_Result tvg_shape_get_stroke_cap(const Tvg_Paint* paint, Tvg_Stroke_Cap* cap)
{
    if (!paint || !cap) return TVG_RESULT_INVALID_ARGUMENT;
    *cap = (Tvg_Stroke_Cap) reinterpret_cast<const Shape*>(paint)->strokeCap();
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_shape_set_stroke_join(Tvg_Paint* paint, Tvg_Stroke_Join join)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->stroke((StrokeJoin)join);
}


TVG_API Tvg_Result tvg_shape_get_stroke_join(const Tvg_Paint* paint, Tvg_Stroke_Join* join)
{
    if (!paint || !join) return TVG_RESULT_INVALID_ARGUMENT;
    *join = (Tvg_Stroke_Join) reinterpret_cast<const Shape*>(paint)->strokeJoin();
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_shape_set_stroke_miterlimit(Tvg_Paint* paint, float ml)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->strokeMiterlimit(ml);
}


TVG_API Tvg_Result tvg_shape_get_stroke_miterlimit(const Tvg_Paint* paint, float* ml)
{
    if (!paint || !ml) return TVG_RESULT_INVALID_ARGUMENT;
    *ml = reinterpret_cast<const Shape*>(paint)->strokeMiterlimit();
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_shape_set_stroke_trim(Tvg_Paint* paint, float begin, float end, bool simultaneous)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->strokeTrim(begin, end, simultaneous);
}


TVG_API Tvg_Result tvg_shape_set_fill_color(Tvg_Paint* paint, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->fill(r, g, b, a);
}


TVG_API Tvg_Result tvg_shape_get_fill_color(const Tvg_Paint* paint, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<const Shape*>(paint)->fillColor(r, g, b, a);
}


TVG_API Tvg_Result tvg_shape_set_fill_rule(Tvg_Paint* paint, Tvg_Fill_Rule rule)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->fill((FillRule)rule);
}


TVG_API Tvg_Result tvg_shape_get_fill_rule(const Tvg_Paint* paint, Tvg_Fill_Rule* rule)
{
    if (!paint || !rule) return TVG_RESULT_INVALID_ARGUMENT;
    *rule = (Tvg_Fill_Rule) reinterpret_cast<const Shape*>(paint)->fillRule();
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_shape_set_paint_order(Tvg_Paint* paint, bool strokeFirst)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->order(strokeFirst);
}


TVG_API Tvg_Result tvg_shape_set_linear_gradient(Tvg_Paint* paint, Tvg_Gradient* gradient)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->fill(unique_ptr<LinearGradient>((LinearGradient*)(gradient)));
}


TVG_API Tvg_Result tvg_shape_set_radial_gradient(Tvg_Paint* paint, Tvg_Gradient* gradient)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Shape*>(paint)->fill(unique_ptr<RadialGradient>((RadialGradient*)(gradient)));
}


TVG_API Tvg_Result tvg_shape_get_gradient(const Tvg_Paint* paint, Tvg_Gradient** gradient)
{
   if (!paint || !gradient) return TVG_RESULT_INVALID_ARGUMENT;
   *gradient = (Tvg_Gradient*)(reinterpret_cast<const Shape*>(paint)->fill());
   return TVG_RESULT_SUCCESS;
}

/************************************************************************/
/* Picture API                                                          */
/************************************************************************/

TVG_API Tvg_Paint* tvg_picture_new()
{
    return (Tvg_Paint*) Picture::gen().release();
}


TVG_API Tvg_Result tvg_picture_load(Tvg_Paint* paint, const char* path)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Picture*>(paint)->load(path);
}


TVG_API Tvg_Result tvg_picture_load_raw(Tvg_Paint* paint, uint32_t *data, uint32_t w, uint32_t h, bool copy)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Picture*>(paint)->load(data, w, h, copy);
}


TVG_API Tvg_Result tvg_picture_load_data(Tvg_Paint* paint, const char *data, uint32_t size, const char *mimetype, bool copy)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Picture*>(paint)->load(data, size, mimetype ? mimetype : "", copy);
}


TVG_API Tvg_Result tvg_picture_set_size(Tvg_Paint* paint, float w, float h)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Picture*>(paint)->size(w, h);
}


TVG_API Tvg_Result tvg_picture_get_size(const Tvg_Paint* paint, float* w, float* h)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<const Picture*>(paint)->size(w, h);
}


TVG_API const Tvg_Paint* tvg_picture_get_paint(Tvg_Paint* paint, uint32_t id)
{
    if (!paint) return nullptr;
    return (Tvg_Paint*) reinterpret_cast<Picture*>(paint)->paint(id);
}


/************************************************************************/
/* Gradient API                                                         */
/************************************************************************/

TVG_API Tvg_Gradient* tvg_linear_gradient_new()
{
    return (Tvg_Gradient*)LinearGradient::gen().release();
}


TVG_API Tvg_Gradient* tvg_radial_gradient_new()
{
    return (Tvg_Gradient*)RadialGradient::gen().release();
}


TVG_API Tvg_Gradient* tvg_gradient_duplicate(Tvg_Gradient* grad)
{
    if (!grad) return nullptr;
    return (Tvg_Gradient*) reinterpret_cast<Fill*>(grad)->duplicate();
}


TVG_API Tvg_Result tvg_gradient_del(Tvg_Gradient* grad)
{
    if (!grad) return TVG_RESULT_INVALID_ARGUMENT;
    delete(reinterpret_cast<Fill*>(grad));
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_linear_gradient_set(Tvg_Gradient* grad, float x1, float y1, float x2, float y2)
{
    if (!grad) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<LinearGradient*>(grad)->linear(x1, y1, x2, y2);
}


TVG_API Tvg_Result tvg_linear_gradient_get(Tvg_Gradient* grad, float* x1, float* y1, float* x2, float* y2)
{
    if (!grad) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<LinearGradient*>(grad)->linear(x1, y1, x2, y2);
}


TVG_API Tvg_Result tvg_radial_gradient_set(Tvg_Gradient* grad, float cx, float cy, float radius)
{
    if (!grad) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<RadialGradient*>(grad)->radial(cx, cy, radius);
}


TVG_API Tvg_Result tvg_radial_gradient_get(Tvg_Gradient* grad, float* cx, float* cy, float* radius)
{
    if (!grad) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<RadialGradient*>(grad)->radial(cx, cy, radius);
}


TVG_API Tvg_Result tvg_gradient_set_color_stops(Tvg_Gradient* grad, const Tvg_Color_Stop* color_stop, uint32_t cnt)
{
    if (!grad) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Fill*>(grad)->colorStops(reinterpret_cast<const Fill::ColorStop*>(color_stop), cnt);
}


TVG_API Tvg_Result tvg_gradient_get_color_stops(const Tvg_Gradient* grad, const Tvg_Color_Stop** color_stop, uint32_t* cnt)
{
    if (!grad || !color_stop || !cnt) return TVG_RESULT_INVALID_ARGUMENT;
    *cnt = reinterpret_cast<const Fill*>(grad)->colorStops(reinterpret_cast<const Fill::ColorStop**>(color_stop));
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_gradient_set_spread(Tvg_Gradient* grad, const Tvg_Stroke_Fill spread)
{
    if (!grad) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Fill*>(grad)->spread((FillSpread)spread);
}


TVG_API Tvg_Result tvg_gradient_get_spread(const Tvg_Gradient* grad, Tvg_Stroke_Fill* spread)
{
    if (!grad || !spread) return TVG_RESULT_INVALID_ARGUMENT;
    *spread = (Tvg_Stroke_Fill) reinterpret_cast<const Fill*>(grad)->spread();
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_gradient_set_transform(Tvg_Gradient* grad, const Tvg_Matrix* m)
{
    if (!grad || !m) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Fill*>(grad)->transform(*(reinterpret_cast<const Matrix*>(m)));
}


TVG_API Tvg_Result tvg_gradient_get_transform(const Tvg_Gradient* grad, Tvg_Matrix* m)
{
    if (!grad || !m) return TVG_RESULT_INVALID_ARGUMENT;
    *reinterpret_cast<Matrix*>(m) = reinterpret_cast<Fill*>(const_cast<Tvg_Gradient*>(grad))->transform();
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_gradient_get_type(const Tvg_Gradient* grad, Tvg_Type* type)
{
    if (!grad || !type) return TVG_RESULT_INVALID_ARGUMENT;
    *type = static_cast<Tvg_Type>(reinterpret_cast<const Fill*>(grad)->type());
    return TVG_RESULT_SUCCESS;
}


TVG_DEPRECATED TVG_API Tvg_Result tvg_gradient_get_identifier(const Tvg_Gradient* grad, Tvg_Identifier* identifier)
{
    return tvg_gradient_get_type(grad, (Tvg_Type*) identifier);
}

/************************************************************************/
/* Scene API                                                            */
/************************************************************************/

TVG_API Tvg_Paint* tvg_scene_new()
{
    return (Tvg_Paint*) Scene::gen().release();
}


TVG_API Tvg_Result tvg_scene_reserve(Tvg_Paint* scene, uint32_t size)
{
    return TVG_RESULT_NOT_SUPPORTED;
}


TVG_API Tvg_Result tvg_scene_push(Tvg_Paint* scene, Tvg_Paint* paint)
{
    if (!scene || !paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Scene*>(scene)->push(unique_ptr<Paint>((Paint*)paint));
}


TVG_API Tvg_Result tvg_scene_clear(Tvg_Paint* scene, bool free)
{
    if (!scene) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Scene*>(scene)->clear(free);
}


/************************************************************************/
/* Text API                                                            */
/************************************************************************/

TVG_API Tvg_Paint* tvg_text_new()
{
    return (Tvg_Paint*)Text::gen().release();
}


TVG_API Tvg_Result tvg_text_set_font(Tvg_Paint* paint, const char* name, float size, const char* style)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Text*>(paint)->font(name, size, style);
}


TVG_API Tvg_Result tvg_text_set_text(Tvg_Paint* paint, const char* text)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Text*>(paint)->text(text);
}


TVG_API Tvg_Result tvg_text_set_fill_color(Tvg_Paint* paint, uint8_t r, uint8_t g, uint8_t b)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Text*>(paint)->fill(r, g, b);
}


TVG_API Tvg_Result tvg_text_set_gradient(Tvg_Paint* paint, Tvg_Gradient* gradient)
{
    if (!paint) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Text*>(paint)->fill(unique_ptr<Fill>((Fill*)(gradient)));
}


TVG_API Tvg_Result tvg_font_load(const char* path)
{
    return (Tvg_Result) Text::load(path);
}


TVG_API Tvg_Result tvg_font_load_data(const char* name, const char* data, uint32_t size, const char *mimetype, bool copy)
{
    return (Tvg_Result) Text::load(name, data, size, mimetype ? mimetype : "", copy);
}


TVG_API Tvg_Result tvg_font_unload(const char* path)
{
    return (Tvg_Result) Text::unload(path);
}


/************************************************************************/
/* Saver API                                                            */
/************************************************************************/

TVG_API Tvg_Saver* tvg_saver_new()
{
    return (Tvg_Saver*) Saver::gen().release();
}


TVG_API Tvg_Result tvg_saver_save(Tvg_Saver* saver, Tvg_Paint* paint, const char* path, bool compress)
{
    if (!saver || !paint || !path) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Saver*>(saver)->save(unique_ptr<Paint>((Paint*)paint), path, compress);
}


TVG_API Tvg_Result tvg_saver_sync(Tvg_Saver* saver)
{
    if (!saver) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Saver*>(saver)->sync();
}


TVG_API Tvg_Result tvg_saver_del(Tvg_Saver* saver)
{
    if (!saver) return TVG_RESULT_INVALID_ARGUMENT;
    delete(reinterpret_cast<Saver*>(saver));
    return TVG_RESULT_SUCCESS;
}


/************************************************************************/
/* Animation API                                                        */
/************************************************************************/

TVG_API Tvg_Animation* tvg_animation_new()
{
    return (Tvg_Animation*) Animation::gen().release();
}


TVG_API Tvg_Result tvg_animation_set_frame(Tvg_Animation* animation, float no)
{
    if (!animation) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Animation*>(animation)->frame(no);
}


TVG_API Tvg_Result tvg_animation_get_frame(Tvg_Animation* animation, float* no)
{
    if (!animation || !no) return TVG_RESULT_INVALID_ARGUMENT;
    *no = reinterpret_cast<Animation*>(animation)->curFrame();
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_animation_get_total_frame(Tvg_Animation* animation, float* cnt)
{
    if (!animation || !cnt) return TVG_RESULT_INVALID_ARGUMENT;
    *cnt = reinterpret_cast<Animation*>(animation)->totalFrame();
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Paint* tvg_animation_get_picture(Tvg_Animation* animation)
{
    if (!animation) return nullptr;
    return (Tvg_Paint*) reinterpret_cast<Animation*>(animation)->picture();
}


TVG_API Tvg_Result tvg_animation_get_duration(Tvg_Animation* animation, float* duration)
{
    if (!animation || !duration) return TVG_RESULT_INVALID_ARGUMENT;
    *duration = reinterpret_cast<Animation*>(animation)->duration();
    return TVG_RESULT_SUCCESS;
}


TVG_API Tvg_Result tvg_animation_set_segment(Tvg_Animation* animation, float start, float end)
{
    if (!animation) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Animation*>(animation)->segment(start, end);
}


TVG_API Tvg_Result tvg_animation_get_segment(Tvg_Animation* animation, float* start, float* end)
{
    if (!animation) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<Animation*>(animation)->segment(start, end);
}


TVG_API Tvg_Result tvg_animation_del(Tvg_Animation* animation)
{
    if (!animation) return TVG_RESULT_INVALID_ARGUMENT;
    delete(reinterpret_cast<Animation*>(animation));
    return TVG_RESULT_SUCCESS;
}


/************************************************************************/
/* Accessor API                                                         */
/************************************************************************/

TVG_API uint32_t tvg_accessor_generate_id(const char* name)
{
    return Accessor::id(name);
}


/************************************************************************/
/* Lottie Animation API                                                 */
/************************************************************************/

TVG_API Tvg_Animation* tvg_lottie_animation_new()
{
#ifdef THORVG_LOTTIE_LOADER_SUPPORT
    return (Tvg_Animation*) LottieAnimation::gen().release();
#endif
    return nullptr;
}


TVG_API Tvg_Result tvg_lottie_animation_override(Tvg_Animation* animation, const char* slot)
{
#ifdef THORVG_LOTTIE_LOADER_SUPPORT
    if (!animation) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<LottieAnimation*>(animation)->override(slot);
#endif
    return TVG_RESULT_NOT_SUPPORTED;
}


TVG_API Tvg_Result tvg_lottie_animation_set_marker(Tvg_Animation* animation, const char* marker)
{
#ifdef THORVG_LOTTIE_LOADER_SUPPORT
    if (!animation) return TVG_RESULT_INVALID_ARGUMENT;
    return (Tvg_Result) reinterpret_cast<LottieAnimation*>(animation)->segment(marker);
#endif
    return TVG_RESULT_NOT_SUPPORTED;
}


TVG_API Tvg_Result tvg_lottie_animation_get_markers_cnt(Tvg_Animation* animation, uint32_t* cnt)
{
#ifdef THORVG_LOTTIE_LOADER_SUPPORT
    if (!animation || !cnt) return TVG_RESULT_INVALID_ARGUMENT;
    *cnt = reinterpret_cast<LottieAnimation*>(animation)->markersCnt();
    return TVG_RESULT_SUCCESS;
#endif
    return TVG_RESULT_NOT_SUPPORTED;
}


TVG_API Tvg_Result tvg_lottie_animation_get_marker(Tvg_Animation* animation, uint32_t idx, const char** name)
{
#ifdef THORVG_LOTTIE_LOADER_SUPPORT
    if (!animation || !name) return TVG_RESULT_INVALID_ARGUMENT;
    *name = reinterpret_cast<LottieAnimation*>(animation)->marker(idx);
    if (!(*name)) return TVG_RESULT_INVALID_ARGUMENT;
    return TVG_RESULT_SUCCESS;
#endif
    return TVG_RESULT_NOT_SUPPORTED;
}

#ifdef __cplusplus
}
#endif

#endif /* LV_USE_THORVG_INTERNAL */

