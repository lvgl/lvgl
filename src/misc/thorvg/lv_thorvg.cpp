/**
 * @file lv_thorvg.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_thorvg_private.h"

#if LV_USE_THORVG_EXTERNAL || LV_USE_THORVG_INTERNAL

#include "../../draw/lv_draw_vector.h"
#include "../lv_matrix.h"
#include "../lv_area_private.h"
#include "../lv_assert.h"

#include "../../draw/lv_draw_vector_private.h"
#include "../../draw/sw/blend/lv_draw_sw_blend_private.h"
#include "../../draw/sw/blend/lv_draw_sw_blend_to_argb8888.h"


#if LV_USE_THORVG_EXTERNAL
    #include <thorvg.h>
    #include <tvgRender.h>
    #include <tvgCanvas.h>
    #include <tvgLoader.h>
#else
    #include "../../libs/thorvg/thorvg.h"
    #include "../../libs/thorvg/tvgRender.h"
    #include "../../libs/thorvg/tvgCanvas.h"
    #include "../../libs/thorvg/tvgLoader.h"
#endif

#include <math.h>

using namespace tvg;

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

class LvCanvas;
struct LvSurface;
struct LvCompositor;
class LvRenderer;

class LvCanvas : public Canvas
{
    public:
        LvCanvas(lv_display_t * disp);
        ~LvCanvas();

        Result target(uint32_t * buffer, uint32_t stride, uint32_t w, uint32_t h);
};

struct LvSurface
{
    LvCompositor * compositor;
    BlendMethod blend_method_tvg;
    lv_vector_blend_t blend_method;
    lv_layer_t * layer;
};

struct LvCompositor : RenderCompositor
{
    LvSurface * surf_prev;
    LvCompositor * comp_prev;
    lv_layer_t layer;
};

class LvRenderer : public RenderMethod
{
    public:
        RenderRegion m_vport;
        lv_draw_buf_t m_draw_buf;
        lv_display_t * m_disp;
        LvSurface * m_surface;
        LvSurface m_root_surface;
        lv_layer_t m_root_layer;

        LvRenderer(lv_display_t * disp);
        ~LvRenderer();

        void target(pixel_t * data, uint32_t stride, uint32_t w, uint32_t h);

        RenderData prepare(const RenderShape & rshape, RenderData data, const Matrix & transform, Array<RenderData> & clips,
                           uint8_t opacity, RenderUpdateFlag flags, bool clipper) override;
        RenderData prepare(RenderSurface * surface, RenderData data, const Matrix & transform, Array<RenderData> & clips,
                           uint8_t opacity, RenderUpdateFlag flags) override;
        bool preRender() override;
        bool renderShape(RenderData data) override;
        bool renderImage(RenderData data) override;
        bool postRender() override;
        void dispose(RenderData data) override;
        RenderRegion region(RenderData data) override;
        RenderRegion viewport() override;
        bool viewport(const RenderRegion & vp) override;
        bool blend(BlendMethod method) override;
        ColorSpace colorSpace() override;
        const RenderSurface * mainSurface() override;

        bool clear() override;
        bool sync() override;

        RenderCompositor * target(const RenderRegion & region, ColorSpace cs) override;
        bool beginComposite(RenderCompositor * cmp, CompositeMethod method, uint8_t opacity) override;
        bool endComposite(RenderCompositor * cmp) override;

        bool prepare(RenderEffect * effect) override;
        bool effect(RenderCompositor * cmp, const RenderEffect * effect) override;

    private:
        void finish_layer(lv_layer_t * layer);
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lvmat_from_tvgmat(lv_matrix_t * dst, const Matrix * src);
static float path_length(const RenderShape & rshape);
static Point line_at(const Line & line, float at);
static lv_grad_stop_t * lvstops_from_tvgstops(const Fill::ColorStop * stops_tvg, uint32_t cnt);
static void invert_alpha(lv_opa_t * data, uint32_t w, uint32_t h, uint32_t stride);
static void argb8888_to_a8(const lv_draw_sw_blend_image_dsc_t * dsc);
static void blend_image_to_a8(const lv_draw_sw_blend_image_dsc_t * dsc);
static void a8_to_argb8888(const lv_draw_sw_blend_image_dsc_t * dsc);
static void blend_image_from_a8(const lv_draw_sw_blend_image_dsc_t * dsc);
static bool blend_img_dsc_prepare(
    lv_draw_sw_blend_image_dsc_t * dsc,
    lv_layer_t * dst_layer,
    lv_layer_t * src_layer,
    lv_layer_t * mask_layer
);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

extern "C" {

    Tvg_Canvas * lv_thorvg_canvas_create(lv_display_t * disp)
    {
        return (Tvg_Canvas *) new LvCanvas(disp);
    }

    Tvg_Result lv_thorvg_canvas_set_target(Tvg_Canvas * canvas, uint32_t * buffer, uint32_t stride, uint32_t w, uint32_t h)
    {
        if(!canvas) return TVG_RESULT_INVALID_ARGUMENT;
        LvCanvas * lvcanvas = (LvCanvas *) canvas;
        lvcanvas->target(buffer, stride, w, h);
        return (Tvg_Result) Result::Success;
    }

} /* extern "C" */

