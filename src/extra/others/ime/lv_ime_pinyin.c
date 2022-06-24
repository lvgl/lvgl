/**
 * @file lv_ime_pinyin.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_ime_pinyin.h"
#if LV_USE_IME_PINYIN != 0

#include <stdio.h>

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS    &lv_ime_pinyin_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_ime_pinyin_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_ime_pinyin_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_ime_pinyin_style_change_event(lv_event_t * e);
static void lv_ime_pinyin_kb_event(lv_event_t * e);
static void lv_ime_pinyin_cand_panel_event(lv_event_t * e);

static void init_pinyin_dict(lv_obj_t * obj, lv_pinyin_dict_t * dict);
static void pinyin_input_proc(lv_obj_t * obj);
static void pinyin_page_proc(lv_obj_t * obj, uint16_t btn);
static char * pinyin_search_matching(lv_obj_t * obj, char * strInput_py_str, uint16_t * cand_num);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_ime_pinyin_class = {
    .constructor_cb = lv_ime_pinyin_constructor,
    .destructor_cb  = lv_ime_pinyin_destructor,
    .width_def      = LV_SIZE_CONTENT,
    .height_def     = LV_SIZE_CONTENT,
    .group_def      = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .instance_size  = sizeof(lv_ime_pinyin_t),
    .base_class     = &lv_obj_class
};

static char   lv_pinyin_cand_str[LV_IME_PINYIN_CAND_TEXT_NUM][4];
static char * lv_btnm_def_pinyin_sel_map[LV_IME_PINYIN_CAND_TEXT_NUM + 3];

#if LV_IME_PINYIN_USE_DEFAULT_DICT
lv_pinyin_dict_t lv_ime_pinyin_def_dict[] = {
    { "a", "啊" },
    { "ai", "愛" },
    { "an", "安暗案" },
    { "ba", "吧把爸八" },
    { "bai", "百白敗" },
    { "ban", "半般辦" },
    { "bang", "旁" },
    { "bao", "保薄包報" },
    { "bei", "被背悲北杯備" },
    { "ben", "本" },
    { "bi", "必比避鼻彼筆秘閉" },
    { "bian", "便邊變変辺" },
    { "biao", "表標" },
    { "bie", "別" },
    { "bing", "病並氷" },
    { "bo", "波薄泊" },
    { "bu", "不布步部捕補歩" },
    { "ca", "察" },
    { "cai", "才材菜財採" },
    { "can", "参残參" },
    { "ce", "策側" },
    { "ceng", "曾" },
    { "cha", "差查茶" },
    { "chai", "差" },
    { "chan", "產産單" },
    { "chang", "場廠" },
    { "chao", "超朝" },
    { "che", "車" },
    { "cheng", "成程乗" },
    { "chi", "尺吃持赤池遅歯" },
    { "chong", "充种重種" },
    { "chu", "出初楚触處処" },
    { "chuan", "川船傳" },
    { "chuang", "創窓" },
    { "chun", "春" },
    { "ci", "此次辞差" },
    { "cong", "從従" },
    { "cu", "卒" },
    { "cun", "存村" },
    { "cuo", "錯" },
    { "da", "大打答達" },
    { "dai", "代待帯帶貸" },
    { "dan", "但担擔誕單単" },
    { "dang", "当党當黨" },
    { "dao", "到道盗導島辺" },
    { "de", "的得" },
    { "dei", "" },
    { "deng", "等" },
    { "di", "地得低底弟第締" },
    { "dian", "点电店點電" },
    { "diao", "調" },
    { "ding", "定町" },
    { "dong", "冬東動働凍" },
    { "du", "独度都渡読" },
    { "duan", "段断短斷" },
    { "dui", "對対" },
    { "duo", "多駄" },
    { "e", "嗯悪" },
    { "en", "嗯" },
    { "er", "而耳二兒" },
    { "fa", "乏法發発髪" },
    { "fan", "反返犯番仮販飯範払" },
    { "fang", "方放房坊訪" },
    { "fei", "非飛費" },
    { "fen", "分份" },
    { "feng", "風豐" },
    { "fou", "否不" },
    { "fu", "父夫富服符付附府幅婦復複負払" },
    { "gai", "改概該" },
    { "gan", "甘感敢" },
    { "gang", "港剛" },
    { "gao", "告高" },
    { "ge", "各格歌革割個" },
    { "gei", "給" },
    { "gen", "跟根" },
    { "geng", "更" },
    { "gong", "工共供功公" },
    { "gou", "夠構溝" },
    { "gu", "古故鼓" },
    { "guai", "掛" },
    { "guan", "官管慣館觀関關" },
    { "guang", "光広" },
    { "gui", "規帰" },
    { "guo", "果国裏菓國過" },
    { "hai", "孩海害還" },
    { "han", "寒漢" },
    { "hang", "航行" },
    { "hao", "好号" },
    { "he", "合和喝何荷" },
    { "hei", "黒" },
    { "hen", "很" },
    { "heng", "行横" },
    { "hou", "厚喉候後" },
    { "hu", "乎呼湖護" },
    { "hua", "化画花話畫劃" },
    { "huai", "壊劃" },
    { "huan", "緩環歡還換" },
    { "huang", "黄" },
    { "hui", "回会慧絵揮會" },
    { "hun", "混婚" },
    { "huo", "活或火獲" },
    { "i", "" },
    { "ji", "己计及机既急季寄技即集基祭系奇紀積計記済幾際極繼績機濟" },
    { "jia", "家加價" },
    { "jian", "件建健肩見減間検簡漸" },
    { "jiang", "降強講將港" },
    { "jiao", "叫教交角覚覺較學" },
    { "jie", "介借接姐皆届界解結階節價" },
    { "jin", "今近禁金僅進" },
    { "jing", "京境景静精經経" },
    { "jiu", "就久九酒究" },
    { "ju", "句具局居決挙據舉" },
    { "jue", "角覚覺" },
    { "jun", "均" },
    { "kai", "開" },
    { "kan", "看刊" },
    { "kang", "康" },
    { "kao", "考" },
    { "ke", "可刻科克客渇課" },
    { "ken", "肯" },
    { "kong", "空控" },
    { "kou", "口" },
    { "ku", "苦庫" },
    { "kuai", "快塊会會" },
    { "kuang", "況" },
    { "kun", "困" },
    { "kuo", "括拡適" },
    { "la", "拉啦落" },
    { "lai", "来來頼" },
    { "lao", "老絡落" },
    { "le", "了楽樂" },
    { "lei", "類" },
    { "leng", "冷" },
    { "li", "力立利理例礼離麗裡勵歷" },
    { "lian", "連練臉聯" },
    { "liang", "良量涼兩両" },
    { "liao", "料" },
    { "lie", "列" },
    { "lin", "林隣賃" },
    { "ling", "另令領" },
    { "liu", "六留流" },
    { "lu", "律路録緑陸履慮" },
    { "lv", "旅" },
    { "lun", "輪論" },
    { "luo", "落絡" },
    { "ma", "媽嗎嘛" },
    { "mai", "買売" },
    { "man", "滿" },
    { "mang", "忙" },
    { "mao", "毛猫貿" },
    { "me", "麼" },
    { "mei", "美妹每沒毎媒" },
    { "men", "們" },
    { "mi", "米密秘" },
    { "mian", "免面勉眠" },
    { "miao", "描" },
    { "min", "民皿" },
    { "ming", "命明名" },
    { "mo", "末模麼" },
    { "mou", "某" },
    { "mu", "母木目模" },
    { "na", "那哪拿內南" },
    { "nan", "男南難" },
    { "nao", "腦" },
    { "ne", "那哪呢" },
    { "nei", "内那哪內" },
    { "neng", "能" },
    { "ni", "你妳呢" },
    { "nian", "年念" },
    { "niang", "娘" },
    { "nin", "您" },
    { "ning", "凝" },
    { "niu", "牛" },
    { "nong", "農濃" },
    { "nu", "女努" },
    { "nuan", "暖" },
    { "o", "" },
    { "ou", "歐" },
    { "pa", "怕" },
    { "pian", "片便" },
    { "pai", "迫派排" },
    { "pan", "判番" },
    { "pang", "旁" },
    { "pei", "配" },
    { "peng", "朋" },
    { "pi", "疲否" },
    { "pin", "品貧" },
    { "ping", "平評" },
    { "po", "迫破泊頗" },
    { "pu", "普僕" },
    { "qi", "起其奇七气期泣企妻契気" },
    { "qian", "嵌浅千前鉛錢針" },
    { "qiang", "強將" },
    { "qiao", "橋繰" },
    { "qie", "且切契" },
    { "qin", "寝勤親" },
    { "qing", "青清情晴輕頃請軽" },
    { "qiu", "求秋球" },
    { "qu", "去取趣曲區" },
    { "quan", "全犬券" },
    { "que", "缺確卻" },
    { "ran", "然" },
    { "rang", "讓" },
    { "re", "熱" },
    { "ren", "人任認" },
    { "reng", "仍" },
    { "ri", "日" },
    { "rong", "容" },
    { "rou", "弱若肉" },
    { "ru", "如入" },
    { "ruan", "軟" },
    { "sai", "賽" },
    { "san", "三" },
    { "sao", "騒繰" },
    { "se", "色" },
    { "sen", "森" },
    { "sha", "砂" },
    { "shan", "善山單" },
    { "shang", "上尚商" },
    { "shao", "少紹" },
    { "shaung", "雙" },
    { "she", "社射設捨渉" },
    { "shei", "誰" },
    { "shen", "什申深甚身伸沈神" },
    { "sheng", "生声昇勝乗聲" },
    { "shi", "是失示食时事式十石施使世实史室市始柿氏士仕拭時視師試適実實識" },
    { "shou", "手首守受授" },
    { "shu", "束数暑殊樹書屬輸術" },
    { "shui", "水説說誰" },
    { "shuo", "数説說" },
    { "si", "思寺司四私似死価" },
    { "song", "送" },
    { "su", "速宿素蘇訴" },
    { "suan", "算酸" },
    { "sui", "隨雖歲歳" },
    { "sun", "孫" },
    { "suo", "所" },
    { "ta", "她他它牠" },
    { "tai", "太台態臺" },
    { "tan", "探談曇" },
    { "tang", "糖" },
    { "tao", "桃逃套討" },
    { "te", "特" },
    { "ti", "体提替題體戻" },
    { "tian", "天田" },
    { "tiao", "条條調" },
    { "tie", "鉄" },
    { "ting", "停庭聽町" },
    { "tong", "同童通痛统統" },
    { "tou", "投透頭" },
    { "tu", "土徒茶図" },
    { "tuan", "團" },
    { "tui", "推退" },
    { "tuo", "脱駄" },
    { "u", "" },
    { "v", "" },
    { "wai", "外" },
    { "wan", "完万玩晩腕灣" },
    { "wang", "忘望亡往網" },
    { "wei", "危位未味委為謂維違圍" },
    { "wen", "文温問聞" },
    { "wo", "我" },
    { "wu", "午物五無屋亡鳥務汚" },
    { "xi", "夕息西洗喜系昔席希析嬉膝細習係" },
    { "xia", "下夏狭暇" },
    { "xian", "先限嫌洗現見線顯" },
    { "xiang", "向相香像想象降項詳響" },
    { "xiao", "小笑消效校削咲" },
    { "xie", "写携些解邪械協謝寫契" },
    { "xin", "心信新辛" },
    { "xing", "行形性幸型星興" },
    { "xiong", "兄胸" },
    { "xiu", "休秀修" },
    { "xu", "須需許續緒続" },
    { "xuan", "選懸" },
    { "xue", "学雪削靴學" },
    { "xun", "訓訊" },
    { "ya", "呀押壓" },
    { "yan", "言顔研煙嚴厳験驗塩" },
    { "yang", "央洋陽樣様" },
    { "yao", "要揺腰薬曜" },
    { "ye", "也野夜邪業葉" },
    { "yi", "一已亦依以移意医易伊役異億義議藝醫訳" },
    { "yin", "因引音飲銀" },
    { "ying", "英迎影映應營営" },
    { "yong", "永用泳擁" },
    { "you", "又有右友由尤油遊郵誘優" },
    { "yu", "予育余雨浴欲愈御宇域語於魚與込" },
    { "yuan", "元原源院員円園遠猿願" },
    { "yue", "月越約楽" },
    { "yun", "雲伝運" },
    { "za", "雑" },
    { "zai", "在再載災" },
    { "zang", "蔵" },
    { "zao", "早造" },
    { "ze", "則擇責" },
    { "zen", "怎" },
    { "zeng", "曾增増" },
    { "zha", "札" },
    { "zhai", "宅擇" },
    { "zhan", "站展戰戦" },
    { "zhang", "丈長障帳張" },
    { "zhao", "找着朝招" },
    { "zhe", "者這" },
    { "zhen", "真震針" },
    { "zheng", "正整争政爭" },
    { "zhi", "之只知支止制至治直指值置智値紙製質誌織隻識職執" },
    { "zhong", "中种終重種眾" },
    { "zhou", "周州昼宙洲週" },
    { "zhu", "助主住柱株祝逐注著諸屬術" },
    { "zhuan", "专專転" },
    { "zhuang", "状狀" },
    { "zhui", "追" },
    { "zhun", "準" },
    { "zhuo", "着" },
    { "zi", "子自字姉資" },
    { "zong", "總" },
    { "zuo", "左做昨坐座作" },
    { "zu", "足祖族卒組" },
    { "zui", "最酔" },
    { "zou", "走" },
    {NULL, NULL}
};
#endif


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t * lv_ime_pinyin_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}


/*=====================
 * Setter functions
 *====================*/

