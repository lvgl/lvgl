/**
 * @file lv_pinyin_ime.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_pinyin_ime.h"
#if LV_USE_PINYIN_IME != 0

#include <stdio.h>

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS    &lv_pinyin_ime_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_pinyin_ime_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_pinyin_ime_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_pinyin_ime_style_change_event(lv_event_t *e);
static void lv_pinyin_ime_kb_event(lv_event_t * e);
static void lv_pinyin_ime_cand_panel_event(lv_event_t * e);

static void init_pinyin_dict(lv_obj_t * obj, lv_pinyin_dict_t * dict);
static void pinyin_input_proc(lv_obj_t * obj);
static void pinyin_page_proc(lv_obj_t * obj, uint16_t btn);
static char * pinyin_search_matching(lv_obj_t * obj, char * strInput_py_str, uint16_t * cand_num);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_pinyin_ime_class = {
    .constructor_cb = lv_pinyin_ime_constructor,
    .destructor_cb  = lv_pinyin_ime_destructor,
    .width_def      = LV_PCT(100),
    .height_def     = LV_PCT(50),
    .group_def      = LV_OBJ_CLASS_GROUP_DEF_TRUE,
    .instance_size  = sizeof(lv_pinyin_ime_t),
    .base_class     = &lv_obj_class
};

static char   lv_pinyin_cand_str[LV_PINYIN_IME_CAND_TEXT_NUM][4];
static char * lv_btnm_def_pinyin_sel_map[LV_PINYIN_IME_CAND_TEXT_NUM + 3];

#if LV_PINYIN_IME_USE_DEFAULT_DICT
lv_pinyin_dict_t lv_pinyin_def_dict[] = {
            { "a", "啊阿呵吖" },
            { "ai", "埃挨哎唉哀皑蔼矮碍爱隘癌艾" },
            { "an", "按安暗岸俺案鞍氨胺厂广庵揞犴铵桉谙鹌埯黯" },
            { "ang", "昂肮盎仰" },
            { "ao", "凹敖熬翱袄傲奥懊澳" },
            { "ba", "芭捌叭吧笆八疤巴拔跋靶把坝霸罢爸扒耙" },
            { "bai", "白摆佰败拜柏百稗伯" },
            { "ban", "斑班搬扳颁板版扮拌伴瓣半办绊般" },
            { "bang", "邦帮梆榜绑棒镑傍谤膀磅蚌" },
            { "bao", "苞胞包褒雹保饱宝抱报豹鲍爆剥薄暴刨炮曝瀑堡" },
            { "bei", "杯碑悲卑北辈背贝钡倍狈备惫焙被" },
            { "ben", "奔苯本笨" },
            { "beng", "崩绷甭泵蹦迸蚌" },
            { "bi", "逼鼻比鄙笔彼碧蓖蔽毕毙毖币庇痹闭敝弊必壁避陛辟臂秘" },
            { "bian", "鞭边编贬变卞辨辩辫扁便遍" },
            { "biao", "标彪膘表" },
            { "bie", "鳖憋别瘪" },
            { "bin", "彬斌濒滨宾摈" },
            { "bing", "兵冰柄丙秉饼炳病并屏" },
            { "bo", "玻菠播拨钵博勃搏铂箔帛舶脖膊渤驳柏剥薄波泊卜般伯" },
            { "bu", "捕哺补埠布步簿部怖卜不埔堡" },
            { "ca", "擦" },
            { "cai", "猜裁材才财睬踩采彩菜蔡" },
            { "can", "餐残惭惨灿蚕参掺" },
            { "cang", "苍舱仓沧藏" },
            { "cao", "操糙槽曹草" },
            { "ce", "策册测厕侧" },
            { "cen", "参" },
            { "ceng", "层蹭曾" },
            { "cha", "插叉茶碴搽察岔诧茬查刹喳差" },
            { "chai", "柴豺拆差" },
            { "chan", "搀蝉馋谗缠铲产阐颤掺单" },
            { "chang", "昌猖场尝常偿肠厂畅唱倡长敞裳" },
            { "chao", "超抄钞潮巢吵炒朝嘲绰剿" },
            { "che", "扯撤掣彻澈车" },
            { "chen", "郴臣辰尘晨忱沉陈趁衬橙沈称秤" },
            { "cheng", "撑城成呈程惩诚承逞骋橙乘澄盛称秤" },
            { "chi", "痴持池迟弛驰耻齿侈赤翅斥炽吃匙尺" },
            { "chong", "充冲崇宠虫重" },
            { "chou", "抽酬畴踌稠愁筹仇绸瞅丑臭" },
            { "chu", "初出橱厨躇锄雏滁除楚础储矗搐触处畜" },
            { "chuai", "揣" },
            { "chuan", "川穿椽船喘串传" },
            { "chuang", "疮窗床闯创" },
            { "chui", "吹炊捶锤垂椎" },
            { "chun", "春椿醇唇淳纯蠢" },
            { "chuo", "戳绰" },
            { "ci", "疵茨磁雌辞慈瓷词此刺赐次伺兹差" },
            { "cong", "聪葱囱匆从丛" },
            { "cou", "凑" },
            { "cu", "粗醋簇促卒" },
            { "cuan", "蹿篡窜攒" },
            { "cui", "摧崔催脆瘁粹淬翠" },
            { "cun", "村存寸" },
            { "cuo", "磋搓措挫错撮" },
            { "da", "搭达答瘩打大" },
            { "dai", "歹傣戴带殆代贷袋待逮怠大呆" },
            { "dan", "耽担丹郸胆旦氮但惮淡诞蛋掸弹石单" },
            { "dang", "当挡党荡档" },
            { "dao", "刀捣蹈倒岛祷导到稻悼道盗" },
            { "de", "德得的地" },
            { "dei", "" },
            { "deng", "蹬灯登等瞪凳邓澄" },
            { "di", "低滴迪敌笛狄涤嫡抵蒂第帝弟递缔的堤翟底地提" },
            { "dian", "颠掂滇碘点典靛垫电甸店惦奠淀殿佃" },
            { "diao", "碉叼雕凋刁掉吊钓调" },
            { "die", "跌爹碟蝶迭谍叠" },
            { "ding", "盯叮钉顶鼎锭定订丁" },
            { "diu", "丢" },
            { "dong", "东冬董懂动栋冻洞侗恫" },
            { "dou", "兜抖斗陡豆逗痘都" },
            { "du", "督毒犊独堵睹赌杜镀肚渡妒都读度" },
            { "duan", "端短锻段断缎" },
            { "dui", "兑队对堆" },
            { "dun", "墩吨钝遁蹲敦顿囤盾" },
            { "duo", "掇哆多夺垛躲朵跺剁惰度舵堕" },
            { "e", "峨鹅俄额讹娥厄扼遏鄂饿阿蛾恶哦" },
            { "en", "恩" },
            { "er", "而耳尔饵洱二贰儿" },
            { "fa", "发罚筏伐乏阀法珐" },
            { "fan", "藩帆翻樊矾钒凡烦反返范贩犯饭泛番繁" },
            { "fang", "坊芳方肪房防妨仿访纺放" },
            { "fei", "菲非啡飞肥匪诽吠肺废沸费" },
            { "fen", "芬酚吩氛分纷坟焚汾粉奋份忿愤粪" },
            { "feng", "丰封枫蜂峰锋风疯烽逢缝讽奉凤冯" },
            { "fo", "佛" },
            { "fou", "否" },
            { "fu", "夫敷肤孵扶辐幅氟符伏俘服浮涪福袱弗甫抚辅俯釜斧腑府腐赴副覆赋复傅付阜父腹负富讣附妇缚咐佛拂脯" },
            { "ga", "噶嘎夹咖" },
            { "gai", "该改概钙溉盖芥" },
            { "gan", "干甘杆柑竿肝赶感秆敢赣乾" },
            { "gang", "冈刚钢缸肛纲岗港杠扛" },
            { "gao", "篙皋高膏羔糕搞稿镐告" },
            { "ge", "哥歌搁戈鸽疙割葛格阁隔铬个各胳革蛤咯" },
            { "gei", "给" },
            { "gen", "根跟" },
            { "geng", "耕更庚羹埂耿梗粳颈" },
            { "gong", "工攻功恭龚供躬公宫弓巩拱贡共汞" },
            { "gou", "钩勾沟苟狗垢构购够" },
            { "gu", "辜菇咕箍估沽孤姑古蛊骨股故顾固雇鼓谷贾" },
            { "gua", "刮瓜剐寡挂褂" },
            { "guai", "乖拐怪" },
            { "guan", "棺关官冠观管馆罐惯灌贯纶" },
            { "guang", "光逛广" },
            { "gui", "瑰规圭归闺轨鬼诡癸桂柜跪贵刽硅傀炔龟" },
            { "gun", "辊滚棍" },
            { "guo", "锅郭国果裹过涡" },
            { "ha", "蛤哈" },
            { "hai", "骸孩海氦亥害骇还咳" },
            { "han", "酣憨邯韩含涵寒函喊罕翰撼捍旱憾悍焊汗汉" },
            { "hang", "杭航夯吭巷行" },
            { "hao", "壕嚎豪毫郝好耗号浩镐貉" },
            { "he", "喝荷菏禾何盒阂河赫褐鹤贺核合涸吓呵貉和" },
            { "hei", "黑嘿" },
            { "hen", "痕很狠恨" },
            { "heng", "亨横衡恒哼行" },
            { "hong", "轰哄烘虹鸿洪宏弘红" },
            { "hou", "喉侯猴吼厚候后" },
            { "hu", "呼乎忽瑚壶葫胡蝴狐糊湖弧虎护互沪户唬和" },
            { "hua", "花华猾画化话哗滑划" },
            { "huai", "槐怀淮徊坏" },
            { "huan", "欢环桓缓换患唤痪豢焕涣宦幻还" },
            { "huang", "荒慌黄磺蝗簧皇凰惶煌晃幌恍谎" },
            { "hui", "灰挥辉徽恢蛔回毁悔慧卉惠晦贿秽烩汇讳诲绘会" },
            { "hun", "昏婚魂浑混荤" },
            { "huo", "活伙火获或惑霍货祸豁和" },
            { "i", "" },
            { "ji", "击圾基机畸积箕肌饥迹激讥鸡姬绩吉极棘辑籍集及急疾汲即嫉级挤几脊己蓟技冀季伎剂悸济寄寂计记既忌际妓继纪给稽缉祭藉期奇齐系" },
            { "jia", "嘉枷佳加荚颊甲钾假稼架驾嫁夹贾价搅茄缴家" },
            { "jian", "歼监坚尖笺间煎兼肩艰奸缄茧检柬碱拣捡简俭剪减荐鉴践贱键箭件健舰剑饯渐溅涧建槛见浅" },
            { "jiang", "僵姜浆江疆蒋桨奖讲匠酱将降强" },
            { "jiao", "椒礁焦胶交郊浇骄娇脚教轿较叫窖蕉嚼搅铰狡饺绞酵觉校矫侥角缴剿" },
            { "jie", "揭接皆秸街阶截劫节杰捷睫竭洁结姐戒界借介疥诫届桔解藉芥" },
            { "jin", "巾筋斤金今津襟紧锦仅谨进靳晋禁近烬浸尽劲" },
            { "jing", "荆兢茎睛晶鲸京惊精经井警静境敬镜径痉靖竟竞净劲粳景颈" },
            { "jiong", "炯窘" },
            { "jiu", "揪究纠玖韭久灸九酒厩救旧臼舅咎就疚" },
            { "ju", "鞠拘狙疽驹菊局矩举沮聚拒据巨具距踞锯俱惧炬剧车桔居咀句蛆足" },
            { "juan", "捐鹃娟倦眷绢卷圈" },
            { "jue", "撅攫抉掘倔爵决诀绝嚼觉角" },
            { "jun", "菌钧军君峻俊竣郡骏均浚" },
            { "ka", "喀咖卡咯" },
            { "kai", "开揩凯慨楷" },
            { "kan", "刊堪勘坎砍看槛嵌" },
            { "kang", "康慷糠抗亢炕扛" },
            { "kao", "考拷烤靠" },
            { "ke", "坷苛柯棵磕颗科可渴克刻客课壳呵咳" },
            { "ken", "肯啃垦恳" },
            { "keng", "坑吭" },
            { "kong", "空恐孔控" },
            { "kou", "抠口扣寇" },
            { "ku", "枯哭窟苦酷库裤" },
            { "kua", "夸垮挎跨胯" },
            { "kuai", "块筷侩快会" },
            { "kuan", "宽款" },
            { "kuang", "匡筐狂框矿眶旷况" },
            { "kui", "亏盔岿窥葵奎魁馈愧傀溃" },
            { "kun", "坤昆捆困" },
            { "kuo", "扩廓阔括" },
            { "la", "垃拉喇辣啦蜡腊落" },
            { "lai", "莱来赖" },
            { "lan", "婪栏拦篮阑兰澜谰揽览懒缆烂滥蓝" },
            { "lang", "琅榔狼廊郎朗浪" },
            { "lao", "捞劳牢老佬涝姥酪烙潦落" },
            { "le", "勒乐肋了" },
            { "lei", "雷镭蕾磊累儡垒擂类泪勒肋" },
            { "leng", "楞冷棱" },
            { "li", "厘梨犁黎篱狸离漓理李里鲤礼莉荔吏栗丽厉励砾历利例俐痢立粒沥隶力璃哩" },
            { "lian", "联莲连镰廉涟帘敛脸链恋炼练怜" },
            { "liang", "粮凉梁粱良两辆量晾亮谅俩" },
            { "liao", "撩聊僚疗燎寥辽撂镣廖料潦了" },
            { "lie", "列裂烈劣猎" },
            { "lin", "琳林磷霖临邻鳞淋凛赁吝拎" },
            { "ling", "玲菱零龄铃伶羚凌灵陵岭领另令棱怜" },
            { "liu", "溜琉榴硫馏留刘瘤流柳六陆" },
            { "long", "龙聋咙笼窿隆垄拢陇弄" },
            { "lou", "楼娄搂篓漏陋露" },
            { "lu", "芦卢颅庐炉掳卤虏鲁麓路赂鹿潞禄录戮吕六碌露陆绿" },
            { "lv", "驴铝侣旅履屡缕虑氯律滤绿率" },
            { "lve", "掠略" },
            { "luan", "峦挛孪滦卵乱" },
            { "lun", "抡轮伦仑沦论纶" },
            { "luo", "萝螺罗逻锣箩骡裸洛骆烙络落咯" },
            { "ma", "妈麻玛码蚂马骂嘛吗摩抹么" },
            { "mai", "买麦卖迈埋脉" },
            { "man", "瞒馒蛮满曼慢漫谩埋蔓" },
            { "mang", "茫盲氓忙莽芒" },
            { "mao", "猫茅锚毛矛铆卯茂帽貌贸冒" },
            { "me", "么" },
            { "mei", "玫枚梅酶霉煤眉媒镁每美昧寐妹媚没糜" },
            { "men", "门闷们" },
            { "meng", "萌蒙檬锰猛梦孟盟" },
            { "mi", "眯醚靡迷弥米觅蜜密幂糜谜泌秘" },
            { "mian", "棉眠绵冕免勉缅面娩" },
            { "miao", "苗描瞄藐秒渺庙妙" },
            { "mie", "蔑灭" },
            { "min", "民抿皿敏悯闽" },
            { "ming", "明螟鸣铭名命" },
            { "miu", "谬" },
            { "mo", "摸摹蘑膜磨魔末莫墨默沫漠寞陌脉没模摩抹" },
            { "mou", "谋某牟" },
            { "mu", "拇牡亩姆母墓暮幕募慕木目睦牧穆姥模牟" },
            { "na", "拿钠纳呐那娜哪" },
            { "nai", "氖乃奶耐奈哪" },
            { "nan", "南男难" },
            { "nang", "囊" },
            { "nao", "挠脑恼闹淖" },
            { "ne", "呢哪" },
            { "nei", "馁内那哪" },
            { "nen", "嫩" },
            { "neng", "能" },
            { "ni", "妮霓倪泥尼拟你匿腻逆溺呢" },
            { "nian", "蔫拈年碾撵捻念粘" },
            { "niang", "娘酿" },
            { "niao", "鸟尿" },
            { "nie", "捏聂孽啮镊镍涅" },
            { "nin", "您" },
            { "ning", "柠狞凝宁拧泞" },
            { "niu", "牛扭钮纽" },
            { "nong", "脓浓农弄" },
            { "nu", "奴怒努" },
            { "nv", "女" },
            { "nve", "虐疟" },
            { "nuan", "暖" },
            { "nuo", "挪懦糯诺娜" },
            { "o", "哦喔噢" },
            { "ou", "欧鸥殴藕呕偶沤区" },
            { "pa", "啪趴爬帕怕扒耙琶" },
            { "pai", "拍排牌徘湃派迫" },
            { "pan", "攀潘盘磐盼畔判叛番胖般" },
            { "pang", "乓庞耪膀磅旁胖" },
            { "pao", "抛咆袍跑泡刨炮" },
            { "pei", "呸胚培裴赔陪配佩沛坏" },
            { "pen", "喷盆" },
            { "peng", "砰抨烹澎彭蓬棚硼篷膨朋鹏捧碰" },
            { "pi", "坯砒霹批披劈琵毗啤脾疲皮痞僻屁譬辟否匹坏" },
            { "pian", "篇偏片骗扁便" },
            { "piao", "飘漂瓢票朴" },
            { "pie", "撇瞥" },
            { "pin", "拼频贫品聘" },
            { "ping", "乒坪萍平凭瓶评苹屏" },
            { "po", "坡泼颇婆破粕泊迫魄朴" },
            { "pou", "剖" },
            { "pu", "扑铺仆莆葡菩蒲圃普浦谱脯埔曝瀑堡朴" },
            { "qi", "欺戚妻七凄柒沏棋歧崎脐旗祈祁骑起岂乞企启器气迄弃汽讫稽缉期栖其奇畦齐砌泣漆契" },
            { "qia", "掐卡洽" },
            { "qian", "牵扦钎千迁签仟谦黔钱钳前潜遣谴堑欠歉铅乾浅嵌纤" },
            { "qiang", "枪呛腔羌墙蔷抢强" },
            { "qiao", "锹敲悄桥乔侨巧撬翘峭俏窍壳橇瞧鞘雀" },
            { "qie", "切窃砌茄且怯" },
            { "qin", "钦侵秦琴勤芹擒禽寝亲沁" },
            { "qing", "青轻氢倾卿清擎晴氰情顷请庆亲" },
            { "qiong", "琼穷" },
            { "qiu", "秋丘邱球求囚酋泅" },
            { "qu", "趋曲躯屈驱渠取娶龋去区蛆趣" },
            { "quan", "颧权醛泉全痊拳犬券劝卷圈" },
            { "que", "缺瘸却鹊榷确炔雀" },
            { "qun", "裙群" },
            { "ran", "然燃冉染" },
            { "rang", "瓤壤攘嚷让" },
            { "rao", "饶扰绕" },
            { "re", "惹热" },
            { "ren", "壬仁人忍韧任认刃妊纫" },
            { "reng", "扔仍" },
            { "ri", "日" },
            { "rong", "戎茸蓉荣融熔溶容绒冗" },
            { "rou", "揉柔肉" },
            { "ru", "茹儒孺如辱乳汝入褥蠕" },
            { "ruan", "软阮" },
            { "rui", "蕊瑞锐" },
            { "run", "闰润" },
            { "ruo", "弱若" },
            { "sa", "撒洒萨" },
            { "sai", "腮鳃赛塞" },
            { "san", "三叁伞散" },
            { "sang", "桑嗓丧" },
            { "sao", "搔骚扫嫂梢" },
            { "se", "瑟涩塞色" },
            { "sen", "森" },
            { "seng", "僧" },
            { "sha", "砂杀沙纱傻啥煞莎刹杉厦" },
            { "shai", "筛晒色" },
            { "shan", "珊苫山删煽衫闪陕擅赡膳善汕扇缮杉栅掺单" },
            { "shang", "墒伤商赏晌上尚裳汤" },
            { "shao", "捎稍烧芍勺韶少哨邵绍鞘梢召" },
            { "she", "奢赊舌舍赦摄慑涉社设蛇拾折射" },
            { "shei", "谁" },
            { "shen", "砷申呻伸身深绅神审婶肾慎渗沈甚参娠什" },
            { "sheng", "声生甥牲升绳剩胜圣乘省盛" },
            { "shi", "师失狮施湿诗尸虱十时蚀实史矢使屎驶始式示士世柿事拭誓逝势是嗜噬适仕侍释饰市恃室视试匙石拾食识氏似嘘殖峙什" },
            { "shou", "收手首守寿授售受瘦兽熟" },
            { "shu", "蔬枢梳殊抒输叔舒淑疏书赎孰薯暑曙署蜀黍鼠述树束戍竖墅庶漱恕熟属术数" },
            { "shua", "刷耍" },
            { "shuai", "摔甩帅衰率" },
            { "shuan", "栓拴" },
            { "shuang", "霜双爽" },
            { "shui", "水睡税谁说" },
            { "shun", "吮瞬顺舜" },
            { "shuo", "硕朔烁数说" },
            { "si", "斯撕嘶私司丝死肆寺嗣四饲巳食思伺似" },
            { "song", "松耸怂颂送宋讼诵" },
            { "sou", "搜擞嗽艘" },
            { "su", "苏酥俗素速粟僳塑溯诉肃宿缩" },
            { "suan", "酸蒜算" },
            { "sui", "虽隋随绥髓碎岁穗遂隧祟尿" },
            { "sun", "孙损笋" },
            { "suo", "蓑梭唆琐索锁所莎缩" },
            { "ta", "塌他它她獭挞蹋踏塔拓" },
            { "tai", "胎苔抬台泰酞太态汰" },
            { "tan", "坍摊贪瘫滩坛檀痰潭谭谈坦毯袒碳探叹炭弹" },
            { "tang", "塘搪堂棠膛唐糖躺淌趟烫敞汤倘" },
            { "tao", "掏涛滔绦萄桃逃淘讨套陶" },
            { "te", "特" },
            { "teng", "藤腾疼誊" },
            { "ti", "梯剔踢锑题蹄啼体替嚏惕涕剃屉提" },
            { "tian", "天添填田甜恬舔腆蚕" },
            { "tiao", "挑条迢眺跳调" },
            { "tie", "贴铁帖" },
            { "ting", "厅烃汀廷停亭庭挺艇听" },
            { "tong", "通桐酮瞳同铜彤童桶捅筒统痛侗恫" },
            { "tou", "偷投头透" },
            { "tu", "秃突图徒途涂屠土吐兔凸余" },
            { "tuan", "湍团" },
            { "tui", "推颓腿蜕退褪" },
            { "tun", "吞屯臀囤" },
            { "tuo", "拖托脱鸵陀驼椭妥唾驮拓" },
            { "u", "" },
            { "v", "" },
            { "wa", "挖哇蛙洼娃瓦袜" },
            { "wai", "歪外" },
            { "wan", "豌弯湾玩顽丸烷完碗挽晚惋婉腕蔓皖宛万" },
            { "wang", "汪王枉网往旺望忘妄亡" },
            { "wei", "威巍微危韦违桅围唯惟为潍维苇萎委伟伪纬未味畏胃喂魏位渭谓慰卫尾蔚尉" },
            { "wen", "瘟温蚊文闻纹吻稳紊问" },
            { "weng", "嗡翁瓮" },
            { "wo", "挝蜗窝我斡卧握沃涡" },
            { "wu", "巫呜钨乌污诬屋芜梧吾吴毋武五捂午舞伍侮坞戊雾晤物勿务悟误恶无" },
            { "xi", "昔熙析西硒矽晰嘻吸锡牺稀息希悉膝夕惜熄烯汐犀檄袭席习媳喜隙细栖溪铣洗系戏" },
            { "xia", "瞎匣霞辖暇峡侠狭下虾厦夏吓" },
            { "xian", "掀锨先仙鲜咸贤衔舷闲涎弦嫌显险现献县腺馅羡宪陷限线铣纤" },
            { "xiang", "相厢镶香箱襄湘乡翔祥详想响享项橡像向象降巷" },
            { "xiao", "萧硝霄哮销消宵晓小孝肖啸笑效削嚣淆校" },
            { "xie", "楔些歇鞋协携胁谐写械卸蟹懈泄泻谢屑解蝎挟邪斜血叶契" },
            { "xin", "薪芯锌欣辛新忻心衅信" },
            { "xing", "星腥猩惺兴刑型形邢醒幸杏性姓省行" },
            { "xiong", "兄凶胸匈汹雄熊" },
            { "xiu", "休修羞朽嗅锈秀袖绣臭宿" },
            { "xu", "墟需虚须徐许蓄酗叙旭序恤絮婿绪续戌嘘畜吁" },
            { "xuan", "轩喧宣悬旋玄选癣眩绚" },
            { "xue", "靴薛学穴雪削血" },
            { "xun", "勋熏循旬询驯巡殉汛训讯逊迅浚寻" },
            { "ya", "压押鸦鸭呀丫牙蚜衙涯雅哑亚讶芽崖轧" },
            { "yan", "焉阉淹盐严研蜒岩延言颜阎炎沿奄掩眼衍演艳堰燕厌砚雁唁彦焰宴谚验铅咽烟殷" },
            { "yang", "殃央鸯秧杨扬佯疡羊洋阳氧仰痒养样漾" },
            { "yao", "邀腰妖瑶摇尧遥窑谣姚咬舀药要耀约钥侥" },
            { "ye", "椰噎耶爷野冶也页业夜咽掖叶腋液拽曳" },
            { "yi", "一壹医揖铱依伊衣颐夷移仪胰疑沂宜姨彝椅蚁倚已乙矣以艺抑易邑亿役臆逸肄疫亦裔意毅忆义益溢诣议谊译异翼翌绎遗屹" },
            { "yin", "茵荫因音阴姻吟银淫寅饮尹引隐印殷" },
            { "ying", "英樱婴鹰应缨莹萤营荧蝇迎赢盈影颖硬映" },
            { "yo", "哟" },
            { "yong", "拥佣臃痈庸雍踊蛹咏泳永恿勇用涌" },
            { "you", "幽优悠忧尤由邮铀犹油游酉有友右佑釉诱又幼" },
            { "yu", "迂淤于盂榆虞愚舆逾鱼愉渝渔隅予娱雨与屿禹宇语羽玉域芋郁遇喻峪御愈欲狱誉浴寓裕预豫驭尉余俞吁育" },
            { "yuan", "鸳渊冤元垣袁原援辕园圆猿源缘远苑愿怨院员" },
            { "yue", "曰越跃岳粤月悦阅乐约钥" },
            { "yun", "耘云郧匀陨允运蕴酝晕韵孕均员" },
            { "za", "匝砸杂扎咱咋" },
            { "zai", "栽哉灾宰载再在仔" },
            { "zan", "暂赞攒咱" },
            { "zang", "赃脏葬藏" },
            { "zao", "遭糟藻枣早澡蚤躁噪造皂灶燥凿" },
            { "ze", "责则泽择侧咋" },
            { "zei", "贼" },
            { "zen", "怎" },
            { "zeng", "增憎赠曾综" },
            { "zha", "渣札铡闸眨榨乍炸诈查扎喳栅柞轧咋" },
            { "zhai", "斋债寨翟祭择摘宅窄侧" },
            { "zhan", "瞻毡詹沾盏斩辗崭展蘸栈占战站湛绽颤粘" },
            { "zhang", "樟章彰漳张掌涨杖丈帐账仗胀瘴障长" },
            { "zhao", "招昭找沼赵照罩兆肇朝召爪着" },
            { "zhe", "遮哲蛰辙者蔗浙折锗这着" },
            { "zhen", "珍斟真甄砧臻贞针侦枕疹诊震振镇阵帧" },
            { "zheng", "蒸挣睁征狰争怔整拯正政症郑证" },
            { "zhi", "芝支蜘知肢脂汁之织职直植执值侄址指止趾只旨纸志挚掷至致置帜制智秩稚质炙痔滞治窒识枝吱殖峙" },
            { "zhong", "中盅忠钟衷终肿仲众种重" },
            { "zhou", "舟周州洲诌轴肘帚咒皱宙昼骤粥" },
            { "zhu", "珠株蛛朱猪诸诛逐竹烛煮拄瞩嘱主柱助蛀贮铸筑住注祝驻属著" },
            { "zhua", "抓爪" },
            { "zhuai", "拽" },
            { "zhuan", "专砖撰篆传转赚" },
            { "zhuang", "桩庄装妆壮状幢撞" },
            { "zhui", "锥追赘坠缀椎" },
            { "zhun", "谆准" },
            { "zhuo", "捉拙卓桌茁酌啄灼浊琢缴着" },
            { "zi", "咨资姿滋淄孜紫籽滓子自渍字吱兹仔" },
            { "zong", "鬃棕踪宗总纵综" },
            { "zou", "邹走奏揍" },
            { "zu", "租族祖诅阻组足卒" },
            { "zuan", "钻纂" },
            { "zui", "嘴醉最罪" },
            { "zun", "尊遵" },
            { "zuo", "昨左佐做作坐座撮琢柞"},
            {NULL, NULL}
};
#endif


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
lv_obj_t * lv_pinyin_ime_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}


/*=====================
 * Setter functions
 *====================*/