/**********************
 *   STATIC FUNCTIONS
 **********************/

LvCanvas::LvCanvas(lv_display_t * disp) : Canvas(new LvRenderer(disp))
{
}

LvCanvas::~LvCanvas()
{
}

Result LvCanvas::target(uint32_t * buffer, uint32_t stride, uint32_t w, uint32_t h)
{
    if(Canvas::pImpl->status != Status::Damaged && Canvas::pImpl->status != Status::Synced) {
        return Result::InsufficientCondition;
    }

    LvRenderer * renderer = (LvRenderer *) Canvas::pImpl->renderer;

    // if(renderer->m_compositor) {
    //     LV_LOG_WARN("Not prepared to set a target while the renderer has an active compositor.");
    //     return Result::InsufficientCondition;
    // }

    renderer->target(buffer, stride, w, h);
    Canvas::pImpl->vport = {0, 0, (int32_t)w, (int32_t)h};
    renderer->viewport(Canvas::pImpl->vport);

    ImageLoader::cs = ColorSpace::ARGB8888;

    //Paints must be updated again with this new target.
    Canvas::pImpl->status = Status::Damaged;

    return Result::Success;
}

LvRenderer::LvRenderer(lv_display_t * disp)
{
    m_disp = disp;
    m_surface = &m_root_surface;
    m_root_surface.compositor = nullptr;
    m_root_surface.blend_method_tvg = BlendMethod::Normal;
    m_root_surface.blend_method = LV_VECTOR_BLEND_SRC_OVER;
    m_root_surface.layer = &m_root_layer;
}

LvRenderer::~LvRenderer()
{
}

void LvRenderer::target(pixel_t * data, uint32_t stride, uint32_t w, uint32_t h)
{
    lv_draw_buf_init(&m_draw_buf, w, h, LV_COLOR_FORMAT_ARGB8888, stride * 4, data, stride * 4 * h);
    lv_draw_buf_clear(&m_draw_buf, NULL);

    const lv_area_t area = { 0, 0, (int32_t) w - 1, (int32_t) h - 1 };
    lv_layer_init(&m_root_layer);
    m_root_layer.draw_buf = &m_draw_buf;
    m_root_layer.color_format = LV_COLOR_FORMAT_ARGB8888;
    m_root_layer.buf_area = area;
    m_root_layer._clip_area = area;
    m_root_layer.phy_clip_area = area;
}