/**
 * Set the keyboard of Pinyin input method.
 * @param obj  pointer to a Pinyin input method object
 * @param dict pointer to a Pinyin input method keyboard
 */
void lv_ime_pinyin_set_keyboard(lv_obj_t * obj, lv_obj_t * kb)
{
    if(kb) {
        LV_ASSERT_OBJ(kb, &lv_keyboard_class);
    }

    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_ime_pinyin_t * pinyin_ime = (lv_ime_pinyin_t *)obj;

    pinyin_ime->kb = kb;
    lv_obj_add_event_cb(pinyin_ime->kb, lv_ime_pinyin_kb_event, LV_EVENT_VALUE_CHANGED, obj);
    lv_obj_align_to(pinyin_ime->cand_panel, pinyin_ime->kb, LV_ALIGN_OUT_TOP_MID, 0, 0);
}

/**
 * Set the dictionary of Pinyin input method.
 * @param obj  pointer to a Pinyin input method object
 * @param dict pointer to a Pinyin input method dictionary
 */
void lv_ime_pinyin_set_dict(lv_obj_t * obj, lv_pinyin_dict_t * dict)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    init_pinyin_dict(obj, dict);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Set the dictionary of Pinyin input method.
 * @param obj  pointer to a Pinyin IME object
 * @return     pointer to the Pinyin IME keyboard
 */
