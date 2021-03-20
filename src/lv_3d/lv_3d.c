/**
 * @file lv_3b.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_3d.h"
#if LV_USE_3D != 0

#include "../lv_misc/lv_debug.h"
#include "../lv_core/lv_group.h"
#include "../lv_themes/lv_theme.h"
#include "./lvgl/src/lv_misc/lv_math.h"
#include "./lvgl/src/lv_draw/lv_draw_triangle.h"
/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_3d"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_3d_signal(lv_obj_t * cb, lv_signal_t sign, void * param);
static lv_design_res_t lv_3d_design(lv_obj_t * obj, const lv_area_t * clip_area, lv_design_mode_t mode);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

#define WID_L LV_DPI

static uint16_t vertex_num = 8;
static int vertex_array[] =
{
    -WID_L, -WID_L, -WID_L,  // 0
     WID_L, -WID_L, -WID_L,  // 1
     WID_L,  WID_L, -WID_L,  // 2
    -WID_L,  WID_L, -WID_L,  // 3
    -WID_L, -WID_L,  WID_L,  // 4
     WID_L, -WID_L,  WID_L,  // 5
     WID_L,  WID_L,  WID_L,  // 6
    -WID_L,  WID_L,  WID_L,  // 7
};

static uint16_t surface_num = 6;
static uint16_t surface_array[] =
{
    0, 1, 2, 3,  // front
    7, 6, 5, 4,  // back
    4, 5, 1, 0,  // bottom
    3, 2, 6, 7,  // top
    7, 4, 0, 3,  // left
    2, 1, 5, 6,  // right
};

#define RGBto565(a,b,c) ((((a>>3)%32)<<11)|((((b>>2))%64)<<5)|((c>>3)%32))

#define COL11 RGBto565(255,0,0)
#define COL12 RGBto565(0,255,0)
#define COL13 RGBto565(0,0,255)

#define COL21 RGBto565(255,255,0)
#define COL22 RGBto565(0,255,255)
#define COL23 RGBto565(255,0,255)

static lv_color_t color_array[] =
{
    COL13,
    COL22,
    COL11,
    COL23,
    COL12,
    COL21,
};

_3d_obj_data demo_3d_obj = {
    (uint16_t *)&vertex_num,
    #if OBJ_3D_USE_FLOAT
        (float *)vertex_array,
    #else
        (int *)vertex_array,
    #endif
    (uint16_t *)&surface_num,
    (uint16_t *)surface_array,
    color_array,
};

static _3d_obj_para obj_para={
    LV_DPI,
    LV_DPI,

    1,
    0,
    0,
    0,
    0,
    0,

    150,
    0,
    0,
};

#define MAXVERTS 500
#define MAXSURFS 800

static int transVerts[MAXVERTS*3];
static int projVerts[MAXVERTS*2];
static int sortedPolys[MAXSURFS];
static int normZ[MAXSURFS];

#define MAXSIN 255
#define swap(a, b) { int t = a; a = b; b = t; }

#define USE_STAR_BG 1

#if USE_STAR_BG
typedef struct {
    int16_t x,y,z;
    int16_t x2d,y2d, x2dOld,y2dOld;
} Star;

#define NUM_STARS 150
Star stars[NUM_STARS];
#endif

const static uint8_t sinTab[91] =
{
    0,4,8,13,17,22,26,31,35,39,44,48,53,57,61,65,70,74,78,83,87,91,95,99,103,107,111,115,119,123,127,
    131,135,138,142,146,149,153,156,160,163,167,170,173,177,180,183,186,189,192,195,198,200,203,206,
    208,211,213,216,218,220,223,225,227,229,231,232,234,236,238,239,241,242,243,245,246,247,248,249,
    250,251,251,252,253,253,254,254,254,254,254,255
};

static int fastSin(int i)
{
    while(i<0) i+=360;
    while(i>=360) i-=360;
    if(i<90)
        return(sinTab[i]);
    else if(i<180)
        return(sinTab[180-i]);
    else if(i<270)
        return(-sinTab[i-180]);
    else
        return(-sinTab[360-i]);
}

static int fastCos(int i)
{
    return fastSin(i+90);
}

static void cullQuads(lv_obj_t * obj,int *v)
{
    int x1,y1,x2,y2,z;

    lv_3d_ext_t * ext = lv_obj_get_ext_attr(obj);
    ext->obj_para.numVisible = 0;

    for(int i=0; i<*(ext->obj_data.vertex_num); i++)
    {
        if(ext->obj_para.bfCull)
        {
            x1 = v[3*ext->obj_data.surface_array[4*i+0]+0]-v[3*ext->obj_data.surface_array[4*i+1]+0];
            y1 = v[3*ext->obj_data.surface_array[4*i+0]+1]-v[3*ext->obj_data.surface_array[4*i+1]+1];
            x2 = v[3*ext->obj_data.surface_array[4*i+2]+0]-v[3*ext->obj_data.surface_array[4*i+1]+0];
            y2 = v[3*ext->obj_data.surface_array[4*i+2]+1]-v[3*ext->obj_data.surface_array[4*i+1]+1];
            z = x1*y2-y1*x2;
            normZ[i] = z<0? -z : z;
            if((!ext->obj_para.orient && z<0) || (ext->obj_para.orient && z>0))
            {
                sortedPolys[ext->obj_para.numVisible++] = i;
            }
        }
        else
        {
            sortedPolys[ext->obj_para.numVisible++] = i;
        }
    }
    int i,j,zPoly[ext->obj_para.numVisible];
    for(i=0; i<ext->obj_para.numVisible; ++i)
    {
        zPoly[i] = 0.0f;
        for(j=0; j<4; ++j)
        {
            zPoly[i] += v[3*ext->obj_data.surface_array[4*sortedPolys[i]+j]+2];
        }
    }
    for(i=0; i<ext->obj_para.numVisible-1; ++i)
    {
        for(j=i; j<ext->obj_para.numVisible; ++j)
        {
            if(zPoly[i]<zPoly[j])
            {
                swap(zPoly[j],zPoly[i]);
                swap(sortedPolys[j],sortedPolys[i]);
            }
        }
    }
}

static void cullTris(lv_obj_t * obj,int *v)
{
    int x1,y1,x2,y2,z;

    lv_3d_ext_t * ext = lv_obj_get_ext_attr(obj);
    ext->obj_para.numVisible = 0;

    for(int i=0; i<*(ext->obj_data.vertex_num); i++)
    {
        if(ext->obj_para.bfCull)
        {
            x1 = v[3*ext->obj_data.surface_array[3*i+0]+0]-v[3*ext->obj_data.surface_array[3*i+1]+0];
            y1 = v[3*ext->obj_data.surface_array[3*i+0]+1]-v[3*ext->obj_data.surface_array[3*i+1]+1];
            x2 = v[3*ext->obj_data.surface_array[3*i+2]+0]-v[3*ext->obj_data.surface_array[3*i+1]+0];
            y2 = v[3*ext->obj_data.surface_array[3*i+2]+1]-v[3*ext->obj_data.surface_array[3*i+1]+1];
            z = x1*y2-y1*x2;
            normZ[i] = z<0? -z : z;
            if((!ext->obj_para.orient && z<0) || (ext->obj_para.orient && z>0))
            {
                sortedPolys[ext->obj_para.numVisible++] = i;
            }
        }
        else
        {
            sortedPolys[ext->obj_para.numVisible++] = i;
        }
    }
    int i,j,zPoly[ext->obj_para.numVisible];// average Z of the polygon
    for(i=0; i<ext->obj_para.numVisible; ++i)
    {
        zPoly[i] = 0.0;
        for(j=0; j<3; ++j)
        {
            zPoly[i] += v[3*ext->obj_data.surface_array[3*sortedPolys[i]+j]+2];
        }
    }
    // sort by Z
    for(i=0; i<ext->obj_para.numVisible-1; ++i)
    {
        for(j=i; j<ext->obj_para.numVisible; ++j)
        {
            if(zPoly[i]<zPoly[j])
            {
                swap(zPoly[j],zPoly[i]);
                swap(sortedPolys[j],sortedPolys[i]);
            }
        }
    }
}

static void drawQuads(lv_obj_t * obj,lv_area_t * clip_area,int *v2d)
{
    int q,v0,v1,v2,v3,c,i;

    lv_point_t point[4];
    lv_coord_t x = lv_obj_get_x(obj);
    lv_coord_t y = lv_obj_get_y(obj);

    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.border_width = 0;
    rect_dsc.outline_width = 0;
    rect_dsc.shadow_width = 0;
    rect_dsc.radius = 0;
    rect_dsc.bg_blend_mode = LV_BLEND_MODE_NORMAL;
    rect_dsc.pattern_blend_mode = LV_BLEND_MODE_SUBTRACTIVE;

    //lv_obj_init_draw_rect_dsc(obj, LV_3D_PART_BG, &rect_dsc);

    lv_3d_ext_t * ext = lv_obj_get_ext_attr(obj);

    for(i=0; i<ext->obj_para.numVisible; i++)
    {
        q = sortedPolys[i];
        v0 = ext->obj_data.surface_array[4*q+0];
        v1 = ext->obj_data.surface_array[4*q+1];
        v2 = ext->obj_data.surface_array[4*q+2];
        v3 = ext->obj_data.surface_array[4*q+3];

        point[0].x = v2d[2*v0+0] + x;point[0].y = v2d[2*v0+1] + y;
        point[1].x = v2d[2*v1+0] + x;point[1].y = v2d[2*v1+1] + y;
        point[2].x = v2d[2*v2+0] + x;point[2].y = v2d[2*v2+1] + y;
        point[3].x = v2d[2*v3+0] + x;point[3].y = v2d[2*v3+1] + y;
        //printf("x1:%d,%d\r\nx2:%d,%d\r\nx3:%d,%d\r\nx4:%d,%d\r\n",point[0].x,point[0].y,point[1].x,point[1].y,point[2].x,point[2].y,point[3].x,point[3].y);
        if(ext->obj_para.lightShade>0)
        {
            c = normZ[q]*255/ext->obj_para.lightShade;
            if(c>255) c=255;
            rect_dsc.bg_color.full = RGBto565(c,c/3,c);
        }
        else
        {
            rect_dsc.bg_color = ext->obj_data.color_array[q];
        }
        lv_draw_polygon(point,4,clip_area,&rect_dsc);
    }
}

static void drawTris(lv_obj_t * obj,lv_area_t * clip_area,int *v2d)
{
    int q,v0,v1,v2,v3,c,i;

    lv_point_t point[3];
    lv_coord_t x = lv_obj_get_x(obj);
    lv_coord_t y = lv_obj_get_y(obj);

    lv_draw_rect_dsc_t rect_dsc;
    lv_draw_rect_dsc_init(&rect_dsc);
    rect_dsc.border_width = 0;
    rect_dsc.outline_width = 0;
    rect_dsc.shadow_width = 0;
    rect_dsc.radius = 0;
    lv_obj_init_draw_rect_dsc(obj, LV_3D_PART_BG, &rect_dsc);

    lv_3d_ext_t * ext = lv_obj_get_ext_attr(obj);

    for(i=0; i<ext->obj_para.numVisible; i++)
    {
        q = sortedPolys[i];
        v0 = ext->obj_data.surface_array[3*q+0];
        v1 = ext->obj_data.surface_array[3*q+1];
        v2 = ext->obj_data.surface_array[3*q+2];

        point[0].x = v2d[2*v0+0] + x;point[0].y = v2d[2*v0+1] + y;
        point[1].x = v2d[2*v1+0] + x;point[1].y = v2d[2*v1+1] + y;
        point[2].x = v2d[2*v2+0] + x;point[2].y = v2d[2*v2+1] + y;

        if(ext->obj_para.lightShade>0)
        {
            c = normZ[q]*255/ext->obj_para.lightShade;
            if(c>255) c=255;
            rect_dsc.bg_color.full = RGBto565(c,c,c/2);
        }
        else
        {
            rect_dsc.bg_color = ext->obj_data.color_array[q];
        }
        lv_draw_polygon(point,3,clip_area,&rect_dsc);
    }
}

static void render3D(lv_obj_t * obj,lv_area_t * clip_area,int mod)
{
    int cos0,sin0,cos1,sin1;
    int i,x0,y0,z0,fac;
    int camZ = 300;
    int scaleFactor;
    int near = 300;

    lv_3d_ext_t * ext = lv_obj_get_ext_attr(obj);
    scaleFactor = ext->obj_para.widget / 2;

#if 0
    int anglex,angley,anglez;
    int xx,xy,xz,yx,yy,yz,zx,zy,zz;
    for(i=0; i<*(ext->obj_data.vertex_num); i++)
    {
        x0 = ext->obj_data.vertex_array[3*i+0];
        y0 = ext->obj_data.vertex_array[3*i+1];
        z0 = ext->obj_data.vertex_array[3*i+2];

        zx = x0 * cos(anglez) - y0 * sin(anglez);
        zy = y0 * cos(anglez) + x0 * sin(anglez);
        zz = z0;

        xx = x0;
        xy = y0 * cos(anglex) - z0 * sin(anglex);
        xz = z0 * cos(anglex) + y0 * sin(anglex);

        yx = x0 * cos(angley) - z0 * sin(angley);
        yy = y0;
        yz = z0 * cos(angley) + x0 * sin(angley);

        x0 += (xx + yx + zx);
        y0 += (xy + yy + zy);
        z0 += (xz + yz + zz);

        //rx = p * x / (z + p);ry = p * y / (z + p);
        projVerts[2*i+0] = (100*ext->obj_para.widget/2 + fac*transVerts[3*i+0] + 100/2)/100;
        projVerts[2*i+1] = (100*ext->obj_para.widget/2 + fac*transVerts[3*i+1] + 100/2)/100;
    }
#else
    cos0 = fastCos(ext->obj_para.Str_rotx);
    sin0 = fastSin(ext->obj_para.Str_rotx);
    cos1 = fastCos(ext->obj_para.Str_roty);
    sin1 = fastSin(ext->obj_para.Str_roty);
    for(i=0; i<*(ext->obj_data.vertex_num); i++)
    {
        x0 = ext->obj_data.vertex_array[3*i+0];
        y0 = ext->obj_data.vertex_array[3*i+1];
        z0 = ext->obj_data.vertex_array[3*i+2];
        transVerts[3*i+0] = (cos0*x0 + sin0*z0)/MAXSIN;
        transVerts[3*i+1] = (cos1*y0 + (cos0*sin1*z0-sin0*sin1*x0)/MAXSIN)/MAXSIN;
        transVerts[3*i+2] = camZ + ((cos0*cos1*z0-sin0*cos1*x0)/MAXSIN - sin1*y0)/MAXSIN;

        fac = scaleFactor * near / (transVerts[3*i+2]+near+ext->obj_para.Str_dis_to_obj);

        projVerts[2*i+0] = (100*ext->obj_para.widget/2 + fac*transVerts[3*i+0] + 100/2)/100;
        projVerts[2*i+1] = (100*ext->obj_para.widget/2 + fac*transVerts[3*i+1] + 100/2)/100;
    }
#endif
    if(mod == 0)
    {
        cullQuads(obj,transVerts);
        drawQuads(obj,clip_area,projVerts);
    }
    else if(mod == 1)
    {
        cullTris(obj,transVerts);
        drawTris(obj,clip_area,projVerts);
    }
}

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a 3d objects
 */