void lv_pinyin_ime_set_dict(lv_obj_t * obj, lv_pinyin_dict_t * dict)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    init_pinyin_dict(obj, dict);
}

/*=====================
 * Getter functions
 *====================*/
lv_obj_t * lv_pinyin_ime_get_kb(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_pinyin_ime_t * pinyin_ime = (lv_pinyin_ime_t *)obj;

    return pinyin_ime->kb;
}

lv_obj_t * lv_pinyin_ime_get_cand_panel(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_pinyin_ime_t * pinyin_ime = (lv_pinyin_ime_t *)obj;

    return pinyin_ime->cand_panel;
}


lv_pinyin_dict_t * lv_pinyin_ime_get_dict(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_pinyin_ime_t * pinyin_ime = (lv_pinyin_ime_t *)obj;

    return pinyin_ime->dict;
}

/*=====================
 * Other functions
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_pinyin_ime_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_pinyin_ime_t * pinyin_ime = (lv_pinyin_ime_t *)obj;

    uint16_t py_str_i = 0;
    for(uint16_t btnm_i = 0; btnm_i < (LV_PINYIN_IME_CAND_TEXT_NUM + 3); btnm_i++)
    {
        if (btnm_i == 0)
        {
            lv_btnm_def_pinyin_sel_map[btnm_i] = "<";
        }
        else if (btnm_i == (LV_PINYIN_IME_CAND_TEXT_NUM + 1))
        {
            lv_btnm_def_pinyin_sel_map[btnm_i] = ">";
        }
        else if (btnm_i == (LV_PINYIN_IME_CAND_TEXT_NUM + 2))
        {
            lv_btnm_def_pinyin_sel_map[btnm_i] = "";
        }
        else
        {
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

    pinyin_ime->kb = lv_keyboard_create(lv_scr_act());
    lv_obj_align_to(pinyin_ime->kb, obj, LV_ALIGN_BOTTOM_MID, 0, 0);

    init_pinyin_dict(obj, lv_pinyin_def_dict);

    /* Init pinyin_ime->cand_panel */
    pinyin_ime->cand_panel = lv_btnmatrix_create(lv_scr_act());
    lv_btnmatrix_set_map(pinyin_ime->cand_panel, (const char **)lv_btnm_def_pinyin_sel_map);
    lv_obj_set_size(pinyin_ime->cand_panel, LV_PCT(100), LV_PCT(5));
    lv_obj_align_to(pinyin_ime->cand_panel, pinyin_ime->kb, LV_ALIGN_OUT_TOP_MID, 0, 0);
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
    lv_obj_add_event_cb(pinyin_ime->cand_panel, lv_pinyin_ime_cand_panel_event, LV_EVENT_VALUE_CHANGED, obj);
    lv_obj_add_event_cb(pinyin_ime->kb, lv_pinyin_ime_kb_event, LV_EVENT_VALUE_CHANGED, obj);
    lv_obj_add_event_cb(obj, lv_pinyin_ime_style_change_event, LV_EVENT_STYLE_CHANGED, NULL);
}