RenderData LvRenderer::prepare(const RenderShape & rshape, RenderData data, const Matrix & transform,
                               Array<RenderData> & clips, uint8_t opacity, RenderUpdateFlag flags, bool clipper)
{
    LV_UNUSED(clips);
    LV_UNUSED(opacity);
    LV_UNUSED(clipper);
    LV_UNUSED(flags);

    // LV_LOG_USER("prepare");

    lv_draw_vector_dsc_t * dsc = (lv_draw_vector_dsc_t *) data;
    if(dsc) {
        lv_draw_vector_dsc_delete(dsc);
    }
    dsc = lv_draw_vector_dsc_create(m_surface->layer);

    lv_matrix_t mat;
    Matrix mat_tvg;
    uint32_t stop_cnt;
    const Fill::ColorStop * stops_tvg;
    lv_grad_stop_t * stops;


    lvmat_from_tvgmat(&mat, &transform);
    lv_draw_vector_dsc_set_transform(dsc, &mat);


    /* fill */

    lv_draw_vector_dsc_set_fill_rule(dsc, rshape.rule == FillRule::Winding ? LV_VECTOR_FILL_NONZERO :
                                     rshape.rule == FillRule::EvenOdd ? LV_VECTOR_FILL_EVENODD :
                                     LV_VECTOR_FILL_NONZERO);

    if(rshape.fill) {
        lv_draw_vector_dsc_set_fill_opa(dsc, LV_OPA_COVER);

        FillSpread spread = rshape.fill->spread();
        lv_draw_vector_dsc_set_fill_gradient_spread(dsc, spread == FillSpread::Pad ? LV_VECTOR_GRADIENT_SPREAD_PAD :
                                                    spread == FillSpread::Reflect ? LV_VECTOR_GRADIENT_SPREAD_REFLECT :
                                                    spread == FillSpread::Repeat ? LV_VECTOR_GRADIENT_SPREAD_REPEAT :
                                                    LV_VECTOR_GRADIENT_SPREAD_PAD);
        mat_tvg = rshape.fill->transform();
        lvmat_from_tvgmat(&mat, &mat_tvg);
        lv_draw_vector_dsc_set_fill_transform(dsc, &mat);
        stop_cnt = rshape.fill->colorStops(&stops_tvg);
        stops = lvstops_from_tvgstops(stops_tvg, stop_cnt);
        lv_draw_vector_dsc_set_fill_gradient_color_stops(dsc, stops, stop_cnt);
        lv_free(stops);
        if(rshape.fill->type() == Type::LinearGradient) {
            const LinearGradient * linear_grad = (const LinearGradient *) rshape.fill;
            float x1, y1, x2, y2;
            if(linear_grad->linear(&x1, &y1, &x2, &y2) == Result::Success) {
                lv_draw_vector_dsc_set_fill_linear_gradient(dsc, x1, y1, x2, y2);
            }
        }
        else if(rshape.fill->type() == Type::RadialGradient) {
            const RadialGradient * linear_grad = (const RadialGradient *) rshape.fill;
            float cx, cy, radius;
            if(linear_grad->radial(&cx, &cy, &radius) == Result::Success) {
                lv_draw_vector_dsc_set_fill_radial_gradient(dsc, cx, cy, radius);
            }
        }
    }
    else {
        lv_draw_vector_dsc_set_fill_opa(dsc, rshape.color[3]);
        lv_color_t fill_color;
        fill_color.red = rshape.color[0];
        fill_color.green = rshape.color[1];
        fill_color.blue = rshape.color[2];
        lv_draw_vector_dsc_set_fill_color(dsc, fill_color);
    }


    /* stroke */

    if(rshape.stroke) {
        lv_draw_vector_dsc_set_stroke_width(dsc, rshape.stroke->width);
        lv_draw_vector_dsc_set_stroke_opa(dsc, 255);
        lv_color32_t stroke_color;
        stroke_color.red = rshape.stroke->color[0];
        stroke_color.green = rshape.stroke->color[1];
        stroke_color.blue = rshape.stroke->color[2];
        stroke_color.alpha = rshape.stroke->color[3];
        lv_draw_vector_dsc_set_stroke_color32(dsc, stroke_color);
        lv_draw_vector_dsc_set_stroke_cap(dsc, rshape.stroke->cap == StrokeCap::Square ? LV_VECTOR_STROKE_CAP_SQUARE :
                                          rshape.stroke->cap == StrokeCap::Round ? LV_VECTOR_STROKE_CAP_ROUND :
                                          rshape.stroke->cap == StrokeCap::Butt ? LV_VECTOR_STROKE_CAP_BUTT :
                                          LV_VECTOR_STROKE_CAP_BUTT);
        lv_draw_vector_dsc_set_stroke_join(dsc, rshape.stroke->join == StrokeJoin::Bevel ? LV_VECTOR_STROKE_JOIN_BEVEL :
                                           rshape.stroke->join == StrokeJoin::Round ? LV_VECTOR_STROKE_JOIN_ROUND :
                                           rshape.stroke->join == StrokeJoin::Miter ? LV_VECTOR_STROKE_JOIN_MITER :
                                           LV_VECTOR_STROKE_JOIN_MITER);
        lv_draw_vector_dsc_set_stroke_miter_limit(dsc, rshape.stroke->miterlimit);

        if(rshape.stroke->dashPattern) {
            lv_draw_vector_dsc_set_stroke_dash(dsc, rshape.stroke->dashPattern, rshape.stroke->dashCnt);
        }

        if(rshape.stroke->fill) {
            FillSpread spread = rshape.stroke->fill->spread();
            lv_draw_vector_dsc_set_stroke_gradient_spread(dsc, spread == FillSpread::Pad ? LV_VECTOR_GRADIENT_SPREAD_PAD :
                                                          spread == FillSpread::Reflect ? LV_VECTOR_GRADIENT_SPREAD_REFLECT :
                                                          spread == FillSpread::Repeat ? LV_VECTOR_GRADIENT_SPREAD_REPEAT :
                                                          LV_VECTOR_GRADIENT_SPREAD_PAD);
            mat_tvg = rshape.stroke->fill->transform();
            lvmat_from_tvgmat(&mat, &mat_tvg);
            lv_draw_vector_dsc_set_stroke_transform(dsc, &mat);
            stop_cnt = rshape.stroke->fill->colorStops(&stops_tvg);
            stops = lvstops_from_tvgstops(stops_tvg, stop_cnt);
            lv_draw_vector_dsc_set_stroke_gradient_color_stops(dsc, stops, stop_cnt);
            lv_free(stops);
            if(rshape.stroke->fill->type() == Type::LinearGradient) {
                const LinearGradient * linear_grad = (const LinearGradient *) rshape.stroke->fill;
                float x1, y1, x2, y2;
                if(linear_grad->linear(&x1, &y1, &x2, &y2) == Result::Success) {
                    lv_draw_vector_dsc_set_stroke_linear_gradient(dsc, x1, y1, x2, y2);
                }
            }
            else if(rshape.stroke->fill->type() == Type::RadialGradient) {
                const RadialGradient * linear_grad = (const RadialGradient *) rshape.stroke->fill;
                float cx, cy, radius;
                if(linear_grad->radial(&cx, &cy, &radius) == Result::Success) {
                    lv_draw_vector_dsc_set_stroke_radial_gradient(dsc, cx, cy, radius);
                }
            }
        }
    }


    /* path */

    lv_vector_path_t * vp = lv_vector_path_create(LV_VECTOR_PATH_QUALITY_HIGH);

    float l = 0;

    float trim_begin = rshape.stroke ? rshape.stroke->trim.begin : -INFINITY;
    float trim_end = rshape.stroke ? rshape.stroke->trim.end : INFINITY;
    if(trim_end < trim_begin) {
        float tmp = trim_begin;
        trim_begin = trim_end;
        trim_end = tmp;
    }
    if(trim_begin <= 0.0f && trim_end >= 1.0f) {
        trim_begin = -INFINITY;
        trim_end = INFINITY;
    }
    else {
        float tot_l = path_length(rshape);
        trim_begin *= tot_l;
        trim_end *= tot_l;
    }

    const Point zero_point = {0, 0};
    const Point * first = rshape.path.cmds.count ? &rshape.path.pts[0] : &zero_point;
    const Point * prev = &zero_point;

    bool did_a_moveto = false;
    uint32_t pt_i = 0;
    for(uint32_t i = 0; i < rshape.path.cmds.count; i++) {
        lv_fpoint_t pts[3];
        switch(rshape.path.cmds[i]) {
            case PathCommand::Close:
                if(l >= trim_begin && l <= trim_end) {
                    lv_vector_path_close(vp);
                }
                l += length(prev, first);
                break;
            case PathCommand::MoveTo:
                if(l >= trim_begin && l <= trim_end) {
                    did_a_moveto = true;
                    pts[0].x = rshape.path.pts[pt_i].x;
                    pts[0].y = rshape.path.pts[pt_i].y;
                    lv_vector_path_move_to(vp, &pts[0]);
                }
                prev = &rshape.path.pts[pt_i];
                pt_i++;
                break;
            case PathCommand::LineTo: {
                    const Line line = {*prev, rshape.path.pts[pt_i]};

                    float seg_len = line.length();
                    float seg_start = l;
                    l += seg_len;

                    if(!(l < trim_begin || seg_start > trim_end)) {
                        if(!did_a_moveto) {
                            did_a_moveto = true;
                            float trimmed_start = trim_begin - seg_start;
                            Point moveto_point = line_at(line, trimmed_start);
                            pts[0].x = moveto_point.x;
                            pts[0].y = moveto_point.y;
                            lv_vector_path_move_to(vp, &pts[0]);
                        }

                        Point end_point;
                        if(trim_end >= l) {
                            end_point = line.pt2;
                        }
                        else {
                            float trimmed_end = trim_end - seg_start;
                            end_point = line_at(line, trimmed_end);
                        }
                        pts[0].x = end_point.x;
                        pts[0].y = end_point.y;
                        lv_vector_path_line_to(vp, &pts[0]);
                    }

                    prev = &rshape.path.pts[pt_i];
                    pt_i++;
                    break;
                }
            case PathCommand::CubicTo: {
                    Bezier bez = {*prev, rshape.path.pts[pt_i], rshape.path.pts[pt_i + 1], rshape.path.pts[pt_i + 2]};

                    float seg_len = bez.length();
                    float seg_start = l;
                    l += seg_len;

                    if(!(l < trim_begin || seg_start > trim_end)) {
                        Bezier bez_left;

                        if(!did_a_moveto) {
                            did_a_moveto = true;
                            float trimmed_start = trim_begin - seg_start;
                            float t = trimmed_start / seg_len;
                            /* `bez` becomes the right split. `bez_left` unused */
                            bez.split(t, bez_left);
                            pts[0].x = bez.start.x;
                            pts[0].y = bez.start.y;
                            lv_vector_path_move_to(vp, &pts[0]);
                        }

                        const Bezier * cubic_to;
                        if(trim_end >= l) {
                            cubic_to = &bez;
                        }
                        else {
                            float trimmed_end = trim_end - seg_start;
                            float t = trimmed_end / seg_len;
                            /* `bez` becomes the right split */
                            bez.split(t, bez_left);
                            cubic_to = &bez_left;
                        }
                        pts[0].x = cubic_to->ctrl1.x;
                        pts[0].y = cubic_to->ctrl1.y;
                        pts[1].x = cubic_to->ctrl2.x;
                        pts[1].y = cubic_to->ctrl2.y;
                        pts[2].x = cubic_to->end.x;
                        pts[2].y = cubic_to->end.y;
                        lv_vector_path_cubic_to(vp, &pts[0], &pts[1], &pts[2]);
                    }

                    prev = &rshape.path.pts[pt_i + 2];
                    pt_i += 3;
                    break;
                }
            default:
                LV_ASSERT(0);
                break;
        }
    }

    lv_draw_vector_dsc_add_path(dsc, vp);

    lv_vector_path_delete(vp);

    return dsc;
}

