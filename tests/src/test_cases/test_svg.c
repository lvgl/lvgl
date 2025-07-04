#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#define LV_ARRAY_GET(array, index, type) ((type*)lv_array_at((array), (index)))

static const char * svg_str_1 = \
                                "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
                                "<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.1//EN\" \"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd\">"
                                "<!-- some comment content <tag> <desc> ... -->"
                                "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.2\" baseProfile=\"tiny\""
                                "width=\"300px\" height=\"600px\" viewBox =\"0 0 10 10\">"
                                "<rect xml:id=rect1 fill=\"white\" x=\"10\" y=\"10\" width=\"100\" height=\"100\"/>"
                                "<desc> SVG Test </desc>"
                                "</svg>";

void setUp(void)
{
}

void tearDown(void)
{
}

void testSvgParser(void)
{
    lv_svg_node_t * svg = lv_svg_load_data(svg_str_1, lv_strlen(svg_str_1));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    lv_svg_node_delete(svg);
}

void testNode(void)
{
    lv_svg_node_t * svg = lv_svg_node_create(NULL);

    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    lv_svg_node_delete(svg);
}

void testNodeTree(void)
{
    lv_svg_node_t * root = lv_svg_node_create(NULL);

    lv_svg_node_t * node1 = lv_svg_node_create(root);
    lv_svg_node_t * node2 = lv_svg_node_create(node1);
    lv_svg_node_t * node3 = lv_svg_node_create(root);

    lv_svg_attr_t attr1;
    attr1.id = LV_SVG_ATTR_X;
    attr1.val_type = LV_SVG_ATTR_VALUE_DATA;
    attr1.value.fval = 10.0f;

    lv_array_push_back(&node3->attrs, &attr1);

    TEST_ASSERT_EQUAL(lv_array_size(&node3->attrs), 1);
    TEST_ASSERT_EQUAL_FLOAT((LV_ARRAY_GET(&node3->attrs, 0, lv_svg_attr_t))->value.fval, 10.0f);

    lv_svg_node_delete(node2);
    lv_svg_node_delete(root);
}

