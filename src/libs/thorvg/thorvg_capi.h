/*!
* \file thorvg_capi.h
*
* \brief The module provides C bindings for the ThorVG library.
* Please refer to src/examples/Capi.cpp to find the thorvg_capi usage examples.
*
* The thorvg_capi module allows to implement the ThorVG client and provides
* the following functionalities:
* - drawing shapes: line, curve, polygon, circle, user-defined, ...
* - filling: solid, linear and radial gradient
* - scene graph & affine transformation (translation, rotation, scale, ...)
* - stroking: width, join, cap, dash
* - composition: blending, masking, path clipping
* - pictures: SVG, PNG, JPG, bitmap
*
*/

#include "../../lv_conf_internal.h"
#if LV_USE_THORVG_INTERNAL
#define TVG_BUILD 1

#ifndef __THORVG_CAPI_H__
#define __THORVG_CAPI_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef TVG_API
    #undef TVG_API
#endif

#ifndef TVG_STATIC
    #ifdef _WIN32
        #if TVG_BUILD
            #define TVG_API __declspec(dllexport)
        #else
            #define TVG_API __declspec(dllimport)
        #endif
    #elif (defined(__SUNPRO_C)  || defined(__SUNPRO_CC))
        #define TVG_API __global
    #else
        #if (defined(__GNUC__) && __GNUC__ >= 4) || defined(__INTEL_COMPILER)
            #define TVG_API __attribute__ ((visibility("default")))
        #else
            #define TVG_API
        #endif
    #endif
#else
    #define TVG_API
#endif

#ifdef TVG_DEPRECATED
    #undef TVG_DEPRECATED
#endif

#ifdef _WIN32
    #define TVG_DEPRECATED __declspec(deprecated)
#elif __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1)
    #define TVG_DEPRECATED __attribute__ ((__deprecated__))