RenderData LvRenderer::prepare(RenderSurface * surface, RenderData data, const Matrix & transform,
                               Array<RenderData> & clips, uint8_t opacity, RenderUpdateFlag flags)
{
    return nullptr;
}

bool LvRenderer::preRender()
{
    return true;
}

bool LvRenderer::renderShape(RenderData data)
{
    if(!data) {
        // LV_LOG_USER("renderShape (null)");
        return false;
    }
    lv_draw_vector_dsc_t * dsc = (lv_draw_vector_dsc_t *) data;
    // lv_color32_t col = dsc->ctx->fill_dsc.color;
    // LV_LOG_USER("renderShape %p %d %d %d %d", dsc, col.red, col.green, col.blue, col.alpha);

    dsc->base.layer = m_surface->layer;
    lv_draw_vector_dsc_set_blend_mode(dsc, m_surface->blend_method);

    lv_draw_vector(dsc);

    finish_layer(m_surface->layer);

    return true;
}

bool LvRenderer::renderImage(RenderData data)
{
    return false;
}

bool LvRenderer::postRender()
{
    return true;
}

void LvRenderer::dispose(RenderData data)
{
    if(!data) return;
    lv_draw_vector_dsc_t * dsc = (lv_draw_vector_dsc_t *) data;
    lv_draw_vector_dsc_delete(dsc);
}