lv_obj_t * lv_ime_pinyin_get_kb(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_ime_pinyin_t * pinyin_ime = (lv_ime_pinyin_t *)obj;

    return pinyin_ime->kb;
}

/**
 * Set the dictionary of Pinyin input method.
 * @param obj  pointer to a Pinyin input method object
 * @return     pointer to the Pinyin input method candidate panel
 */
lv_obj_t * lv_ime_pinyin_get_cand_panel(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_ime_pinyin_t * pinyin_ime = (lv_ime_pinyin_t *)obj;

    return pinyin_ime->cand_panel;
}

/**
 * Set the dictionary of Pinyin input method.
 * @param obj  pointer to a Pinyin input method object
 * @return     pointer to the Pinyin input method dictionary
 */
lv_pinyin_dict_t * lv_ime_pinyin_get_dict(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_ime_pinyin_t * pinyin_ime = (lv_ime_pinyin_t *)obj;

    return pinyin_ime->dict;
}

/*=====================
 * Other functions
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_ime_pinyin_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_ime_pinyin_t * pinyin_ime = (lv_ime_pinyin_t *)obj;

    uint16_t py_str_i = 0;
    for(uint16_t btnm_i = 0; btnm_i < (LV_IME_PINYIN_CAND_TEXT_NUM + 3); btnm_i++) {
        if(btnm_i == 0) {
            lv_btnm_def_pinyin_sel_map[btnm_i] = "<";
        }
        else if(btnm_i == (LV_IME_PINYIN_CAND_TEXT_NUM + 1)) {
            lv_btnm_def_pinyin_sel_map[btnm_i] = ">";
        }
        else if(btnm_i == (LV_IME_PINYIN_CAND_TEXT_NUM + 2)) {
            lv_btnm_def_pinyin_sel_map[btnm_i] = "";
        }
        else {
            lv_pinyin_cand_str[py_str_i][0] = ' ';
            lv_btnm_def_pinyin_sel_map[btnm_i] = lv_pinyin_cand_str[py_str_i];
            py_str_i++;
        }
    }

    pinyin_ime->py_page = 0;
    pinyin_ime->ta_count = 0;
    pinyin_ime->cand_num = 0;
    lv_memset_00(pinyin_ime->input_char, sizeof(pinyin_ime->input_char));
    lv_memset_00(pinyin_ime->py_num, sizeof(pinyin_ime->py_num));
    lv_memset_00(pinyin_ime->py_pos, sizeof(pinyin_ime->py_pos));

    lv_obj_set_size(obj, LV_PCT(100), LV_PCT(55));
    lv_obj_align(obj, LV_ALIGN_BOTTOM_MID, 0, 0);

#if LV_IME_PINYIN_USE_DEFAULT_DICT
    init_pinyin_dict(obj, lv_ime_pinyin_def_dict);
#endif

    /* Init pinyin_ime->cand_panel */
    pinyin_ime->cand_panel = lv_btnmatrix_create(lv_scr_act());
    lv_btnmatrix_set_map(pinyin_ime->cand_panel, (const char **)lv_btnm_def_pinyin_sel_map);
    lv_obj_set_size(pinyin_ime->cand_panel, LV_PCT(100), LV_PCT(5));
    lv_obj_add_flag(pinyin_ime->cand_panel, LV_OBJ_FLAG_HIDDEN);

    lv_btnmatrix_set_one_checked(pinyin_ime->cand_panel, true);

    /* Set cand_panel style*/
    // Default style
    lv_obj_set_style_bg_opa(pinyin_ime->cand_panel, LV_OPA_0, 0);
    lv_obj_set_style_border_width(pinyin_ime->cand_panel, 0, 0);
    lv_obj_set_style_pad_all(pinyin_ime->cand_panel, 8, 0);
    lv_obj_set_style_pad_gap(pinyin_ime->cand_panel, 0, 0);
    lv_obj_set_style_radius(pinyin_ime->cand_panel, 0, 0);
    lv_obj_set_style_pad_gap(pinyin_ime->cand_panel, 0, 0);
    lv_obj_set_style_base_dir(pinyin_ime->cand_panel, LV_BASE_DIR_LTR, 0);

    // LV_PART_ITEMS style
    lv_obj_set_style_radius(pinyin_ime->cand_panel, 12, LV_PART_ITEMS);
    lv_obj_set_style_bg_color(pinyin_ime->cand_panel, lv_color_white(), LV_PART_ITEMS);
    lv_obj_set_style_bg_opa(pinyin_ime->cand_panel, LV_OPA_0, LV_PART_ITEMS);
    lv_obj_set_style_shadow_opa(pinyin_ime->cand_panel, LV_OPA_0, LV_PART_ITEMS);

    // LV_PART_ITEMS | LV_STATE_PRESSED style
    lv_obj_set_style_bg_opa(pinyin_ime->cand_panel, LV_OPA_COVER, LV_PART_ITEMS | LV_STATE_PRESSED);
    lv_obj_set_style_bg_color(pinyin_ime->cand_panel, lv_color_white(), LV_PART_ITEMS | LV_STATE_PRESSED);

    /* event handler */
    lv_obj_add_event_cb(pinyin_ime->cand_panel, lv_ime_pinyin_cand_panel_event, LV_EVENT_VALUE_CHANGED, obj);
    lv_obj_add_event_cb(obj, lv_ime_pinyin_style_change_event, LV_EVENT_STYLE_CHANGED, NULL);
}