static void lv_pinyin_ime_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);

    lv_pinyin_ime_t * pinyin_ime = (lv_pinyin_ime_t *)obj;

    if (pinyin_ime->kb) lv_obj_del(pinyin_ime->kb);
    if (pinyin_ime->cand_panel) lv_obj_del(pinyin_ime->cand_panel);
}


static void lv_pinyin_ime_kb_event(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * kb = lv_event_get_target(e);
    lv_obj_t * obj = lv_event_get_user_data(e);

    lv_pinyin_ime_t * pinyin_ime = (lv_pinyin_ime_t *)obj;

    if(code == LV_EVENT_VALUE_CHANGED)
    {
        uint16_t btn_id  = lv_btnmatrix_get_selected_btn(kb);
        if(btn_id == LV_BTNMATRIX_BTN_NONE) return;

        const char * txt = lv_btnmatrix_get_btn_text(kb, lv_btnmatrix_get_selected_btn(kb));
        if(txt == NULL) return;

        if(strcmp(txt, "Enter") == 0 || strcmp(txt, LV_SYMBOL_NEW_LINE) == 0)
        {
            lv_memset_00(lv_pinyin_cand_str, (sizeof(lv_pinyin_cand_str)));
            pinyin_ime->ta_count = 0;
            lv_memset_00(pinyin_ime->input_char, sizeof(pinyin_ime->input_char));
        }
        else if(strcmp(txt, LV_SYMBOL_BACKSPACE) == 0)
        {
            // del input char
            for (int i = strlen(pinyin_ime->input_char) - 1; i >= 0; i--){
                if (pinyin_ime->input_char[i] != '\0'){
                    pinyin_ime->input_char[i] = '\0';
                    lv_obj_add_flag(pinyin_ime->cand_panel, LV_OBJ_FLAG_HIDDEN);
                    break;
                }
            }
            pinyin_input_proc(obj);
            pinyin_ime->ta_count--;
        }
        else if ((strcmp(txt, "ABC") == 0) || (strcmp(txt, "abc") == 0) || (strcmp(txt, "1#") == 0))
        {
            pinyin_ime->ta_count = 0;
            lv_memset_00(pinyin_ime->input_char, sizeof(pinyin_ime->input_char));
            return;
        }
        else if((strcmp(txt, LV_SYMBOL_KEYBOARD) == 0) || (strcmp(txt, LV_SYMBOL_OK) == 0))
        {
            pinyin_ime->ta_count = 0;
            lv_memset_00(pinyin_ime->input_char, sizeof(pinyin_ime->input_char));
            lv_obj_add_flag(pinyin_ime->cand_panel, LV_OBJ_FLAG_HIDDEN);
        }
        else if ((txt[0] >= 'a' && txt[0] <= 'z') || (txt[0] >= 'A' && txt[0] <= 'Z')){
            strcat(pinyin_ime->input_char, txt);
            pinyin_input_proc(obj);
            pinyin_ime->ta_count++;
        }
    }
}