#else
    #define TVG_DEPRECATED
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
* \defgroup ThorVG_CAPI ThorVG_CAPI
* \brief ThorVG C language binding APIs.
*
* \{
*/


/**
* \brief A structure responsible for managing and drawing graphical elements.
*
* It sets up the target buffer, which can be drawn on the screen. It stores the Tvg_Paint objects (Shape, Scene, Picture).
*/
typedef struct _Tvg_Canvas Tvg_Canvas;


/**
* \brief A structure representing a graphical element.
*
* \warning The TvgPaint objects cannot be shared between Canvases.
*/
typedef struct _Tvg_Paint Tvg_Paint;


/**
* \brief A structure representing a gradient fill of a Tvg_Paint object.
*/
typedef struct _Tvg_Gradient Tvg_Gradient;


/**
* \brief A structure representing an object that enables to save a Tvg_Paint object into a file.
*/
typedef struct _Tvg_Saver Tvg_Saver;

/**
* \brief A structure representing an animation controller object.
*/
typedef struct _Tvg_Animation Tvg_Animation;


/**
* \brief Enumeration specifying the engine type used for the graphics backend. For multiple backends bitwise operation is allowed.
*
* \ingroup ThorVGCapi_Initializer
*/
typedef enum {
    TVG_ENGINE_SW = (1 << 1),   ///< CPU rasterizer.
    TVG_ENGINE_GL = (1 << 2)    ///< OpenGL rasterizer.
} Tvg_Engine;


/**
 * \brief Enumeration specifying the result from the APIs.
 *
 * All ThorVG APIs could potentially return one of the values in the list.
 * Please note that some APIs may additionally specify the reasons that trigger their return values.
 *
 */
typedef enum {
    TVG_RESULT_SUCCESS = 0,            ///< The value returned in case of a correct request execution.
    TVG_RESULT_INVALID_ARGUMENT,       ///< The value returned in the event of a problem with the arguments given to the API - e.g. empty paths or null pointers.
    TVG_RESULT_INSUFFICIENT_CONDITION, ///< The value returned in case the request cannot be processed - e.g. asking for properties of an object, which does not exist.
    TVG_RESULT_FAILED_ALLOCATION,      ///< The value returned in case of unsuccessful memory allocation.
    TVG_RESULT_MEMORY_CORRUPTION,      ///< The value returned in the event of bad memory handling - e.g. failing in pointer releasing or casting
    TVG_RESULT_NOT_SUPPORTED,          ///< The value returned in case of choosing unsupported engine features(options).
    TVG_RESULT_UNKNOWN                 ///< The value returned in all other cases.
} Tvg_Result;


/**
 * \brief Enumeration indicating the method used in the composition of two objects - the target and the source.
 *
 * \ingroup ThorVGCapi_Paint
 */
typedef enum {
    TVG_COMPOSITE_METHOD_NONE = 0,           ///< No composition is applied.
    TVG_COMPOSITE_METHOD_CLIP_PATH,          ///< The intersection of the source and the target is determined and only the resulting pixels from the source are rendered. Note that ClipPath only supports the Shape type. @deprecated Use Paint::clip() instead.
    TVG_COMPOSITE_METHOD_ALPHA_MASK,         ///< The pixels of the source and the target are alpha blended. As a result, only the part of the source, which intersects with the target is visible.
    TVG_COMPOSITE_METHOD_INVERSE_ALPHA_MASK, ///< The pixels of the source and the complement to the target's pixels are alpha blended. As a result, only the part of the source which is not covered by the target is visible.
    TVG_COMPOSITE_METHOD_LUMA_MASK,          ///< The source pixels are converted to grayscale (luma value) and alpha blended with the target. As a result, only the part of the source which intersects with the target is visible. \since 0.9
    TVG_COMPOSITE_METHOD_INVERSE_LUMA_MASK   ///< The source pixels are converted to grayscale (luma value) and complement to the target's pixels are alpha blended. As a result, only the part of the source which is not covered by the target is visible. \since 0.14
} Tvg_Composite_Method;

/**
 * @brief Enumeration indicates the method used for blending paint. Please refer to the respective formulas for each method.
 *
 * \ingroup ThorVGCapi_Paint
 *
 * \since 0.15
 */
typedef enum {
    TVG_BLEND_METHOD_NORMAL = 0,        ///< Perform the alpha blending(default). S if (Sa == 255), otherwise (Sa * S) + (255 - Sa) * D
    TVG_BLEND_METHOD_MULTIPLY,          ///< Takes the RGB channel values from 0 to 255 of each pixel in the top layer and multiples them with the values for the corresponding pixel from the bottom layer. (S * D)
    TVG_BLEND_METHOD_SCREEN,            ///< The values of the pixels in the two layers are inverted, multiplied, and then inverted again. (S + D) - (S * D)
    TVG_BLEND_METHOD_OVERLAY,           ///< Combines Multiply and Screen blend modes. (2 * S * D) if (2 * D < Da), otherwise (Sa * Da) - 2 * (Da - S) * (Sa - D)
    TVG_BLEND_METHOD_SRCOVER,           ///< Replace the bottom layer with the top layer.
    TVG_BLEND_METHOD_DARKEN,            ///< Creates a pixel that retains the smallest components of the top and bottom layer pixels. min(S, D)
    TVG_BLEND_METHOD_LIGHTEN,           ///< Only has the opposite action of Darken Only. max(S, D)
    TVG_BLEND_METHOD_COLORDODGE,        ///< Divides the bottom layer by the inverted top layer. D / (255 - S)
    TVG_BLEND_METHOD_COLORBURN,         ///< Divides the inverted bottom layer by the top layer, and then inverts the result. 255 - (255 - D) / S
    TVG_BLEND_METHOD_HARDLIGHT,         ///< The same as Overlay but with the color roles reversed. (2 * S * D) if (S < Sa), otherwise (Sa * Da) - 2 * (Da - S) * (Sa - D)
    TVG_BLEND_METHOD_SOFTLIGHT,         ///< The same as Overlay but with applying pure black or white does not result in pure black or white. (1 - 2 * S) * (D ^ 2) + (2 * S * D)
    TVG_BLEND_METHOD_DIFFERENCE,        ///< Subtracts the bottom layer from the top layer or the other way around, to always get a non-negative value. (S - D) if (S > D), otherwise (D - S)
    TVG_BLEND_METHOD_EXCLUSION,         ///< The result is twice the product of the top and bottom layers, subtracted from their sum. s + d - (2 * s * d)
    TVG_BLEND_METHOD_HUE,               ///< Reserved. Not supported.
    TVG_BLEND_METHOD_SATURATION,        ///< Reserved. Not supported.
    TVG_BLEND_METHOD_COLOR,             ///< Reserved. Not supported.
    TVG_BLEND_METHOD_LUMINOSITY,        ///< Reserved. Not supported.
    TVG_BLEND_METHOD_ADD,               ///< Simply adds pixel values of one layer with the other. (S + D)
    TVG_BLEND_METHOD_HARDMIX            ///< Reserved. Not supported.
} Tvg_Blend_Method;


/**
 * \see Tvg_Type
 * \deprecated
 */
typedef enum {
    TVG_IDENTIFIER_UNDEF = 0,   ///< Undefined type.
    TVG_IDENTIFIER_SHAPE,       ///< A shape type paint.
    TVG_IDENTIFIER_SCENE,       ///< A scene type paint.
    TVG_IDENTIFIER_PICTURE,     ///< A picture type paint.
    TVG_IDENTIFIER_LINEAR_GRAD, ///< A linear gradient type.
    TVG_IDENTIFIER_RADIAL_GRAD, ///< A radial gradient type.
    TVG_IDENTIFIER_TEXT         ///< A text type paint.
} Tvg_Identifier;


/**
 * \brief Enumeration indicating the ThorVG object type value.
 *
 * ThorVG's drawing objects can return object type values, allowing you to identify the specific type of each object.
 *
 * \ingroup ThorVGCapi_Paint
 *
 * \see tvg_paint_get_type()
 * \see tvg_gradient_get_type()
 *
 * \note Experimental API
 */
typedef enum {
    TVG_TYPE_UNDEF = 0,        ///< Undefined type.
    TVG_TYPE_SHAPE,            ///< A shape type paint.
    TVG_TYPE_SCENE,            ///< A scene type paint.
    TVG_TYPE_PICTURE,          ///< A picture type paint.
    TVG_TYPE_TEXT,             ///< A text type paint.
    TVG_TYPE_LINEAR_GRAD = 10, ///< A linear gradient type.
    TVG_TYPE_RADIAL_GRAD       ///< A radial gradient type.
} Tvg_Type;


/**
 * \addtogroup ThorVGCapi_Shape
 * \{
 */

/**
 * \brief Enumeration specifying the values of the path commands accepted by TVG.
 *
 * Not to be confused with the path commands from the svg path element (like M, L, Q, H and many others).
 * TVG interprets all of them and translates to the ones from the PathCommand values.
 */
typedef enum {
    TVG_PATH_COMMAND_CLOSE = 0, ///< Ends the current sub-path and connects it with its initial point - corresponds to Z command in the svg path commands.
    TVG_PATH_COMMAND_MOVE_TO,   ///< Sets a new initial point of the sub-path and a new current point - corresponds to M command in the svg path commands.
    TVG_PATH_COMMAND_LINE_TO,   ///< Draws a line from the current point to the given point and sets a new value of the current point - corresponds to L command in the svg path commands.
    TVG_PATH_COMMAND_CUBIC_TO   ///< Draws a cubic Bezier curve from the current point to the given point using two given control points and sets a new value of the current point - corresponds to C command in the svg path commands.
} Tvg_Path_Command;


/**
 * \brief Enumeration determining the ending type of a stroke in the open sub-paths.
 */
typedef enum {
    TVG_STROKE_CAP_SQUARE = 0, ///< The stroke is extended in both endpoints of a sub-path by a rectangle, with the width equal to the stroke width and the length equal to the half of the stroke width. For zero length sub-paths the square is rendered with the size of the stroke width.
    TVG_STROKE_CAP_ROUND,      ///< The stroke is extended in both endpoints of a sub-path by a half circle, with a radius equal to the half of a stroke width. For zero length sub-paths a full circle is rendered.
    TVG_STROKE_CAP_BUTT        ///< The stroke ends exactly at each of the two endpoints of a sub-path. For zero length sub-paths no stroke is rendered.
} Tvg_Stroke_Cap;


/**
 * \brief Enumeration specifying how to fill the area outside the gradient bounds.
 */
typedef enum {
    TVG_STROKE_JOIN_BEVEL = 0, ///< The outer corner of the joined path segments is bevelled at the join point. The triangular region of the corner is enclosed by a straight line between the outer corners of each stroke.
    TVG_STROKE_JOIN_ROUND,     ///< The outer corner of the joined path segments is rounded. The circular region is centered at the join point.
    TVG_STROKE_JOIN_MITER      ///< The outer corner of the joined path segments is spiked. The spike is created by extension beyond the join point of the outer edges of the stroke until they intersect. In case the extension goes beyond the limit, the join style is converted to the Bevel style.
} Tvg_Stroke_Join;


/**
 * \brief Enumeration specifying how to fill the area outside the gradient bounds.
 */
typedef enum {
    TVG_STROKE_FILL_PAD = 0, ///< The remaining area is filled with the closest stop color.
    TVG_STROKE_FILL_REFLECT, ///< The gradient pattern is reflected outside the gradient area until the expected region is filled.
    TVG_STROKE_FILL_REPEAT   ///< The gradient pattern is repeated continuously beyond the gradient area until the expected region is filled.
} Tvg_Stroke_Fill;


/**
 * \brief Enumeration specifying the algorithm used to establish which parts of the shape are treated as the inside of the shape.
 */
typedef enum {
    TVG_FILL_RULE_WINDING = 0, ///< A line from the point to a location outside the shape is drawn. The intersections of the line with the path segment of the shape are counted. Starting from zero, if the path segment of the shape crosses the line clockwise, one is added, otherwise one is subtracted. If the resulting sum is non zero, the point is inside the shape.
    TVG_FILL_RULE_EVEN_ODD     ///< A line from the point to a location outside the shape is drawn and its intersections with the path segments of the shape are counted. If the number of intersections is an odd number, the point is inside the shape.
} Tvg_Fill_Rule;

/** \} */   // end addtogroup ThorVGCapi_Shape


/*!
* \addtogroup ThorVGCapi_Gradient
* \{
*/

/*!
* \brief A data structure storing the information about the color and its relative position inside the gradient bounds.
*/
typedef struct
{
    float offset; /**< The relative position of the color. */
    uint8_t r;    /**< The red color channel value in the range [0 ~ 255]. */
    uint8_t g;    /**< The green color channel value in the range [0 ~ 255]. */
    uint8_t b;    /**< The blue color channel value in the range [0 ~ 255]. */
    uint8_t a;    /**< The alpha channel value in the range [0 ~ 255], where 0 is completely transparent and 255 is opaque. */
} Tvg_Color_Stop;

/** \} */   // end addtogroup ThorVGCapi_Gradient


/**
 * \brief A data structure representing a point in two-dimensional space.
 */
typedef struct
{
    float x, y;
} Tvg_Point;


/**
 * \brief A data structure representing a three-dimensional matrix.
 *
 * The elements e11, e12, e21 and e22 represent the rotation matrix, including the scaling factor.
 * The elements e13 and e23 determine the translation of the object along the x and y-axis, respectively.
 * The elements e31 and e32 are set to 0, e33 is set to 1.
 */
typedef struct
{
    float e11, e12, e13;
    float e21, e22, e23;
    float e31, e32, e33;
} Tvg_Matrix;


/**
* \defgroup ThorVGCapi_Initializer Initializer
* \brief A module enabling initialization and termination of the TVG engines.
*
* \{
*/

/************************************************************************/
/* Engine API                                                           */
/************************************************************************/
/*!
* \brief Initializes TVG engines.
*
* TVG requires the running-engine environment.
* TVG runs its own task-scheduler for parallelizing rendering tasks efficiently.
* You can indicate the number of threads, the count of which is designated @p threads.
* In the initialization step, TVG will generate/spawn the threads as set by @p threads count.
*
* \code
* tvg_engine_init(TVG_ENGINE_SW, 0);  //Initialize software renderer and use the main thread only
* \endcode
*
* \param[in] engine_method The engine types to initialize. This is relative to the Canvas types, in which it will be used. For multiple backends bitwise operation is allowed.
*   - TVG_ENGINE_SW: CPU rasterizer
*   - TVG_ENGINE_GL: OpenGL rasterizer (not supported yet)
* \param[in] threads The number of additional threads used to perform rendering. Zero indicates only the main thread is to be used.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT Unknown engine type.
* \retval TVG_RESULT_NOT_SUPPORTED Unsupported engine type.
*
* \note The Initializer keeps track of the number of times it was called. Threads count is fixed at the first init() call.
* \see tvg_engine_term()
* \see Tvg_Engine
*/
TVG_API Tvg_Result tvg_engine_init(Tvg_Engine engine_method, unsigned threads);


/*!
* \brief Terminates TVG engines.
*
* It should be called in case of termination of the TVG client with the same engine types as were passed when tvg_engine_init() was called.
*
* \code
* tvg_engine_init(TVG_ENGINE_SW, 0);
* //define canvas and shapes, update shapes, general rendering calls
* tvg_engine_term(TVG_ENGINE_SW);
* \endcode
*
* \param engine_method The engine types to terminate. This is relative to the Canvas types, in which it will be used. For multiple backends bitwise operation is allowed
*   - TVG_ENGINE_SW: CPU rasterizer
*   - TVG_ENGINE_GL: OpenGL rasterizer (not supported yet)
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION Nothing to be terminated.
* \retval TVG_RESULT_INVALID_ARGUMENT Unknown engine type.
* \retval TVG_RESULT_NOT_SUPPORTED Unsupported engine type.
*
* \see tvg_engine_init()
* \see Tvg_Engine
*/
TVG_API Tvg_Result tvg_engine_term(Tvg_Engine engine_method);


/**
* \brief Retrieves the version of the TVG engine.
*
* \param[out] major A major version number.
* \param[out] minor A minor version number.
* \param[out] micro A micro version number.
* \param[out] version The version of the engine in the format major.minor.micro, or a @p nullptr in case of an internal error.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_SUCCESS.
*
* \since 0.15
*/
TVG_API Tvg_Result tvg_engine_version(uint32_t* major, uint32_t* minor, uint32_t* micro, const char** version);

/** \} */   // end defgroup ThorVGCapi_Initializer


/**
* \defgroup ThorVGCapi_Canvas Canvas
* \brief A module for managing and drawing graphical elements.
*
* A canvas is an entity responsible for drawing the target. It sets up the drawing engine and the buffer, which can be drawn on the screen. It also manages given Paint objects.
*
* \note A Canvas behavior depends on the raster engine though the final content of the buffer is expected to be identical.
* \warning The Paint objects belonging to one Canvas can't be shared among multiple Canvases.
\{
*/


/**
* \defgroup ThorVGCapi_SwCanvas SwCanvas
* \ingroup ThorVGCapi_Canvas
*
* \brief A module for rendering the graphical elements using the software engine.
*
* \{
*/

/************************************************************************/
/* SwCanvas API                                                         */
/************************************************************************/

/**
 * \brief Enumeration specifying the methods of Memory Pool behavior policy.
 */
typedef enum {
    TVG_MEMPOOL_POLICY_DEFAULT = 0, ///< Default behavior that ThorVG is designed to.
    TVG_MEMPOOL_POLICY_SHAREABLE,   ///< Memory Pool is shared among canvases.
    TVG_MEMPOOL_POLICY_INDIVIDUAL   ///< Allocate designated memory pool that is used only by the current canvas instance.
} Tvg_Mempool_Policy;


/**
 * \brief Enumeration specifying the methods of combining the 8-bit color channels into 32-bit color.
 */
typedef enum {
    TVG_COLORSPACE_ABGR8888 = 0, ///< The channels are joined in the order: alpha, blue, green, red. Colors are alpha-premultiplied. (a << 24 | b << 16 | g << 8 | r)
    TVG_COLORSPACE_ARGB8888,     ///< The channels are joined in the order: alpha, red, green, blue. Colors are alpha-premultiplied. (a << 24 | r << 16 | g << 8 | b)
    TVG_COLORSPACE_ABGR8888S,    ///< The channels are joined in the order: alpha, blue, green, red. Colors are un-alpha-premultiplied. @since 0.13
    TVG_COLORSPACE_ARGB8888S     ///< The channels are joined in the order: alpha, red, green, blue. Colors are un-alpha-premultiplied. @since 0.13
} Tvg_Colorspace;


/*!
* \brief Creates a Canvas object.
*
* \code
* Tvg_Canvas *canvas = NULL;
*
* tvg_engine_init(TVG_ENGINE_SW, 4);
* canvas = tvg_swcanvas_create();
*
* //set up the canvas buffer
* uint32_t *buffer = NULL;
* buffer = (uint32_t*) malloc(sizeof(uint32_t) * 100 * 100);
* if (!buffer) return;
*
* tvg_swcanvas_set_target(canvas, buffer, 100, 100, 100, TVG_COLORSPACE_ARGB8888);
*
* //set up paints and add them into the canvas before drawing it
*
* tvg_canvas_destroy(canvas);
* tvg_engine_term(TVG_ENGINE_SW);
* \endcode
*
* \return A new Tvg_Canvas object.
*/
TVG_API Tvg_Canvas* tvg_swcanvas_create(void);


/*!
* \brief Sets the buffer used in the rasterization process and defines the used colorspace.
*
* For optimisation reasons TVG does not allocate memory for the output buffer on its own.
* The buffer of a desirable size should be allocated and owned by the caller.
*
* \param[in] canvas The Tvg_Canvas object managing the @p buffer.
* \param[in] buffer A pointer to the allocated memory block of the size @p stride x @p h.
* \param[in] stride The stride of the raster image - in most cases same value as @p w.
* \param[in] w The width of the raster image.
* \param[in] h The height of the raster image.
* \param[in] cs The colorspace value defining the way the 32-bits colors should be read/written.
* - TVG_COLORSPACE_ABGR8888
* - TVG_COLORSPACE_ARGB8888
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENTS An invalid canvas or buffer pointer passed or one of the @p stride, @p w or @p h being zero.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION if the canvas is performing rendering. Please ensure the canvas is synced.
* \retval TVG_RESULT_NOT_SUPPORTED The software engine is not supported.
*
* \warning Do not access @p buffer during tvg_canvas_draw() - tvg_canvas_sync(). It should not be accessed while the engine is writing on it.
*
* \see Tvg_Colorspace
*/
TVG_API Tvg_Result tvg_swcanvas_set_target(Tvg_Canvas* canvas, uint32_t* buffer, uint32_t stride, uint32_t w, uint32_t h, Tvg_Colorspace cs);


/*!
* \brief Sets the software engine memory pool behavior policy.
*
* ThorVG draws a lot of shapes, it allocates/deallocates a few chunk of memory
* while processing rendering. It internally uses one shared memory pool
* which can be reused among the canvases in order to avoid memory overhead.
*
* Thus ThorVG suggests using a memory pool policy to satisfy user demands,
* if it needs to guarantee the thread-safety of the internal data access.
*
* \param[in] canvas The Tvg_Canvas object of which the Memory Pool behavior is to be specified.
* \param[in] policy The method specifying the Memory Pool behavior. The default value is @c TVG_MEMPOOL_POLICY_DEFAULT.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENTS An invalid canvas pointer passed.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION The canvas contains some paints already.
* \retval TVG_RESULT_NOT_SUPPORTED The software engine is not supported.
*
* \note When @c policy is set as @c TVG_MEMPOOL_POLICY_INDIVIDUAL, the current instance of canvas uses its own individual
*       memory data, which is not shared with others. This is necessary when the canvas is accessed on a worker-thread.
*
* \warning It's not allowed after pushing any paints.
*/
TVG_API Tvg_Result tvg_swcanvas_set_mempool(Tvg_Canvas* canvas, Tvg_Mempool_Policy policy);

/** \} */   // end defgroup ThorVGCapi_SwCanvas


/************************************************************************/
/* Common Canvas API                                                    */
/************************************************************************/
/*!
* \brief Clears the canvas internal data, releases all paints stored by the canvas and destroys the canvas object itself.
*
* \code
* static Tvg_Canvas *canvas = NULL;
* static uint32_t *buffer = NULL;
*
* static void _init() {
*   canvas = tvg_swcanvas_create();
*   buffer = (uint32_t*) malloc(sizeof(uint32_t) * 100 * 100);
*   tvg_swcanvas_set_target(canvas, buffer, 100, 100, 100, TVG_COLORSPACE_ARGB8888);
* }
*
* //a task called from main function in a loop
* static void _job(const int cmd) {
*   //define a valid rectangle shape
*   switch (cmd) {
*     case CMD_EXIT: return 0;
*     case CMD_ADD_RECT:
*       tvg_canvas_push(canvas, rect);
*       break;
*     case CMD_DEL_RECT:
*       tvg_paint_del(rect);
*       //now to safely delete Tvg_Canvas, tvg_canvas_clear() API have to be used
*       break;
*     default:
*       break;
*   }
* }
*
* int main(int argc, char **argv) {
*   int cmd = 0;
*   int stop = 1;
*
*   tvg_engine_init(TVG_ENGINE_SW, 4);
*
*   while (stop) {
*      //wait for a command e.g. from a console
*      stop = _job(cmd);
*   }
*   tvg_canvas_clear(canvas, false);
*   tvg_canvas_destroy(canvas);
*   tvg_engine_term(TVG_ENGINE_SW);
*   return 0;
* }
*
* tvg_canvas_destroy(canvas);
* tvg_engine_term(TVG_ENGINE_SW)
* \endcode
*
* \param[in] canvas The Tvg_Canvas object to be destroyed.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid pointer to the Tvg_Canvas object is passed.
*
* \note If the paints from the canvas should not be released, the tvg_canvas_clear() with a @c free argument value set to @c false should be called.
* Please be aware that in such a case TVG is not responsible for the paints release anymore and it has to be done manually in order to avoid memory leaks.
*
* \see tvg_paint_del(), tvg_canvas_clear()
*/
TVG_API Tvg_Result tvg_canvas_destroy(Tvg_Canvas* canvas);


/*!
* \brief Inserts a drawing element into the canvas using a Tvg_Paint object.
*
* \param[in] canvas The Tvg_Canvas object managing the @p paint.
* \param[in] paint The Tvg_Paint object to be drawn.
*
* Only the paints pushed into the canvas will be drawing targets.
* They are retained by the canvas until you call tvg_canvas_clear().
*
* \return Tvg_Result return values:
* \retval TVG_RESULT_INVALID_ARGUMENT In case a @c nullptr is passed as the argument.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION An internal error.
*
* \note The rendering order of the paints is the same as the order as they were pushed. Consider sorting the paints before pushing them if you intend to use layering.
* \see tvg_canvas_clear()
*/
TVG_API Tvg_Result tvg_canvas_push(Tvg_Canvas* canvas, Tvg_Paint* paint);


/*!
* \brief Reserves a memory block where the objects pushed into a canvas are stored.
*
* If the number of Tvg_Paints to be stored in a canvas is known in advance, calling this function reduces the multiple
* memory allocations thus improves the performance.
*
* \code
* Tvg_Canvas *canvas = NULL;
*
* tvg_engine_init(TVG_ENGINE_SW, 4);
* canvas = tvg_swcanvas_create();
*
* uint32_t *buffer = NULL;
* buffer = (uint32_t*) malloc(sizeof(uint32_t) * 100 * 100);
* if (!buffer) return;
*
* tvg_swcanvas_set_target(canvas, buffer, 100, 100, 100, TVG_COLORSPACE_ARGB8888);
*
* tvg_canvas_destroy(canvas);
* tvg_engine_term(TVG_ENGINE_SW)
* \endcode
*
* \param[in] canvas The Tvg_Canvas object managing the reserved memory.
* \param[in] n The number of objects for which the memory is to be reserved.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Canvas pointer.
*/
TVG_DEPRECATED TVG_API Tvg_Result tvg_canvas_reserve(Tvg_Canvas* canvas, uint32_t n);


/*!
* \brief Sets the total number of the paints pushed into the canvas to be zero.
* Tvg_Paint objects stored in the canvas are released if @p free is set to @c true, otherwise the memory is not deallocated and
* all paints should be released manually in order to avoid memory leaks.
*
* \param[in] canvas The Tvg_Canvas object to be cleared.
* \param[in] free If @c true the memory occupied by paints is deallocated, otherwise it is not.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Canvas pointer.
*
* \see tvg_canvas_destroy()
*/
TVG_API Tvg_Result tvg_canvas_clear(Tvg_Canvas* canvas, bool free);


/*!
* \brief Updates all paints in a canvas.
*
* Should be called before drawing in order to prepare paints for the rendering.
*
* \code
* //A frame drawing example. Thread safety and events implementation is skipped to show only TVG code.
*
* static Tvg_Canvas *canvas = NULL;
* static Tvg_Paint *rect = NULL;
*
* int _frame_render(void) {
*   tvg_canvas_update(canvas);
*   tvg_canvas_draw(canvas);
*   tvg_canvas_sync(canvas);
* }
*
* //event handler from your code or third party library
* void _event_handler(event *event_data) {
*   if (!event_data) return NULL;
*     switch(event_data.type) {
*       case EVENT_RECT_ADD:
*         if (!rect) {
*           tvg_shape_append_rect(rect, 10, 10, 50, 50, 0, 0);
*           tvg_shape_set_stroke_width(rect, 1.0f);
*           tvg_shape_set_stroke_color(rect, 255, 0, 0, 255);
*           tvg_canvas_push(canvas, rect);
*         }
*         break;
*       case EVENT_RECT_MOVE:
*         if (rect) tvg_paint_translate(rect, 10.0, 10.0);
*           break;
*         default:
*           break;
*   }
* }
*
* int main(int argc, char **argv) {
*   //example handler from your code or third party lib
*   event_handler_add(handler, _event_handler);
*
*   //create frame rendering process which calls _frame_render() function.
*   app_loop_begin(_frame_render);
*   app_loop_finish();
*   cleanup();
* }
* \endcode
*
* \param[in] canvas The Tvg_Canvas object to be updated.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Canvas pointer.
*
* \see tvg_canvas_update_paint()
*/
TVG_API Tvg_Result tvg_canvas_update(Tvg_Canvas* canvas);


/*!
* \brief Updates the given Tvg_Paint object from the canvas before the rendering.
*
* If a client application using the TVG library does not update the entire canvas with tvg_canvas_update() in the frame
* rendering process, Tvg_Paint objects previously added to the canvas should be updated manually with this function.
*
* \param[in] canvas The Tvg_Canvas object to which the @p paint belongs.
* \param[in] paint The Tvg_Paint object to be updated.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT In case a @c nullptr is passed as the argument.
*
* \see tvg_canvas_update()
*/
TVG_API Tvg_Result tvg_canvas_update_paint(Tvg_Canvas* canvas, Tvg_Paint* paint);


/*!
* \brief Requests the canvas to draw the Tvg_Paint objects.
*
* All paints from the given canvas will be rasterized to the buffer.
*
* \param[in] canvas The Tvg_Canvas object containing elements to be drawn.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Canvas pointer.
*
* \note Drawing can be asynchronous based on the assigned thread number. To guarantee the drawing is done, call tvg_canvas_sync() afterwards.
* \see tvg_canvas_sync()
*/
TVG_API Tvg_Result tvg_canvas_draw(Tvg_Canvas* canvas);


/*!
* \brief Guarantees that the drawing process is finished.
*
* Since the canvas rendering can be performed asynchronously, it should be called after the tvg_canvas_draw().
*
* \param[in] canvas The Tvg_Canvas object containing elements which were drawn.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Canvas pointer.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION @p canvas is either already in sync condition or in a damaged condition (a draw is required before syncing).
*
* \see tvg_canvas_draw()
*/
TVG_API Tvg_Result tvg_canvas_sync(Tvg_Canvas* canvas);


/*!
* \brief Sets the drawing region in the canvas.
*
* This function defines the rectangular area of the canvas that will be used for drawing operations.
* The specified viewport is used to clip the rendering output to the boundaries of the rectangle.
*
* \param[in] canvas The Tvg_Canvas object containing elements which were drawn.
* \param[in] x The x-coordinate of the upper-left corner of the rectangle.
* \param[in] y The y-coordinate of the upper-left corner of the rectangle.
* \param[in] w The width of the rectangle.
* \param[in] h The height of the rectangle.
*
* \return Tvg_Result enumeration.
*
* \warning It's not allowed to change the viewport during tvg_canvas_update() - tvg_canvas_sync() or tvg_canvas_push() - tvg_canvas_sync().
*
* \note When resetting the target, the viewport will also be reset to the target size.
* \see tvg_swcanvas_set_target()
* \since 0.15
*/
TVG_API Tvg_Result tvg_canvas_set_viewport(Tvg_Canvas* canvas, int32_t x, int32_t y, int32_t w, int32_t h);

/** \} */   // end defgroup ThorVGCapi_Canvas


/**
* \defgroup ThorVGCapi_Paint Paint
* \brief A module for managing graphical elements. It enables duplication, transformation and composition.
*
* \{
*/

/************************************************************************/
/* Paint API                                                            */
/************************************************************************/
/*!
* \brief Releases the given Tvg_Paint object.
*
* \code
* //example of cleanup function
* Tvg_Paint *rect = NULL; //rectangle shape added in other function
*
* //rectangle delete API
* int rectangle_delete(void) {
*   if (rect) tvg_paint_del(rect);
*   rect = NULL;
* }
*
* int cleanup(void) {
*   tvg_canvas_clear(canvas, false);
*   tvg_canvas_destroy(canvas);
*   canvas = NULL;
* }
* \endcode
*
* \param[in] paint The Tvg_Paint object to be released.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*
* \warning If this function is used, tvg_canvas_clear() with the @c free argument value set to @c false should be used in order to avoid unexpected behaviours.
*
* \see tvg_canvas_clear(), tvg_canvas_destroy()
*/
TVG_API Tvg_Result tvg_paint_del(Tvg_Paint* paint);


/*!
* \brief Scales the given Tvg_Paint object by the given factor.
*
* \param[in] paint The Tvg_Paint object to be scaled.
* \param[in] factor The value of the scaling factor. The default value is 1.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION in case a custom transform is applied.
*
* \see tvg_paint_set_transform()
*/
TVG_API Tvg_Result tvg_paint_scale(Tvg_Paint* paint, float factor);


/*!
* \brief Rotates the given Tvg_Paint by the given angle.
*
* The angle in measured clockwise from the horizontal axis.
* The rotational axis passes through the point on the object with zero coordinates.
*
* \param[in] paint The Tvg_Paint object to be rotated.
* \param[in] degree The value of the rotation angle in degrees.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION in case a custom transform is applied.
*
* \see tvg_paint_set_transform()
*/
TVG_API Tvg_Result tvg_paint_rotate(Tvg_Paint* paint, float degree);


/*!
* \brief Moves the given Tvg_Paint in a two-dimensional space.
*
* The origin of the coordinate system is in the upper-left corner of the canvas.
* The horizontal and vertical axes point to the right and down, respectively.
*
* \param[in] paint The Tvg_Paint object to be shifted.
* \param[in] x The value of the horizontal shift.
* \param[in] y The value of the vertical shift.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION in case a custom transform is applied.
*
* \see tvg_paint_set_transform()
*/
TVG_API Tvg_Result tvg_paint_translate(Tvg_Paint* paint, float x, float y);


/*!
* \brief Transforms the given Tvg_Paint using the augmented transformation matrix.
*
* The augmented matrix of the transformation is expected to be given.
*
* \param[in] paint The Tvg_Paint object to be transformed.
* \param[in] m The 3x3 augmented matrix.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A @c nullptr is passed as the argument.
*/
TVG_API Tvg_Result tvg_paint_set_transform(Tvg_Paint* paint, const Tvg_Matrix* m);


/*!
* \brief Gets the matrix of the affine transformation of the given Tvg_Paint object.
*
* In case no transformation was applied, the identity matrix is returned.
*
* \param[in] paint The Tvg_Paint object of which to get the transformation matrix.
* \param[out] m The 3x3 augmented matrix.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A @c nullptr is passed as the argument.
*/
TVG_API Tvg_Result tvg_paint_get_transform(Tvg_Paint* paint, Tvg_Matrix* m);


/*!
* \brief Sets the opacity of the given Tvg_Paint.
*
* \param[in] paint The Tvg_Paint object of which the opacity value is to be set.
* \param[in] opacity The opacity value in the range [0 ~ 255], where 0 is completely transparent and 255 is opaque.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*
* \note Setting the opacity with this API may require multiple renderings using a composition. It is recommended to avoid changing the opacity if possible.
*/
TVG_API Tvg_Result tvg_paint_set_opacity(Tvg_Paint* paint, uint8_t opacity);


/*!
* \brief Gets the opacity of the given Tvg_Paint.
*
* \param[in] paint The Tvg_Paint object of which to get the opacity value.
* \param[out] opacity The opacity value in the range [0 ~ 255], where 0 is completely transparent and 255 is opaque.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT In case a @c nullptr is passed as the argument.
*/
TVG_API Tvg_Result tvg_paint_get_opacity(const Tvg_Paint* paint, uint8_t* opacity);


/*!
* \brief Duplicates the given Tvg_Paint object.
*
* Creates a new object and sets its all properties as in the original object.
*
* \param[in] paint The Tvg_Paint object to be copied.
*
* \return A copied Tvg_Paint object if succeed, @c nullptr otherwise.
*/
TVG_API Tvg_Paint* tvg_paint_duplicate(Tvg_Paint* paint);


/*!
* \brief Gets the axis-aligned bounding box of the Tvg_Paint object.
*
* \param[in] paint The Tvg_Paint object of which to get the bounds.
* \param[out] x The x-coordinate of the upper-left corner of the object.
* \param[out] y The y-coordinate of the upper-left corner of the object.
* \param[out] w The width of the object.
* \param[out] h The height of the object.
* \param[in] transformed If @c true, the paint's transformations are taken into account in the scene it belongs to. Otherwise they aren't.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*
* \note This is useful when you need to figure out the bounding box of the paint in the canvas space.
* \note The bounding box doesn't indicate the actual drawing region. It's the smallest rectangle that encloses the object.
* \note If @p transformed is @c true, the paint needs to be pushed into a canvas and updated before this api is called.
* \see tvg_canvas_update_paint()
*/
TVG_API Tvg_Result tvg_paint_get_bounds(const Tvg_Paint* paint, float* x, float* y, float* w, float* h, bool transformed);


/*!
* \brief Sets the composition target object and the composition method.
*
* \param[in] paint The source object of the composition.
* \param[in] target The target object of the composition.
* \param[in] method The method used to composite the source object with the target.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid @p paint or @p target object or the @p method equal to TVG_COMPOSITE_METHOD_NONE.
*/
TVG_API Tvg_Result tvg_paint_set_composite_method(Tvg_Paint* paint, Tvg_Paint* target, Tvg_Composite_Method method);


/**
* \brief Gets the composition target object and the composition method.
*
* \param[in] paint The source object of the composition.
* \param[out] target The target object of the composition.
* \param[out] method The method used to composite the source object with the target.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A @c nullptr is passed as the argument.
*/
TVG_API Tvg_Result tvg_paint_get_composite_method(const Tvg_Paint* paint, const Tvg_Paint** target, Tvg_Composite_Method* method);


/*!
* \brief Clip the drawing region of the paint object.
*
* This function restricts the drawing area of the paint object to the specified shape's paths.
*
* \param[in] paint The target object of the clipping.
* \param[in] clipper The shape object as the clipper.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT In case a @c nullptr is passed as the argument.
* \retval TVG_RESULT_NOT_SUPPORTED If the @p clipper type is not Shape.
*
* \note Experimental API
*/
TVG_API Tvg_Result tvg_paint_set_clip(Tvg_Paint* paint, Tvg_Paint* clipper);


/**
* \brief Gets the unique value of the paint instance indicating the instance type.
*
* \param[in] paint The Tvg_Paint object of which to get the type value.
* \param[out] type The unique type of the paint instance type.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT In case a @c nullptr is passed as the argument.
*
* \note Experimental API
*/
TVG_API Tvg_Result tvg_paint_get_type(const Tvg_Paint* paint, Tvg_Type* type);


/**
* \see tvg_paint_get_type()
*/
TVG_DEPRECATED TVG_API Tvg_Result tvg_paint_get_identifier(const Tvg_Paint* paint, Tvg_Identifier* identifier);


/**
 * @brief Sets the blending method for the paint object.
 *
 * The blending feature allows you to combine colors to create visually appealing effects, including transparency, lighting, shading, and color mixing, among others.
 * its process involves the combination of colors or images from the source paint object with the destination (the lower layer image) using blending operations.
 * The blending operation is determined by the chosen @p BlendMethod, which specifies how the colors or images are combined.
 *
 * \param[in] paint The Tvg_Paint object of which to set the blend method.
 * \param[in] method The blending method to be set.
 *
 * \return Tvg_Result enumeration.
 * \retval TVG_RESULT_INVALID_ARGUMENT In case a @c nullptr is passed as the argument.
 *
 * \since 0.15
 */
TVG_API Tvg_Result tvg_paint_set_blend_method(Tvg_Paint* paint, Tvg_Blend_Method method);


/** \} */   // end defgroup ThorVGCapi_Paint

/**
* \defgroup ThorVGCapi_Shape Shape
*
* \brief A module for managing two-dimensional figures and their properties.
*
* A shape has three major properties: shape outline, stroking, filling. The outline in the shape is retained as the path.
* Path can be composed by accumulating primitive commands such as tvg_shape_move_to(), tvg_shape_line_to(), tvg_shape_cubic_to() or complete shape interfaces such as tvg_shape_append_rect(), tvg_shape_append_circle(), etc.
* Path can consists of sub-paths. One sub-path is determined by a close command.
*
* The stroke of a shape is an optional property in case the shape needs to be represented with/without the outline borders.
* It's efficient since the shape path and the stroking path can be shared with each other. It's also convenient when controlling both in one context.
*
* \{
*/

/************************************************************************/
/* Shape API                                                            */
/************************************************************************/
/*!
* \brief Creates a new shape object.
*
* \return A new shape object.
*/
TVG_API Tvg_Paint* tvg_shape_new(void);


/*!
* \brief Resets the shape path properties.
*
* The color, the fill and the stroke properties are retained.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*
* \note The memory, where the path data is stored, is not deallocated at this stage for caching effect.
*/
TVG_API Tvg_Result tvg_shape_reset(Tvg_Paint* paint);


/*!
* \brief Sets the initial point of the sub-path.
*
* The value of the current point is set to the given point.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] x The horizontal coordinate of the initial point of the sub-path.
* \param[in] y The vertical coordinate of the initial point of the sub-path.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*/
TVG_API Tvg_Result tvg_shape_move_to(Tvg_Paint* paint, float x, float y);


/*!
* \brief Adds a new point to the sub-path, which results in drawing a line from the current point to the given end-point.
*
* The value of the current point is set to the given end-point.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] x The horizontal coordinate of the end-point of the line.
* \param[in] y The vertical coordinate of the end-point of the line.

* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*
* \note In case this is the first command in the path, it corresponds to the tvg_shape_move_to() call.
*/
TVG_API Tvg_Result tvg_shape_line_to(Tvg_Paint* paint, float x, float y);


/*!
* \brief Adds new points to the sub-path, which results in drawing a cubic Bezier curve.
*
* The Bezier curve starts at the current point and ends at the given end-point (@p x, @p y). Two control points (@p cx1, @p cy1) and (@p cx2, @p cy2) are used to determine the shape of the curve.
* The value of the current point is set to the given end-point.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] cx1 The horizontal coordinate of the 1st control point.
* \param[in] cy1 The vertical coordinate of the 1st control point.
* \param[in] cx2 The horizontal coordinate of the 2nd control point.
* \param[in] cy2 The vertical coordinate of the 2nd control point.
* \param[in] x The horizontal coordinate of the endpoint of the curve.
* \param[in] y The vertical coordinate of the endpoint of the curve.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*
* \note In case this is the first command in the path, no data from the path are rendered.
*/
TVG_API Tvg_Result tvg_shape_cubic_to(Tvg_Paint* paint, float cx1, float cy1, float cx2, float cy2, float x, float y);


/*!
* \brief Closes the current sub-path by drawing a line from the current point to the initial point of the sub-path.
*
* The value of the current point is set to the initial point of the closed sub-path.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*
* \note In case the sub-path does not contain any points, this function has no effect.
*/
TVG_API Tvg_Result tvg_shape_close(Tvg_Paint* paint);


/*!
* \brief Appends a rectangle to the path.
*
* The rectangle with rounded corners can be achieved by setting non-zero values to @p rx and @p ry arguments.
* The @p rx and @p ry values specify the radii of the ellipse defining the rounding of the corners.
*
* The position of the rectangle is specified by the coordinates of its upper-left corner -  @p x and @p y arguments.
*
* The rectangle is treated as a new sub-path - it is not connected with the previous sub-path.
*
* The value of the current point is set to (@p x + @p rx, @p y) - in case @p rx is greater
* than @p w/2 the current point is set to (@p x + @p w/2, @p y)
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] x The horizontal coordinate of the upper-left corner of the rectangle.
* \param[in] y The vertical coordinate of the upper-left corner of the rectangle.
* \param[in] w The width of the rectangle.
* \param[in] h The height of the rectangle.
* \param[in] rx The x-axis radius of the ellipse defining the rounded corners of the rectangle.
* \param[in] ry The y-axis radius of the ellipse defining the rounded corners of the rectangle.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*
& \note For @p rx and @p ry greater than or equal to the half of @p w and the half of @p h, respectively, the shape become an ellipse.
*/
TVG_API Tvg_Result tvg_shape_append_rect(Tvg_Paint* paint, float x, float y, float w, float h, float rx, float ry);


/*!
* \brief Appends an ellipse to the path.
*
* The position of the ellipse is specified by the coordinates of its center - @p cx and @p cy arguments.
*
* The ellipse is treated as a new sub-path - it is not connected with the previous sub-path.
*
* The value of the current point is set to (@p cx, @p cy - @p ry).
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] cx The horizontal coordinate of the center of the ellipse.
* \param[in] cy The vertical coordinate of the center of the ellipse.
* \param[in] rx The x-axis radius of the ellipse.
* \param[in] ry The y-axis radius of the ellipse.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*/
TVG_API Tvg_Result tvg_shape_append_circle(Tvg_Paint* paint, float cx, float cy, float rx, float ry);


/*!
* \brief Appends a circular arc to the path.
*
* The arc is treated as a new sub-path - it is not connected with the previous sub-path.
* The current point value is set to the end-point of the arc in case @p pie is @c false, and to the center of the arc otherwise.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] cx The horizontal coordinate of the center of the arc.
* \param[in] cy The vertical coordinate of the center of the arc.
* \param[in] radius The radius of the arc.
* \param[in] startAngle The start angle of the arc given in degrees, measured counter-clockwise from the horizontal line.
* \param[in] sweep The central angle of the arc given in degrees, measured counter-clockwise from @p startAngle.
* \param[in] pie Specifies whether to draw radii from the arc's center to both of its end-point - drawn if @c true.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*
* \note Setting @p sweep value greater than 360 degrees, is equivalent to calling tvg_shape_append_circle(paint, cx, cy, radius, radius).
*/
TVG_API Tvg_Result tvg_shape_append_arc(Tvg_Paint* paint, float cx, float cy, float radius, float startAngle, float sweep, uint8_t pie);


/*!
* \brief Appends a given sub-path to the path.
*
* The current point value is set to the last point from the sub-path.
* For each command from the @p cmds array, an appropriate number of points in @p pts array should be specified.
* If the number of points in the @p pts array is different than the number required by the @p cmds array, the shape with this sub-path will not be displayed on the screen.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] cmds The array of the commands in the sub-path.
* \param[in] cmdCnt The length of the @p cmds array.
* \param[in] pts The array of the two-dimensional points.
* \param[in] ptsCnt The length of the @p pts array.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A @c nullptr passed as the argument or @p cmdCnt or @p ptsCnt equal to zero.
*/
TVG_API Tvg_Result tvg_shape_append_path(Tvg_Paint* paint, const Tvg_Path_Command* cmds, uint32_t cmdCnt, const Tvg_Point* pts, uint32_t ptsCnt);


/*!
* \brief Gets the points values of the path.
*
* The function does not allocate any data, it operates on internal memory. There is no need to free the @p pts array.
*
* \code
* Tvg_Paint *shape = tvg_shape_new();
* Tvg_Point *coords = NULL;
* uint32_t len = 0;
*
* tvg_shape_append_circle(shape, 10, 10, 50, 50);
* tvg_shape_get_path_coords(shape, (const Tvg_Point**)&coords, &len);
* //TVG approximates a circle by four Bezier curves. In the example above the coords array stores their coordinates.
* \endcode
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[out] pts The pointer to the array of the two-dimensional points from the path.
* \param[out] cnt The length of the @p pts array.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A @c nullptr passed as the argument.
*/
TVG_API Tvg_Result tvg_shape_get_path_coords(const Tvg_Paint* paint, const Tvg_Point** pts, uint32_t* cnt);


/*!
* \brief Gets the commands data of the path.
*
* The function does not allocate any data. There is no need to free the @p cmds array.
*
* \code
* Tvg_Paint *shape = tvg_shape_new();
* Tvg_Path_Command *cmds = NULL;
* uint32_t len = 0;
*
* tvg_shape_append_circle(shape, 10, 10, 50, 50);
* tvg_shape_get_path_commands(shape, (const Tvg_Path_Command**)&cmds, &len);
* //TVG approximates a circle by four Bezier curves. In the example above the cmds array stores the commands of the path data.
* \endcode
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[out] cmds The pointer to the array of the commands from the path.
* \param[out] cnt The length of the @p cmds array.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A @c nullptr passed as the argument.
*/
TVG_API Tvg_Result tvg_shape_get_path_commands(const Tvg_Paint* paint, const Tvg_Path_Command** cmds, uint32_t* cnt);


/*!
* \brief Sets the stroke width for all of the figures from the @p paint.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] width The width of the stroke. The default value is 0.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_width(Tvg_Paint* paint, float width);


/*!
* \brief Gets the shape's stroke width.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[out] width The stroke width.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid pointer passed as an argument.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_width(const Tvg_Paint* paint, float* width);


/*!
* \brief Sets the shape's stroke color.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] r The red color channel value in the range [0 ~ 255]. The default value is 0.
* \param[in] g The green color channel value in the range [0 ~ 255]. The default value is 0.
* \param[in] b The blue color channel value in the range [0 ~ 255]. The default value is 0.
* \param[in] a The alpha channel value in the range [0 ~ 255], where 0 is completely transparent and 255 is opaque.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*
* \note Either a solid color or a gradient fill is applied, depending on what was set as last.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_color(Tvg_Paint* paint, uint8_t r, uint8_t g, uint8_t b, uint8_t a);


/*!
* \brief Gets the shape's stroke color.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[out] r The red color channel value in the range [0 ~ 255]. The default value is 0.
* \param[out] g The green color channel value in the range [0 ~ 255]. The default value is 0.
* \param[out] b The blue color channel value in the range [0 ~ 255]. The default value is 0.
* \param[out] a The alpha channel value in the range [0 ~ 255], where 0 is completely transparent and 255 is opaque.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION No stroke was set.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_color(const Tvg_Paint* paint, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);


/*!
* \brief Sets the linear gradient fill of the stroke for all of the figures from the path.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] grad The linear gradient fill.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
* \retval TVG_RESULT_MEMORY_CORRUPTION An invalid Tvg_Gradient pointer or an error with accessing it.
*
* \note Either a solid color or a gradient fill is applied, depending on what was set as last.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_linear_gradient(Tvg_Paint* paint, Tvg_Gradient* grad);


/*!
* \brief Sets the radial gradient fill of the stroke for all of the figures from the path.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] grad The radial gradient fill.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
* \retval TVG_RESULT_MEMORY_CORRUPTION An invalid Tvg_Gradient pointer or an error with accessing it.
*
* \note Either a solid color or a gradient fill is applied, depending on what was set as last.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_radial_gradient(Tvg_Paint* paint, Tvg_Gradient* grad);


/*!
* \brief Gets the gradient fill of the shape's stroke.
*
* The function does not allocate any memory.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[out] grad The gradient fill.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid pointer passed as an argument.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_gradient(const Tvg_Paint* paint, Tvg_Gradient** grad);


/*!
* \brief Sets the shape's stroke dash pattern.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] dashPattern The array of consecutive pair values of the dash length and the gap length.
* \param[in] cnt The size of the @p dashPattern array.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid pointer passed as an argument and @p cnt > 0, the given length of the array is less than two or any of the @p dashPattern values is zero or less.
*
* \note To reset the stroke dash pattern, pass @c nullptr to @p dashPattern and zero to @p cnt.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_dash(Tvg_Paint* paint, const float* dashPattern, uint32_t cnt);


/*!
* \brief Gets the dash pattern of the stroke.
*
* The function does not allocate any memory.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[out] dashPattern The array of consecutive pair values of the dash length and the gap length.
* \param[out] cnt The size of the @p dashPattern array.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid pointer passed as an argument.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_dash(const Tvg_Paint* paint, const float** dashPattern, uint32_t* cnt);


/*!
* \brief Sets the cap style used for stroking the path.
*
* The cap style specifies the shape to be used at the end of the open stroked sub-paths.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] cap The cap style value. The default value is @c TVG_STROKE_CAP_SQUARE.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_cap(Tvg_Paint* paint, Tvg_Stroke_Cap cap);


/*!
* \brief Gets the stroke cap style used for stroking the path.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[out] cap The cap style value.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid pointer passed as an argument.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_cap(const Tvg_Paint* paint, Tvg_Stroke_Cap* cap);


/*!
* \brief Sets the join style for stroked path segments.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] join The join style value. The default value is @c TVG_STROKE_JOIN_BEVEL.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*/
TVG_API Tvg_Result tvg_shape_set_stroke_join(Tvg_Paint* paint, Tvg_Stroke_Join join);


/*!
* \brief The function gets the stroke join method
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[out] join The join style value.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid pointer passed as an argument.
*/
TVG_API Tvg_Result tvg_shape_get_stroke_join(const Tvg_Paint* paint, Tvg_Stroke_Join* join);


/*!
* \brief Sets the stroke miterlimit.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] miterlimit The miterlimit imposes a limit on the extent of the stroke join when the @c TVG_STROKE_JOIN_MITER join style is set. The default value is 4.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer or Unsupported @p miterlimit values (less than zero).
*
* \since 0.11
*/
TVG_API Tvg_Result tvg_shape_set_stroke_miterlimit(Tvg_Paint* paint, float miterlimit);


/*!
* \brief The function gets the stroke miterlimit.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[out] miterlimit The stroke miterlimit.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid pointer passed as an argument.
*
* \since 0.11
*/
TVG_API Tvg_Result tvg_shape_get_stroke_miterlimit(const Tvg_Paint* paint, float* miterlimit);


/*!
* \brief Sets the trim of the stroke along the defined path segment, allowing control over which part of the stroke is visible.
*
* If the values of the arguments @p begin and @p end exceed the 0-1 range, they are wrapped around in a manner similar to angle wrapping, effectively treating the range as circular.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] begin Specifies the start of the segment to display along the path.
* \param[in] end Specifies the end of the segment to display along the path.
* \param[in] simultaneous Determines how to trim multiple paths within a single shape. If set to @c true (default), trimming is applied simultaneously to all paths;
* Otherwise, all paths are treated as a single entity with a combined length equal to the sum of their individual lengths and are trimmed as such.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*
* \note Experimental API
*/
TVG_API Tvg_Result tvg_shape_set_stroke_trim(Tvg_Paint* paint, float begin, float end, bool simultaneous);


/*!
* \brief Sets the shape's solid color.
*
* The parts of the shape defined as inner are colored.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] r The red color channel value in the range [0 ~ 255]. The default value is 0.
* \param[in] g The green color channel value in the range [0 ~ 255]. The default value is 0.
* \param[in] b The blue color channel value in the range [0 ~ 255]. The default value is 0.
* \param[in] a The alpha channel value in the range [0 ~ 255], where 0 is completely transparent and 255 is opaque. The default value is 0.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*
* \note Either a solid color or a gradient fill is applied, depending on what was set as last.
* \see tvg_shape_set_fill_rule()
*/
TVG_API Tvg_Result tvg_shape_set_fill_color(Tvg_Paint* paint, uint8_t r, uint8_t g, uint8_t b, uint8_t a);


/*!
* \brief Gets the shape's solid color.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[out] r The red color channel value in the range [0 ~ 255]. The default value is 0.
* \param[out] g The green color channel value in the range [0 ~ 255]. The default value is 0.
* \param[out] b The blue color channel value in the range [0 ~ 255]. The default value is 0.
* \param[out] a The alpha channel value in the range [0 ~ 255], where 0 is completely transparent and 255 is opaque. The default value is 0.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*/
TVG_API Tvg_Result tvg_shape_get_fill_color(const Tvg_Paint* paint, uint8_t* r, uint8_t* g, uint8_t* b, uint8_t* a);


/*!
* \brief Sets the shape's fill rule.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] rule The fill rule value. The default value is @c TVG_FILL_RULE_WINDING.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*/
TVG_API Tvg_Result tvg_shape_set_fill_rule(Tvg_Paint* paint, Tvg_Fill_Rule rule);


/*!
* \brief Gets the shape's fill rule.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[out] rule shape's fill rule
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid pointer passed as an argument.
*/
TVG_API Tvg_Result tvg_shape_get_fill_rule(const Tvg_Paint* paint, Tvg_Fill_Rule* rule);


/*!
* \brief Sets the rendering order of the stroke and the fill.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] strokeFirst If @c true the stroke is rendered before the fill, otherwise the stroke is rendered as the second one (the default option).
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*
* \since 0.10
*/
TVG_API Tvg_Result tvg_shape_set_paint_order(Tvg_Paint* paint, bool strokeFirst);


/*!
* \brief Sets the linear gradient fill for all of the figures from the path.
*
* The parts of the shape defined as inner are filled.
*
* \code
* Tvg_Gradient* grad = tvg_linear_gradient_new();
* tvg_linear_gradient_set(grad, 700, 700, 800, 800);
* Tvg_Color_Stop color_stops[4] =
* {
*   {0.0 , 0,   0,   0,   255},
*   {0.25, 255, 0,   0,   255},
*   {0.5 , 0,   255, 0,   255},
*   {1.0 , 0,   0,   255, 255}
* };
* tvg_gradient_set_color_stops(grad, color_stops, 4);
* tvg_shape_set_linear_gradient(shape, grad);
* \endcode
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] grad The linear gradient fill.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
* \retval TVG_RESULT_MEMORY_CORRUPTION An invalid Tvg_Gradient pointer.
*
* \note Either a solid color or a gradient fill is applied, depending on what was set as last.
* \see tvg_shape_set_fill_rule()
*/
TVG_API Tvg_Result tvg_shape_set_linear_gradient(Tvg_Paint* paint, Tvg_Gradient* grad);


/*!
* \brief Sets the radial gradient fill for all of the figures from the path.
*
* The parts of the shape defined as inner are filled.
*
* \code
* Tvg_Gradient* grad = tvg_radial_gradient_new();
* tvg_radial_gradient_set(grad, 550, 550, 50);
* Tvg_Color_Stop color_stops[4] =
* {
*   {0.0 , 0,   0,   0,   255},
*   {0.25, 255, 0,   0,   255},
*   {0.5 , 0,   255, 0,   255},
*   {1.0 , 0,   0,   255, 255}
* };
* tvg_gradient_set_color_stops(grad, color_stops, 4);
* tvg_shape_set_radial_gradient(shape, grad);
* \endcode
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[in] grad The radial gradient fill.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
* \retval TVG_RESULT_MEMORY_CORRUPTION An invalid Tvg_Gradient pointer.
*
* \note Either a solid color or a gradient fill is applied, depending on what was set as last.
* \see tvg_shape_set_fill_rule()
*/
TVG_API Tvg_Result tvg_shape_set_radial_gradient(Tvg_Paint* paint, Tvg_Gradient* grad);


/*!
* \brief Gets the gradient fill of the shape.
*
* The function does not allocate any data.
*
* \param[in] paint A Tvg_Paint pointer to the shape object.
* \param[out] grad The gradient fill.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid pointer passed as an argument.
*/
TVG_API Tvg_Result tvg_shape_get_gradient(const Tvg_Paint* paint, Tvg_Gradient** grad);


/** \} */   // end defgroup ThorVGCapi_Shape


/**
* \defgroup ThorVGCapi_Gradient Gradient
* \brief A module managing the gradient fill of objects.
*
* The module enables to set and to get the gradient colors and their arrangement inside the gradient bounds,
* to specify the gradient bounds and the gradient behavior in case the area defined by the gradient bounds
* is smaller than the area to be filled.
*
* \{
*/

/************************************************************************/
/* Gradient API                                                         */
/************************************************************************/
/*!
* \brief Creates a new linear gradient object.
*
* \code
* Tvg_Paint* shape = tvg_shape_new();
* tvg_shape_append_rect(shape, 700, 700, 100, 100, 20, 20);
* Tvg_Gradient* grad = tvg_linear_gradient_new();
* tvg_linear_gradient_set(grad, 700, 700, 800, 800);
* Tvg_Color_Stop color_stops[2] =
* {
*   {0.0, 0, 0,   0, 255},
*   {1.0, 0, 255, 0, 255},
* };
* tvg_gradient_set_color_stops(grad, color_stops, 2);
* tvg_shape_set_linear_gradient(shape, grad);
* \endcode
*
* \return A new linear gradient object.
*/
TVG_API Tvg_Gradient* tvg_linear_gradient_new(void);


/*!
* \brief Creates a new radial gradient object.
*
* \code
* Tvg_Paint* shape = tvg_shape_new();
* tvg_shape_append_rect(shape, 700, 700, 100, 100, 20, 20);
* Tvg_Gradient* grad = tvg_radial_gradient_new();
* tvg_radial_gradient_set(grad, 550, 550, 50);
* Tvg_Color_Stop color_stops[2] =
* {
*   {0.0, 0, 0,   0, 255},
*   {1.0, 0, 255, 0, 255},
* };
* tvg_gradient_set_color_stops(grad, color_stops, 2);
* tvg_shape_set_radial_gradient(shape, grad);
* \endcode
*
* \return A new radial gradient object.
*/
TVG_API Tvg_Gradient* tvg_radial_gradient_new(void);


/*!
* \brief Sets the linear gradient bounds.
*
* The bounds of the linear gradient are defined as a surface constrained by two parallel lines crossing
* the given points (@p x1, @p y1) and (@p x2, @p y2), respectively. Both lines are perpendicular to the line linking
* (@p x1, @p y1) and (@p x2, @p y2).
*
* \param[in] grad The Tvg_Gradient object of which bounds are to be set.
* @param[in] x1 The horizontal coordinate of the first point used to determine the gradient bounds.
* @param[in] y1 The vertical coordinate of the first point used to determine the gradient bounds.
* @param[in] x2 The horizontal coordinate of the second point used to determine the gradient bounds.
* @param[in] y2 The vertical coordinate of the second point used to determine the gradient bounds.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Gradient pointer.
*
* \note In case the first and the second points are equal, an object is filled with a single color using the last color specified in the tvg_gradient_set_color_stops().
* \see tvg_gradient_set_color_stops()
*/
TVG_API Tvg_Result tvg_linear_gradient_set(Tvg_Gradient* grad, float x1, float y1, float x2, float y2);


/*!
* \brief Gets the linear gradient bounds.
*
* The bounds of the linear gradient are defined as a surface constrained by two parallel lines crossing
* the given points (@p x1, @p y1) and (@p x2, @p y2), respectively. Both lines are perpendicular to the line linking
* (@p x1, @p y1) and (@p x2, @p y2).
*
* \param[in] grad The Tvg_Gradient object of which to get the bounds.
* \param[out] x1 The horizontal coordinate of the first point used to determine the gradient bounds.
* \param[out] y1 The vertical coordinate of the first point used to determine the gradient bounds.
* \param[out] x2 The horizontal coordinate of the second point used to determine the gradient bounds.
* \param[out] y2 The vertical coordinate of the second point used to determine the gradient bounds.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Gradient pointer.
*/
TVG_API Tvg_Result tvg_linear_gradient_get(Tvg_Gradient* grad, float* x1, float* y1, float* x2, float* y2);


/*!
* \brief Sets the radial gradient bounds.
*
* The radial gradient bounds are defined as a circle centered in a given point (@p cx, @p cy) of a given radius.
*
* \param[in] grad The Tvg_Gradient object of which bounds are to be set.
* \param[in] cx The horizontal coordinate of the center of the bounding circle.
* \param[in] cy The vertical coordinate of the center of the bounding circle.
* \param[in] radius The radius of the bounding circle.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Gradient pointer or the @p radius value less than zero.
*
* \note In case the @p radius is zero, an object is filled with a single color using the last color specified in the specified in the tvg_gradient_set_color_stops().
* \see tvg_gradient_set_color_stops()
*/
TVG_API Tvg_Result tvg_radial_gradient_set(Tvg_Gradient* grad, float cx, float cy, float radius);


/*!
* \brief The function gets radial gradient center point ant radius
*
* \param[in] grad The Tvg_Gradient object of which bounds are to be set.
* \param[out] cx The horizontal coordinate of the center of the bounding circle.
* \param[out] cy The vertical coordinate of the center of the bounding circle.
* \param[out] radius The radius of the bounding circle.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Gradient pointer.
*/
TVG_API Tvg_Result tvg_radial_gradient_get(Tvg_Gradient* grad, float* cx, float* cy, float* radius);


/*!
* \brief Sets the parameters of the colors of the gradient and their position.
*
* \param[in] grad The Tvg_Gradient object of which the color information is to be set.
* \param[in] color_stop An array of Tvg_Color_Stop data structure.
* \param[in] cnt The size of the @p color_stop array equal to the colors number used in the gradient.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Gradient pointer.
*/
TVG_API Tvg_Result tvg_gradient_set_color_stops(Tvg_Gradient* grad, const Tvg_Color_Stop* color_stop, uint32_t cnt);


/*!
* \brief Gets the parameters of the colors of the gradient, their position and number
*
* The function does not allocate any memory.
*
* \param[in] grad The Tvg_Gradient object of which to get the color information.
* \param[out] color_stop An array of Tvg_Color_Stop data structure.
* \param[out] cnt The size of the @p color_stop array equal to the colors number used in the gradient.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A @c nullptr passed as the argument.
*/
TVG_API Tvg_Result tvg_gradient_get_color_stops(const Tvg_Gradient* grad, const Tvg_Color_Stop** color_stop, uint32_t* cnt);


/*!
* \brief Sets the Tvg_Stroke_Fill value, which specifies how to fill the area outside the gradient bounds.
*
* \param[in] grad The Tvg_Gradient object.
* \param[in] spread The FillSpread value.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Gradient pointer.
*/
TVG_API Tvg_Result tvg_gradient_set_spread(Tvg_Gradient* grad, const Tvg_Stroke_Fill spread);


/*!
* \brief Gets the FillSpread value of the gradient object.
*
* \param[in] grad The Tvg_Gradient object.
* \param[out] spread The FillSpread value.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A @c nullptr passed as the argument.
*/
TVG_API Tvg_Result tvg_gradient_get_spread(const Tvg_Gradient* grad, Tvg_Stroke_Fill* spread);


/*!
* \brief Sets the matrix of the affine transformation for the gradient object.
*
* The augmented matrix of the transformation is expected to be given.
*
* \param[in] grad The Tvg_Gradient object to be transformed.
* \param[in] m The 3x3 augmented matrix.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A @c nullptr is passed as the argument.
*/
TVG_API Tvg_Result tvg_gradient_set_transform(Tvg_Gradient* grad, const Tvg_Matrix* m);


/*!
* \brief Gets the matrix of the affine transformation of the gradient object.
*
* In case no transformation was applied, the identity matrix is set.
*
* \param[in] grad The Tvg_Gradient object of which to get the transformation matrix.
* \param[out] m The 3x3 augmented matrix.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A @c nullptr is passed as the argument.
*/
TVG_API Tvg_Result tvg_gradient_get_transform(const Tvg_Gradient* grad, Tvg_Matrix* m);

/**
* \brief Gets the unique value of the gradient instance indicating the instance type.
*
* \param[in] grad The Tvg_Gradient object of which to get the type value.
* \param[out] type The unique type of the gradient instance type.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT In case a @c nullptr is passed as the argument.
*
* \note Experimental API
*/
TVG_API Tvg_Result tvg_gradient_get_type(const Tvg_Gradient* grad, Tvg_Type* type);


/**
* \see tvg_gradient_get_type()
*/
TVG_DEPRECATED TVG_API Tvg_Result tvg_gradient_get_identifier(const Tvg_Gradient* grad, Tvg_Identifier* identifier);


/*!
* \brief Duplicates the given Tvg_Gradient object.
*
* Creates a new object and sets its all properties as in the original object.
*
* \param[in] grad The Tvg_Gradient object to be copied.
*
* \return A copied Tvg_Gradient object if succeed, @c nullptr otherwise.
*/
TVG_API Tvg_Gradient* tvg_gradient_duplicate(Tvg_Gradient* grad);


/*!
* \brief Deletes the given gradient object.
*
* \param[in] grad The gradient object to be deleted.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Gradient pointer.
*/
TVG_API Tvg_Result tvg_gradient_del(Tvg_Gradient* grad);


/** \} */   // end defgroup ThorVGCapi_Gradient


/**
* \defgroup ThorVGCapi_Picture Picture
*
* \brief A module enabling to create and to load an image in one of the supported formats: svg, png, jpg, lottie and raw.
*
*
* \{
*/

/************************************************************************/
/* Picture API                                                          */
/************************************************************************/
/*!
* \brief Creates a new picture object.
*
* \return A new picture object.
*/
TVG_API Tvg_Paint* tvg_picture_new(void);


/*!
* \brief Loads a picture data directly from a file.
*
* ThorVG efficiently caches the loaded data using the specified @p path as a key.
* This means that loading the same file again will not result in duplicate operations;
* instead, ThorVG will reuse the previously loaded picture data.
*
* \param[in] paint A Tvg_Paint pointer to the picture object.
* \param[in] path The absolute path to the image file.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer or an empty @p path.
* \retval TVG_RESULT_NOT_SUPPORTED A file with an unknown extension.
*/
TVG_API Tvg_Result tvg_picture_load(Tvg_Paint* paint, const char* path);


/*!
* \brief Loads a picture data from a memory block of a given size.
*
* ThorVG efficiently caches the loaded data using the specified @p data address as a key
* when the @p copy has @c false. This means that loading the same data again will not result in duplicate operations
* for the sharable @p data. Instead, ThorVG will reuse the previously loaded picture data.
*
* \param[in] paint A Tvg_Paint pointer to the picture object.
* \param[in] data A pointer to a memory location where the content of the picture raw data is stored.
* \param[in] w The width of the image @p data in pixels.
* \param[in] h The height of the image @p data in pixels.
* \param[in] premultiplied If @c true, the given image data is alpha-premultiplied.
* \param[in] copy If @c true the data are copied into the engine local buffer, otherwise they are not.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer or no data are provided or the @p width or @p height value is zero or less.
* \retval TVG_RESULT_FAILED_ALLOCATION A problem with memory allocation occurs.
*
* \since 0.9
*/
TVG_API Tvg_Result tvg_picture_load_raw(Tvg_Paint* paint, uint32_t *data, uint32_t w, uint32_t h, bool copy);


/*!
* \brief Loads a picture data from a memory block of a given size.
*
* ThorVG efficiently caches the loaded data using the specified @p data address as a key
* when the @p copy has @c false. This means that loading the same data again will not result in duplicate operations
* for the sharable @p data. Instead, ThorVG will reuse the previously loaded picture data.
*
* \param[in] paint A Tvg_Paint pointer to the picture object.
* \param[in] data A pointer to a memory location where the content of the picture file is stored. A null-terminated string is expected for non-binary data if @p copy is @c false
* \param[in] size The size in bytes of the memory occupied by the @p data.
* \param[in] mimetype Mimetype or extension of data such as "jpg", "jpeg", "svg", "svg+xml", "lottie", "png", etc. In case an empty string or an unknown type is provided, the loaders will be tried one by one.
* \param[in] copy If @c true the data are copied into the engine local buffer, otherwise they are not.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT In case a @c nullptr is passed as the argument or the @p size is zero or less.
* \retval TVG_RESULT_NOT_SUPPORTED A file with an unknown extension.
*
* \warning: It's the user responsibility to release the @p data memory if the @p copy is @c true.
*/
TVG_API Tvg_Result tvg_picture_load_data(Tvg_Paint* paint, const char *data, uint32_t size, const char *mimetype, bool copy);


/*!
* \brief Resizes the picture content to the given width and height.
*
* The picture content is resized while keeping the default size aspect ratio.
* The scaling factor is established for each of dimensions and the smaller value is applied to both of them.
*
* \param[in] paint A Tvg_Paint pointer to the picture object.
* \param[in] w A new width of the image in pixels.
* \param[in] h A new height of the image in pixels.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*/
TVG_API Tvg_Result tvg_picture_set_size(Tvg_Paint* paint, float w, float h);


/*!
* \brief Gets the size of the loaded picture.
*
* \param[in] paint A Tvg_Paint pointer to the picture object.
* \param[out] w A width of the image in pixels.
* \param[out] h A height of the image in pixels.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*/
TVG_API Tvg_Result tvg_picture_get_size(const Tvg_Paint* paint, float* w, float* h);


/*!
* \brief Retrieve a paint object from the Picture scene by its Unique ID.
*
* This function searches for a paint object within the Picture scene that matches the provided @p id.
*
* \param[in] paint A Tvg_Paint pointer to the picture object.
* \param[in] id The Unique ID of the paint object.

* \return A pointer to the paint object that matches the given identifier, or @c nullptr if no matching paint object is found.
*
* \see tvg_accessor_generate_id()
* \note experimental API
*/
TVG_API const Tvg_Paint* tvg_picture_get_paint(Tvg_Paint* paint, uint32_t id);


/** \} */   // end defgroup ThorVGCapi_Picture


/**
* \defgroup ThorVGCapi_Scene Scene
* \brief A module managing the multiple paints as one group paint.
*
* As a group, scene can be transformed, translucent, composited with other target paints,
* its children will be affected by the scene world.
*
* \{
*/

/************************************************************************/
/* Scene API                                                            */
/************************************************************************/
/*!
* \brief Creates a new scene object.
*
* A scene object is used to group many paints into one object, which can be manipulated using TVG APIs.
*
* \return A new scene object.
*/
TVG_API Tvg_Paint* tvg_scene_new(void);


/*!
* \brief Sets the size of the container, where all the paints pushed into the scene are stored.
*
* If the number of objects pushed into the scene is known in advance, calling the function
* prevents multiple memory reallocation, thus improving the performance.
*
* \param[in] scene A Tvg_Paint pointer to the scene object.
* \param[in] size The number of objects for which the memory is to be reserved.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_FAILED_ALLOCATION An internal error with a memory allocation.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Paint pointer.
*/
TVG_DEPRECATED TVG_API Tvg_Result tvg_scene_reserve(Tvg_Paint* scene, uint32_t size);


/*!
* \brief Passes drawing elements to the scene using Tvg_Paint objects.
*
* Only the paints pushed into the scene will be the drawn targets.
* The paints are retained by the scene until the tvg_scene_clear() is called.
* If you know the number of pushed objects in advance, please call tvg_scene_reserve().
*
* \param[in] scene A Tvg_Paint pointer to the scene object.
* \param[in] paint A graphical object to be drawn.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A @c nullptr passed as the argument.
*
* \note The rendering order of the paints is the same as the order as they were pushed. Consider sorting the paints before pushing them if you intend to use layering.
*/
TVG_API Tvg_Result tvg_scene_push(Tvg_Paint* scene, Tvg_Paint* paint);


/*!
* \brief Clears a scene objects from pushed paints.
*
* Tvg_Paint objects stored in the scene are released if @p free is set to @c true, otherwise the memory is not deallocated and
* all paints should be released manually in order to avoid memory leaks.
*
* \param[in] scene The scene object to be cleared.
* \param[in] free If @c true the memory occupied by paints is deallocated, otherwise it is not.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Canvas pointer.
*
* \warning Please use the @p free argument only when you know how it works, otherwise it's not recommended.
*/
TVG_API Tvg_Result tvg_scene_clear(Tvg_Paint* scene, bool free);

/** \} */   // end defgroup ThorVGCapi_Scene



/**
* \defgroup ThorVGCapi_Text Text
* \brief A class to represent text objects in a graphical context, allowing for rendering and manipulation of unicode text.
*
* \since 0.15
*
* \{
*/

/************************************************************************/
/* Text API                                                            */
/************************************************************************/
/*!
* \brief Creates a new text object.
*
* \return A new text object.
*
* \since 0.15
*/
TVG_API Tvg_Paint* tvg_text_new(void);


/**
* \brief Sets the font properties for the text.
*
* This function allows you to define the font characteristics used for text rendering.
* It sets the font name, size and optionally the style.
*
* \param[in] paint A Tvg_Paint pointer to the text object.
* \param[in] name The name of the font. This should correspond to a font available in the canvas.
* \param[in] size The size of the font in points.
* \param[in] style The style of the font. If empty, the default style is used. Currently only 'italic' style is supported.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A \c nullptr passed as the \p paint argument.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION  The specified \p name cannot be found.
*
* \note Experimental API
*/
TVG_API Tvg_Result tvg_text_set_font(Tvg_Paint* paint, const char* name, float size, const char* style);


/**
* \brief Assigns the given unicode text to be rendered.
*
* This function sets the unicode text that will be displayed by the rendering system.
* The text is set according to the specified UTF encoding method, which defaults to UTF-8.
*
* \param[in] paint A Tvg_Paint pointer to the text object.
* \param[in] text The multi-byte text encoded with utf8 string to be rendered.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A \c nullptr passed as the \p paint argument.
*
* \note Experimental API
*/
TVG_API Tvg_Result tvg_text_set_text(Tvg_Paint* paint, const char* text);


/**
* \brief Sets the text solid color.
*
* \param[in] paint A Tvg_Paint pointer to the text object.
* \param[in] r The red color channel value in the range [0 ~ 255]. The default value is 0.
* \param[in] g The green color channel value in the range [0 ~ 255]. The default value is 0.
* \param[in] b The blue color channel value in the range [0 ~ 255]. The default value is 0.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A \c nullptr passed as the \p paint argument.
*
* \note Either a solid color or a gradient fill is applied, depending on what was set as last.
* \see tvg_text_set_font()
*
* \since 0.15
*/
TVG_API Tvg_Result tvg_text_set_fill_color(Tvg_Paint* paint, uint8_t r, uint8_t g, uint8_t b);


/**
* \brief Sets the gradient fill for the text.
*
* \param[in] paint A Tvg_Paint pointer to the text object.
* \param[in] grad The linear or radial gradient fill
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A \c nullptr passed as the \p paint argument.
* \retval TVG_RESULT_MEMORY_CORRUPTION An invalid Tvg_Gradient pointer.
*
* \note Either a solid color or a gradient fill is applied, depending on what was set as last.
* \see tvg_text_set_font()
*
* \since 0.15
*/
TVG_API Tvg_Result tvg_text_set_gradient(Tvg_Paint* paint, Tvg_Gradient* gradient);

/**
* \brief Loads a scalable font data from a file.
*
* ThorVG efficiently caches the loaded data using the specified \p path as a key.
* This means that loading the same file again will not result in duplicate operations;
* instead, ThorVG will reuse the previously loaded font data.
*
* \param[in] path The path to the font file.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid \p path passed as an argument.
* \retval TVG_RESULT_NOT_SUPPORTED When trying to load a file with an unknown extension.
*
* \see tvg_font_unload()
*
* \since 0.15
*/
TVG_API Tvg_Result tvg_font_load(const char* path);


/**
* \brief Loads a scalable font data from a memory block of a given size.
*
* ThorVG efficiently caches the loaded font data using the specified \p name as a key.
* This means that loading the same fonts again will not result in duplicate operations.
* Instead, ThorVG will reuse the previously loaded font data.
*
* \param[in] name The name under which the font will be stored and accessible (e.x. in a \p tvg_text_set_font API).
* \param[in] data A pointer to a memory location where the content of the font data is stored.
* \param[in] size The size in bytes of the memory occupied by the @p data.
* \param[in] mimetype Mimetype or extension of font data. In case a \c NULL or an empty "" value is provided the loader will be determined automatically.
* \param[in] copy If @c true the data are copied into the engine local buffer, otherwise they are not (default).
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT If no name is provided or if \p size is zero while \p data points to a valid memory location.
* \retval TVG_RESULT_NOT_SUPPORTED When trying to load a file with an unknown extension.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION When trying to unload the font data that has not been previously loaded.
*
* \warning: It's the user responsibility to release the \p data memory.
*
* \note To unload the font data loaded using this API, pass the proper \p name and \c nullptr as \p data.
*
* \since 0.15
*/
TVG_API Tvg_Result tvg_font_load_data(const char* name, const char* data, uint32_t size, const char *mimetype, bool copy);


/**
* \brief Unloads the specified scalable font data that was previously loaded.
*
* This function is used to release resources associated with a font file that has been loaded into memory.
*
* \param[in] path The path to the loaded font file.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION The loader is not initialized.
*
* \note If the font data is currently in use, it will not be immediately unloaded.
* \see tvg_font_load()
*
* \since 0.15
*/
TVG_API Tvg_Result tvg_font_unload(const char* path);


/** \} */   // end defgroup ThorVGCapi_Text


/**
* \defgroup ThorVGCapi_Saver Saver
* \brief A module for exporting a paint object into a specified file.
*
* The module enables to save the composed scene and/or image from a paint object.
* Once it's successfully exported to a file, it can be recreated using the Picture module.
*
* \{
*/

/************************************************************************/
/* Saver API                                                            */
/************************************************************************/
/*!
* \brief Creates a new Tvg_Saver object.
*
* \return A new Tvg_Saver object.
*/
TVG_API Tvg_Saver* tvg_saver_new(void);


/*!
* \brief Exports the given @p paint data to the given @p path
*
* If the saver module supports any compression mechanism, it will optimize the data size.
* This might affect the encoding/decoding time in some cases. You can turn off the compression
* if you wish to optimize for speed.
*
* \param[in] saver The Tvg_Saver object connected with the saving task.
* \param[in] paint The paint to be saved with all its associated properties.
* \param[in] path A path to the file, in which the paint data is to be saved.
* \param[in] compress If @c true then compress data if possible.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A @c nullptr passed as the argument.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION Currently saving other resources.
* \retval TVG_RESULT_NOT_SUPPORTED Trying to save a file with an unknown extension or in an unsupported format.
* \retval TVG_RESULT_UNKNOWN An empty paint is to be saved.
*
* \note Saving can be asynchronous if the assigned thread number is greater than zero. To guarantee the saving is done, call tvg_saver_sync() afterwards.
* \see tvg_saver_sync()
*/
TVG_API Tvg_Result tvg_saver_save(Tvg_Saver* saver, Tvg_Paint* paint, const char* path, bool compress);


/*!
* \brief Guarantees that the saving task is finished.
*
* The behavior of the Saver module works on a sync/async basis, depending on the threading setting of the Initializer.
* Thus, if you wish to have a benefit of it, you must call tvg_saver_sync() after the tvg_saver_save() in the proper delayed time.
* Otherwise, you can call tvg_saver_sync() immediately.
*
* \param[in] saver The Tvg_Saver object connected with the saving task.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT A @c nullptr passed as the argument.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION No saving task is running.
*
* \note The asynchronous tasking is dependent on the Saver module implementation.
* \see tvg_saver_save()
*/
TVG_API Tvg_Result tvg_saver_sync(Tvg_Saver* saver);


/*!
* \brief Deletes the given Tvg_Saver object.
*
* \param[in] saver The Tvg_Saver object to be deleted.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Saver pointer.
*/
TVG_API Tvg_Result tvg_saver_del(Tvg_Saver* saver);


/** \} */   // end defgroup ThorVGCapi_Saver


/**
* \defgroup ThorVGCapi_Animation Animation
* \brief A module for manipulation of animatable images.
*
* The module supports the display and control of animation frames.
*
* \{
*/

/************************************************************************/
/* Animation API                                                        */
/************************************************************************/

/*!
* \brief Creates a new Animation object.
*
* \return Tvg_Animation A new Tvg_Animation object.
*
* \since 0.13
*/
TVG_API Tvg_Animation* tvg_animation_new(void);


/*!
* \brief Specifies the current frame in the animation.
*
* \param[in] animation A Tvg_Animation pointer to the animation object.
* \param[in] no The index of the animation frame to be displayed. The index should be less than the tvg_animation_get_total_frame().
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Animation pointer.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION if the given @p no is the same as the current frame value.
* \retval TVG_RESULT_NOT_SUPPORTED The picture data does not support animations.
*
* \note For efficiency, ThorVG ignores updates to the new frame value if the difference from the current frame value
*       is less than 0.001. In such cases, it returns @c Result::InsufficientCondition.
*       Values less than 0.001 may be disregarded and may not be accurately retained by the Animation.
* \see tvg_animation_get_total_frame()
*
* \since 0.13
*/
TVG_API Tvg_Result tvg_animation_set_frame(Tvg_Animation* animation, float no);


/*!
* \brief Retrieves a picture instance associated with this animation instance.
*
* This function provides access to the picture instance that can be used to load animation formats, such as Lottie(json).
* After setting up the picture, it can be pushed to the designated canvas, enabling control over animation frames
* with this Animation instance.
*
* \param[in] animation A Tvg_Animation pointer to the animation object.
*
* \return A picture instance that is tied to this animation.
*
* \warning The picture instance is owned by Animation. It should not be deleted manually.
*
* \since 0.13
*/
TVG_API Tvg_Paint* tvg_animation_get_picture(Tvg_Animation* animation);


/*!
* \brief Retrieves the current frame number of the animation.
*
* \param[in] animation A Tvg_Animation pointer to the animation object.
* \param[in] no The current frame number of the animation, between 0 and totalFrame() - 1.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Animation pointer or @p no
*
* \see tvg_animation_get_total_frame()
* \see tvg_animation_set_frame()
*
* \since 0.13
*/
TVG_API Tvg_Result tvg_animation_get_frame(Tvg_Animation* animation, float* no);


/*!
* \brief Retrieves the total number of frames in the animation.
*
* \param[in] animation A Tvg_Animation pointer to the animation object.
* \param[in] cnt The total number of frames in the animation.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Animation pointer or @p cnt.
*
* \note Frame numbering starts from 0.
* \note If the Picture is not properly configured, this function will return 0.
*
* \since 0.13
*/
TVG_API Tvg_Result tvg_animation_get_total_frame(Tvg_Animation* animation, float* cnt);


/*!
* \brief Retrieves the duration of the animation in seconds.
*
* \param[in] animation A Tvg_Animation pointer to the animation object.
* \param[in] duration The duration of the animation in seconds.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Animation pointer or @p duration.
*
* \note If the Picture is not properly configured, this function will return 0.
*
* \since 0.13
*/
TVG_API Tvg_Result tvg_animation_get_duration(Tvg_Animation* animation, float* duration);


/*!
* \brief Specifies the playback segment of the animation.
*
* \param[in] animation The Tvg_Animation pointer to the animation object.
* \param[in] begin segment begin.
* \param[in] end segment end.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION In case the animation is not loaded.
* \retval TVG_RESULT_INVALID_ARGUMENT When the given parameters are out of range.
*
* \note Experimental API
*/
TVG_API Tvg_Result tvg_animation_set_segment(Tvg_Animation* animation, float begin, float end);


/*!
* \brief Gets the current segment.
*
* \param[in] animation The Tvg_Animation pointer to the animation object.
* \param[out] begin segment begin.
* \param[out] end segment end.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION In case the animation is not loaded.
* \retval TVG_RESULT_INVALID_ARGUMENT When the given parameters are @c nullptr.
*
* \note Experimental API
*/
TVG_API Tvg_Result tvg_animation_get_segment(Tvg_Animation* animation, float* begin, float* end);


/*!
* \brief Deletes the given Tvg_Animation object.
*
* \param[in] animation The Tvg_Animation object to be deleted.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT An invalid Tvg_Animation pointer.
*
* \since 0.13
*/
TVG_API Tvg_Result tvg_animation_del(Tvg_Animation* animation);


/** \} */   // end defgroup ThorVGCapi_Animation


/**
* \defgroup ThorVGCapi_Accesssor Accessor
* \brief A module for manipulation of the scene tree
*
* This module helps to control the scene tree.
* \{
*/

/************************************************************************/
/* Accessor API                                                         */
/************************************************************************/

/*!
* \brief Generate a unique ID (hash key) from a given name.
*
* This function computes a unique identifier value based on the provided string.
* You can use this to assign a unique ID to the Paint object.
*
* \param[in] name The input string to generate the unique identifier from.
*
* \return The generated unique identifier value.
*
* \note Experimental API
*/
TVG_API uint32_t tvg_accessor_generate_id(const char* name);


/** \} */   // end defgroup ThorVGCapi_Accessor


/**
* \defgroup ThorVGCapi_LottieAnimation LottieAnimation
* \brief A module for manipulation of lottie extension features.
*
* The module enables control of advanced Lottie features.
* \{
*/

/************************************************************************/
/* LottieAnimation Extension API                                        */
/************************************************************************/

/*!
* \brief Creates a new LottieAnimation object.
*
* \return Tvg_Animation A new Tvg_LottieAnimation object.
*
* \since 0.15
*/
TVG_API Tvg_Animation* tvg_lottie_animation_new(void);


/*!
* \brief Override the lottie properties through the slot data.
*
* \param[in] animation The Tvg_Animation object to override the property with the slot.
* \param[in] slot The Lottie slot data in json, or @c nullptr to reset.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION In case the animation is not loaded.
* \retval TVG_RESULT_INVALID_ARGUMENT When the given @p slot is invalid
* \retval TVG_RESULT_NOT_SUPPORTED The Lottie Animation is not supported.
*
* \note Experimental API
*/
TVG_API Tvg_Result tvg_lottie_animation_override(Tvg_Animation* animation, const char* slot);


/*!
* \brief Specifies a segment by marker.
*
* \param[in] animation The Tvg_Animation pointer to the Lottie animation object.
* \param[in] marker The name of the segment marker.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INSUFFICIENT_CONDITION In case the animation is not loaded.
* \retval TVG_RESULT_INVALID_ARGUMENT When the given @p marker is invalid.
* \retval TVG_RESULT_NOT_SUPPORTED The Lottie Animation is not supported.
*
* \note Experimental API
*/
TVG_API Tvg_Result tvg_lottie_animation_set_marker(Tvg_Animation* animation, const char* marker);


/*!
* \brief Gets the marker count of the animation.
*
* \param[in] animation The Tvg_Animation pointer to the Lottie animation object.
* \param[out] cnt The count value of the markers.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT In case a @c nullptr is passed as the argument.
*
* \note Experimental API
*/
TVG_API Tvg_Result tvg_lottie_animation_get_markers_cnt(Tvg_Animation* animation, uint32_t* cnt);


/*!
* \brief Gets the marker name by a given index.
*
* \param[in] animation The Tvg_Animation pointer to the Lottie animation object.
* \param[in] idx The index of the animation marker, starts from 0.
* \param[out] name The name of marker when succeed.
*
* \return Tvg_Result enumeration.
* \retval TVG_RESULT_INVALID_ARGUMENT In case @c nullptr is passed as the argument or @c idx is out of range.
*
* \note Experimental API
*/
TVG_API Tvg_Result tvg_lottie_animation_get_marker(Tvg_Animation* animation, uint32_t idx, const char** name);


/** \} */   // end addtogroup ThorVGCapi_LottieAnimation


/** \} */   // end defgroup ThorVGCapi


#ifdef __cplusplus
}
#endif

#endif //_THORVG_CAPI_H_

#endif /* LV_USE_THORVG_INTERNAL */