RenderRegion LvRenderer::region(RenderData data)
{
    return {0, 0, (int32_t) m_draw_buf.header.w, (int32_t) m_draw_buf.header.h};
}

RenderRegion LvRenderer::viewport()
{
    return m_vport;
}

bool LvRenderer::viewport(const RenderRegion & vp)
{
    m_vport = vp;
    return true;
}

bool LvRenderer::blend(BlendMethod method)
{
    if(method == m_surface->blend_method_tvg) return true;
    m_surface->blend_method_tvg = method;

    if(method == BlendMethod::Multiply) m_surface->blend_method = LV_VECTOR_BLEND_MULTIPLY;
    else if(method == BlendMethod::Screen) m_surface->blend_method = LV_VECTOR_BLEND_SCREEN;
    else if(method == BlendMethod::Add) m_surface->blend_method = LV_VECTOR_BLEND_ADDITIVE;
    else m_surface->blend_method = LV_VECTOR_BLEND_SRC_OVER;

    return false;
}

ColorSpace LvRenderer::colorSpace()
{
    return ColorSpace::ARGB8888;
}

const RenderSurface * LvRenderer::mainSurface()
{
    return nullptr;
}

bool LvRenderer::clear()
{
    return true;
}

bool LvRenderer::sync()
{
    // if(m_compositor) {
    //     LV_LOG_WARN("Was not expecting there to be an unfinished composition at sync");
    // }
    finish_layer(m_root_surface.layer);
    return true;
}

RenderCompositor * LvRenderer::target(const RenderRegion & region, ColorSpace cs)
{
    lv_color_format_t cf;
    switch(cs) {
        case ColorSpace::ARGB8888:
            cf = LV_COLOR_FORMAT_ARGB8888;
            // LV_LOG_USER("target ARGB8888 %d %d", region.w, region.h);
            break;
        case ColorSpace::Grayscale8:
            cf = LV_COLOR_FORMAT_A8;
            // LV_LOG_USER("target A8 %d %d", region.w, region.h);
            break;
        default:
            // LV_LOG_USER("target ??? %d %d", region.w, region.h);
            return nullptr;
    }

    lv_area_t area;
    area.x1 = region.x;
    area.y1 = region.y;
    lv_area_set_width(&area, region.w);
    lv_area_set_height(&area, region.h);

    if(!lv_area_intersect(&area, &area, &m_surface->layer->buf_area)) {
        return nullptr;
    }

    LvSurface * surf = new LvSurface;
    surf->compositor = new LvCompositor;
    surf->compositor->surf_prev = m_surface;
    surf->compositor->comp_prev = m_surface->compositor;
    lv_layer_init(&surf->compositor->layer);
    surf->compositor->layer.color_format = cf;
    surf->compositor->layer.buf_area = area;
    surf->compositor->layer._clip_area = area;
    surf->compositor->layer.phy_clip_area = area;
    surf->compositor->layer.draw_buf = lv_draw_buf_create(lv_area_get_width(&area),
                                                          lv_area_get_height(&area),
                                                          cf, 0);
    lv_draw_buf_clear(surf->compositor->layer.draw_buf, NULL);
    surf->blend_method_tvg = BlendMethod::Normal;
    surf->blend_method = LV_VECTOR_BLEND_SRC_OVER;
    surf->layer = &surf->compositor->layer;

    m_surface = surf;

    return surf->compositor;
}