static void lv_pinyin_ime_cand_panel_event(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * cand_panel = lv_event_get_target(e);
    lv_obj_t * obj = (lv_obj_t *)lv_event_get_user_data(e);

    lv_pinyin_ime_t * pinyin_ime = (lv_pinyin_ime_t *)obj;

    if(code == LV_EVENT_VALUE_CHANGED) {
        uint32_t id = lv_btnmatrix_get_selected_btn(cand_panel);
        if (id == 0)
        {
            pinyin_page_proc(obj, 0);
            return;
        }
        if (id == (LV_PINYIN_IME_CAND_TEXT_NUM + 1))
        {
            pinyin_page_proc(obj, 1);
            return;
        }

        const char * txt = lv_btnmatrix_get_btn_text(cand_panel, id);
        lv_obj_t * ta = lv_keyboard_get_textarea(pinyin_ime->kb);

        for (int i = 0; i < pinyin_ime->ta_count; i++)
        {
            lv_textarea_del_char(ta);
        }

        lv_textarea_add_text(ta, txt);

        lv_memset_00(lv_pinyin_cand_str, (sizeof(lv_pinyin_cand_str)));
        pinyin_ime->ta_count = 0;
        lv_memset_00(pinyin_ime->input_char, sizeof(pinyin_ime->input_char));
        lv_obj_add_flag(pinyin_ime->cand_panel, LV_OBJ_FLAG_HIDDEN);
    }
}