void testSvgElement(void)
{
    const char * svg_1 = \
                         "<svg version=\"1.2\" baseProfile=\"tiny\"></svg>";
    lv_svg_node_t * svg_node1 = lv_svg_load_data(svg_1, lv_strlen(svg_1));
    TEST_ASSERT_EQUAL(lv_array_size(&svg_node1->attrs), 2);
    TEST_ASSERT_EQUAL_STRING((LV_ARRAY_GET(&svg_node1->attrs, 0, lv_svg_attr_t))->value.sval, "1.2");
    TEST_ASSERT_EQUAL_STRING((LV_ARRAY_GET(&svg_node1->attrs, 1, lv_svg_attr_t))->value.sval, "tiny");
    lv_svg_node_delete(svg_node1);

    /* test viewBox */
    const char * svg_viewbox0 = \
                                "<svg viewBox=\"none\"></svg>";
    lv_svg_node_t * svg_node_viewbox = lv_svg_load_data(svg_viewbox0, lv_strlen(svg_viewbox0));
    TEST_ASSERT_EQUAL(0, LV_ARRAY_GET(&svg_node_viewbox->attrs, 0, lv_svg_attr_t)->class_type);
    lv_svg_node_delete(svg_node_viewbox);

    const char * svg_viewbox1 = \
                                "<svg viewBox=\"0 0 10 10\"></svg>";
    lv_svg_node_t * svg_node_viewbox1 = lv_svg_load_data(svg_viewbox1, lv_strlen(svg_viewbox1));
    float ret1[4] = {0.0f, 0.0f, 10.0f, 10.0f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(ret1, (float *)(LV_ARRAY_GET(&svg_node_viewbox1->attrs, 0, lv_svg_attr_t))->value.val, 4);
    lv_svg_node_delete(svg_node_viewbox1);

    const char * svg_viewbox2 = \
                                "<svg viewBox=\"-5,10 +10,-10\"></svg>";
    lv_svg_node_t * svg_node_viewbox2 = lv_svg_load_data(svg_viewbox2, lv_strlen(svg_viewbox2));
    float ret2[4] = {-5.0f, 10.0f, 10.0f, -10.0f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(ret2, (float *)(LV_ARRAY_GET(&svg_node_viewbox2->attrs, 0, lv_svg_attr_t))->value.val, 4);
    lv_svg_node_delete(svg_node_viewbox2);

    const char * svg_viewbox3 = \
                                "<svg viewBox=\"-5,-5\"></svg>";
    lv_svg_node_t * svg_node_viewbox3 = lv_svg_load_data(svg_viewbox3, lv_strlen(svg_viewbox3));
    TEST_ASSERT_EQUAL(0, LV_ARRAY_GET(&svg_node_viewbox3->attrs, 0, lv_svg_attr_t)->class_type);
    lv_svg_node_delete(svg_node_viewbox3);

    const char * svg_viewbox4 = \
                                "<svg viewBox=\"-5,-5 .2 1.5E+1\"></svg>";
    lv_svg_node_t * svg_node_viewbox4 = lv_svg_load_data(svg_viewbox4, lv_strlen(svg_viewbox4));
    float ret4[4] = {-5.0f, -5.0f, 0.2f, 15.0f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(ret4, (float *)(LV_ARRAY_GET(&svg_node_viewbox4->attrs, 0, lv_svg_attr_t))->value.val, 4);
    lv_svg_node_delete(svg_node_viewbox4);

    /* width and height */
    const char * svg_wh = \
                          "<svg width=\"100\" height=\"100px\"></svg>";
    lv_svg_node_t * svg_node_wh = lv_svg_load_data(svg_wh, lv_strlen(svg_wh));
    TEST_ASSERT_EQUAL_FLOAT((LV_ARRAY_GET(&svg_node_wh->attrs, 0, lv_svg_attr_t))->value.fval, 100.0f);
    TEST_ASSERT_EQUAL_FLOAT((LV_ARRAY_GET(&svg_node_wh->attrs, 1, lv_svg_attr_t))->value.fval, 100.0f);
    lv_svg_node_delete(svg_node_wh);

    const char * svg_wh2 = \
                           "<svg width=\"10cm\" height=\"100mm\"></svg>";
    lv_svg_node_t * svg_node_wh2 = lv_svg_load_data(svg_wh2, lv_strlen(svg_wh2));
    TEST_ASSERT_EQUAL_FLOAT((LV_ARRAY_GET(&svg_node_wh2->attrs, 0, lv_svg_attr_t))->value.fval, 377.9528f);
    TEST_ASSERT_EQUAL_FLOAT((LV_ARRAY_GET(&svg_node_wh2->attrs, 1, lv_svg_attr_t))->value.fval, 377.9528f);
    lv_svg_node_delete(svg_node_wh2);

    const char * svg_wh3 = \
                           "<svg width=\"10in\" height=\"10pc\"></svg>";
    lv_svg_node_t * svg_node_wh3 = lv_svg_load_data(svg_wh3, lv_strlen(svg_wh3));
    TEST_ASSERT_EQUAL_FLOAT((LV_ARRAY_GET(&svg_node_wh3->attrs, 0, lv_svg_attr_t))->value.fval, 960.0f);
    TEST_ASSERT_EQUAL_FLOAT((LV_ARRAY_GET(&svg_node_wh3->attrs, 1, lv_svg_attr_t))->value.fval, 160.0f);
    lv_svg_node_delete(svg_node_wh3);

    const char * svg_wh4 = \
                           "<svg width=\"10em\" height=\"10ex\"></svg>";
    lv_svg_node_t * svg_node_wh4 = lv_svg_load_data(svg_wh4, lv_strlen(svg_wh4));
    TEST_ASSERT_EQUAL_FLOAT((LV_ARRAY_GET(&svg_node_wh4->attrs, 0, lv_svg_attr_t))->value.fval, 160.0f);
    TEST_ASSERT_EQUAL_FLOAT((LV_ARRAY_GET(&svg_node_wh4->attrs, 1, lv_svg_attr_t))->value.fval, 83.2f);
    lv_svg_node_delete(svg_node_wh4);

    const char * svg_wh5 = \
                           "<svg width=\"10pt\" height=\"100%\"></svg>";
    lv_svg_node_t * svg_node_wh5 = lv_svg_load_data(svg_wh5, lv_strlen(svg_wh5));
    TEST_ASSERT_EQUAL_FLOAT((LV_ARRAY_GET(&svg_node_wh5->attrs, 0, lv_svg_attr_t))->value.fval, 13.3333f);
    TEST_ASSERT_EQUAL_FLOAT((LV_ARRAY_GET(&svg_node_wh5->attrs, 1, lv_svg_attr_t))->value.fval, 1.0f);
    lv_svg_node_delete(svg_node_wh5);

    const char * svg_wh6 = \
                           "<svg width=\"2048\" height=\"1023.549\"></svg>";
    lv_svg_node_t * svg_node_wh6 = lv_svg_load_data(svg_wh6, lv_strlen(svg_wh6));
    lv_svg_render_obj_t * draw_list = lv_svg_render_create(svg_node_wh6);
    float svg_wh6_w, svg_wh6_h;
    lv_svg_render_get_viewport_size(draw_list, &svg_wh6_w, &svg_wh6_h);
    TEST_ASSERT_EQUAL_FLOAT(svg_wh6_w, 2048.0f);
    TEST_ASSERT_EQUAL_FLOAT(svg_wh6_h, 1023.549f);
    lv_svg_render_delete(draw_list);
    lv_svg_node_delete(svg_node_wh6);

    /* preserveAspectRatio */

    const char * svg_ar0 = \
                           "<svg preserveAspectRatio=\"none meet\"></svg>";
    lv_svg_node_t * svg_node_ar = lv_svg_load_data(svg_ar0, lv_strlen(svg_ar0));
    TEST_ASSERT_EQUAL(lv_array_size(&svg_node_ar->attrs), 1);
    lv_svg_node_delete(svg_node_ar);

    const char * svg_ar1 = \
                           "<svg preserveAspectRatio=\"xMinYMin meet\"></svg>";
    svg_node_ar = lv_svg_load_data(svg_ar1, lv_strlen(svg_ar1));
    TEST_ASSERT_EQUAL((LV_ARRAY_GET(&svg_node_ar->attrs, 0, lv_svg_attr_t))->value.uval, 2);
    lv_svg_node_delete(svg_node_ar);

    const char * svg_ar2 = \
                           "<svg preserveAspectRatio=\"  xMidYMin slice\"></svg>";
    svg_node_ar = lv_svg_load_data(svg_ar2, lv_strlen(svg_ar2));
    TEST_ASSERT_EQUAL((LV_ARRAY_GET(&svg_node_ar->attrs, 0, lv_svg_attr_t))->value.uval, 5);
    lv_svg_node_delete(svg_node_ar);

    const char * svg_ar3 = \
                           "<svg preserveAspectRatio=\"xMaxYMin unknown\"></svg>";
    svg_node_ar = lv_svg_load_data(svg_ar3, lv_strlen(svg_ar3));
    TEST_ASSERT_EQUAL((LV_ARRAY_GET(&svg_node_ar->attrs, 0, lv_svg_attr_t))->value.uval, 6);
    lv_svg_node_delete(svg_node_ar);

    const char * svg_ar4 = \
                           "<svg preserveAspectRatio=\"xMinYMid meet\"></svg>";
    svg_node_ar = lv_svg_load_data(svg_ar4, lv_strlen(svg_ar4));
    TEST_ASSERT_EQUAL((LV_ARRAY_GET(&svg_node_ar->attrs, 0, lv_svg_attr_t))->value.uval, 8);
    lv_svg_node_delete(svg_node_ar);

    const char * svg_ar5 = \
                           "<svg preserveAspectRatio=\"xMidYMid\"></svg>";
    svg_node_ar = lv_svg_load_data(svg_ar5, lv_strlen(svg_ar5));
    TEST_ASSERT_EQUAL((LV_ARRAY_GET(&svg_node_ar->attrs, 0, lv_svg_attr_t))->value.uval, 10);
    lv_svg_node_delete(svg_node_ar);

    const char * svg_ar6 = \
                           "<svg preserveAspectRatio=\"xMaxYMid slice\"></svg>";
    svg_node_ar = lv_svg_load_data(svg_ar6, lv_strlen(svg_ar6));
    TEST_ASSERT_EQUAL((LV_ARRAY_GET(&svg_node_ar->attrs, 0, lv_svg_attr_t))->value.uval, 13);
    lv_svg_node_delete(svg_node_ar);

    const char * svg_ar7 = \
                           "<svg preserveAspectRatio=\"xMinYMax slice\"></svg>";
    svg_node_ar = lv_svg_load_data(svg_ar7, lv_strlen(svg_ar7));
    TEST_ASSERT_EQUAL((LV_ARRAY_GET(&svg_node_ar->attrs, 0, lv_svg_attr_t))->value.uval, 15);
    lv_svg_node_delete(svg_node_ar);

    const char * svg_ar8 = \
                           "<svg preserveAspectRatio=\"xMidYMax meet\"></svg>";
    svg_node_ar = lv_svg_load_data(svg_ar8, lv_strlen(svg_ar8));
    TEST_ASSERT_EQUAL((LV_ARRAY_GET(&svg_node_ar->attrs, 0, lv_svg_attr_t))->value.uval, 16);
    lv_svg_node_delete(svg_node_ar);

    const char * svg_ar9 = \
                           "<svg preserveAspectRatio=\"xMaxYMax meet\"></svg>";
    svg_node_ar = lv_svg_load_data(svg_ar9, lv_strlen(svg_ar9));
    TEST_ASSERT_EQUAL((LV_ARRAY_GET(&svg_node_ar->attrs, 0, lv_svg_attr_t))->value.uval, 18);
    lv_svg_node_delete(svg_node_ar);

    const char * svg_ar10 = \
                            "<svg preserveAspectRatio=\"unknown\"></svg>";
    svg_node_ar = lv_svg_load_data(svg_ar10, lv_strlen(svg_ar10));
    TEST_ASSERT_EQUAL(lv_array_size(&svg_node_ar->attrs), 1);
    lv_svg_node_delete(svg_node_ar);
}

void testPolylineElement(void)
{
    const char * svg_poly1 = \
                             "<svg><polyline points=\"100.0,50 200,150.0 180,110 200,200 210,340\"/></svg>";
    lv_svg_node_t * svg_node_root = lv_svg_load_data(svg_poly1, lv_strlen(svg_poly1));
    lv_svg_node_t * svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL(((lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val)->length,
                      5);
    lv_svg_node_delete(svg_node_root);

    const char * svg_poly2 = \
                             "<svg><polyline points=\"\n100.0,50\t200,150.0\n180,110\r \"/></svg>";
    svg_node_root = lv_svg_load_data(svg_poly2, lv_strlen(svg_poly2));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL(((lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val)->length,
                      3);
    lv_svg_node_delete(svg_node_root);

    const char * svg_poly3 = \
                             "<svg><polyline points=\"100.0 200,150.0 edf,err\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_poly3, lv_strlen(svg_poly3));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL(((lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val)->length,
                      1);
    lv_svg_node_delete(svg_node_root);

    const char * svg_poly4 = \
                             "<svg><polyline points=\"100.0\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_poly4, lv_strlen(svg_poly4));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL(((lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val)->length,
                      0);
    lv_svg_node_delete(svg_node_root);

    const char * svg_poly5 = \
                             "<svg><polyline points=\" \"/></svg>";
    svg_node_root = lv_svg_load_data(svg_poly5, lv_strlen(svg_poly5));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL(lv_array_size(&svg_node->attrs), 0);
    lv_svg_node_delete(svg_node_root);

}

void testPathElement(void)
{
    const char * svg_path1 = \
                             "<svg><path d=\" \"/></svg>";
    lv_svg_node_t * svg_node_root = lv_svg_load_data(svg_path1, lv_strlen(svg_path1));
    lv_svg_node_t * svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL(lv_array_size(&svg_node->attrs), 0);
    lv_svg_node_delete(svg_node_root);

    const char * svg_path2 = \
                             "<svg><path d=\"REt321\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_path2, lv_strlen(svg_path2));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL(((lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val)->length,
                      0);
    lv_svg_node_delete(svg_node_root);

    const char * svg_path3 = \
                             "<svg><path d=\"1223\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_path3, lv_strlen(svg_path3));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL(((lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val)->length,
                      0);
    lv_svg_node_delete(svg_node_root);

    const char * svg_path4 = \
                             "<svg><path d=\"m 100 200 150 180 L300 500 C 400 450 320.0 280.5 400 400 Z\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_path4, lv_strlen(svg_path4));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL(((lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val)->length,
                      5);
    lv_svg_node_delete(svg_node_root);

    const char * svg_path5 = \
                             "<svg><path d=\"M 100 200 300 500 L400 450 l 150 100 H 500 h 100 H 600 v 100 s 100 200 300 400\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_path5, lv_strlen(svg_path5));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL(((lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val)->length,
                      9);
    uint32_t seg_size = sizeof(uint32_t) + sizeof(lv_svg_point_t);
    lv_svg_attr_values_list_t * list = (lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0,
                                                                                  lv_svg_attr_t))->value.val;

    lv_svg_attr_path_value_t * path = (lv_svg_attr_path_value_t *)(&list->data);
    TEST_ASSERT_EQUAL(path->cmd, LV_SVG_PATH_CMD_MOVE_TO);

    path = (lv_svg_attr_path_value_t *)((uint8_t *)&list->data + seg_size);
    TEST_ASSERT_EQUAL(path->cmd, LV_SVG_PATH_CMD_LINE_TO);

    path = (lv_svg_attr_path_value_t *)((uint8_t *)&list->data + seg_size * 2);
    TEST_ASSERT_EQUAL(path->cmd, LV_SVG_PATH_CMD_LINE_TO);

    TEST_ASSERT_EQUAL_FLOAT(((lv_svg_point_t *)(&path->data))->x, 400.0f);
    TEST_ASSERT_EQUAL_FLOAT(((lv_svg_point_t *)(&path->data))->y, 450.0f);

    path = (lv_svg_attr_path_value_t *)((uint8_t *)&list->data + seg_size * 7);
    TEST_ASSERT_EQUAL(path->cmd, LV_SVG_PATH_CMD_LINE_TO);

    TEST_ASSERT_EQUAL_FLOAT(((lv_svg_point_t *)(&path->data))->x, 600.0f);
    TEST_ASSERT_EQUAL_FLOAT(((lv_svg_point_t *)(&path->data))->y, 650.0f);
    lv_svg_node_delete(svg_node_root);

    const char * svg_path6 = \
                             "<svg><path d=\"M 100 200 C300 500 400 450 320.0 280.5 c320 340 230 220 200 200"
                             " S400 400   450 450 s 500 450 550 550 q200 200 300 300 T400 400 l 450 450 480 520 t 500 500\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_path6, lv_strlen(svg_path6));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL(((lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val)->length,
                      10);
    lv_svg_node_delete(svg_node_root);
}