static void lv_ime_pinyin_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_ime_pinyin_t * pinyin_ime = (lv_ime_pinyin_t *)obj;

    if(lv_obj_is_valid(pinyin_ime->kb))
        lv_obj_del(pinyin_ime->kb);

    if(lv_obj_is_valid(pinyin_ime->cand_panel))
        lv_obj_del(pinyin_ime->cand_panel);
}


static void lv_ime_pinyin_kb_event(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * kb = lv_event_get_target(e);
    lv_obj_t * obj = lv_event_get_user_data(e);

    lv_ime_pinyin_t * pinyin_ime = (lv_ime_pinyin_t *)obj;

    if(code == LV_EVENT_VALUE_CHANGED) {
        uint16_t btn_id  = lv_btnmatrix_get_selected_btn(kb);
        if(btn_id == LV_BTNMATRIX_BTN_NONE) return;

        const char * txt = lv_btnmatrix_get_btn_text(kb, lv_btnmatrix_get_selected_btn(kb));
        if(txt == NULL) return;

        if(strcmp(txt, "Enter") == 0 || strcmp(txt, LV_SYMBOL_NEW_LINE) == 0) {
            lv_memset_00(lv_pinyin_cand_str, (sizeof(lv_pinyin_cand_str)));
            pinyin_ime->ta_count = 0;
            lv_memset_00(pinyin_ime->input_char, sizeof(pinyin_ime->input_char));
        }
        else if(strcmp(txt, LV_SYMBOL_BACKSPACE) == 0) {
            // del input char
            for(int i = strlen(pinyin_ime->input_char) - 1; i >= 0; i--) {
                if(pinyin_ime->input_char[i] != '\0') {
                    pinyin_ime->input_char[i] = '\0';
                    lv_obj_add_flag(pinyin_ime->cand_panel, LV_OBJ_FLAG_HIDDEN);
                    break;
                }
            }
            pinyin_input_proc(obj);
            pinyin_ime->ta_count--;
        }
        else if((strcmp(txt, "ABC") == 0) || (strcmp(txt, "abc") == 0) || (strcmp(txt, "1#") == 0)) {
            pinyin_ime->ta_count = 0;
            lv_memset_00(pinyin_ime->input_char, sizeof(pinyin_ime->input_char));
            return;
        }
        else if((strcmp(txt, LV_SYMBOL_KEYBOARD) == 0) || (strcmp(txt, LV_SYMBOL_OK) == 0)) {
            pinyin_ime->ta_count = 0;
            lv_memset_00(pinyin_ime->input_char, sizeof(pinyin_ime->input_char));
            lv_obj_add_flag(pinyin_ime->cand_panel, LV_OBJ_FLAG_HIDDEN);
        }
        else if((txt[0] >= 'a' && txt[0] <= 'z') || (txt[0] >= 'A' && txt[0] <= 'Z')) {
            strcat(pinyin_ime->input_char, txt);
            pinyin_input_proc(obj);
            pinyin_ime->ta_count++;
        }
    }
}