bool LvRenderer::beginComposite(RenderCompositor * cmp, CompositeMethod method, uint8_t opacity)
{
    // LV_LOG_USER("beginComposite %p %d %d", cmp, (int) method, (int) opacity);
    if(!cmp) return false;
    LvCompositor * comp = (LvCompositor *) cmp;

    comp->method = method;
    comp->opacity = opacity;

    if (comp->method != CompositeMethod::None) {
        m_surface = comp->surf_prev;
        m_surface->compositor = comp;
    }

    return true;
}

bool LvRenderer::endComposite(RenderCompositor * cmp)
{
    LvCompositor * comp = (LvCompositor *) cmp;
    // LV_LOG_USER("endComposite %p", cmp);

    m_surface = comp->surf_prev;
    m_surface->compositor = comp->comp_prev;

    if (comp->method == CompositeMethod::None) {
        lv_layer_t * mask_layer = m_surface->compositor
                                  && m_surface->compositor->layer.color_format == LV_COLOR_FORMAT_A8
                                  ? &m_surface->compositor->layer : NULL;

        finish_layer(m_surface->layer);
        if(mask_layer) finish_layer(mask_layer);
        finish_layer(&comp->layer);

        lv_draw_sw_blend_image_dsc_t dsc;
        lv_memzero(&dsc, sizeof(dsc));
        dsc.opa = comp->opacity;
        lv_draw_layer_alloc_buf(m_surface->layer);
        lv_draw_layer_alloc_buf(&comp->layer);
        if(mask_layer) lv_draw_layer_alloc_buf(mask_layer);
        blend_img_dsc_prepare(&dsc, m_surface->layer, &comp->layer, mask_layer);
        if(mask_layer && m_surface->compositor->method == CompositeMethod::InvAlphaMask) {
            invert_alpha(
                (lv_opa_t *) dsc.mask_buf,
                dsc.dest_w,
                dsc.dest_h,
                dsc.mask_stride
            );
        }

        // lv_draw_layer_alloc_buf(m_surface->layer);
        // dsc.dest_buf = m_surface->layer->draw_buf->data;
        // dsc.dest_w = m_surface->layer->draw_buf->header.w;
        // dsc.dest_h = m_surface->layer->draw_buf->header.h;
        // dsc.dest_stride = m_surface->layer->draw_buf->header.stride;
        // if(mask_layer) {
        //     lv_draw_layer_alloc_buf(mask_layer);
        //     dsc.mask_buf = mask_layer->draw_buf->data;
        //     dsc.mask_stride = mask_layer->draw_buf->header.stride;

        //     if(m_surface->compositor->method == CompositeMethod::InvAlphaMask) {
        //         invert_alpha(
        //             mask_layer->draw_buf->data,
        //             mask_layer->draw_buf->header.w,
        //             mask_layer->draw_buf->header.h,
        //             mask_layer->draw_buf->header.stride
        //         );
        //     }
        // }
        // dsc.src_buf = comp->layer.draw_buf->data;
        // dsc.src_stride = comp->layer.draw_buf->header.stride;
        // dsc.src_color_format = comp->layer.color_format;
        // dsc.opa = comp->opacity;
        // // dsc.blend_mode = 
        // dsc.src_area = comp->layer.buf_area;

        if(m_surface->layer->color_format == LV_COLOR_FORMAT_A8) {
            blend_image_to_a8(&dsc);
        }
        else if(dsc.src_color_format == LV_COLOR_FORMAT_A8) {
            blend_image_from_a8(&dsc);
        }
        else {
            lv_draw_sw_blend_image_to_argb8888(&dsc);
        }

        if(mask_layer && m_surface->compositor->method == CompositeMethod::InvAlphaMask) {
            invert_alpha(
                (lv_opa_t *) dsc.mask_buf,
                dsc.dest_w,
                dsc.dest_h,
                dsc.mask_stride
            );
        }

        // if(mask_layer) {
        //     if(m_surface->compositor->method == CompositeMethod::InvAlphaMask) {
        //         invert_alpha(
        //             mask_layer->draw_buf->data,
        //             mask_layer->draw_buf->header.w,
        //             mask_layer->draw_buf->header.h,
        //             mask_layer->draw_buf->header.stride
        //         );
        //     }
        // }

        // lv_draw_image_dsc_init(&dsc);
        // dsc.src = &comp->layer;
        // dsc.bitmap_mask_src = (lv_image_dsc_t *)((LvCompositor *) m_layer_act->user_data)->layer.draw_buf;
        // comp->layer.parent = m_layer_act;
        // lv_draw_layer(m_layer_act, &dsc, &comp->layer.buf_area);
        // finish_layer(m_layer_act);
    }

    lv_draw_buf_destroy(comp->layer.draw_buf);
    delete comp;

    return true;
}