lv_obj_t * lv_3d_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("3d obj create started");

    /*Create the ancestor basic object*/
    lv_obj_t * obj = lv_obj_create(par, copy);
    LV_ASSERT_MEM(obj);
    if(obj == NULL) return NULL;

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(obj);

    lv_3d_ext_t * ext = lv_obj_allocate_ext_attr(obj, sizeof(lv_3d_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(obj);
        return NULL;
    }

    ext->label  = NULL;
    ext->height = LV_DPI;
    ext->widget = LV_DPI;
    ext->obj_data = demo_3d_obj;

    lv_obj_set_signal_cb(obj, lv_3d_signal);
    lv_obj_set_design_cb(obj, lv_3d_design);

    ext->obj_para = obj_para;
    ext->obj_para.bfCull = 1;
    ext->obj_para.orient = 0;
    ext->obj_para.Str_rotx = 30;
    ext->obj_para.Str_roty = 30;
    ext->obj_para.lightShade = 67687;

    /*Init the new checkbox object*/
    if(copy == NULL) {
        lv_obj_set_size(obj,LV_DPI,LV_DPI);
    }
    else {
        lv_3d_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
        ext->obj_data = copy_ext->obj_data;
        ext->obj_para = copy_ext->obj_para;
    }

    LV_LOG_INFO("3d obj created");

    return obj;
}