void testTransform(void)
{
    const char * svg_tr1 = \
                           "<svg><g transform=\" \"/></svg>";
    lv_svg_node_t * svg_node_root = lv_svg_load_data(svg_tr1, lv_strlen(svg_tr1));
    lv_svg_node_t * svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL(lv_array_size(&svg_node->attrs), 0);
    lv_svg_node_delete(svg_node_root);

    const char * svg_tr2 = \
                           "<svg><g transform=\"none\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_tr2, lv_strlen(svg_tr2));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL(lv_array_size(&svg_node->attrs), 1);
    lv_svg_node_delete(svg_node_root);

    const char * svg_tr3 = \
                           "<svg><g transform=\"matrix()\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_tr3, lv_strlen(svg_tr3));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    lv_svg_matrix_t * matrix = (lv_svg_matrix_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val;

    TEST_ASSERT_EQUAL_FLOAT(matrix->m[0][0], 1.0f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[1][1], 1.0f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[2][2], 1.0f);

    lv_svg_node_delete(svg_node_root);

    const char * svg_tr4 = \
                           "<svg><g transform=\"matrix(1.5, 0, 2.0, 2, 10, 20)\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_tr4, lv_strlen(svg_tr4));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    matrix = (lv_svg_matrix_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val;

    TEST_ASSERT_EQUAL_FLOAT(matrix->m[0][0], 1.5f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[1][0], 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[0][1], 2.0f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[1][1], 2.0f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[0][2], 10.0f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[1][2], 20.0f);

    lv_svg_node_delete(svg_node_root);

    const char * svg_tr5 = \
                           "<svg><g transform=\"translate(1, 2) translate(2.0, 2)\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_tr5, lv_strlen(svg_tr5));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    matrix = (lv_svg_matrix_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val;

    TEST_ASSERT_EQUAL_FLOAT(matrix->m[0][2], 3.0f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[1][2], 4.0f);

    lv_svg_node_delete(svg_node_root);

    const char * svg_tr6 = \
                           "<svg><g transform=\" scale(0.5) scale(0.5 0.5)\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_tr6, lv_strlen(svg_tr6));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    matrix = (lv_svg_matrix_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val;

    TEST_ASSERT_EQUAL_FLOAT(matrix->m[0][0], 0.25f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[1][1], 0.25f);

    lv_svg_node_delete(svg_node_root);

    const char * svg_tr7 = \
                           "<svg><g transform=\" translate(10 ) scale( 0.5  0.5) scale(0.5 ) \"/></svg>";
    svg_node_root = lv_svg_load_data(svg_tr7, lv_strlen(svg_tr7));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    matrix = (lv_svg_matrix_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val;

    TEST_ASSERT_EQUAL_FLOAT(matrix->m[0][2], 10.0f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[1][2], 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[0][0], 0.25f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[1][1], 0.25f);

    lv_svg_node_delete(svg_node_root);

    const char * svg_tr8 = \
                           "<svg><g transform=\" rotate(90 10 10) \"/></svg>";
    svg_node_root = lv_svg_load_data(svg_tr8, lv_strlen(svg_tr8));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    matrix = (lv_svg_matrix_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val;

    TEST_ASSERT_EQUAL_FLOAT(matrix->m[0][2], 20.0f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[1][2], 0.0f);

    lv_svg_node_delete(svg_node_root);

    const char * svg_tr9 = \
                           "<svg><g transform=\" rotate(90 ) \"/></svg>";
    svg_node_root = lv_svg_load_data(svg_tr9, lv_strlen(svg_tr9));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    matrix = (lv_svg_matrix_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val;

    TEST_ASSERT_EQUAL_FLOAT(matrix->m[0][2], 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[1][2], 0.0f);

    lv_svg_node_delete(svg_node_root);

    const char * svg_tr10 = \
                            "<svg><g transform=\" skewX(10) skewY(10) \"/></svg>";
    svg_node_root = lv_svg_load_data(svg_tr10, lv_strlen(svg_tr10));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    matrix = (lv_svg_matrix_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val;

    TEST_ASSERT_EQUAL_FLOAT(matrix->m[0][1], 0.176327f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[1][0], 0.176327f);

    lv_svg_node_delete(svg_node_root);
}

void testStrokeFill(void)
{
    const char * svg_sf1 = \
                           "<svg><g fill=\" \"/></svg>";
    lv_svg_node_t * svg_node_root = lv_svg_load_data(svg_sf1, lv_strlen(svg_sf1));
    lv_svg_node_t * svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL(lv_array_size(&svg_node->attrs), 0);
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf2 = \
                           "<svg><g fill=\"none\" stroke=\"inherit\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_sf2, lv_strlen(svg_sf2));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL(lv_array_size(&svg_node->attrs), 2);
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf3 = \
                           "<svg><g fill=\" url(#grad1) \"/></svg>";
    svg_node_root = lv_svg_load_data(svg_sf3, lv_strlen(svg_sf3));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    const char * str = (const char *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.sval;
    TEST_ASSERT_EQUAL_STRING(str, "grad1");
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf4 = \
                           "<svg><g fill=\" url( # grad2 ) \"/></svg>";
    svg_node_root = lv_svg_load_data(svg_sf4, lv_strlen(svg_sf4));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    str = (const char *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.sval;
    TEST_ASSERT_EQUAL_STRING(str, "");
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf5 = \
                           "<svg><g fill=\" url( #grad2 ) \"/></svg>";
    svg_node_root = lv_svg_load_data(svg_sf5, lv_strlen(svg_sf5));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    str = (const char *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.sval;
    TEST_ASSERT_EQUAL_STRING(str, "grad2");
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf6 = \
                           "<svg><g fill=\" url (#grad2) \"/></svg>";
    svg_node_root = lv_svg_load_data(svg_sf6, lv_strlen(svg_sf6));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    uint32_t c = (LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.uval;
    TEST_ASSERT_EQUAL(c, 0);
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf7 = \
                           "<svg><g fill=\"rgb(255, 255, 255)\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_sf7, lv_strlen(svg_sf7));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    c = (LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.uval;
    TEST_ASSERT_EQUAL(c, 0xffffff);
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf8 = \
                           "<svg><g fill=\"rgb(50%, 50%, 50%)\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_sf8, lv_strlen(svg_sf8));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    c = (LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.uval;
    TEST_ASSERT_EQUAL(c, 0x808080);
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf9 = \
                           "<svg><g fill=\"#F00\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_sf9, lv_strlen(svg_sf9));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    c = (LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.uval;
    TEST_ASSERT_EQUAL(c, 0xff0000);
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf10 = \
                            "<svg><g fill=\"#FF8000\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_sf10, lv_strlen(svg_sf10));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    c = (LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.uval;
    TEST_ASSERT_EQUAL(c, 0xff8000);
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf11 = \
                            "<svg><g fill=\"red\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_sf11, lv_strlen(svg_sf11));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    c = (LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.uval;
    TEST_ASSERT_EQUAL(c, 0xff0000);
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf12 = \
                            "<svg><g fill=\"rgba(255, 255, 255, 1.0)\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_sf12, lv_strlen(svg_sf12));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    c = (LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.uval;
    TEST_ASSERT_EQUAL(c, 0xffffffff);
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf13 = \
                            "<svg><g fill=\"rgba(255, 255, 255, 128)\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_sf13, lv_strlen(svg_sf13));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    c = (LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.uval;
    TEST_ASSERT_EQUAL(c, 0x80ffffff);
    lv_svg_node_delete(svg_node_root);
}

void testStrokeFillAttrs(void)
{
    const char * svg_sf0 = \
                           "<svg><g fill-rule=\'\'/></svg>";
    lv_svg_node_t * svg_node_root = lv_svg_load_data(svg_sf0, lv_strlen(svg_sf0));
    lv_svg_node_t * svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL(lv_array_size(&svg_node->attrs), 0);
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf1 = \
                           "<svg><g stroke-width=1 stroke-miterlimit=3 fill-rule=\'inherit\' /></svg>";
    svg_node_root = lv_svg_load_data(svg_sf1, lv_strlen(svg_sf1));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    float f1 = (LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.fval;
    TEST_ASSERT_EQUAL_FLOAT(f1, 1.0f);
    uint32_t f2 = (LV_ARRAY_GET(&svg_node->attrs, 1, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(f2, 3);
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf2 = \
                           "<svg><g fill-rule=\"evenodd\" stroke-width=\'-1\' stroke-miterlimit=\'-5.0\'/></svg>";
    svg_node_root = lv_svg_load_data(svg_sf2, lv_strlen(svg_sf2));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    uint32_t r1 = (LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(r1, LV_SVG_FILL_EVENODD);
    float w1 = (LV_ARRAY_GET(&svg_node->attrs, 1, lv_svg_attr_t))->value.fval;
    TEST_ASSERT_EQUAL_FLOAT(w1, 0.0f);
    uint32_t l1 = (LV_ARRAY_GET(&svg_node->attrs, 2, lv_svg_attr_t))->value.uval;
    TEST_ASSERT_EQUAL(l1, 1);
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf3 = \
                           "<svg><g stroke-linecap=\"round\" stroke-linejoin=\' bevel\' fill-rule=nonzero/></svg>";
    svg_node_root = lv_svg_load_data(svg_sf3, lv_strlen(svg_sf3));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    uint32_t c1 = (LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(c1, LV_SVG_LINE_CAP_ROUND);
    uint32_t c2 = (LV_ARRAY_GET(&svg_node->attrs, 1, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(c2, LV_SVG_LINE_JOIN_BEVEL);
    uint32_t r2 = (LV_ARRAY_GET(&svg_node->attrs, 2, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(r2, LV_SVG_FILL_NONZERO);
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf4 = \
                           "<svg><g stroke-linecap=\" square \" stroke-linejoin=round/></svg>";
    svg_node_root = lv_svg_load_data(svg_sf4, lv_strlen(svg_sf4));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    uint32_t c3 = (LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(c3, LV_SVG_LINE_CAP_SQUARE);
    uint32_t c4 = (LV_ARRAY_GET(&svg_node->attrs, 1, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(c4, LV_SVG_LINE_JOIN_ROUND);
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf5 = \
                           "<svg><g stroke-linecap=\"a\" stroke-linejoin=\'b\' stroke-dasharray=\"none\" stroke-opacity=\"inherit\"/></svg>";
    svg_node_root = lv_svg_load_data(svg_sf5, lv_strlen(svg_sf5));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    uint32_t c5 = (LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(c5, LV_SVG_LINE_CAP_BUTT);
    uint32_t c6 = (LV_ARRAY_GET(&svg_node->attrs, 1, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(c6, LV_SVG_LINE_JOIN_MITER);
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf6 = \
                           "<svg><g stroke-dasharray=\"1,2,3, 2.5, 3 \" stroke-dashoffset=1.2"
                           " fill-opacity=\"2.0\" stroke-dasharray=\"inherit\" /></svg>";
    svg_node_root = lv_svg_load_data(svg_sf6, lv_strlen(svg_sf6));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    lv_svg_attr_values_list_t * list = (lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0,
                                                                                  lv_svg_attr_t))->value.val;
    TEST_ASSERT_EQUAL(list->length, 5);
    float * arr = (float *)(&list->data);
    float ret[5] = {1.0f, 2.0f, 3.0f, 2.5f, 3.0f};
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(ret, arr, 5);

    float c8 = (LV_ARRAY_GET(&svg_node->attrs, 1, lv_svg_attr_t))->value.fval;
    TEST_ASSERT_EQUAL_FLOAT(c8, 1.2f);
    float c9 = (LV_ARRAY_GET(&svg_node->attrs, 2, lv_svg_attr_t))->value.fval;
    TEST_ASSERT_EQUAL_FLOAT(c9, 1.0f);
    lv_svg_node_delete(svg_node_root);
}

void testTextAttrs(void)
{
    const char * svg_sf0 = \
                           "<svg><text font-size=\'16\' font-family=\"arial\" font-variant=inherit>hello world!</text></svg>";
    lv_svg_node_t * svg_node_root = lv_svg_load_data(svg_sf0, lv_strlen(svg_sf0));
    lv_svg_node_t * svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);

    float font_size = (LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.fval;
    TEST_ASSERT_EQUAL_FLOAT(font_size, 16.0f);
    const char * font_family = (LV_ARRAY_GET(&svg_node->attrs, 1, lv_svg_attr_t))->value.sval;
    TEST_ASSERT_EQUAL_STRING(font_family, "arial");
    lv_svg_node_t * svg_node1 = LV_SVG_NODE_CHILD(svg_node, 0);
    const char * content = svg_node1->xml_id;
    TEST_ASSERT_EQUAL_STRING(content, "hello world!");
    lv_svg_node_delete(svg_node_root);

    const char * svg_sf1 = \
                           "<svg><text font-size=\'16em\' font=\'user\' font-style=\" italic \" >hello<tspan>my\n</tspan>world!</text></svg>";
    svg_node_root = lv_svg_load_data(svg_sf1, lv_strlen(svg_sf1));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);

    font_size = (LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.fval;
    TEST_ASSERT_EQUAL_FLOAT(font_size, 256.0f);
    const char * font_style = (LV_ARRAY_GET(&svg_node->attrs, 1, lv_svg_attr_t))->value.sval;
    TEST_ASSERT_EQUAL_STRING(font_style, "italic ");
    svg_node1 = LV_SVG_NODE_CHILD(svg_node, 0);
    const char * content1 = svg_node1->xml_id;
    TEST_ASSERT_EQUAL_STRING(content1, "hello");
    svg_node1 = LV_SVG_NODE_CHILD(svg_node, 2);
    const char * content2 = svg_node1->xml_id;
    TEST_ASSERT_EQUAL_STRING(content2, "world!");
    svg_node1 = LV_SVG_NODE_CHILD(svg_node, 1);
    lv_svg_node_t * svg_node2 = LV_SVG_NODE_CHILD(svg_node1, 0);
    const char * content3 = svg_node2->xml_id;
    TEST_ASSERT_EQUAL_STRING(content3, "my");
    lv_svg_node_delete(svg_node_root);
}

void testGradient(void)
{
    const char * svg_gt1 = \
                           "<svg><linearGradient id=\"gt1\" gradientUnits= objectBoundingBox>"
                           "<stop stop-color='red' offset=0.1/>"
                           "<stop stop-color=\'black\' stop-opacity=\"0.5\" offset=1.0/>"
                           "</linearGradient></svg>";
    lv_svg_node_t * svg_node_root = lv_svg_load_data(svg_gt1, lv_strlen(svg_gt1));
    lv_svg_node_t * svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_EQUAL_STRING(svg_node->xml_id, "gt1");
    lv_svg_node_t * svg_node1 = LV_SVG_NODE_CHILD(svg_node, 1);
    uint32_t c1 = (LV_ARRAY_GET(&svg_node1->attrs, 0, lv_svg_attr_t))->value.uval;
    TEST_ASSERT_EQUAL(c1, 0);
    float o1 = (LV_ARRAY_GET(&svg_node1->attrs, 1, lv_svg_attr_t))->value.fval;
    TEST_ASSERT_EQUAL_FLOAT(o1, 0.5f);
    float o2 = (LV_ARRAY_GET(&svg_node1->attrs, 2, lv_svg_attr_t))->value.fval;
    TEST_ASSERT_EQUAL_FLOAT(o2, 1.0f);
    lv_svg_node_delete(svg_node_root);

    const char * svg_gt2 = \
                           "<svg><defs><radialGradient xml:id=\"gt2\" gradientUnits=\"userSpaceOnUse\""
                           "cx=\"400\" cy=\"200\" r=\"300\">"
                           "<stop offset=\"0\" stop-color=\"red\"/>"
                           "<stop offset=\"0.5\" stop-color=\"blue\"/>"
                           "<stop offset=\"1\" stop-color=\"red\"/>"
                           "</radialGradient></defs></svg>";
    svg_node_root = lv_svg_load_data(svg_gt2, lv_strlen(svg_gt2));
    svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0); //defs
    svg_node1 = LV_SVG_NODE_CHILD(svg_node, 0);
    TEST_ASSERT_EQUAL_STRING(svg_node1->xml_id, "gt2");
    uint32_t g = (LV_ARRAY_GET(&svg_node1->attrs, 0, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(g, LV_SVG_GRADIENT_UNITS_USER_SPACE);
    float cx = (LV_ARRAY_GET(&svg_node1->attrs, 1, lv_svg_attr_t))->value.fval;
    TEST_ASSERT_EQUAL_FLOAT(cx, 400.0f);
    float cy = (LV_ARRAY_GET(&svg_node1->attrs, 2, lv_svg_attr_t))->value.fval;
    TEST_ASSERT_EQUAL_FLOAT(cy, 200.0f);
    lv_svg_node_delete(svg_node_root);
}

void testBadCase(void)
{
    const char * svg_b1 = \
                          "<rect x=10 y=10 width=100 height=100/>";
    lv_svg_node_t * svg = lv_svg_load_data(svg_b1, lv_strlen(svg_b1));
    TEST_ASSERT_EQUAL(NULL, svg);
    lv_svg_node_delete(svg);

    const char * svg_b2 = \
                          "<svg><text><rect x=10 y=10 width=200 height=200/></svg>";
    svg = lv_svg_load_data(svg_b2, lv_strlen(svg_b2));
    TEST_ASSERT_EQUAL(NULL, svg);
    lv_svg_node_delete(svg);

    const char * svg_b3 = \
                          "<svg><rect x=10 y=10 width=200 height/></svg>";
    svg = lv_svg_load_data(svg_b3, lv_strlen(svg_b3));
    TEST_ASSERT_EQUAL(NULL, svg);
    lv_svg_node_delete(svg);

    const char * svg_b4 = \
                          "<svg><g fill=\"url( \"/></svg>";
    svg = lv_svg_load_data(svg_b4, lv_strlen(svg_b4));
    lv_svg_node_t * svg_node = LV_SVG_NODE_CHILD(svg, 0);
    TEST_ASSERT_EQUAL(0, LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t)->class_type);
    lv_svg_node_delete(svg);

    const char * svg_b5 = \
                          "<svg><g transform=matrix/></svg>";
    svg = lv_svg_load_data(svg_b5, lv_strlen(svg_b5));
    svg_node = LV_SVG_NODE_CHILD(svg, 0);
    lv_svg_matrix_t * matrix = (lv_svg_matrix_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val;

    TEST_ASSERT_EQUAL_FLOAT(matrix->m[0][0], 1.0f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[1][1], 1.0f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[2][2], 1.0f);
    lv_svg_node_delete(svg);

    const char * svg_b6 = \
                          "<svg><123><123></svg>";
    svg = lv_svg_load_data(svg_b6, lv_strlen(svg_b6));
    TEST_ASSERT_EQUAL(NULL, svg);
    lv_svg_node_delete(svg);

    const char * svg_b7 = \
                          "<svg><my> bad case <you></svg>";
    svg = lv_svg_load_data(svg_b7, lv_strlen(svg_b7));
    TEST_ASSERT_EQUAL(NULL, svg);
    lv_svg_node_delete(svg);

    const char * svg_b8 = \
                          "<svg><path d=\"M 100  L150 180 L 150 Z\"/></svg>";
    svg = lv_svg_load_data(svg_b8, lv_strlen(svg_b8));
    svg_node = LV_SVG_NODE_CHILD(svg, 0);
    TEST_ASSERT_EQUAL(((lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val)->length,
                      3);
    uint32_t seg_size = sizeof(uint32_t) + sizeof(lv_svg_point_t);
    lv_svg_attr_values_list_t * list = (lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0,
                                                                                  lv_svg_attr_t))->value.val;

    lv_svg_attr_path_value_t * path = (lv_svg_attr_path_value_t *)(&list->data);
    TEST_ASSERT_EQUAL(path->cmd, LV_SVG_PATH_CMD_MOVE_TO);

    TEST_ASSERT_EQUAL_FLOAT(((lv_svg_point_t *)(&path->data))->x, 100.0f);
    TEST_ASSERT_EQUAL_FLOAT(((lv_svg_point_t *)(&path->data))->y, 150.0f);

    path = (lv_svg_attr_path_value_t *)((uint8_t *)&list->data + seg_size);
    TEST_ASSERT_EQUAL(path->cmd, LV_SVG_PATH_CMD_LINE_TO);

    TEST_ASSERT_EQUAL_FLOAT(((lv_svg_point_t *)(&path->data))->x, 180.0f);
    TEST_ASSERT_EQUAL_FLOAT(((lv_svg_point_t *)(&path->data))->y, 150.0f);
    lv_svg_node_delete(svg);

    const char * svg_b9 = \
                          "<svg><path d=\"M 100 200 L150 Z\"/></svg>";
    svg = lv_svg_load_data(svg_b9, lv_strlen(svg_b9));
    svg_node = LV_SVG_NODE_CHILD(svg, 0);
    TEST_ASSERT_EQUAL(((lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val)->length,
                      1);
    seg_size = sizeof(uint32_t) + sizeof(lv_svg_point_t);
    list = (lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val;

    path = (lv_svg_attr_path_value_t *)(&list->data);
    TEST_ASSERT_EQUAL(path->cmd, LV_SVG_PATH_CMD_MOVE_TO);

    TEST_ASSERT_EQUAL_FLOAT(((lv_svg_point_t *)(&path->data))->x, 100.0f);
    TEST_ASSERT_EQUAL_FLOAT(((lv_svg_point_t *)(&path->data))->y, 200.0f);
    lv_svg_node_delete(svg);

    const char * svg_b10 = \
                           "<svg></text>bad case</text></svg>";
    svg = lv_svg_load_data(svg_b10, lv_strlen(svg_b10));
    TEST_ASSERT_EQUAL(NULL, svg);
    lv_svg_node_delete(svg);

    const char * svg_b11 = \
                           "<svg><text font-size></text></svg>";
    svg = lv_svg_load_data(svg_b11, lv_strlen(svg_b11));
    svg_node = LV_SVG_NODE_CHILD(svg, 0);
    TEST_ASSERT_EQUAL(lv_array_size(&svg_node->attrs), 0);
    TEST_ASSERT_EQUAL(LV_TREE_NODE(svg_node)->child_cnt, 0);
    lv_svg_node_delete(svg);

    const char * svg_b12 = \
                           "<svg><text font-size=></text></svg>";
    svg = lv_svg_load_data(svg_b12, lv_strlen(svg_b12));
    svg_node = LV_SVG_NODE_CHILD(svg, 0);
    TEST_ASSERT_EQUAL(lv_array_size(&svg_node->attrs), 0);
    TEST_ASSERT_EQUAL(LV_TREE_NODE(svg_node)->child_cnt, 0);
    lv_svg_node_delete(svg);

    const char * svg_b13 = \
                           "<svg><!-aaaa /></svg>";
    svg = lv_svg_load_data(svg_b13, lv_strlen(svg_b13));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    lv_svg_node_delete(svg);

    const char * svg_b14 = \
                           "<svg><rect"
                           "x=1 y=1 width=10 height=10"
                           "x=1 y=1 width=10 height=10"
                           "x=1 y=1 width=10 height=10"
                           " /></svg>";
    svg = lv_svg_load_data(svg_b14, lv_strlen(svg_b14));
    TEST_ASSERT_NOT_EQUAL(NULL, svg);
    lv_svg_node_delete(svg);

    const char * svg_b15 = \
                           "<svg>"
                           "<path d=\'m-122.3,84.285s0.1,1.894-0.73,1.875c-0.82-0.019-17.27-48.094-37.8-45.851,0,0,17.78-7.353,38.53,43.976z\'/>"
                           "</svg>";
    svg = lv_svg_load_data(svg_b15, lv_strlen(svg_b15));
    svg_node = LV_SVG_NODE_CHILD(svg, 0);

    TEST_ASSERT_EQUAL(((lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val)->length,
                      5);
    seg_size = sizeof(uint32_t) + sizeof(lv_svg_point_t);
    list = (lv_svg_attr_values_list_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val;

    path = (lv_svg_attr_path_value_t *)(&list->data);
    TEST_ASSERT_EQUAL(path->cmd, LV_SVG_PATH_CMD_MOVE_TO);

    TEST_ASSERT_EQUAL_FLOAT(((lv_svg_point_t *)(&path->data))->x, -122.3f);
    TEST_ASSERT_EQUAL_FLOAT(((lv_svg_point_t *)(&path->data))->y, 84.285f);

    lv_svg_node_delete(svg);

    const char * svg_b16 = \
                           "<svg>"
                           "<g transform=\'matrix(1.7656463,0,0,1.7656463,324.90716,255.00942)\'>"
                           "</g>"
                           "</svg>";
    svg = lv_svg_load_data(svg_b16, lv_strlen(svg_b16));
    svg_node = LV_SVG_NODE_CHILD(svg, 0);
    matrix = (lv_svg_matrix_t *)(LV_ARRAY_GET(&svg_node->attrs, 0, lv_svg_attr_t))->value.val;

    TEST_ASSERT_EQUAL_FLOAT(matrix->m[0][0], 1.7656463f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[1][0], 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[0][1], 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[1][1], 1.7656463f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[0][2], 324.90716f);
    TEST_ASSERT_EQUAL_FLOAT(matrix->m[1][2], 255.00942f);

    lv_svg_node_delete(svg);
}

#endif
