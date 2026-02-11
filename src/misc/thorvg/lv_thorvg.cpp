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

class LvCanvas : public Canvas
{
    public:
        LvCanvas(lv_display_t * disp);
        ~LvCanvas();

        Result target(uint32_t * buffer, uint32_t stride, uint32_t w, uint32_t h);
};

class LvRenderer : public RenderMethod
{
    public:
        RenderRegion m_vport;
        lv_draw_buf_t m_draw_buf;
        lv_layer_t m_layer;
        lv_display_t * m_disp;
        BlendMethod m_blend_method_tvg;
        lv_vector_blend_t m_blend_method;

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
        void finish_layer();
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lvmat_from_tvgmat(lv_matrix_t * dst, const Matrix * src);
static float path_length(const RenderShape & rshape);
static Point line_at(const Line & line, float at);
static lv_grad_stop_t * lvstops_from_tvgstops(const Fill::ColorStop * stops_tvg, uint32_t cnt);

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

    renderer->target(buffer, stride, w, h);
    Canvas::pImpl->vport = {0, 0, (int32_t)w, (int32_t)h};
    renderer->viewport(Canvas::pImpl->vport);

    ImageLoader::cs = ColorSpace::ARGB8888;

    //Paints must be updated again with this new target.
    Canvas::pImpl->status = Status::Damaged;

    return Result::Success;
}

LvRenderer::LvRenderer(lv_display_t * disp) :
    m_disp(disp)
{
}

LvRenderer::~LvRenderer()
{
}

void LvRenderer::target(pixel_t * data, uint32_t stride, uint32_t w, uint32_t h)
{
    lv_draw_buf_init(&m_draw_buf, w, h, LV_COLOR_FORMAT_ARGB8888, stride * 4, data, stride * 4 * h);

    const lv_area_t area = { 0, 0, (int32_t) w - 1, (int32_t) h - 1 };
    lv_layer_init(&m_layer);
    m_layer.draw_buf = &m_draw_buf;
    m_layer.color_format = LV_COLOR_FORMAT_ARGB8888;
    m_layer.buf_area = area;
    m_layer._clip_area = area;
    m_layer.phy_clip_area = area;
}

RenderData LvRenderer::prepare(const RenderShape & rshape, RenderData data, const Matrix & transform,
                               Array<RenderData> & clips, uint8_t opacity, RenderUpdateFlag flags, bool clipper)
{
    LV_UNUSED(clips);
    LV_UNUSED(opacity);
    LV_UNUSED(clipper);
    LV_UNUSED(flags);

    lv_draw_vector_dsc_t * dsc = (lv_draw_vector_dsc_t *) data;
    if(dsc) {
        lv_draw_vector_dsc_delete(dsc);
    }
    dsc = lv_draw_vector_dsc_create(&m_layer);

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

    lv_draw_vector_dsc_set_fill_opa(dsc, rshape.color[3]);
    lv_color_t fill_color;
    fill_color.red = rshape.color[0];
    fill_color.green = rshape.color[1];
    fill_color.blue = rshape.color[2];
    lv_draw_vector_dsc_set_fill_color(dsc, fill_color);

    if(rshape.fill) {
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
    if(!data) return false;
    lv_draw_vector_dsc_t * dsc = (lv_draw_vector_dsc_t *) data;

    lv_draw_vector_dsc_set_blend_mode(dsc, m_blend_method);

    lv_draw_vector(dsc);

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
    if(method == m_blend_method_tvg) return true;
    m_blend_method_tvg = method;

    if(method == BlendMethod::Multiply) m_blend_method = LV_VECTOR_BLEND_MULTIPLY;
    else if(method == BlendMethod::Screen) m_blend_method = LV_VECTOR_BLEND_SCREEN;
    else if(method == BlendMethod::Add) m_blend_method = LV_VECTOR_BLEND_ADDITIVE;
    else m_blend_method = LV_VECTOR_BLEND_SRC_OVER;

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
    finish_layer();
    return true;
}

RenderCompositor * LvRenderer::target(const RenderRegion & region, ColorSpace cs)
{
    return nullptr;
}

bool LvRenderer::beginComposite(RenderCompositor * cmp, CompositeMethod method, uint8_t opacity)
{
    return false;
}

bool LvRenderer::endComposite(RenderCompositor * cmp)
{
    return false;
}


bool LvRenderer::prepare(RenderEffect * effect)
{
    return false;
}

bool LvRenderer::effect(RenderCompositor * cmp, const RenderEffect * effect)
{
    return false;
}

void LvRenderer::finish_layer()
{
    if(m_layer.draw_task_head == NULL) return;

    bool task_dispatched;

    while(m_layer.draw_task_head) {
        lv_draw_dispatch_wait_for_request();
        task_dispatched = lv_draw_dispatch_layer(m_disp, &m_layer);

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

#endif /* LV_USE_THORVG_EXTERNAL || LV_USE_THORVG_INTERNAL */