static void pinyin_input_proc(lv_obj_t * obj)
{
    lv_pinyin_ime_t * pinyin_ime = (lv_pinyin_ime_t *)obj;

    pinyin_ime->cand_str = pinyin_search_matching(obj, pinyin_ime->input_char, &pinyin_ime->cand_num);
    if (pinyin_ime->cand_str == NULL) 
    {
        return;
    }

    pinyin_ime->py_page = 0;

    for(uint8_t i = 0; i < LV_PINYIN_IME_CAND_TEXT_NUM; i++)
    {
        memset(lv_pinyin_cand_str[i], 0x00, sizeof(lv_pinyin_cand_str[i]));
        lv_pinyin_cand_str[i][0] = ' ';
    }

    // fill buf
    for(uint8_t i = 0; (i < pinyin_ime->cand_num && i < LV_PINYIN_IME_CAND_TEXT_NUM); i++)
    {
        for(uint8_t j = 0; j < 3; j++)
        {
            lv_pinyin_cand_str[i][j] = pinyin_ime->cand_str[i * 3 + j];
        }
    }
    lv_obj_clear_flag(pinyin_ime->cand_panel, LV_OBJ_FLAG_HIDDEN);
}

static void pinyin_page_proc(lv_obj_t* obj, uint16_t dir)
{
    lv_pinyin_ime_t * pinyin_ime = (lv_pinyin_ime_t *)obj;
    uint16_t page_num = pinyin_ime->cand_num / LV_PINYIN_IME_CAND_TEXT_NUM;
    uint16_t sur = pinyin_ime->cand_num % LV_PINYIN_IME_CAND_TEXT_NUM;
    
    if(dir == 0)
    {
        if(pinyin_ime->py_page) 
        {
            pinyin_ime->py_page--;
        }
    }
    else
    {
        if (sur == 0)
        {
            page_num -= 1;
        }
        if(pinyin_ime->py_page < page_num)
        {
            pinyin_ime->py_page++;
        }
        else return;
    }

    for(uint8_t i = 0; i < LV_PINYIN_IME_CAND_TEXT_NUM; i++)
    {
        memset(lv_pinyin_cand_str[i], 0x00, sizeof(lv_pinyin_cand_str[i]));
        lv_pinyin_cand_str[i][0] = ' ';
    }

    // fill buf
    uint16_t offset = pinyin_ime->py_page * (3 * LV_PINYIN_IME_CAND_TEXT_NUM);
    for(uint8_t i = 0; (i < pinyin_ime->cand_num && i < LV_PINYIN_IME_CAND_TEXT_NUM); i++)
    {
        if ((sur > 0) && (pinyin_ime->py_page == page_num))
        {
            if (i > sur)
            break;
        }
        for(uint8_t j = 0; j < 3; j++)
        {
            lv_pinyin_cand_str[i][j] = pinyin_ime->cand_str[offset + (i * 3) + j];
        }
    }
}


