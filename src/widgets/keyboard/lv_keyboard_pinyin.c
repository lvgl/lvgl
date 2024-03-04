
/**
 * @file lv_keyboard_pinyin.c
 *       reference https://github.com/xiaohua4758/lvgl8.0.1_cn_keyboard
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_keyboard_pinyin.h"

#if LV_KEYBOARD_PINYIN_USE_DEFAULT_DICT

#include <stdio.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static const char lv_pinyin_mb_space [] = {""};
static const char lv_pinyin_mb_a     [] = {"啊阿呵吖嗄腌锕"};
static const char lv_pinyin_mb_ai    [] = {"爱矮挨哎碍癌艾唉哀蔼隘埃皑呆嗌嫒瑷暧捱砹嗳锿霭"};
static const char lv_pinyin_mb_an    [] = {"按安暗岸俺案鞍氨胺厂广庵揞犴铵桉谙鹌埯黯"};
static const char lv_pinyin_mb_ang   [] = {"昂肮盎仰"};
static const char lv_pinyin_mb_ao    [] = {"袄凹傲奥熬懊敖翱澳嚣拗媪廒骜嗷坳遨聱螯獒鏊鳌鏖"};
static const char lv_pinyin_mb_ba    [] = {"把八吧爸拔罢跋巴芭扒坝霸叭靶笆疤耙捌粑茇岜鲅钯魃"};
static const char lv_pinyin_mb_bai   [] = {"百白摆败柏拜佰伯稗捭呗掰"};
static const char lv_pinyin_mb_ban   [] = {"半办班般拌搬版斑板伴扳扮瓣颁绊癍坂钣舨阪"};
static const char lv_pinyin_mb_bang  [] = {"帮棒绑磅镑邦榜蚌傍梆膀谤浜蒡"};
static const char lv_pinyin_mb_bao   [] = {"包抱报饱保暴薄宝爆剥豹刨雹褒堡苞胞鲍炮瀑龅孢煲褓鸨趵葆"};
static const char lv_pinyin_mb_bei   [] = {"被北倍杯背悲备碑卑贝辈钡焙狈惫臂褙悖蓓鹎鐾呗"};
static const char lv_pinyin_mb_ben   [] = {"本奔苯笨夯锛贲畚坌"};
static const char lv_pinyin_mb_beng  [] = {"蹦绷甭崩迸蚌泵甏嘣"};
static const char lv_pinyin_mb_bi    [] = {"比笔闭鼻碧必避逼毕臂彼鄙壁蓖币弊辟蔽毙庇敝陛毖痹秘泌秕薜荸芘萆匕裨畀俾嬖狴筚箅篦舭荜襞庳铋跸吡愎贲滗濞璧哔髀弼妣婢"};
static const char lv_pinyin_mb_bian  [] = {"边变便遍编辩扁贬鞭卞辨辫忭砭匾汴碥蝙褊鳊笾苄窆弁缏"};
static const char lv_pinyin_mb_biao  [] = {"表标彪膘杓婊飑飙鳔瘭飚镳裱骠镖"};
static const char lv_pinyin_mb_bie   [] = {"别憋鳖瘪蹩"};
static const char lv_pinyin_mb_bin   [] = {"宾濒摈彬斌滨豳膑殡缤髌傧槟鬓镔玢"};
static const char lv_pinyin_mb_bing  [] = {"并病兵冰丙饼屏秉柄炳摒槟禀邴"};
static const char lv_pinyin_mb_bo    [] = {"拨波播泊博伯驳玻剥薄勃菠钵搏脖帛般柏舶渤铂箔膊魄卜礴跛檗亳鹁踣啵蕃簸钹饽擘"};
static const char lv_pinyin_mb_bu    [] = {"不步补布部捕卜簿哺堡埠怖埔瓿逋晡钸钚醭卟"};
static const char lv_pinyin_mb_ca    [] = {"擦拆礤嚓"};
static const char lv_pinyin_mb_cai   [] = {"猜才材财裁采彩睬踩菜蔡"};
static const char lv_pinyin_mb_can   [] = {"蚕残掺参惨惭餐灿骖璨孱黪粲"};
static const char lv_pinyin_mb_cang  [] = {"藏仓沧舱苍伧"};
static const char lv_pinyin_mb_cao   [] = {"草操曹槽糙嘈艚螬漕"};
static const char lv_pinyin_mb_ce    [] = {"册侧策测厕恻"};
static const char lv_pinyin_mb_cen   [] = {"参岑涔"};
static const char lv_pinyin_mb_ceng  [] = {"曾层蹭噌"};
static const char lv_pinyin_mb_cha   [] = {"查插叉茶差岔搽察茬碴刹诧楂槎镲衩汊馇檫姹杈锸嚓"};
static const char lv_pinyin_mb_chai  [] = {"菜柴拆差豺钗瘥虿侪"};
static const char lv_pinyin_mb_chan  [] = {"产缠掺搀阐颤铲谗蝉单馋觇婵蒇谄冁廛孱蟾羼镡忏潺禅骣躔澶"};
static const char lv_pinyin_mb_chang [] = {"长唱常场厂尝肠畅昌敞倡偿猖裳鲳氅菖惝嫦徜鬯阊怅伥昶苌娼"};
static const char lv_pinyin_mb_chao  [] = {"朝抄超吵潮巢炒嘲剿绰钞怊焯耖晁"};
static const char lv_pinyin_mb_che   [] = {"车撤扯掣彻尺澈坼砗"};
static const char lv_pinyin_mb_chen  [] = {"趁称辰臣尘晨沉陈衬橙忱郴榇抻谌碜谶宸龀嗔伧琛"};
static const char lv_pinyin_mb_cheng [] = {"成乘盛撑称城程呈诚惩逞骋澄橙承塍柽埕铖噌铛酲晟裎枨蛏丞瞠"};
static const char lv_pinyin_mb_chi   [] = {"吃尺迟池翅痴赤齿耻持斥侈弛驰炽匙踟坻茌墀饬媸豉褫敕哧瘛蚩啻鸱眵螭篪魑叱彳笞嗤"};
static const char lv_pinyin_mb_chong [] = {"冲重虫充宠崇涌种艟忡舂铳憧茺"};
static const char lv_pinyin_mb_chou  [] = {"抽愁臭仇丑稠绸酬筹踌畴瞅惆俦帱瘳雠"};
static const char lv_pinyin_mb_chu   [] = {"出处初锄除触橱楚础储畜滁矗搐躇厨雏楮杵刍怵绌亍憷蹰黜蜍樗"};
static const char lv_pinyin_mb_chuai [] = {"揣膪啜嘬搋踹"};
static const char lv_pinyin_mb_chuan [] = {"穿船传串川喘椽氚遄钏舡舛巛"};
static const char lv_pinyin_mb_chuang[] = {"窗床闯创疮幢怆"};
static const char lv_pinyin_mb_chui  [] = {"吹垂炊锤捶椎槌棰陲"};
static const char lv_pinyin_mb_chun  [] = {"春唇纯蠢醇淳椿蝽莼鹑"};
static const char lv_pinyin_mb_chuo  [] = {"戳绰踔啜龊辍"};
static const char lv_pinyin_mb_ci    [] = {"次此词瓷慈雌磁辞刺茨伺疵赐差兹呲鹚祠糍"};
static const char lv_pinyin_mb_cong  [] = {"从丛葱匆聪囱琮枞淙璁骢苁"};
static const char lv_pinyin_mb_cou   [] = {"凑楱辏腠"};
static const char lv_pinyin_mb_cu    [] = {"粗醋簇促卒徂猝蔟蹙酢殂蹴"};
static const char lv_pinyin_mb_cuan  [] = {"窜蹿篡攒撺汆爨镩"};
static const char lv_pinyin_mb_cui   [] = {"催脆摧翠崔淬衰瘁粹璀啐悴萃毳榱"};
static const char lv_pinyin_mb_cun   [] = {"村寸存蹲忖皴"};
static const char lv_pinyin_mb_cuo   [] = {"错撮搓挫措磋嵯厝鹾脞痤蹉瘥锉矬"};
static const char lv_pinyin_mb_da    [] = {"大答达打搭瘩塔笪耷哒褡疸怛靼妲沓嗒鞑"};
static const char lv_pinyin_mb_dai   [] = {"带代呆戴待袋逮歹贷怠傣大殆呔玳迨岱甙黛骀绐埭"};
static const char lv_pinyin_mb_dan   [] = {"但单蛋担弹掸胆淡丹耽旦氮诞郸惮石疸澹瘅萏殚眈聃箪赕儋啖"};
static const char lv_pinyin_mb_dang  [] = {"当党挡档荡谠铛宕菪凼裆砀"};
static const char lv_pinyin_mb_dao   [] = {"到道倒刀岛盗稻捣悼导蹈祷帱纛忉焘氘叨"};
static const char lv_pinyin_mb_de    [] = {"的地得德底锝"};
static const char lv_pinyin_mb_deng  [] = {"等灯邓登澄瞪凳蹬磴镫噔嶝戥簦"};
static const char lv_pinyin_mb_di    [] = {"地第底低敌抵滴帝递嫡弟缔堤的涤提笛迪狄翟蒂觌邸谛诋嘀柢骶羝氐棣睇娣荻碲镝坻籴砥"};
static const char lv_pinyin_mb_dia   [] = {"嗲"};
static const char lv_pinyin_mb_dian  [] = {"点电店殿淀掂颠垫碘惦奠典佃靛滇甸踮钿坫阽癫簟玷巅癜"};
static const char lv_pinyin_mb_diao  [] = {"掉钓叼吊雕调刁碉凋鸟铞铫鲷貂"};
static const char lv_pinyin_mb_die   [] = {"爹跌叠碟蝶迭谍牒堞瓞揲蹀耋鲽垤喋"};
static const char lv_pinyin_mb_ding  [] = {"顶定盯订叮丁钉鼎锭町玎铤腚碇疔仃耵酊啶"};
static const char lv_pinyin_mb_diu   [] = {"丢铥"};
static const char lv_pinyin_mb_dong  [] = {"动东懂洞冻冬董栋侗恫峒鸫垌胨胴硐氡岽咚"};
static const char lv_pinyin_mb_dou   [] = {"都斗豆逗陡抖痘兜读蚪窦篼蔸"};
static const char lv_pinyin_mb_du    [] = {"读度毒渡堵独肚镀赌睹杜督都犊妒顿蠹笃嘟渎椟牍黩髑芏"};
static const char lv_pinyin_mb_duan  [] = {"段短断端锻缎椴煅簖"};
static const char lv_pinyin_mb_dui   [] = {"对队堆兑敦镦碓怼憝"};
static const char lv_pinyin_mb_dun   [] = {"吨顿蹲墩敦钝盾囤遁不趸沌盹镦礅炖砘"};
static const char lv_pinyin_mb_duo   [] = {"多朵夺舵剁垛跺惰堕掇哆驮度躲踱沲咄铎裰哚缍"};
static const char lv_pinyin_mb_e     [] = {"饿哦额鹅蛾扼俄讹阿遏峨娥恶厄鄂锇谔垩锷阏萼苊轭婀莪鳄颚腭愕呃噩鹗屙"};
static const char lv_pinyin_mb_en    [] = {"恩摁嗯"};
static const char lv_pinyin_mb_er    [] = {"而二耳儿饵尔贰洱珥鲕鸸佴迩铒"};
static const char lv_pinyin_mb_fa    [] = {"发法罚伐乏筏阀珐垡砝"};
static const char lv_pinyin_mb_fan   [] = {"反饭翻番犯凡帆返泛繁烦贩范樊藩矾钒燔蘩畈蕃蹯梵幡"};
static const char lv_pinyin_mb_fang  [] = {"放房防纺芳方访仿坊妨肪钫彷邡枋舫鲂"};
static const char lv_pinyin_mb_fei   [] = {"非飞肥费肺废匪吠沸菲诽啡篚蜚腓扉妃斐狒芾悱镄霏翡榧淝鲱绯痱"};
static const char lv_pinyin_mb_fen   [] = {"分份芬粉坟奋愤纷忿粪酚焚吩氛汾棼瀵鲼玢偾鼢"};
static const char lv_pinyin_mb_feng  [] = {"风封逢缝蜂丰枫疯冯奉讽凤峰锋烽砜俸酆葑沣唪"};
static const char lv_pinyin_mb_fo    [] = {"佛"};
static const char lv_pinyin_mb_fou   [] = {"否缶"};
static const char lv_pinyin_mb_fu    [] = {"副幅扶浮富福负伏付复服附俯斧赴缚拂夫父符孵敷赋辅府腐腹妇抚覆辐肤氟佛俘傅讣弗涪袱市甫釜脯腑阜咐黼砩苻趺跗蚨芾鲋幞茯滏蜉拊菔蝠鳆蝮绂绋赙罘稃匐麸凫桴莩孚馥驸怫祓呋郛芙艴"};
static const char lv_pinyin_mb_ga    [] = {"噶胳夹嘎咖轧钆伽旮尬尕尜呷"};
static const char lv_pinyin_mb_gai   [] = {"该改盖概钙芥溉戤垓丐陔赅"};
static const char lv_pinyin_mb_gan   [] = {"赶干感敢竿甘肝柑杆赣秆旰酐矸疳泔苷擀绀橄澉淦尴坩"};
static const char lv_pinyin_mb_gang  [] = {"刚钢纲港缸岗杠冈肛扛筻罡戆"};
static const char lv_pinyin_mb_gao   [] = {"高搞告稿膏篙羔糕镐皋郜诰杲缟睾槔锆槁藁"};
static const char lv_pinyin_mb_ge    [] = {"个各歌割哥搁格阁隔革咯胳葛蛤戈鸽疙盖屹合铬硌骼颌袼塥虼圪镉仡舸鬲嗝膈搿纥哿"};
static const char lv_pinyin_mb_gei   [] = {"给"};
static const char lv_pinyin_mb_gen   [] = {"跟根哏茛亘艮"};
static const char lv_pinyin_mb_geng  [] = {"更耕颈梗耿庚羹埂赓鲠哽绠"};
static const char lv_pinyin_mb_gong  [] = {"工公功共弓攻宫供恭拱贡躬巩汞龚红肱觥珙蚣"};
static const char lv_pinyin_mb_gou   [] = {"够沟狗钩勾购构苟垢句岣彀枸鞲觏缑笱诟遘媾篝佝"};
static const char lv_pinyin_mb_gu    [] = {"古股鼓谷故孤箍姑顾固雇估咕骨辜沽蛊贾菇梏鸪汩轱崮菰鹄鹘钴臌酤呱鲴诂牯瞽毂锢牿痼觚蛄罟嘏"};
static const char lv_pinyin_mb_gua   [] = {"挂刮瓜寡剐褂卦呱胍鸹栝诖"};
static const char lv_pinyin_mb_guai  [] = {"怪拐乖掴"};
static const char lv_pinyin_mb_guan  [] = {"关管官观馆惯罐灌冠贯棺纶盥矜莞掼涫鳏鹳倌"};
static const char lv_pinyin_mb_guang [] = {"光广逛桄犷咣胱"};
static const char lv_pinyin_mb_gui   [] = {"归贵鬼跪轨规硅桂柜龟诡闺瑰圭刽傀癸炔庋宄桧刿鳜鲑皈匦妫晷簋炅"};
static const char lv_pinyin_mb_gun   [] = {"滚棍辊鲧衮磙绲"};
static const char lv_pinyin_mb_guo   [] = {"过国果裹锅郭涡埚椁聒馘猓崞掴帼呙虢蜾蝈"};
static const char lv_pinyin_mb_ha    [] = {"哈蛤虾铪"};
static const char lv_pinyin_mb_hai   [] = {"还海害咳氦孩骇骸亥嗨醢胲"};
static const char lv_pinyin_mb_han   [] = {"喊含汗寒汉旱酣韩焊涵函憨翰罕撼捍憾悍邯邗菡撖瀚阚顸蚶焓颔晗鼾"};
static const char lv_pinyin_mb_hang  [] = {"行巷航夯杭吭颃沆绗珩"};
static const char lv_pinyin_mb_hao   [] = {"好号浩嚎壕郝毫豪耗貉镐昊颢灏嚆蚝嗥皓蒿濠薅"};
static const char lv_pinyin_mb_he    [] = {"和喝合河禾核何呵荷贺赫褐盒鹤菏貉阂涸吓嗬劾盍翮阖颌壑诃纥曷"};
static const char lv_pinyin_mb_hei   [] = {"黑嘿"};
static const char lv_pinyin_mb_hen   [] = {"很狠恨痕"};
static const char lv_pinyin_mb_heng  [] = {"横恒哼衡亨行桁珩蘅"};
static const char lv_pinyin_mb_hong  [] = {"红轰哄虹洪宏烘鸿弘讧訇蕻闳薨黉荭泓"};
static const char lv_pinyin_mb_hou   [] = {"后厚吼喉侯候猴鲎篌堠後逅糇骺瘊"};
static const char lv_pinyin_mb_hu    [] = {"湖户呼虎壶互胡护糊弧忽狐蝴葫沪乎戏核和瑚唬鹕冱怙鹱笏戽扈鹘浒祜醐琥囫烀轷瓠煳斛鹄猢惚岵滹觳唿槲"};
static const char lv_pinyin_mb_hua   [] = {"话花化画华划滑哗猾豁铧桦骅砉"};
static const char lv_pinyin_mb_huai  [] = {"坏怀淮槐徊划踝"};
static const char lv_pinyin_mb_huan  [] = {"换还唤环患缓欢幻宦涣焕豢桓痪漶獾擐逭鲩郇鬟寰奂锾圜洹萑缳浣"};
static const char lv_pinyin_mb_huang [] = {"黄慌晃荒簧凰皇谎惶蝗磺恍煌幌隍肓潢篁徨鳇"};
static const char lv_pinyin_mb_hui   [] = {"回会灰绘挥汇辉毁悔惠晦徽恢秽慧贿蛔讳徊卉烩诲彗浍珲蕙喙恚哕晖隳麾诙蟪茴洄咴虺荟缋"};
static const char lv_pinyin_mb_hun   [] = {"混昏荤浑婚魂阍珲馄溷诨"};
static const char lv_pinyin_mb_huo   [] = {"或活火伙货和获祸豁霍惑嚯镬耠劐藿攉锪蠖钬夥"};
static const char lv_pinyin_mb_ji    [] = {"几及急既即机鸡积记级极计挤己季寄纪系基激吉脊际击圾畸箕肌饥迹讥姬绩棘辑籍缉集疾汲嫉蓟技冀伎祭剂悸济寂期其奇忌齐妓继给革稽墼洎鲚屐齑戟鲫嵇矶稷戢虮诘笈暨笄剞叽蒺跻嵴掎跽霁唧畿荠瘠玑羁丌偈芨佶赍楫髻咭蕺觊麂"};
static const char lv_pinyin_mb_jia   [] = {"家加假价架甲佳夹嘉驾嫁枷荚颊钾稼茄贾铗葭迦戛浃镓痂恝岬跏嘏伽胛笳珈瘕郏袈蛱"};
static const char lv_pinyin_mb_jian  [] = {"见件减尖间键贱肩兼建检箭煎简剪歼监坚奸健艰荐剑渐溅涧鉴浅践捡柬笺俭碱硷拣舰槛缄茧饯翦鞯戋谏牮枧腱趼缣搛戬毽菅鲣笕谫楗囝蹇裥踺睑謇鹣蒹僭锏湔"};
static const char lv_pinyin_mb_jiang [] = {"将讲江奖降浆僵姜酱蒋疆匠强桨虹豇礓缰犟耩绛茳糨洚"};
static const char lv_pinyin_mb_jiao  [] = {"叫脚交角教较缴觉焦胶娇绞校搅骄狡浇矫郊嚼蕉轿窖椒礁饺铰酵侥剿徼艽僬蛟敫峤跤姣皎茭鹪噍醮佼湫鲛挢"};
static const char lv_pinyin_mb_jie   [] = {"接节街借皆截解界结届姐揭戒介阶劫芥竭洁疥藉价楷秸桔杰捷诫睫家偈桀喈拮骱羯蚧嗟颉鲒婕碣讦孑疖诘"};
static const char lv_pinyin_mb_jin   [] = {"进近今仅紧金斤尽劲禁浸锦晋筋津谨巾襟烬靳廑瑾馑槿衿堇荩矜噤缙卺妗赆觐"};
static const char lv_pinyin_mb_jing  [] = {"竟静井惊经镜京净敬精景警竞境径荆晶鲸粳颈兢茎睛劲痉靖肼獍阱腈弪刭憬婧胫菁儆旌迳靓泾"};
static const char lv_pinyin_mb_jiong [] = {"窘炯扃迥"};
static const char lv_pinyin_mb_jiu   [] = {"就九酒旧久揪救纠舅究韭厩臼玖灸咎疚赳鹫蹴僦柩桕鬏鸠阄啾"};
static const char lv_pinyin_mb_ju    [] = {"句举巨局具距锯剧居聚拘菊矩沮拒惧鞠狙驹且据柜桔俱车咀疽踞炬倨醵裾屦犋苴窭飓锔椐苣琚掬榘龃趄莒雎遽橘踽榉鞫钜讵枸"};
static const char lv_pinyin_mb_juan  [] = {"卷圈倦鹃捐娟眷绢鄄锩蠲镌狷桊涓隽"};
static const char lv_pinyin_mb_jue   [] = {"决绝觉角爵掘诀撅倔抉攫嚼脚桷噱橛嗟觖劂爝矍镢獗珏崛蕨噘谲蹶孓厥"};
static const char lv_pinyin_mb_jun   [] = {"军君均菌俊峻龟竣骏钧浚郡筠麇皲捃"};
static const char lv_pinyin_mb_ka    [] = {"卡喀咯咖胩咔佧"};
static const char lv_pinyin_mb_kai   [] = {"开揩凯慨楷垲剀锎铠锴忾恺蒈"};
static const char lv_pinyin_mb_kan   [] = {"看砍堪刊嵌坎槛勘龛戡侃瞰莰阚"};
static const char lv_pinyin_mb_kang  [] = {"抗炕扛糠康慷亢钪闶伉"};
static const char lv_pinyin_mb_kao   [] = {"靠考烤拷栲犒尻铐"};
static const char lv_pinyin_mb_ke    [] = {"咳可克棵科颗刻课客壳渴苛柯磕坷呵恪岢蝌缂蚵轲窠钶氪颏瞌锞稞珂髁疴嗑溘骒"};
static const char lv_pinyin_mb_ken   [] = {"肯啃恳垦裉"};
static const char lv_pinyin_mb_keng  [] = {"坑吭铿"};
static const char lv_pinyin_mb_kong  [] = {"空孔控恐倥崆箜"};
static const char lv_pinyin_mb_kou   [] = {"口扣抠寇蔻芤眍筘叩"};
static const char lv_pinyin_mb_ku    [] = {"哭库苦枯裤窟酷刳骷喾堀绔"};
static const char lv_pinyin_mb_kua   [] = {"跨垮挎夸胯侉"};
static const char lv_pinyin_mb_kuai  [] = {"快块筷会侩哙蒯浍郐狯脍"};
static const char lv_pinyin_mb_kuan  [] = {"宽款髋"};
static const char lv_pinyin_mb_kuang [] = {"矿筐狂框况旷匡眶诳邝纩夼诓圹贶哐"};
static const char lv_pinyin_mb_kui   [] = {"亏愧奎窥溃葵魁馈盔傀岿匮愦揆睽跬聩篑喹逵暌蒉悝喟馗蝰隗夔"};
static const char lv_pinyin_mb_kun   [] = {"捆困昆坤鲲锟髡琨醌阃悃"};
static const char lv_pinyin_mb_kuo   [] = {"阔扩括廓适蛞栝"};
static const char lv_pinyin_mb_la    [] = {"拉啦辣蜡腊喇垃蓝落瘌邋砬剌旯"};
static const char lv_pinyin_mb_lai   [] = {"来赖莱崃徕涞濑赉睐铼癞籁"};
static const char lv_pinyin_mb_lan   [] = {"蓝兰烂拦篮懒栏揽缆滥阑谰婪澜览榄岚褴镧斓罱漤"};
static const char lv_pinyin_mb_lang  [] = {"浪狼廊郎朗榔琅稂螂莨啷锒阆蒗"};
static const char lv_pinyin_mb_lao   [] = {"老捞牢劳烙涝落姥酪络佬潦耢铹醪铑唠栳崂痨"};
static const char lv_pinyin_mb_le    [] = {"了乐勒肋仂叻泐鳓"};
static const char lv_pinyin_mb_lei   [] = {"类累泪雷垒勒擂蕾肋镭儡磊缧诔耒酹羸嫘檑嘞"};
static const char lv_pinyin_mb_leng  [] = {"冷棱楞塄愣"};
static const char lv_pinyin_mb_li    [] = {"里离力立李例哩理利梨厘礼历丽吏犁黎篱狸漓鲤莉荔栗厉励砾傈俐痢粒沥隶璃俪俚郦坜苈莅蓠鹂笠鳢缡跞蜊锂悝澧粝枥蠡鬲呖砺嫠篥疠疬猁藜溧鲡戾栎唳醴轹詈骊罹逦喱雳黧蛎娌"};
static const char lv_pinyin_mb_lia   [] = {"俩"};
static const char lv_pinyin_mb_lian  [] = {"连联练莲恋脸炼链敛怜廉帘镰涟蠊琏殓蔹鲢奁潋臁裢濂裣楝"};
static const char lv_pinyin_mb_liang [] = {"两亮辆凉粮梁量良晾谅俩粱墚靓踉椋魉莨"};
static const char lv_pinyin_mb_liao  [] = {"了料撩聊撂疗廖燎辽僚寥镣潦钌蓼尥寮缭獠鹩嘹"};
static const char lv_pinyin_mb_lie   [] = {"列裂猎劣烈咧埒捩鬣趔躐冽洌"};
static const char lv_pinyin_mb_lin   [] = {"林临淋邻磷鳞赁吝拎琳霖凛遴嶙蔺粼麟躏辚廪懔瞵檩膦啉"};
static const char lv_pinyin_mb_ling  [] = {"另令领零铃玲灵岭龄凌陵菱伶羚棱翎蛉苓绫瓴酃呤泠棂柃鲮聆囹"};
static const char lv_pinyin_mb_liu   [] = {"六流留刘柳溜硫瘤榴琉馏碌陆绺锍鎏镏浏骝旒鹨熘遛"};
static const char lv_pinyin_mb_long  [] = {"龙拢笼聋隆垄弄咙窿陇垅胧珑茏泷栊癃砻"};
static const char lv_pinyin_mb_lou   [] = {"楼搂漏陋露娄篓偻蝼镂蒌耧髅喽瘘嵝"};
static const char lv_pinyin_mb_lu    [] = {"路露录鹿陆炉卢鲁卤芦颅庐碌掳绿虏赂戮潞禄麓六鲈栌渌蓼逯泸轳氇簏橹辂垆胪噜"};
static const char lv_pinyin_mb_luan  [] = {"乱卵滦峦孪挛栾銮脔娈鸾"};
static const char lv_pinyin_mb_lue   [] = {"略掠锊"};
static const char lv_pinyin_mb_lun   [] = {"论轮抡伦沦仑纶囵"};
static const char lv_pinyin_mb_luo   [] = {"落罗锣裸骡烙箩螺萝洛骆逻络咯荦漯蠃雒倮硌椤捋脶瘰摞泺珞镙猡"};
static const char lv_pinyin_mb_lv    [] = {"绿率铝驴旅屡滤吕律氯缕侣虑履偻膂榈闾捋褛稆"};
static const char lv_pinyin_mb_ma    [] = {"吗妈马嘛麻骂抹码玛蚂摩唛蟆犸嬷杩"};
static const char lv_pinyin_mb_mai   [] = {"买卖迈埋麦脉劢霾荬"};
static const char lv_pinyin_mb_man   [] = {"满慢瞒漫蛮蔓曼馒埋谩幔鳗墁螨镘颟鞔缦熳"};
static const char lv_pinyin_mb_mang  [] = {"忙芒盲莽茫氓硭邙蟒漭"};
static const char lv_pinyin_mb_mao   [] = {"毛冒帽猫矛卯貌茂贸铆锚茅耄茆瑁蝥髦懋昴牦瞀峁袤蟊旄泖"};
static const char lv_pinyin_mb_me    [] = {"么麽"};
static const char lv_pinyin_mb_mei   [] = {"没每煤镁美酶妹枚霉玫眉梅寐昧媒糜媚谜沫嵋猸袂湄浼鹛莓魅镅楣"};
static const char lv_pinyin_mb_men   [] = {"门们闷扪焖懑钔"};
static const char lv_pinyin_mb_meng  [] = {"猛梦蒙锰孟盟檬萌氓礞蜢勐懵甍蠓虻朦艋艨瞢"};
static const char lv_pinyin_mb_mi    [] = {"米密迷眯蜜谜觅秘弥幂靡糜泌醚蘼縻咪汨麋祢猕弭谧芈脒宓敉嘧糸"};
static const char lv_pinyin_mb_mian  [] = {"面棉免绵眠缅勉冕娩腼湎眄沔黾渑"};
static const char lv_pinyin_mb_miao  [] = {"秒苗庙妙描瞄藐渺眇缪缈邈淼喵杪鹋"};
static const char lv_pinyin_mb_mie   [] = {"灭蔑咩篾蠛乜"};
static const char lv_pinyin_mb_min   [] = {"民抿敏闽皿悯珉愍缗闵玟苠泯黾鳘岷"};
static const char lv_pinyin_mb_ming  [] = {"名明命鸣铭螟盟冥瞑暝茗溟酩"};
static const char lv_pinyin_mb_miu   [] = {"谬缪"};
static const char lv_pinyin_mb_mo    [] = {"摸磨抹末膜墨没莫默魔模摩摹漠陌脉沫万无冒寞秣蓦麽茉嘿嬷蘑瘼殁镆嫫谟貊貘馍耱"};
static const char lv_pinyin_mb_mou   [] = {"某谋牟眸蛑鍪侔缪哞"};
static const char lv_pinyin_mb_mu    [] = {"木母亩幕目墓牧牟模穆暮牡拇募慕睦姆姥钼毪坶沐仫苜"};
static const char lv_pinyin_mb_na    [] = {"那拿哪纳钠娜呐南衲捺镎肭"};
static const char lv_pinyin_mb_nai   [] = {"乃耐奶奈氖哪萘艿柰鼐佴"};
static const char lv_pinyin_mb_nan   [] = {"难南男赧囡蝻楠喃腩"};
static const char lv_pinyin_mb_nang  [] = {"囊馕曩囔攮"};
static const char lv_pinyin_mb_nao   [] = {"闹脑恼挠淖孬铙瑙垴呶蛲猱硇"};
static const char lv_pinyin_mb_ne    [] = {"呢哪那呐讷"};
static const char lv_pinyin_mb_nei   [] = {"内哪馁那"};
static const char lv_pinyin_mb_nen   [] = {"嫩恁"};
static const char lv_pinyin_mb_neng  [] = {"能"};
static const char lv_pinyin_mb_ni    [] = {"你泥拟腻逆呢溺倪尼匿妮霓铌昵坭祢猊伲怩鲵睨旎"};
static const char lv_pinyin_mb_nian  [] = {"年念捻撵拈碾蔫粘廿黏辇鲇鲶埝"};
static const char lv_pinyin_mb_niang [] = {"娘酿"};
static const char lv_pinyin_mb_niao  [] = {"鸟尿袅茑脲嬲"};
static const char lv_pinyin_mb_nie   [] = {"捏镍聂孽涅镊啮陧蘖嗫臬蹑颞乜"};
static const char lv_pinyin_mb_nin   [] = {"您恁"};
static const char lv_pinyin_mb_ning  [] = {"拧凝宁柠狞泞佞甯咛聍"};
static const char lv_pinyin_mb_niu   [] = {"牛扭纽钮狃忸妞拗"};
static const char lv_pinyin_mb_nong  [] = {"弄浓农脓哝侬"};
static const char lv_pinyin_mb_nu    [] = {"怒努奴孥胬驽弩"};
static const char lv_pinyin_mb_nuan  [] = {"暖"};
static const char lv_pinyin_mb_nue   [] = {"虐疟谑"};
static const char lv_pinyin_mb_nuo   [] = {"挪诺懦糯娜喏傩锘搦"};
static const char lv_pinyin_mb_nv    [] = {"女钕恧衄"};
static const char lv_pinyin_mb_o     [] = {"哦喔噢"};
static const char lv_pinyin_mb_ou    [] = {"偶呕欧藕鸥区沤殴怄瓯讴耦"};
static const char lv_pinyin_mb_pa    [] = {"怕爬趴啪耙扒帕琶派筢杷葩"};
static const char lv_pinyin_mb_pai   [] = {"派排拍牌迫徘湃哌俳蒎"};
static const char lv_pinyin_mb_pan   [] = {"盘盼判攀畔潘叛磐番般胖襻蟠袢泮拚爿蹒"};
static const char lv_pinyin_mb_pang  [] = {"旁胖耪庞乓膀磅滂彷逄螃"};
static const char lv_pinyin_mb_pao   [] = {"跑抛炮泡刨袍咆狍匏庖疱脬"};
static const char lv_pinyin_mb_pei   [] = {"陪配赔呸胚佩培沛裴旆锫帔醅霈辔"};
static const char lv_pinyin_mb_pen   [] = {"喷盆湓"};
static const char lv_pinyin_mb_peng  [] = {"碰捧棚砰蓬朋彭鹏烹硼膨抨澎篷怦堋蟛嘭"};
static const char lv_pinyin_mb_pi    [] = {"批皮披匹劈辟坯屁脾僻疲痞霹琵毗啤譬砒否貔丕圮媲癖仳擗郫甓枇睥蜱鼙邳吡陂铍庀罴埤纰陴淠噼蚍裨"};
static const char lv_pinyin_mb_pian  [] = {"片篇骗偏便扁翩缏犏骈胼蹁谝"};
static const char lv_pinyin_mb_piao  [] = {"票飘漂瓢朴螵莩嫖瞟殍缥嘌骠剽"};
static const char lv_pinyin_mb_pie   [] = {"瞥撇苤氕"};
static const char lv_pinyin_mb_pin   [] = {"品贫聘拼频嫔榀姘牝颦"};
static const char lv_pinyin_mb_ping  [] = {"平凭瓶评屏乒萍苹坪冯娉鲆枰俜"};
static const char lv_pinyin_mb_po    [] = {"破坡颇婆泼迫泊魄朴繁粕笸皤钋陂鄱攴叵珀钷"};
static const char lv_pinyin_mb_pou   [] = {"剖掊裒"};
static const char lv_pinyin_mb_pu    [] = {"扑铺谱脯仆蒲葡朴菩曝莆瀑埔圃浦堡普暴镨噗匍溥濮氆蹼璞镤僕"};
static const char lv_pinyin_mb_qi    [] = {"起其七气期齐器妻骑汽棋奇欺漆启戚柒岂砌弃泣祁凄企乞契歧祈栖畦脐崎稽迄缉沏讫旗祺颀骐屺岐蹊萁蕲桤憩芪荠萋芑汔亟鳍俟槭嘁蛴綦亓欹琪麒琦蜞圻杞葺碛淇祗耆绮"};
static const char lv_pinyin_mb_qia   [] = {"恰卡掐洽袷葜髂"};
static const char lv_pinyin_mb_qian  [] = {"前钱千牵浅签欠铅嵌钎迁钳乾谴谦潜歉纤扦遣黔堑仟岍钤褰箝掮搴倩慊悭愆虔芡荨缱佥芊阡肷茜椠犍骞"};
static const char lv_pinyin_mb_qiang [] = {"强枪墙抢腔呛羌蔷将蜣跄戗襁戕炝镪锖锵羟樯嫱"};
static const char lv_pinyin_mb_qiao  [] = {"桥瞧敲巧翘锹壳鞘撬悄俏窍雀乔侨峭橇樵荞跷硗憔谯鞒愀缲诮劁"};
static const char lv_pinyin_mb_qie   [] = {"切且怯窃茄砌郄趄惬锲妾箧慊伽挈"};
static const char lv_pinyin_mb_qin   [] = {"亲琴侵勤擒寝秦芹沁禽钦吣覃矜衾芩溱廑嗪螓噙揿檎锓"};
static const char lv_pinyin_mb_qing  [] = {"请轻清青情晴氢倾庆擎顷亲卿氰圊謦檠箐苘蜻黥罄鲭磬綮"};
static const char lv_pinyin_mb_qiong [] = {"穷琼跫穹邛蛩茕銎筇"};
static const char lv_pinyin_mb_qiu   [] = {"求球秋丘泅仇邱囚酋龟楸蚯裘糗蝤巯逑俅虬赇鳅犰湫鼽遒"};
static const char lv_pinyin_mb_qu    [] = {"去取区娶渠曲趋趣屈驱蛆躯龋戌蠼蘧祛蕖磲劬诎鸲阒麴癯衢黢璩氍觑蛐朐瞿岖苣"};
static const char lv_pinyin_mb_quan  [] = {"全权劝圈拳犬泉券颧痊醛铨筌绻诠辁畎鬈悛蜷荃"};
static const char lv_pinyin_mb_que   [] = {"却缺确雀瘸鹊炔榷阙阕悫"};
static const char lv_pinyin_mb_qun   [] = {"群裙逡麇"};
static const char lv_pinyin_mb_ran   [] = {"染燃然冉髯苒蚺"};
static const char lv_pinyin_mb_rang  [] = {"让嚷瓤攘壤穰禳"};
static const char lv_pinyin_mb_rao   [] = {"饶绕扰荛桡娆"};
static const char lv_pinyin_mb_re    [] = {"热若惹喏"};
static const char lv_pinyin_mb_ren   [] = {"人任忍认刃仁韧妊纫壬饪轫仞荏葚衽稔"};
static const char lv_pinyin_mb_reng  [] = {"扔仍"};
static const char lv_pinyin_mb_ri    [] = {"日"};
static const char lv_pinyin_mb_rong  [] = {"容绒融溶熔荣戎蓉冗茸榕狨嵘肜蝾"};
static const char lv_pinyin_mb_rou   [] = {"肉揉柔糅蹂鞣"};
static const char lv_pinyin_mb_ru    [] = {"如入汝儒茹乳褥辱蠕孺蓐襦铷嚅缛濡薷颥溽洳"};
static const char lv_pinyin_mb_ruan  [] = {"软阮朊"};
static const char lv_pinyin_mb_rui   [] = {"瑞蕊锐睿芮蚋枘蕤"};
static const char lv_pinyin_mb_run   [] = {"闰润"};
static const char lv_pinyin_mb_ruo   [] = {"若弱偌箬"};
static const char lv_pinyin_mb_sa    [] = {"撒洒萨挲仨卅飒脎"};
static const char lv_pinyin_mb_sai   [] = {"塞腮鳃思赛噻"};
static const char lv_pinyin_mb_san   [] = {"三散伞叁馓糁毵霰"};
static const char lv_pinyin_mb_sang  [] = {"桑丧嗓颡磉搡"};
static const char lv_pinyin_mb_sao   [] = {"扫嫂搔骚梢埽鳋臊缫瘙"};
static const char lv_pinyin_mb_se    [] = {"色涩瑟塞啬铯穑"};
static const char lv_pinyin_mb_sen   [] = {"森"};
static const char lv_pinyin_mb_seng  [] = {"僧"};
static const char lv_pinyin_mb_sha   [] = {"杀沙啥纱傻砂刹莎厦煞杉嗄唼鲨霎铩痧裟挲歃"};
static const char lv_pinyin_mb_shai  [] = {"晒筛色酾"};
static const char lv_pinyin_mb_shan  [] = {"山闪衫善扇杉删煽单珊掺赡栅苫掸膳陕汕擅缮嬗蟮芟禅跚鄯潸鳝姗剡骟疝膻讪钐舢埏"};
static const char lv_pinyin_mb_shang [] = {"上伤尚商赏晌墒汤裳熵觞绱殇垧"};
static const char lv_pinyin_mb_shao  [] = {"少烧捎哨勺梢稍邵韶绍芍召鞘苕劭潲艄蛸筲"};
static const char lv_pinyin_mb_she   [] = {"社射蛇设舌摄舍折涉赊赦慑奢歙厍畲猞麝滠佘"};
static const char lv_pinyin_mb_shen  [] = {"身伸深婶神甚渗肾审申沈绅呻参砷什娠慎葚糁莘诜谂矧椹渖蜃哂胂"};
static const char lv_pinyin_mb_sheng [] = {"声省剩生升绳胜盛圣甥牲乘晟渑眚笙嵊"};
static const char lv_pinyin_mb_shi   [] = {"是使十时事室市石师试史式识虱矢拾屎驶始似嘘示士世柿匙拭誓逝势什殖峙嗜噬失适仕侍释饰氏狮食恃蚀视实施湿诗尸豕莳埘铈舐鲥鲺贳轼蓍筮炻谥弑酾螫"};
static const char lv_pinyin_mb_shou  [] = {"手受收首守瘦授兽售熟寿艏狩绶"};
static const char lv_pinyin_mb_shu   [] = {"书树数熟输梳叔属束术述蜀黍鼠淑赎孰蔬疏戍竖墅庶薯漱恕枢暑殊抒曙署舒姝摅秫"};
static const char lv_pinyin_mb_shua  [] = {"刷耍唰"};
static const char lv_pinyin_mb_shuai [] = {"摔甩率帅衰蟀"};
static const char lv_pinyin_mb_shuan [] = {"栓拴闩涮"};
static const char lv_pinyin_mb_shuang[] = {"双霜爽泷孀"};
static const char lv_pinyin_mb_shui  [] = {"水谁睡税说"};
static const char lv_pinyin_mb_shun  [] = {"顺吮瞬舜"};
static const char lv_pinyin_mb_shuo  [] = {"说数硕烁朔搠妁槊蒴铄"};
static const char lv_pinyin_mb_si    [] = {"四死丝撕似私嘶思寺司斯食伺厕肆饲嗣巳耜驷兕蛳厮汜锶泗笥咝鸶姒厶缌祀澌俟"};
static const char lv_pinyin_mb_song  [] = {"送松耸宋颂诵怂讼竦菘淞悚嵩凇崧忪"};
static const char lv_pinyin_mb_sou   [] = {"艘搜擞嗽嗾嗖飕叟薮锼馊瞍溲螋擞"};
static const char lv_pinyin_mb_su    [] = {"素速诉塑宿俗苏肃粟酥缩溯僳愫簌觫稣夙嗉谡蔌涑"};
static const char lv_pinyin_mb_suan  [] = {"酸算蒜狻"};
static const char lv_pinyin_mb_sui   [] = {"岁随碎虽穗遂尿隋髓绥隧祟眭谇濉邃燧荽睢"};
static const char lv_pinyin_mb_sun   [] = {"孙损笋榫荪飧狲隼"};
static const char lv_pinyin_mb_suo   [] = {"所缩锁琐索梭蓑莎唆挲睃嗍唢桫嗦娑羧"};
static const char lv_pinyin_mb_ta    [] = {"他她它踏塔塌拓獭挞蹋溻趿鳎沓榻漯遢铊闼嗒"};
static const char lv_pinyin_mb_tai   [] = {"太抬台态胎苔泰酞汰炱肽跆呔鲐钛薹邰骀"};
static const char lv_pinyin_mb_tan   [] = {"谈叹探滩弹碳摊潭贪坛痰毯坦炭瘫谭坍檀袒郯昙忐钽锬澹镡覃"};
static const char lv_pinyin_mb_tang  [] = {"躺趟堂糖汤塘烫倘淌唐搪棠膛螳樘羰醣瑭镗傥饧溏耥帑铴螗"};
static const char lv_pinyin_mb_tao   [] = {"套掏逃桃讨淘涛滔陶绦萄鼗啕洮韬饕叨焘"};
static const char lv_pinyin_mb_te    [] = {"特忑慝铽忒"};
static const char lv_pinyin_mb_teng  [] = {"疼腾藤誊滕"};
static const char lv_pinyin_mb_ti    [] = {"提替体题踢蹄剃剔梯锑啼涕嚏惕屉醍鹈绨缇倜裼逖荑悌"};
static const char lv_pinyin_mb_tian  [] = {"天田添填甜舔恬腆佃掭钿阗忝殄畋"};
static const char lv_pinyin_mb_tiao  [] = {"条跳挑调迢眺龆笤祧蜩髫佻窕鲦苕粜"};
static const char lv_pinyin_mb_tie   [] = {"铁贴帖萜餮"};
static const char lv_pinyin_mb_ting  [] = {"听停挺厅亭艇庭廷烃汀莛葶婷梃蜓霆町铤"};
static const char lv_pinyin_mb_tong  [] = {"同通痛铜桶筒捅统童彤桐瞳恫侗酮潼茼仝砼峒恸佟嗵垌"};
static const char lv_pinyin_mb_tou   [] = {"头偷透投钭骰"};
static const char lv_pinyin_mb_tu    [] = {"土图兔涂吐秃突徒凸途屠堍荼菟钍酴"};
static const char lv_pinyin_mb_tuan  [] = {"团湍疃抟彖"};
static const char lv_pinyin_mb_tui   [] = {"腿推退褪颓蜕煺忒"};
static const char lv_pinyin_mb_tun   [] = {"吞屯褪臀囤氽饨暾豚"};
static const char lv_pinyin_mb_tuo   [] = {"拖脱托妥驮拓驼椭唾鸵陀魄乇佗坨庹沱柝柁橐跎箨酡砣鼍"};
static const char lv_pinyin_mb_wa    [] = {"挖瓦蛙哇娃洼凹袜佤娲腽"};
static const char lv_pinyin_mb_wai   [] = {"外歪崴"};
static const char lv_pinyin_mb_wan   [] = {"完万晚碗玩弯挽湾丸腕宛婉烷顽豌惋娩皖蔓莞脘蜿绾芄琬纨剜畹菀"};
static const char lv_pinyin_mb_wang  [] = {"望忘王往网亡枉旺汪妄芒罔惘辋魍"};
static const char lv_pinyin_mb_wei   [] = {"为位未围喂胃微味尾伪威伟卫危违委魏唯维畏惟韦巍蔚谓尉潍纬慰桅萎苇渭遗葳帏艉鲔娓逶闱隈沩玮涠帷崴隗诿洧偎猥猬嵬軎韪炜煨圩薇痿"};
static const char lv_pinyin_mb_wen   [] = {"问文闻稳温吻蚊纹瘟紊汶阌刎雯璺"};
static const char lv_pinyin_mb_weng  [] = {"翁嗡瓮蓊蕹"};
static const char lv_pinyin_mb_wo    [] = {"我握窝卧挝沃蜗涡斡倭幄龌肟莴喔渥硪"};
static const char lv_pinyin_mb_wu    [] = {"无五屋物舞雾误捂污悟勿钨武戊务呜伍吴午吾侮乌毋恶诬芜巫晤梧坞妩蜈牾寤兀怃阢邬忤骛於鋈仵杌鹜婺迕痦芴焐唔庑鹉鼯浯圬"};
static const char lv_pinyin_mb_xi    [] = {"西洗细吸戏系喜席稀溪熄锡膝息袭惜习嘻夕悉矽熙希檄牺晰昔媳硒铣烯腊析隙栖汐犀蜥奚浠葸饩屣玺嬉禊兮翕穸禧僖淅蓰舾蹊醯欷皙蟋羲茜徙隰郗唏曦螅歙樨阋粞熹觋菥鼷裼舄"};
static const char lv_pinyin_mb_xia   [] = {"下吓夏峡虾瞎霞狭匣侠辖厦暇唬狎遐瑕柙硖罅黠呷"};
static const char lv_pinyin_mb_xian  [] = {"先线县现显掀闲献嫌陷险鲜弦衔馅限咸锨仙腺贤纤宪舷涎羡铣见苋藓岘痫莶籼娴蚬猃祆冼燹跣跹酰暹氙鹇筅霰"};
static const char lv_pinyin_mb_xiang [] = {"想向象项响香乡相像箱巷享镶厢降翔祥橡详湘襄芗葙饷庠骧缃蟓鲞飨"};
static const char lv_pinyin_mb_xiao  [] = {"小笑消削销萧效宵晓肖孝硝啸霄哮嚣校骁哓潇逍枭绡淆崤箫枵筱魈蛸"};
static const char lv_pinyin_mb_xie   [] = {"写些鞋歇斜血谢卸挟屑蟹泻懈泄楔邪协械谐蝎携胁解契叶绁颉缬獬榭廨撷偕瀣渫亵榍邂薤躞燮勰"};
static const char lv_pinyin_mb_xin   [] = {"新心欣信芯薪锌辛寻衅忻歆囟莘馨鑫昕镡"};
static const char lv_pinyin_mb_xing  [] = {"性行型形星醒姓腥刑杏兴幸邢猩惺省硎悻荥陉擤荇"};
static const char lv_pinyin_mb_xiong [] = {"胸雄凶兄熊汹匈芎"};
static const char lv_pinyin_mb_xiu   [] = {"修锈绣休羞宿嗅袖秀朽臭咻岫馐庥溴鸺貅髹"};
static const char lv_pinyin_mb_xu    [] = {"许须需虚嘘蓄续序叙畜絮婿戌徐旭绪吁酗恤墟糈勖栩浒蓿顼圩洫胥醑诩溆煦盱砉"};
static const char lv_pinyin_mb_xuan  [] = {"选悬旋玄宣喧轩绚眩癣券儇炫谖萱揎泫渲漩璇楦暄煊碹铉镟痃"};
static const char lv_pinyin_mb_xue   [] = {"学雪血靴穴削薛踅噱鳕泶"};
static const char lv_pinyin_mb_xun   [] = {"寻讯熏训循殉旬巡迅驯汛逊勋荤询浚巽埙荀蕈薰峋徇獯恂洵浔曛醺鲟郇窨"};
static const char lv_pinyin_mb_ya    [] = {"呀压牙押芽鸭轧崖哑亚涯丫雅衙鸦讶蚜伢垭揠岈迓娅琊桠氩砑睚痖疋"};
static const char lv_pinyin_mb_yan   [] = {"眼烟沿盐言演严咽淹炎掩厌宴岩研延堰验艳阉砚雁唁彦焰蜒衍谚燕颜阎焉奄厣菸魇琰滟焱筵赝兖恹檐湮谳偃胭晏闫俨郾鄢妍崦嫣罨酽餍鼹铅殷阽芫阏腌剡"};
static const char lv_pinyin_mb_yang  [] = {"样养羊洋仰扬秧氧痒杨漾阳殃央鸯佯疡炀恙徉鞅泱蛘烊怏"};
static const char lv_pinyin_mb_yao   [] = {"要摇药咬腰窑舀邀妖谣遥姚瑶耀尧钥侥夭爻吆崾徭幺珧杳轺曜肴鹞窈鳐疟陶约铫"};
static const char lv_pinyin_mb_ye    [] = {"也夜业野叶爷页液掖腋冶噎耶咽曳椰邪谒邺晔烨揶铘靥"};
static const char lv_pinyin_mb_yi    [] = {"一以已亿衣移依易医乙仪亦椅益倚姨翼译伊蛇遗食艾胰疑沂宜异彝壹蚁谊揖铱矣翌艺抑绎邑蛾屹尾役臆逸肄疫颐裔意毅忆义夷溢诣议怿痍镒癔怡驿旖熠酏翊欹峄圯殪嗌咦懿噫劓诒饴漪佚咿瘗猗眙羿弈苡荑仡佾贻钇缢迤刈悒黟翳弋奕蜴埸挹嶷薏呓轶镱"};
static const char lv_pinyin_mb_yin   [] = {"因引印银音饮阴隐荫吟尹寅茵淫殷姻烟堙鄞喑夤胤龈吲圻狺垠霪蚓氤铟窨瘾洇茚"};
static const char lv_pinyin_mb_ying  [] = {"应硬影营迎映蝇赢鹰英颖莹盈婴樱缨荧萤萦楹蓥瘿茔鹦媵莺璎郢嘤撄瑛滢潆嬴罂瀛膺荥颍"};
static const char lv_pinyin_mb_yo    [] = {"哟育唷"};
static const char lv_pinyin_mb_yong  [] = {"用涌永拥蛹勇雍咏泳佣踊痈庸臃恿俑壅墉喁慵邕镛甬鳙饔"};
static const char lv_pinyin_mb_you   [] = {"有又由右油游幼优友铀忧尤犹诱悠邮酉佑幽釉攸卣侑莠莜莸呦囿宥柚猷牖铕疣蚰蚴蝣鱿黝鼬蝤繇"};
static const char lv_pinyin_mb_yu    [] = {"与于欲鱼雨余遇语愈狱玉渔予誉育愚羽虞娱淤舆屿禹宇迂俞逾域芋郁谷吁盂喻峪御愉粥渝尉榆隅浴寓裕预豫驭蔚妪嵛雩馀阈窬鹆妤揄窳觎臾舁龉蓣煜钰谀纡於竽瑜禺聿欤俣伛圄鹬庾昱萸瘐谕鬻圉瘀熨饫毓燠腴狳菀蜮蝓"};
static const char lv_pinyin_mb_yuan  [] = {"远员元院圆原愿园援猿怨冤源缘袁渊苑垣鸳辕圜鼋橼媛爰眢鸢掾芫沅瑗螈箢塬垸"};
static const char lv_pinyin_mb_yue   [] = {"月越约跃阅乐岳悦曰说粤钥瀹钺刖龠栎樾"};
static const char lv_pinyin_mb_yun   [] = {"云运晕允匀韵陨孕耘蕴酝郧员熨氲恽愠郓芸筠韫昀狁殒纭"};
static const char lv_pinyin_mb_za    [] = {"杂砸咋匝扎咱咂拶"};
static const char lv_pinyin_mb_zai   [] = {"在再灾载栽宰哉崽甾仔"};
static const char lv_pinyin_mb_zan   [] = {"咱暂攒赞簪趱糌瓒拶昝錾"};
static const char lv_pinyin_mb_zang  [] = {"脏葬赃藏臧驵奘"};
static const char lv_pinyin_mb_zao   [] = {"早造遭糟灶燥枣凿躁藻皂噪澡蚤唣"};
static const char lv_pinyin_mb_ze    [] = {"则责择泽咋侧箦舴帻迮啧仄昃笮赜"};
static const char lv_pinyin_mb_zei   [] = {"贼"};
static const char lv_pinyin_mb_zen   [] = {"怎谮"};
static const char lv_pinyin_mb_zeng  [] = {"增赠憎曾综缯罾甑锃"};
static const char lv_pinyin_mb_zha   [] = {"扎炸渣闸眨榨乍轧诈喳札铡揸吒咤哳猹砟痄蚱齄查蜡栅咋喋楂柞"};
static const char lv_pinyin_mb_zhai  [] = {"摘窄债斋寨择翟宅侧祭砦瘵"};
static const char lv_pinyin_mb_zhan  [] = {"站占战盏沾粘毡展栈詹颤蘸湛绽斩辗崭瞻谵搌旃"};
static const char lv_pinyin_mb_zhang [] = {"张章长帐仗丈掌涨账樟杖彰漳胀瘴障仉嫜幛鄣璋嶂獐蟑"};
static const char lv_pinyin_mb_zhao  [] = {"找着照招罩爪兆朝昭沼肇嘲召赵棹钊笊诏啁"};
static const char lv_pinyin_mb_zhe   [] = {"着这者折遮蛰哲蔗锗辙浙柘辄赭摺鹧磔褶蜇谪"};
static const char lv_pinyin_mb_zhen  [] = {"真阵镇针震枕振斟珍疹诊甄砧臻贞侦缜蓁祯箴轸榛稹赈朕鸩胗浈桢畛圳椹"};
static const char lv_pinyin_mb_zheng [] = {"正整睁争挣征怔证症郑拯蒸狰政帧峥钲铮筝诤徵丁鲭"};
static const char lv_pinyin_mb_zhi   [] = {"只之直知制指纸支芝枝稚吱蜘质肢脂汁炙织职痔植抵殖执值侄址智滞止趾治旨窒志挚掷至致置帜识峙氏秩帙摭黹桎枳轵忮祉蛭膣觯郅栀彘芷祗咫鸷絷踬胝骘轾痣陟踯雉埴贽卮酯豸跖栉伎"};
static const char lv_pinyin_mb_zhong [] = {"中重种钟肿众终盅忠仲衷踵舯螽锺冢忪"};
static const char lv_pinyin_mb_zhou  [] = {"周洲皱粥州轴舟昼骤宙诌肘帚咒繇胄纣荮啁碡绉籀妯酎"};
static const char lv_pinyin_mb_zhu   [] = {"住主猪竹株煮筑著珠蛛朱诸诛逐烛拄瞩嘱柱助蛀贮铸注祝驻伫潴洙瘃翥茱苎橥舳杼箸炷侏铢疰渚褚躅麈邾槠竺属术"};
static const char lv_pinyin_mb_zhua  [] = {"抓爪挝"};
static const char lv_pinyin_mb_zhuai [] = {"拽转"};
static const char lv_pinyin_mb_zhuan [] = {"转专砖赚传撰篆颛馔啭沌"};
static const char lv_pinyin_mb_zhuang[] = {"装撞庄壮桩状幢妆僮奘戆"};
static const char lv_pinyin_mb_zhui  [] = {"追坠缀锥赘椎骓惴缒隹"};
static const char lv_pinyin_mb_zhun  [] = {"准谆屯肫窀"};
static const char lv_pinyin_mb_zhuo  [] = {"捉桌着啄拙灼浊卓琢缴茁酌擢焯濯诼浞涿倬镯禚斫"};
static const char lv_pinyin_mb_zi    [] = {"字自子紫籽资姿吱滓仔咨孜渍滋淄谘茈嵫姊孳缁梓辎赀恣眦锱秭耔笫粢趑訾龇鲻髭兹觜"};
static const char lv_pinyin_mb_zong  [] = {"总纵宗棕综踪鬃偬粽枞腙"};
static const char lv_pinyin_mb_zou   [] = {"走揍奏邹鲰鄹陬驺诹"};
static const char lv_pinyin_mb_zu    [] = {"组族足阻租祖诅菹镞卒俎"};
static const char lv_pinyin_mb_zuan  [] = {"钻纂赚攥缵躜"};
static const char lv_pinyin_mb_zui   [] = {"最嘴醉罪堆咀觜蕞"};
static const char lv_pinyin_mb_zun   [] = {"尊遵撙樽鳟"};
static const char lv_pinyin_mb_zuo   [] = {"做作坐左座昨凿佐阼唑怍胙祚撮琢嘬笮酢柞"};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_a[] = {
    {"", lv_pinyin_mb_a},
    {"i", lv_pinyin_mb_ai},
    {"n", lv_pinyin_mb_an},
    {"ng", lv_pinyin_mb_ang},
    {"o", lv_pinyin_mb_ao},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_b[] = {
    {"a", lv_pinyin_mb_ba},
    {"ai", lv_pinyin_mb_bai},
    {"an", lv_pinyin_mb_ban},
    {"ang", lv_pinyin_mb_bang},
    {"ao", lv_pinyin_mb_bao},
    {"ei", lv_pinyin_mb_bei},
    {"en", lv_pinyin_mb_ben},
    {"eng", lv_pinyin_mb_beng},
    {"i", lv_pinyin_mb_bi},
    {"ian", lv_pinyin_mb_bian},
    {"iao", lv_pinyin_mb_biao},
    {"ie", lv_pinyin_mb_bie},
    {"in", lv_pinyin_mb_bin},
    {"ing", lv_pinyin_mb_bing},
    {"o", lv_pinyin_mb_bo},
    {"u", lv_pinyin_mb_bu},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_c[] = {
    {"a", lv_pinyin_mb_ca},
    {"ai", lv_pinyin_mb_cai},
    {"an", lv_pinyin_mb_can},
    {"ang", lv_pinyin_mb_cang},
    {"ao", lv_pinyin_mb_cao},
    {"e", lv_pinyin_mb_ce},
    {"en", lv_pinyin_mb_cen},
    {"eng", lv_pinyin_mb_ceng},
    {"ha", lv_pinyin_mb_cha},
    {"hai", lv_pinyin_mb_chai},
    {"han", lv_pinyin_mb_chan},
    {"hang", lv_pinyin_mb_chang},
    {"hao", lv_pinyin_mb_chao},
    {"he", lv_pinyin_mb_che},
    {"hen", lv_pinyin_mb_chen},
    {"heng", lv_pinyin_mb_cheng},
    {"hi", lv_pinyin_mb_chi},
    {"hong", lv_pinyin_mb_chong},
    {"hou", lv_pinyin_mb_chou},
    {"hu", lv_pinyin_mb_chu},
    {"huai", lv_pinyin_mb_chuai},
    {"huan", lv_pinyin_mb_chuan},
    {"huang", lv_pinyin_mb_chuang},
    {"hui", lv_pinyin_mb_chui},
    {"hun", lv_pinyin_mb_chun},
    {"huo", lv_pinyin_mb_chuo},
    {"i", lv_pinyin_mb_ci},
    {"ong", lv_pinyin_mb_cong},
    {"ou", lv_pinyin_mb_cou},
    {"u", lv_pinyin_mb_cu},
    {"uan", lv_pinyin_mb_cuan},
    {"ui", lv_pinyin_mb_cui},
    {"un", lv_pinyin_mb_cun},
    {"uo", lv_pinyin_mb_cuo},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_d[] = {
    {"a   ", lv_pinyin_mb_da},
    {"ai", lv_pinyin_mb_dai},
    {"an", lv_pinyin_mb_dan},
    {"ang", lv_pinyin_mb_dang},
    {"ao", lv_pinyin_mb_dao},
    {"e", lv_pinyin_mb_de},
    {"eng", lv_pinyin_mb_deng},
    {"i", lv_pinyin_mb_di},
    {"ia", lv_pinyin_mb_dia},
    {"ian", lv_pinyin_mb_dian},
    {"iao", lv_pinyin_mb_diao},
    {"ie", lv_pinyin_mb_die},
    {"ing", lv_pinyin_mb_ding},
    {"iu", lv_pinyin_mb_diu},
    {"ong", lv_pinyin_mb_dong},
    {"ou", lv_pinyin_mb_dou},
    {"u", lv_pinyin_mb_du},
    {"uan", lv_pinyin_mb_duan},
    {"ui", lv_pinyin_mb_dui},
    {"un", lv_pinyin_mb_dun},
    {"uo", lv_pinyin_mb_duo},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_e[] = {
    {"", lv_pinyin_mb_e},
    {"n", lv_pinyin_mb_en},
    {"r", lv_pinyin_mb_er},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_f[] = {
    {"a", lv_pinyin_mb_fa},
    {"an", lv_pinyin_mb_fan},
    {"ang", lv_pinyin_mb_fang},
    {"ei", lv_pinyin_mb_fei},
    {"en", lv_pinyin_mb_fen},
    {"eng", lv_pinyin_mb_feng},
    {"o", lv_pinyin_mb_fo},
    {"ou", lv_pinyin_mb_fou},
    {"u", lv_pinyin_mb_fu},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_g[] = {
    {"a", lv_pinyin_mb_ga},
    {"ai", lv_pinyin_mb_gai},
    {"an", lv_pinyin_mb_gan},
    {"ang", lv_pinyin_mb_gang},
    {"ao", lv_pinyin_mb_gao},
    {"e", lv_pinyin_mb_ge},
    {"ei", lv_pinyin_mb_gei},
    {"en", lv_pinyin_mb_gen},
    {"eng", lv_pinyin_mb_geng},
    {"ong", lv_pinyin_mb_gong},
    {"ou", lv_pinyin_mb_gou},
    {"u", lv_pinyin_mb_gu},
    {"ua", lv_pinyin_mb_gua},
    {"uai", lv_pinyin_mb_guai},
    {"uan", lv_pinyin_mb_guan},
    {"uang", lv_pinyin_mb_guang},
    {"ui", lv_pinyin_mb_gui},
    {"un", lv_pinyin_mb_gun},
    {"uo", lv_pinyin_mb_guo},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_h[] = {
    {"a", lv_pinyin_mb_ha},
    {"ai", lv_pinyin_mb_hai},
    {"an", lv_pinyin_mb_han},
    {"ang", lv_pinyin_mb_hang},
    {"ao", lv_pinyin_mb_hao},
    {"e", lv_pinyin_mb_he},
    {"ei", lv_pinyin_mb_hei},
    {"en", lv_pinyin_mb_hen},
    {"eng", lv_pinyin_mb_heng},
    {"ong", lv_pinyin_mb_hong},
    {"ou", lv_pinyin_mb_hou},
    {"u", lv_pinyin_mb_hu},
    {"ua", lv_pinyin_mb_hua},
    {"uai", lv_pinyin_mb_huai},
    {"uan", lv_pinyin_mb_huan},
    {"uang ", lv_pinyin_mb_huang},
    {"ui", lv_pinyin_mb_hui},
    {"un", lv_pinyin_mb_hun},
    {"uo", lv_pinyin_mb_huo},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_i[] = {
    {"", lv_pinyin_mb_space},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_j[] = {
    {"i", lv_pinyin_mb_ji},
    {"ia", lv_pinyin_mb_jia},
    {"ian", lv_pinyin_mb_jian},
    {"iang", lv_pinyin_mb_jiang},
    {"iao", lv_pinyin_mb_jiao},
    {"ie", lv_pinyin_mb_jie},
    {"in", lv_pinyin_mb_jin},
    {"ing", lv_pinyin_mb_jing},
    {"iong", lv_pinyin_mb_jiong},
    {"iu", lv_pinyin_mb_jiu},
    {"u", lv_pinyin_mb_ju},
    {"uan", lv_pinyin_mb_juan},
    {"ue", lv_pinyin_mb_jue},
    {"un", lv_pinyin_mb_jun},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_k[] = {
    {"a", lv_pinyin_mb_ka},
    {"ai", lv_pinyin_mb_kai},
    {"an", lv_pinyin_mb_kan},
    {"ang", lv_pinyin_mb_kang},
    {"ao", lv_pinyin_mb_kao},
    {"e", lv_pinyin_mb_ke},
    {"en", lv_pinyin_mb_ken},
    {"eng", lv_pinyin_mb_keng},
    {"ong", lv_pinyin_mb_kong},
    {"ou", lv_pinyin_mb_kou},
    {"u", lv_pinyin_mb_ku},
    {"ua", lv_pinyin_mb_kua},
    {"uai", lv_pinyin_mb_kuai},
    {"uan", lv_pinyin_mb_kuan},
    {"uang", lv_pinyin_mb_kuang},
    {"ui", lv_pinyin_mb_kui},
    {"un", lv_pinyin_mb_kun},
    {"uo", lv_pinyin_mb_kuo},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_l[] = {
    {"a", lv_pinyin_mb_la},
    {"ai", lv_pinyin_mb_lai},
    {"an", lv_pinyin_mb_lan},
    {"ang", lv_pinyin_mb_lang},
    {"ao", lv_pinyin_mb_lao},
    {"e", lv_pinyin_mb_le},
    {"ei", lv_pinyin_mb_lei},
    {"eng", lv_pinyin_mb_leng},
    {"i", lv_pinyin_mb_li},
    {"ia", lv_pinyin_mb_lia},
    {"ian", lv_pinyin_mb_lian},
    {"iang", lv_pinyin_mb_liang},
    {"iao", lv_pinyin_mb_liao},
    {"ie", lv_pinyin_mb_lie},
    {"in", lv_pinyin_mb_lin},
    {"ing", lv_pinyin_mb_ling},
    {"iu", lv_pinyin_mb_liu},
    {"ong", lv_pinyin_mb_long},
    {"ou", lv_pinyin_mb_lou},
    {"u", lv_pinyin_mb_lu},
    {"uan", lv_pinyin_mb_luan},
    {"ue", lv_pinyin_mb_lue},
    {"un", lv_pinyin_mb_lun},
    {"uo", lv_pinyin_mb_luo},
    {"v", lv_pinyin_mb_lv},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_m[] = {
    {"a", lv_pinyin_mb_ma},
    {"ai", lv_pinyin_mb_mai},
    {"an", lv_pinyin_mb_man},
    {"ang", lv_pinyin_mb_mang},
    {"ao", lv_pinyin_mb_mao},
    {"e", lv_pinyin_mb_me},
    {"ei", lv_pinyin_mb_mei},
    {"en", lv_pinyin_mb_men},
    {"eng", lv_pinyin_mb_meng},
    {"i", lv_pinyin_mb_mi},
    {"ian", lv_pinyin_mb_mian},
    {"iao", lv_pinyin_mb_miao},
    {"ie", lv_pinyin_mb_mie},
    {"in", lv_pinyin_mb_min},
    {"ing", lv_pinyin_mb_ming},
    {"iu", lv_pinyin_mb_miu},
    {"o", lv_pinyin_mb_mo},
    {"ou", lv_pinyin_mb_mou},
    {"u", lv_pinyin_mb_mu},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_n[] = {
    {"a", lv_pinyin_mb_na},
    {"ai", lv_pinyin_mb_nai},
    {"an", lv_pinyin_mb_nan},
    {"ang", lv_pinyin_mb_nang},
    {"ao", lv_pinyin_mb_nao},
    {"e", lv_pinyin_mb_ne},
    {"ei", lv_pinyin_mb_nei},
    {"en", lv_pinyin_mb_nen},
    {"eng", lv_pinyin_mb_neng},
    {"i", lv_pinyin_mb_ni},
    {"ian", lv_pinyin_mb_nian},
    {"iang", lv_pinyin_mb_niang},
    {"iao", lv_pinyin_mb_niao},
    {"ie", lv_pinyin_mb_nie},
    {"in", lv_pinyin_mb_nin},
    {"ing", lv_pinyin_mb_ning},
    {"iu", lv_pinyin_mb_niu},
    {"ong", lv_pinyin_mb_nong},
    {"u", lv_pinyin_mb_nu},
    {"uan", lv_pinyin_mb_nuan},
    {"ue", lv_pinyin_mb_nue},
    {"uo", lv_pinyin_mb_nuo},
    {"v", lv_pinyin_mb_nv},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_o[] = {
    {"", lv_pinyin_mb_o},
    {"u", lv_pinyin_mb_ou}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_p[] = {
    {"a", lv_pinyin_mb_pa},
    {"ai", lv_pinyin_mb_pai},
    {"an", lv_pinyin_mb_pan},
    {"ang", lv_pinyin_mb_pang},
    {"ao", lv_pinyin_mb_pao},
    {"ei", lv_pinyin_mb_pei},
    {"en", lv_pinyin_mb_pen},
    {"eng", lv_pinyin_mb_peng},
    {"i", lv_pinyin_mb_pi},
    {"ian", lv_pinyin_mb_pian},
    {"iao", lv_pinyin_mb_piao},
    {"ie", lv_pinyin_mb_pie},
    {"in", lv_pinyin_mb_pin},
    {"ing", lv_pinyin_mb_ping},
    {"o", lv_pinyin_mb_po},
    {"ou", lv_pinyin_mb_pou},
    {"u", lv_pinyin_mb_pu},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_q[] = {
    {"i", lv_pinyin_mb_qi},
    {"ia", lv_pinyin_mb_qia},
    {"ian", lv_pinyin_mb_qian},
    {"iang", lv_pinyin_mb_qiang},
    {"iao", lv_pinyin_mb_qiao},
    {"ie", lv_pinyin_mb_qie},
    {"in", lv_pinyin_mb_qin},
    {"ing", lv_pinyin_mb_qing},
    {"iong", lv_pinyin_mb_qiong},
    {"iu", lv_pinyin_mb_qiu},
    {"u", lv_pinyin_mb_qu},
    {"uan", lv_pinyin_mb_quan},
    {"ue", lv_pinyin_mb_que},
    {"un", lv_pinyin_mb_qun},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_r[] = {
    {"an", lv_pinyin_mb_ran},
    {"ang", lv_pinyin_mb_rang},
    {"ao", lv_pinyin_mb_rao},
    {"e", lv_pinyin_mb_re},
    {"en", lv_pinyin_mb_ren},
    {"eng", lv_pinyin_mb_reng},
    {"i", lv_pinyin_mb_ri},
    {"ong", lv_pinyin_mb_rong},
    {"ou", lv_pinyin_mb_rou},
    {"u", lv_pinyin_mb_ru},
    {"uan", lv_pinyin_mb_ruan},
    {"ui", lv_pinyin_mb_rui},
    {"un", lv_pinyin_mb_run},
    {"uo", lv_pinyin_mb_ruo},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_s[] = {
    {"a", lv_pinyin_mb_sa},
    {"ai", lv_pinyin_mb_sai},
    {"an", lv_pinyin_mb_san},
    {"ang", lv_pinyin_mb_sang},
    {"ao", lv_pinyin_mb_sao},
    {"e", lv_pinyin_mb_se},
    {"en", lv_pinyin_mb_sen},
    {"eng", lv_pinyin_mb_seng},
    {"ha", lv_pinyin_mb_sha},
    {"hai", lv_pinyin_mb_shai},
    {"han", lv_pinyin_mb_shan},
    {"hang ", lv_pinyin_mb_shang},
    {"hao", lv_pinyin_mb_shao},
    {"he", lv_pinyin_mb_she},
    {"hen", lv_pinyin_mb_shen},
    {"heng", lv_pinyin_mb_sheng},
    {"hi", lv_pinyin_mb_shi},
    {"hou", lv_pinyin_mb_shou},
    {"hu", lv_pinyin_mb_shu},
    {"hua", lv_pinyin_mb_shua},
    {"huai", lv_pinyin_mb_shuai},
    {"huan", lv_pinyin_mb_shuan},
    {"huang", lv_pinyin_mb_shuang},
    {"hui", lv_pinyin_mb_shui},
    {"hun", lv_pinyin_mb_shun},
    {"huo", lv_pinyin_mb_shuo},
    {"i", lv_pinyin_mb_si},
    {"ong", lv_pinyin_mb_song},
    {"ou", lv_pinyin_mb_sou},
    {"u", lv_pinyin_mb_su},
    {"uan", lv_pinyin_mb_suan},
    {"ui", lv_pinyin_mb_sui},
    {"un", lv_pinyin_mb_sun},
    {"uo", lv_pinyin_mb_suo},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_t[] = {
    {"a", lv_pinyin_mb_ta},
    {"ai", lv_pinyin_mb_tai},
    {"an", lv_pinyin_mb_tan},
    {"ang", lv_pinyin_mb_tang},
    {"ao", lv_pinyin_mb_tao},
    {"e", lv_pinyin_mb_te},
    {"eng", lv_pinyin_mb_teng},
    {"i", lv_pinyin_mb_ti},
    {"ian", lv_pinyin_mb_tian},
    {"iao", lv_pinyin_mb_tiao},
    {"ie", lv_pinyin_mb_tie},
    {"ing", lv_pinyin_mb_ting},
    {"ong", lv_pinyin_mb_tong},
    {"ou", lv_pinyin_mb_tou},
    {"u", lv_pinyin_mb_tu},
    {"uan", lv_pinyin_mb_tuan},
    {"ui", lv_pinyin_mb_tui},
    {"un", lv_pinyin_mb_tun},
    {"uo", lv_pinyin_mb_tuo},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_u[] = {
    {"", lv_pinyin_mb_space},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_v[] = {
    {"", lv_pinyin_mb_space},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_w[] = {
    {"a", lv_pinyin_mb_wa},
    {"ai", lv_pinyin_mb_wai},
    {"an", lv_pinyin_mb_wan},
    {"ang", lv_pinyin_mb_wang},
    {"ei", lv_pinyin_mb_wei},
    {"en", lv_pinyin_mb_wen},
    {"eng", lv_pinyin_mb_weng},
    {"o", lv_pinyin_mb_wo},
    {"u", lv_pinyin_mb_wu},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_x[] = {
    {"i", lv_pinyin_mb_xi},
    {"ia", lv_pinyin_mb_xia},
    {"ian", lv_pinyin_mb_xian},
    {"iang", lv_pinyin_mb_xiang},
    {"iao", lv_pinyin_mb_xiao},
    {"ie", lv_pinyin_mb_xie},
    {"in", lv_pinyin_mb_xin},
    {"ing", lv_pinyin_mb_xing},
    {"iong", lv_pinyin_mb_xiong},
    {"iu", lv_pinyin_mb_xiu},
    {"u", lv_pinyin_mb_xu},
    {"uan", lv_pinyin_mb_xuan},
    {"ue", lv_pinyin_mb_xue},
    {"un", lv_pinyin_mb_xun},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_y[] = {
    {"a", lv_pinyin_mb_ya},
    {"an", lv_pinyin_mb_yan},
    {"ang", lv_pinyin_mb_yang},
    {"ao", lv_pinyin_mb_yao},
    {"e", lv_pinyin_mb_ye},
    {"i", lv_pinyin_mb_yi},
    {"in", lv_pinyin_mb_yin},
    {"ing", lv_pinyin_mb_ying},
    {"o", lv_pinyin_mb_yo},
    {"ong", lv_pinyin_mb_yong},
    {"ou", lv_pinyin_mb_you},
    {"u", lv_pinyin_mb_yu},
    {"uan", lv_pinyin_mb_yuan},
    {"ue", lv_pinyin_mb_yue},
    {"un", lv_pinyin_mb_yun},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_z[] = {
    {"a", lv_pinyin_mb_za},
    {"ai", lv_pinyin_mb_zai},
    {"an", lv_pinyin_mb_zan},
    {"ang", lv_pinyin_mb_zang},
    {"ao", lv_pinyin_mb_zao},
    {"e", lv_pinyin_mb_ze},
    {"ei", lv_pinyin_mb_zei},
    {"en", lv_pinyin_mb_zen},
    {"eng", lv_pinyin_mb_zeng},
    {"ha", lv_pinyin_mb_zha},
    {"hai", lv_pinyin_mb_zhai},
    {"han", lv_pinyin_mb_zhan},
    {"hang", lv_pinyin_mb_zhang},
    {"hao", lv_pinyin_mb_zhao},
    {"he", lv_pinyin_mb_zhe},
    {"hen", lv_pinyin_mb_zhen},
    {"heng", lv_pinyin_mb_zheng},
    {"hi", lv_pinyin_mb_zhi},
    {"hong", lv_pinyin_mb_zhong},
    {"hou", lv_pinyin_mb_zhou},
    {"hu", lv_pinyin_mb_zhu},
    {"hua", lv_pinyin_mb_zhua},
    {"huai", lv_pinyin_mb_zhuai},
    {"huan", lv_pinyin_mb_zhuan},
    {"huang", lv_pinyin_mb_zhuang},
    {"hui", lv_pinyin_mb_zhui},
    {"hun", lv_pinyin_mb_zhun},
    {"huo", lv_pinyin_mb_zhuo},
    {"i", lv_pinyin_mb_zi},
    {"ong", lv_pinyin_mb_zong},
    {"ou", lv_pinyin_mb_zou},
    {"u", lv_pinyin_mb_zu},
    {"uan", lv_pinyin_mb_zuan},
    {"ui", lv_pinyin_mb_zui},
    {"un", lv_pinyin_mb_zun},
    {"uo", lv_pinyin_mb_zuo},
    {NULL, NULL}
};

static const lv_keyboard_pinyin_dict_t lv_pinyin_dict_end[] = {
    {"", lv_pinyin_mb_space},
    {NULL, NULL}
};

const lv_keyboard_pinyin_dict_t * lv_def_pinyin_dict[] = {
    lv_pinyin_dict_a,
    lv_pinyin_dict_b,
    lv_pinyin_dict_c,
    lv_pinyin_dict_d,
    lv_pinyin_dict_e,
    lv_pinyin_dict_f,
    lv_pinyin_dict_g,
    lv_pinyin_dict_h,
    lv_pinyin_dict_i,
    lv_pinyin_dict_j,
    lv_pinyin_dict_k,
    lv_pinyin_dict_l,
    lv_pinyin_dict_m,
    lv_pinyin_dict_n,
    lv_pinyin_dict_o,
    lv_pinyin_dict_p,
    lv_pinyin_dict_q,
    lv_pinyin_dict_r,
    lv_pinyin_dict_s,
    lv_pinyin_dict_t,
    lv_pinyin_dict_u,
    lv_pinyin_dict_v,
    lv_pinyin_dict_w,
    lv_pinyin_dict_x,
    lv_pinyin_dict_y,
    lv_pinyin_dict_z,
    lv_pinyin_dict_end
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/*=====================
 * Other functions
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif  /*LV_KEYBOARD_PINYIN_USE_DEFAULT_DICT*/