bool LvRenderer::prepare(RenderEffect * effect)
{
    return false;
}

bool LvRenderer::effect(RenderCompositor * cmp, const RenderEffect * effect)
{
    return false;
}

void LvRenderer::finish_layer(lv_layer_t * layer)
{
    while(layer->draw_task_head) {
        lv_draw_dispatch_wait_for_request();
        bool task_dispatched = lv_draw_dispatch_layer(m_disp, layer);

        if(!task_dispatched) {
            lv_draw_wait_for_finish();
            lv_draw_dispatch_request();
        }
    }
}

static void lvmat_from_tvgmat(lv_matrix_t * dst, const Matrix * src)
{
    dst->m[0][0] = src->e11;
    dst->m[0][1] = src->e12;
    dst->m[0][2] = src->e13;
    dst->m[1][0] = src->e21;
    dst->m[1][1] = src->e22;
    dst->m[1][2] = src->e23;
    dst->m[2][0] = src->e31;
    dst->m[2][1] = src->e32;
    dst->m[2][2] = src->e33;
}

static float path_length(const RenderShape & rshape)
{
    float l = 0;

    const Point zero_point = {0, 0};
    const Point * first = rshape.path.cmds.count ? &rshape.path.pts[0] : &zero_point;
    const Point * prev = &zero_point;

    uint32_t pt_i = 0;
    for(uint32_t i = 0; i < rshape.path.cmds.count; i++) {
        switch(rshape.path.cmds[i]) {
            case PathCommand::Close:
                l += length(prev, first);
                break;
            case PathCommand::MoveTo:
                prev = &rshape.path.pts[pt_i];
                pt_i++;
                break;
            case PathCommand::LineTo:
                l += length(prev, &rshape.path.pts[pt_i]);
                prev = &rshape.path.pts[pt_i];
                pt_i++;
                break;
            case PathCommand::CubicTo:
                l += Bezier{*prev, rshape.path.pts[pt_i], rshape.path.pts[pt_i + 1], rshape.path.pts[pt_i + 2]}.length();
                prev = &rshape.path.pts[pt_i + 2];
                pt_i += 3;
                break;
            default:
                LV_ASSERT(0);
                break;
        }
    }

    return l;
}

static Point line_at(const Line & line, float at)
{
    float len = line.length();
    return {
        ((line.pt2.x - line.pt1.x) / len) * at,
        ((line.pt2.y - line.pt1.y) / len) * at
    };
}

static lv_grad_stop_t * lvstops_from_tvgstops(const Fill::ColorStop * stops_tvg, uint32_t cnt)
{
    lv_grad_stop_t * stops = (lv_grad_stop_t *) lv_malloc(cnt * sizeof(*stops));
    for(uint32_t i = 0; i < cnt; i++) {
        stops[i].color.blue = stops_tvg[i].b;
        stops[i].color.green = stops_tvg[i].g;
        stops[i].color.red = stops_tvg[i].r;
        stops[i].frac = stops_tvg[i].offset * 255.0f;
        stops[i].opa = stops_tvg[i].a;
    }
    return stops;
}

static void invert_alpha(lv_opa_t * data, uint32_t w, uint32_t h, uint32_t stride)
{
    lv_opa_t * row = data;

    for(uint32_t y = 0; y < h; y++) {
        for(uint32_t x = 0; x < w; x++) {
            row[x] = 255 - row[x];
        }

        row += stride;
    }
}

static void argb8888_to_a8(const lv_draw_sw_blend_image_dsc_t * dsc)
{
    const int32_t h = dsc->dest_h;
    const int32_t w = dsc->dest_w;
    const int32_t src_stride = dsc->src_stride;
    const int32_t dst_stride = dsc->dest_stride;

    const lv_color32_t * src_p = (const lv_color32_t *) dsc->src_buf;
    lv_opa_t * dst_p = (lv_opa_t *) dsc->dest_buf;

    for(int32_t y = 0; y < h; y++) {
        for(int32_t x = 0; x < w; x++) {
            dst_p[x] = src_p[x].alpha;
        }

        src_p += src_stride / 4;
        dst_p += dst_stride;
    }
}