void lv_3d_set_angle(lv_obj_t * obj, uint16_t x,uint16_t y,uint16_t z)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);

    lv_3d_ext_t * ext = lv_obj_get_ext_attr(obj);

    if((x < 360)&&(y < 360))
    {
        ext->obj_para.Str_rotx = x;
        ext->obj_para.Str_roty = y;
        lv_obj_invalidate(obj);
    }
}

void lv_3d_set_distance(lv_obj_t * obj, int dis)
{
    LV_ASSERT_OBJ(obj, LV_OBJX_NAME);
    lv_3d_ext_t * ext = lv_obj_get_ext_attr(obj);
    ext->obj_para.Str_dis_to_obj = dis;
    lv_obj_invalidate(obj);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_design_res_t lv_3d_design(lv_obj_t * obj, const lv_area_t * clip_area, lv_design_mode_t mode)
{
    /*A obj never covers an area*/
    if(mode == LV_DESIGN_COVER_CHK)
        return LV_DESIGN_RES_NOT_COVER;
    else if(mode == LV_DESIGN_DRAW_MAIN) {
        lv_3d_ext_t * ext = lv_obj_get_ext_attr(obj);
        if((ext->height > 0)&&(ext->widget > 0))
        {
            if((ext->obj_data.surface_num >= 1)&&(ext->obj_data.vertex_num >= 3)&&\
               (ext->obj_data.surface_array != NULL)&&(ext->obj_data.vertex_array != NULL)&&\
               (ext->obj_data.surface_array != NULL))
            {
                render3D(obj,clip_area,0);
            }
        }
    }
    return LV_DESIGN_RES_OK;
}

/**
 * Signal function of the check box
 * @param cb pointer to a check box object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_3d_signal(lv_obj_t * cb, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(cb, sign, param);
    if(res != LV_RES_OK) return res;
    return res;
}

#endif