static void lv_ime_pinyin_cand_panel_event(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * cand_panel = lv_event_get_target(e);
    lv_obj_t * obj = (lv_obj_t *)lv_event_get_user_data(e);

    lv_ime_pinyin_t * pinyin_ime = (lv_ime_pinyin_t *)obj;

    if(code == LV_EVENT_VALUE_CHANGED) {
        uint32_t id = lv_btnmatrix_get_selected_btn(cand_panel);
        if(id == 0) {
            pinyin_page_proc(obj, 0);
            return;
        }
        if(id == (LV_IME_PINYIN_CAND_TEXT_NUM + 1)) {
            pinyin_page_proc(obj, 1);
            return;
        }

        const char * txt = lv_btnmatrix_get_btn_text(cand_panel, id);
        lv_obj_t * ta = lv_keyboard_get_textarea(pinyin_ime->kb);

        for(int i = 0; i < pinyin_ime->ta_count; i++) {
            lv_textarea_del_char(ta);
        }

        lv_textarea_add_text(ta, txt);

        pinyin_ime->ta_count = 0;
        lv_memset_00(lv_pinyin_cand_str, (sizeof(lv_pinyin_cand_str)));
        lv_memset_00(pinyin_ime->input_char, sizeof(pinyin_ime->input_char));
        lv_obj_add_flag(pinyin_ime->cand_panel, LV_OBJ_FLAG_HIDDEN);
    }
}