static void blend_image_to_a8(const lv_draw_sw_blend_image_dsc_t * dsc)
{
    if(dsc->mask_buf == NULL) {
        argb8888_to_a8(dsc);
        return;
    }

    const int32_t h = dsc->dest_h;
    const int32_t w = dsc->dest_w;
    const int32_t src_stride = dsc->src_stride;
    const int32_t dst_stride = dsc->dest_stride;
    const int32_t mask_stride = dsc->mask_stride;

    const lv_color32_t * src_p = (const lv_color32_t *) dsc->src_buf;
    lv_opa_t * dst_p = (lv_opa_t *) dsc->dest_buf;
    const lv_opa_t * mask_p = (lv_opa_t *) dsc->mask_buf;

    for(int32_t y = 0; y < h; y++) {
        for(int32_t x = 0; x < w; x++) {
            dst_p[x] = LV_OPA_MIX2(src_p[x].alpha, mask_p[x]);
        }

        src_p += src_stride / 4;
        dst_p += dst_stride;
        mask_p += mask_stride;
    }
}

static void a8_to_argb8888(const lv_draw_sw_blend_image_dsc_t * dsc)
{
    const int32_t h = dsc->dest_h;
    const int32_t w = dsc->dest_w;
    const int32_t src_stride = dsc->src_stride;
    const int32_t dst_stride = dsc->dest_stride;

    const lv_opa_t * src_p = (const lv_opa_t *) dsc->src_buf;
    lv_color32_t * dst_p = (lv_color32_t *) dsc->dest_buf;

    for(int32_t y = 0; y < h; y++) {
        for(int32_t x = 0; x < w; x++) {
            dst_p[x].alpha = src_p[x];
        }

        src_p += src_stride;
        dst_p += dst_stride / 4;
    }
}

static void blend_image_from_a8(const lv_draw_sw_blend_image_dsc_t * dsc)
{
    if(dsc->mask_buf == NULL) {
        a8_to_argb8888(dsc);
        return;
    }

    const int32_t h = dsc->dest_h;
    const int32_t w = dsc->dest_w;
    const int32_t src_stride = dsc->src_stride;
    const int32_t dst_stride = dsc->dest_stride;
    const int32_t mask_stride = dsc->mask_stride;

    const lv_opa_t * src_p = (const lv_opa_t *) dsc->src_buf;
    lv_color32_t * dst_p = (lv_color32_t *) dsc->dest_buf;
    const lv_opa_t * mask_p = (lv_opa_t *) dsc->mask_buf;

    for(int32_t y = 0; y < h; y++) {
        for(int32_t x = 0; x < w; x++) {
            dst_p[x].alpha = LV_OPA_MIX2(src_p[x], mask_p[x]);
        }

        src_p += src_stride;
        dst_p += dst_stride / 4;
        mask_p += mask_stride;
    }
}

static bool blend_img_dsc_prepare(
    lv_draw_sw_blend_image_dsc_t * dsc,
    lv_layer_t * dst_layer,
    lv_layer_t * src_layer,
    lv_layer_t * mask_layer
)
{
    lv_area_t area;
    if(!lv_area_intersect(&area, &dst_layer->buf_area, &src_layer->buf_area)) {
        return false;
    }
    if(mask_layer) {
        if(!lv_area_intersect(&area, &area, &mask_layer->buf_area)) {
            return false;
        }
    }

    dsc->dest_w = lv_area_get_width(&area);
    dsc->dest_h = lv_area_get_height(&area);
    dsc->dest_buf = lv_draw_layer_go_to_xy(dst_layer, area.x1 - dst_layer->buf_area.x1, area.y1 - dst_layer->buf_area.y1);
    dsc->dest_stride = dst_layer->draw_buf->header.stride
                       + (lv_color_format_get_size(dst_layer->color_format)
                          * (lv_area_get_width(&dst_layer->buf_area)
                             - lv_area_get_width(&area)));
    dsc->src_buf = lv_draw_layer_go_to_xy(src_layer, area.x1 - src_layer->buf_area.x1, area.y1 - src_layer->buf_area.y1);
    dsc->src_stride = src_layer->draw_buf->header.stride;
                    //   + (lv_color_format_get_size(src_layer->color_format)
                    //      * (lv_area_get_width(&src_layer->buf_area)
                    //         - lv_area_get_width(&area)));
    dsc->src_color_format = src_layer->color_format;
    if(mask_layer) {
        dsc->mask_buf = (lv_opa_t *) lv_draw_layer_go_to_xy(mask_layer, area.x1 - mask_layer->buf_area.x1, area.y1 - mask_layer->buf_area.y1);
        dsc->mask_stride = mask_layer->draw_buf->header.stride;
                        //    + (lv_area_get_width(&dst_layer->buf_area)
                        //       - lv_area_get_width(&area));
    }

    return true;
}

#endif /* LV_USE_THORVG_EXTERNAL || LV_USE_THORVG_INTERNAL */
