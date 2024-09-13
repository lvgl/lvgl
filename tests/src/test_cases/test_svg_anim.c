#if LV_BUILD_TEST
#include "../lvgl.h"

#include "unity/unity.h"

#define LV_ARRAY_GET(array, index, type) ((type*)lv_array_at((array), (index)))

void setUp(void)
{
}

void tearDown(void)
{
}

void testAnimate(void)
{
    const char * svg_anim0 = \
                             "<svg><rect xml:id=\"RectElement\" x=\"300\" y=\"100\" width=\"300\" height=\"100\">"
                             "<animate attributeName=\"x\" dur=\"9s\" fill=\"freeze\" from=\"300\" to=\"0\"/>"
                             "</rect></svg>";

    lv_svg_node_t * svg_node_root = lv_svg_load_data(svg_anim0, lv_strlen(svg_anim0));
    lv_svg_node_t * svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    lv_svg_node_t * anim_node = LV_SVG_NODE_CHILD(svg_node, 0);
    TEST_ASSERT_NOT_EQUAL(NULL, anim_node);
    TEST_ASSERT_EQUAL(LV_SVG_TAG_ANIMATE, anim_node->type);
    lv_svg_attr_type_t at = (LV_ARRAY_GET(&anim_node->attrs, 0, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(LV_SVG_ATTR_X, at);
    float dur = (LV_ARRAY_GET(&anim_node->attrs, 1, lv_svg_attr_t))->value.fval;
    TEST_ASSERT_EQUAL_FLOAT(dur, 9000.0f);

    int ft = (LV_ARRAY_GET(&anim_node->attrs, 2, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(ft, LV_SVG_ANIM_FREEZE);

    float fr = (LV_ARRAY_GET(&anim_node->attrs, 3, lv_svg_attr_t))->value.fval;
    TEST_ASSERT_EQUAL_FLOAT(fr, 300.0f);

    float to = (LV_ARRAY_GET(&anim_node->attrs, 4, lv_svg_attr_t))->value.fval;
    TEST_ASSERT_EQUAL_FLOAT(to, 0.0f);

    lv_svg_node_delete(svg_node_root);
}

void testSet(void)
{
    const char * svg_anim0 = \
                             "<svg><rect xml:id=\"RectElement\" x=\"300\" y=\"100\" width=\"300\" height=\"100\">"
                             "<set attributeName=\"x\" to=\"500\" values=\"0\"/>"
                             "</rect></svg>";

    lv_svg_node_t * svg_node_root = lv_svg_load_data(svg_anim0, lv_strlen(svg_anim0));
    lv_svg_node_t * svg_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    lv_svg_node_t * anim_node = LV_SVG_NODE_CHILD(svg_node, 0);
    TEST_ASSERT_NOT_EQUAL(NULL, anim_node);
    TEST_ASSERT_EQUAL(LV_SVG_TAG_SET, anim_node->type);
    lv_svg_attr_type_t at = (LV_ARRAY_GET(&anim_node->attrs, 0, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(LV_SVG_ATTR_X, at);
    float to = (LV_ARRAY_GET(&anim_node->attrs, 1, lv_svg_attr_t))->value.fval;
    TEST_ASSERT_EQUAL_FLOAT(to, 500.0f);

    lv_svg_node_delete(svg_node_root);
}

void testAnimateMotion(void)
{
    const char * svg_anim0 = \
                             "<svg><path xml:id=\"path1\" d=\"M100,250 C 100,50 400,50 400,250\" "
                             "fill=\"none\" stroke=\"blue\" stroke-width=\"7.06\"/>"
                             "<animateMotion dur=\"6s\" repeatCount=\"indefinite\" rotate=\"auto\">"
                             "<mpath xlink:href=\"#path1\"/>"
                             "</animateMotion>"
                             "</svg>";

    lv_svg_node_t * svg_node_root = lv_svg_load_data(svg_anim0, lv_strlen(svg_anim0));
    lv_svg_node_t * path_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_NOT_EQUAL(NULL, path_node);
    lv_svg_node_t * anim_node = LV_SVG_NODE_CHILD(svg_node_root, 1);
    TEST_ASSERT_NOT_EQUAL(NULL, anim_node);
    TEST_ASSERT_EQUAL(LV_SVG_TAG_ANIMATE_MOTION, anim_node->type);

    lv_svg_node_t * mpath_node = LV_SVG_NODE_CHILD(anim_node, 0);
    TEST_ASSERT_NOT_EQUAL(NULL, mpath_node);
    TEST_ASSERT_EQUAL(LV_SVG_TAG_MPATH, mpath_node->type);

    const char * xlink = (LV_ARRAY_GET(&mpath_node->attrs, 0, lv_svg_attr_t))->value.sval;
    TEST_ASSERT_EQUAL_STRING(xlink, path_node->xml_id);

    uint32_t rp = (LV_ARRAY_GET(&anim_node->attrs, 1, lv_svg_attr_t))->value.uval;
    TEST_ASSERT_EQUAL(rp, 0);

    float rt = (LV_ARRAY_GET(&anim_node->attrs, 2, lv_svg_attr_t))->value.fval;
    TEST_ASSERT_EQUAL_FLOAT(rt, 0.0f);
    lv_svg_node_delete(svg_node_root);

    const char * svg_anim1 = \
                             "<svg><circle r=\"5\" fill=\"blue\">"
                             "<animateMotion begin=\"500ms\" dur=\"3.1s\" calcMode=\"linear\" keyPoints=\"0.5; 0.8; 1.0\" path=\"M15,43 C15,43 36,20 65,33\"/>"
                             "</circle>"
                             "</svg>";

    svg_node_root = lv_svg_load_data(svg_anim1, lv_strlen(svg_anim1));
    path_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    anim_node = LV_SVG_NODE_CHILD(path_node, 0);
    TEST_ASSERT_NOT_EQUAL(NULL, anim_node);
    TEST_ASSERT_EQUAL(LV_SVG_TAG_ANIMATE_MOTION, anim_node->type);

    lv_svg_attr_values_list_t * lb = (LV_ARRAY_GET(&anim_node->attrs, 0, lv_svg_attr_t))->value.val;
    TEST_ASSERT_EQUAL(lb->length, 1);
    float * fb = (float *)(&lb->data);
    TEST_ASSERT_EQUAL_FLOAT(*fb, 500.0f);

    float dr = (LV_ARRAY_GET(&anim_node->attrs, 1, lv_svg_attr_t))->value.fval;
    TEST_ASSERT_EQUAL(dr, 3100.0f);

    int cm = (LV_ARRAY_GET(&anim_node->attrs, 2, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(cm, LV_SVG_ANIM_CALC_MODE_LINEAR);

    lv_svg_attr_values_list_t * l = (LV_ARRAY_GET(&anim_node->attrs, 3, lv_svg_attr_t))->value.val;
    TEST_ASSERT_EQUAL(l->length, 3);

    float * pt = (float *)(&l->data);
    TEST_ASSERT_EQUAL_FLOAT(pt[0], 0.5f);
    TEST_ASSERT_EQUAL_FLOAT(pt[1], 0.8f);
    TEST_ASSERT_EQUAL_FLOAT(pt[2], 1.0f);

    lv_svg_attr_values_list_t * lp = (LV_ARRAY_GET(&anim_node->attrs, 4, lv_svg_attr_t))->value.val;
    TEST_ASSERT_EQUAL(lp->length, 2);
    lv_svg_node_delete(svg_node_root);

    const char * svg_anim2 = \
                             "<svg><circle r=\"5\" fill=\"blue\">"
                             "<animateMotion begin=\"5s;2s\" end=\"8s;10s\" values=\"100, 50;200 200\" keyTimes=\"100ms;200ms\""
                             " keySplines=\"0 0 1.5 1.0; 0.5 0.5, 2.0,1.5\" additive=\"sum\" accumulate=\"none\"/>"
                             "</circle>"
                             "</svg>";

    svg_node_root = lv_svg_load_data(svg_anim2, lv_strlen(svg_anim2));
    path_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    anim_node = LV_SVG_NODE_CHILD(path_node, 0);
    TEST_ASSERT_NOT_EQUAL(NULL, anim_node);
    TEST_ASSERT_EQUAL(LV_SVG_TAG_ANIMATE_MOTION, anim_node->type);

    lb = (LV_ARRAY_GET(&anim_node->attrs, 0, lv_svg_attr_t))->value.val;
    TEST_ASSERT_EQUAL(lb->length, 2);
    fb = (float *)(&lb->data);
    TEST_ASSERT_EQUAL_FLOAT(fb[0], 5000.0f);
    TEST_ASSERT_EQUAL_FLOAT(fb[1], 2000.0f);

    lb = (LV_ARRAY_GET(&anim_node->attrs, 1, lv_svg_attr_t))->value.val;
    TEST_ASSERT_EQUAL(lb->length, 2);
    fb = (float *)(&lb->data);
    TEST_ASSERT_EQUAL_FLOAT(fb[0], 8000.0f);
    TEST_ASSERT_EQUAL_FLOAT(fb[1], 10000.0f);

    lb = (LV_ARRAY_GET(&anim_node->attrs, 2, lv_svg_attr_t))->value.val;
    TEST_ASSERT_EQUAL(lb->length, 2);
    lv_svg_point_t * ps = (lv_svg_point_t *)(&lb->data);
    TEST_ASSERT_EQUAL_FLOAT(ps[0].x, 100.0f);
    TEST_ASSERT_EQUAL_FLOAT(ps[0].y, 50.0f);
    TEST_ASSERT_EQUAL_FLOAT(ps[1].x, 200.0f);
    TEST_ASSERT_EQUAL_FLOAT(ps[1].y, 200.0f);

    lb = (LV_ARRAY_GET(&anim_node->attrs, 3, lv_svg_attr_t))->value.val;
    TEST_ASSERT_EQUAL(lb->length, 2);
    fb = (float *)(&lb->data);
    TEST_ASSERT_EQUAL_FLOAT(fb[0], 100.0f);
    TEST_ASSERT_EQUAL_FLOAT(fb[1], 200.0f);

    l = (LV_ARRAY_GET(&anim_node->attrs, 4, lv_svg_attr_t))->value.val;
    TEST_ASSERT_EQUAL(l->length, 4);

    ps = (lv_svg_point_t *)(&l->data);
    TEST_ASSERT_EQUAL_FLOAT(ps[0].x, 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(ps[0].y, 0.0f);
    TEST_ASSERT_EQUAL_FLOAT(ps[1].x, 1.5f);
    TEST_ASSERT_EQUAL_FLOAT(ps[1].y, 1.0f);
    TEST_ASSERT_EQUAL_FLOAT(ps[2].x, 0.5f);
    TEST_ASSERT_EQUAL_FLOAT(ps[2].y, 0.5f);
    TEST_ASSERT_EQUAL_FLOAT(ps[3].x, 2.0f);
    TEST_ASSERT_EQUAL_FLOAT(ps[3].y, 1.5f);

    lv_svg_node_delete(svg_node_root);
}

void testAnimateTransform(void)
{
    const char * svg_anim0 = \
                             "<svg><rect transform=\"skewX(30)\" x=0 y=0 width=100 height=100>"
                             "<animateTransform attributeName=\"transform\" attributeType=\"XML\""
                             "type=\"rotate\" from=\"0\" to=\"90\" dur=\"5s\""
                             "additive=\"sum\" fill=\"freeze\"/>"
                             "<animateTransform attributeName=\"transform\" attributeType=\"XML\""
                             "type=\"scale\" from=\"1\" to=\"2\" dur=\"5s\" values=\"0.5; 0.2, 0.2\""
                             "additive=\"sum\" fill=\"freeze\"/>"
                             "</rect></svg>";

    lv_svg_node_t * svg_node_root = lv_svg_load_data(svg_anim0, lv_strlen(svg_anim0));
    lv_svg_node_t * path_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_NOT_EQUAL(NULL, path_node);
    lv_svg_node_t * anim_node1 = LV_SVG_NODE_CHILD(path_node, 0);
    TEST_ASSERT_NOT_EQUAL(NULL, anim_node1);
    TEST_ASSERT_EQUAL(LV_SVG_TAG_ANIMATE_TRANSFORM, anim_node1->type);
    lv_svg_node_t * anim_node2 = LV_SVG_NODE_CHILD(path_node, 1);
    TEST_ASSERT_NOT_EQUAL(NULL, anim_node2);
    TEST_ASSERT_EQUAL(LV_SVG_TAG_ANIMATE_TRANSFORM, anim_node2->type);

    lv_svg_attr_type_t at = (LV_ARRAY_GET(&anim_node1->attrs, 0, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(LV_SVG_ATTR_TRANSFORM, at);

    lv_svg_transform_type_t tt = (LV_ARRAY_GET(&anim_node1->attrs, 1, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(LV_SVG_TRANSFORM_TYPE_ROTATE, tt);

    lv_svg_attr_values_list_t * l = (LV_ARRAY_GET(&anim_node1->attrs, 2, lv_svg_attr_t))->value.val;
    TEST_ASSERT_EQUAL(l->length, 1);
    float * pt = (float *)(&l->data);
    TEST_ASSERT_EQUAL_FLOAT(pt[0], 0.0f);

    l = (LV_ARRAY_GET(&anim_node2->attrs, 5, lv_svg_attr_t))->value.val;
    TEST_ASSERT_EQUAL(l->length, 2);
    lv_svg_attr_values_list_t * ll = (lv_svg_attr_values_list_t *)(&l->data);
    TEST_ASSERT_EQUAL(ll->length, 1);
    pt = (float *)(&ll->data);
    TEST_ASSERT_EQUAL_FLOAT(pt[0], 0.5f);

    ll = (lv_svg_attr_values_list_t *)((uint8_t *)(&l->data) + sizeof(uint32_t) + sizeof(float) * 4);
    TEST_ASSERT_EQUAL(ll->length, 2);
    pt = (float *)(&ll->data);
    TEST_ASSERT_EQUAL_FLOAT(pt[0], 0.2f);
    TEST_ASSERT_EQUAL_FLOAT(pt[1], 0.2f);

    lv_svg_node_delete(svg_node_root);
}

void testAnimateColor(void)
{
    const char * svg_anim0 = \
                             "<svg><rect color=\"yellow\" fill=\"black\">"
                             "<animateColor attributeName=\"fill\" from=\"red\" to=\"#DDF\" "
                             "begin=\"1s\" dur=\"5s\" fill=\"freeze\" additive=\"sum\" repeatCount=5 restart=\"whenNotActive\" values=\"rgb(0,255,0);black\" />"
                             "</rect></svg>";

    lv_svg_node_t * svg_node_root = lv_svg_load_data(svg_anim0, lv_strlen(svg_anim0));
    lv_svg_node_t * path_node = LV_SVG_NODE_CHILD(svg_node_root, 0);
    TEST_ASSERT_NOT_EQUAL(NULL, path_node);
    lv_svg_node_t * anim_node = LV_SVG_NODE_CHILD(path_node, 0);
    TEST_ASSERT_NOT_EQUAL(NULL, anim_node);
    TEST_ASSERT_EQUAL(LV_SVG_TAG_ANIMATE_COLOR, anim_node->type);

    lv_svg_attr_type_t at = (LV_ARRAY_GET(&anim_node->attrs, 0, lv_svg_attr_t))->value.ival;
    TEST_ASSERT_EQUAL(LV_SVG_ATTR_FILL, at);

    uint32_t c = (LV_ARRAY_GET(&anim_node->attrs, 1, lv_svg_attr_t))->value.uval;
    TEST_ASSERT_EQUAL(c, 0xFF0000);

    c = (LV_ARRAY_GET(&anim_node->attrs, 2, lv_svg_attr_t))->value.uval;
    TEST_ASSERT_EQUAL(c, 0xDDDDFF);

    lv_svg_attr_values_list_t * l = (LV_ARRAY_GET(&anim_node->attrs, 9, lv_svg_attr_t))->value.val;
    TEST_ASSERT_EQUAL(l->length, 2);
    uint32_t * pc = (uint32_t *)(&l->data);
    TEST_ASSERT_EQUAL(pc[0], 0x00FF00);
    TEST_ASSERT_EQUAL(pc[1], 0x000000);

    lv_svg_node_delete(svg_node_root);
}
#endif