static void pinyin_input_proc(lv_obj_t * obj)
{
    lv_ime_pinyin_t * pinyin_ime = (lv_ime_pinyin_t *)obj;

    pinyin_ime->cand_str = pinyin_search_matching(obj, pinyin_ime->input_char, &pinyin_ime->cand_num);
    if(pinyin_ime->cand_str == NULL) {
        return;
    }

    pinyin_ime->py_page = 0;

    for(uint8_t i = 0; i < LV_IME_PINYIN_CAND_TEXT_NUM; i++) {
        memset(lv_pinyin_cand_str[i], 0x00, sizeof(lv_pinyin_cand_str[i]));
        lv_pinyin_cand_str[i][0] = ' ';
    }

    // fill buf
    for(uint8_t i = 0; (i < pinyin_ime->cand_num && i < LV_IME_PINYIN_CAND_TEXT_NUM); i++) {
        for(uint8_t j = 0; j < 3; j++) {
            lv_pinyin_cand_str[i][j] = pinyin_ime->cand_str[i * 3 + j];
        }
    }

    lv_obj_clear_flag(pinyin_ime->cand_panel, LV_OBJ_FLAG_HIDDEN);
}

static void pinyin_page_proc(lv_obj_t * obj, uint16_t dir)
{
    lv_ime_pinyin_t * pinyin_ime = (lv_ime_pinyin_t *)obj;
    uint16_t page_num = pinyin_ime->cand_num / LV_IME_PINYIN_CAND_TEXT_NUM;
    uint16_t sur = pinyin_ime->cand_num % LV_IME_PINYIN_CAND_TEXT_NUM;

    if(dir == 0) {
        if(pinyin_ime->py_page) {
            pinyin_ime->py_page--;
        }
    }
    else {
        if(sur == 0) {
            page_num -= 1;
        }
        if(pinyin_ime->py_page < page_num) {
            pinyin_ime->py_page++;
        }
        else return;
    }

    for(uint8_t i = 0; i < LV_IME_PINYIN_CAND_TEXT_NUM; i++) {
        memset(lv_pinyin_cand_str[i], 0x00, sizeof(lv_pinyin_cand_str[i]));
        lv_pinyin_cand_str[i][0] = ' ';
    }

    // fill buf
    uint16_t offset = pinyin_ime->py_page * (3 * LV_IME_PINYIN_CAND_TEXT_NUM);
    for(uint8_t i = 0; (i < pinyin_ime->cand_num && i < LV_IME_PINYIN_CAND_TEXT_NUM); i++) {
        if((sur > 0) && (pinyin_ime->py_page == page_num)) {
            if(i > sur)
                break;
        }
        for(uint8_t j = 0; j < 3; j++) {
            lv_pinyin_cand_str[i][j] = pinyin_ime->cand_str[offset + (i * 3) + j];
        }
    }
}