static void lv_pinyin_ime_style_change_event(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);

    lv_pinyin_ime_t * pinyin_ime = (lv_pinyin_ime_t *)obj;

    if(code == LV_EVENT_STYLE_CHANGED) {
        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        lv_obj_set_style_text_font(pinyin_ime->cand_panel, font, 0);
    }
}


static void init_pinyin_dict(lv_obj_t * obj, lv_pinyin_dict_t * dict)
{
    lv_pinyin_ime_t * pinyin_ime = (lv_pinyin_ime_t *)obj;

    char headletter = 'a';
    uint16_t offset_sum = 0;
    uint16_t offset_count = 0;
    uint16_t letter_calc = 0;

    pinyin_ime->dict = dict;

    for(uint16_t i = 0; ; i++)
    {
        if ((NULL == (dict[i].py)) || (NULL == (dict[i].py_mb)))
        {
            headletter = dict[i-1].py[0];
            letter_calc = headletter - 'a';
            pinyin_ime->py_num[letter_calc] = offset_count;
            break;
        }   
        if (headletter == (dict[i].py[0]))
        {
            offset_count++;
        }
        else
        {
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
    lv_pinyin_ime_t * pinyin_ime = (lv_pinyin_ime_t *)obj;

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

    while(count--)
    {
        for(i = 0; i < cInputStrLength; i++)
        {
            if(*(strInput_py_str + i) != *((cpHZ->py) + i))
            {
                break;
            }
        }
        // perfect match
        if(cInputStrLength == 1 || i == cInputStrLength)
        {
            // The Chinese character in UTF-8 encoding format is 3 bytes
            * cand_num = strlen((const char*)(cpHZ->py_mb)) / 3;
            return (char*)(cpHZ->py_mb);
        }       
        cpHZ++;
    }
    return NULL;
}


#endif  /*LV_USE_PINYIN_IME*/