static void lv_ime_pinyin_style_change_event(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    lv_ime_pinyin_t * pinyin_ime = (lv_ime_pinyin_t *)obj;

    if(code == LV_EVENT_STYLE_CHANGED) {
        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        lv_obj_set_style_text_font(pinyin_ime->cand_panel, font, 0);
    }
}


static void init_pinyin_dict(lv_obj_t * obj, lv_pinyin_dict_t * dict)
{
    lv_ime_pinyin_t * pinyin_ime = (lv_ime_pinyin_t *)obj;

    char headletter = 'a';
    uint16_t offset_sum = 0;
    uint16_t offset_count = 0;
    uint16_t letter_calc = 0;

    pinyin_ime->dict = dict;

    for(uint16_t i = 0; ; i++) {
        if((NULL == (dict[i].py)) || (NULL == (dict[i].py_mb))) {
            headletter = dict[i - 1].py[0];
            letter_calc = headletter - 'a';
            pinyin_ime->py_num[letter_calc] = offset_count;
            break;
        }

        if(headletter == (dict[i].py[0])) {
            offset_count++;
        }
        else {
            headletter = dict[i].py[0];
            letter_calc = headletter - 'a';
            pinyin_ime->py_num[letter_calc - 1] = offset_count;
            offset_sum += offset_count;
            pinyin_ime->py_pos[letter_calc] = offset_sum;

            offset_count = 1;
        }
    }
}


static char * pinyin_search_matching(lv_obj_t * obj, char * strInput_py_str, uint16_t * cand_num)
{
    lv_ime_pinyin_t * pinyin_ime = (lv_ime_pinyin_t *)obj;

    lv_pinyin_dict_t * cpHZ;
    uint8_t i, cInputStrLength = 0, offset;
    volatile uint8_t count = 0;

    if(*strInput_py_str == '\0')    return NULL;
    if(*strInput_py_str == 'i')     return NULL;
    if(*strInput_py_str == 'u')     return NULL;
    if(*strInput_py_str == 'v')     return NULL;

    offset = strInput_py_str[0] - 'a';
    cInputStrLength = strlen(strInput_py_str);

    cpHZ  = &pinyin_ime->dict[pinyin_ime->py_pos[offset]];
    count = pinyin_ime->py_num[offset];

    while(count--) {
        for(i = 0; i < cInputStrLength; i++) {
            if(*(strInput_py_str + i) != *((cpHZ->py) + i)) {
                break;
            }
        }

        // perfect match
        if(cInputStrLength == 1 || i == cInputStrLength) {
            // The Chinese character in UTF-8 encoding format is 3 bytes
            * cand_num = strlen((const char *)(cpHZ->py_mb)) / 3;
            return (char *)(cpHZ->py_mb);
        }
        cpHZ++;
    }
    return NULL;
}


#endif  /*LV_USE_IME_PINYIN*/
