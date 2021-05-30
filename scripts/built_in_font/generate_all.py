#!/usr/bin/env python3.6

import os

print("Generating 8 px")
os.system("./built_in_font_gen.py --size 8 -o lv_font_montserrat_8.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_8.c')

print("\nGenerating 10 px")
os.system("./built_in_font_gen.py --size 10 -o lv_font_montserrat_10.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_10.c')

print("\nGenerating 12 px")
os.system("./built_in_font_gen.py --size 12 -o lv_font_montserrat_12.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_12.c')

print("\nGenerating 14 px")
os.system("./built_in_font_gen.py --size 14 -o lv_font_montserrat_14.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_14.c')

print("\nGenerating 16 px")
os.system("./built_in_font_gen.py --size 16 -o lv_font_montserrat_16.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_16.c')

print("\nGenerating 18 px")
os.system("./built_in_font_gen.py --size 18 -o lv_font_montserrat_18.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_18.c')

print("\nGenerating 20 px")
os.system("./built_in_font_gen.py --size 20 -o lv_font_montserrat_20.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_20.c')

print("\nGenerating 22 px")
os.system("./built_in_font_gen.py --size 22 -o lv_font_montserrat_22.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_22.c')

print("\nGenerating 24 px")
os.system("./built_in_font_gen.py --size 24 -o lv_font_montserrat_24.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_24.c')

print("\nGenerating 26 px")
os.system("./built_in_font_gen.py --size 26 -o lv_font_montserrat_26.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_26.c')

print("\nGenerating 28 px")
os.system("./built_in_font_gen.py --size 28 -o lv_font_montserrat_28.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_28.c')

print("\nGenerating 30 px")
os.system("./built_in_font_gen.py --size 30 -o lv_font_montserrat_30.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_30.c')

print("\nGenerating 32 px")
os.system("./built_in_font_gen.py --size 32 -o lv_font_montserrat_32.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_32.c')

print("\nGenerating 34 px")
os.system("./built_in_font_gen.py --size 34 -o lv_font_montserrat_34.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_34.c')

print("\nGenerating 36 px")
os.system("./built_in_font_gen.py --size 36 -o lv_font_montserrat_36.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_36.c')

print("\nGenerating 38 px")
os.system("./built_in_font_gen.py --size 38 -o lv_font_montserrat_38.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_38.c')

print("\nGenerating 40 px")
os.system("./built_in_font_gen.py --size 40 -o lv_font_montserrat_40.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_40.c')

print("\nGenerating 42 px")
os.system("./built_in_font_gen.py --size 42 -o lv_font_montserrat_42.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_42.c')

print("\nGenerating 44 px")
os.system("./built_in_font_gen.py --size 44 -o lv_font_montserrat_44.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_44.c')

print("\nGenerating 46 px")
os.system("./built_in_font_gen.py --size 46 -o lv_font_montserrat_46.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_46.c')

print("\nGenerating 48 px")
os.system("./built_in_font_gen.py --size 48 -o lv_font_montserrat_48.c --bpp 4")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_48.c')

print("\nGenerating 12 px subpx")
os.system("./built_in_font_gen.py --size 12 -o lv_font_montserrat_12_subpx.c --bpp 4 --subpx")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_12_subpx.c')

print("\nGenerating 28 px compressed")
os.system("./built_in_font_gen.py --size 28 -o lv_font_montserrat_28_compressed.c --bpp 4 --compressed")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_montserrat_28_compressed.c')

print("\nGenerating 16 px CJK")
os.system(u"./built_in_font_gen.py --size 16 -o lv_font_simsun_16_cjk.c --bpp 4 --font SimSun.woff -r 0x20-0x7f --symbols （），盗提陽帯鼻画輕ッ冊ェル写父ぁフ結想正四O夫源庭場天續鳥れ講猿苦階給了製守8祝己妳薄泣塩帰ぺ吃変輪那着仍嗯爭熱創味保字宿捨準查達肯ァ薬得査障該降察ね網加昼料等図邪秋コ態品屬久原殊候路願楽確針上被怕悲風份重歡っ附ぷ既4黨價娘朝凍僅際洋止右航よ专角應酸師個比則響健昇豐筆歷適修據細忙跟管長令家ザ期般花越ミ域泳通些油乏ラ。營ス返調農叫樹刊愛間包知把ヤ貧橋拡普聞前ジ建当繰ネ送習渇用補ィ覺體法遊宙ョ酔余利壊語くつ払皆時辺追奇そ們只胸械勝住全沈力光ん深溝二類北面社值試9和五勵ゃ貿幾逐打課ゲて領3鼓辦発評１渉詳暇込计駄供嘛郵頃腦反構絵お容規借身妻国慮剛急乗静必議置克土オ乎荷更肉還混古渡授合主離條値決季晴東大尚央州が嗎験流先医亦林田星晩拿60旅婦量為痛テ孫う環友況玩務其ぼち揺坐一肩腰犯タょ希即果ぶ物練待み高九找やヶ都グ去」サ、气仮雑酒許終企笑録形リ銀切ギ快問滿役単黄集森毎實研喜蘇司鉛洲川条媽ノ才兩話言雖媒出客づ卻現異故り誌逮同訊已視本題ぞを横開音第席費持眾怎選元退限ー賽処喝就残無いガ多ケ沒義遠歌隣錢某雪析嬉採自透き側員予ゼ白婚电へ顯呀始均畫似懸格車騒度わ親店週維億締慣免帳電甚來園浴ゅ愈京と杯各海怒ぜ排敗挙老買7極模実紀ヒ携隻告シ並屋這孩讓質ワブ富賃争康由辞マ火於短樣削弟材注節另室ダ招擁ぃ若套底波行勤關著泊背疲狭作念推ぐ民貸祖介說ビ代温契你我レ入描變再札ソ派頭智遅私聽舉灣山伸放直安ト誕煙付符幅ふ絡她届耳飲忘参革團仕様載ど歩獲嫌息の汚交興魚指資雙與館初学年幸史位柱族走括び考青也共腕Lで販擔理病イ今逃當寺猫邊菓係ム秘示解池影ド文例斷曾事茶寫明科桃藝売便え導禁財飛替而亡到し具空寝辛業ウ府セ國何基菜厳市努張缺雲根外だ断万砂ゴ超使台实ぽ礼最慧算軟界段律像夕丈窓助刻月夏政呼ぴざ擇趣除動従涼方勉名線対存請子氏將5少否諸論美感或西者定食御表は參歳緑命進易性錯房も捕皿判中觀戦ニ緩町ピ番ず金千ろ?不た象治関ャ每看徒卒統じ手範訪押座步号ベ旁以母すほ密減成往歲件緒読歯效院种七謂凝濃嵌震喉繼クュ拭死円2積水欲如ポにさ寒道區精啦姐ア聯能足及停思壓２春且メ裏株官答概黒過氷柿戻厚ぱ党祭織引計け委暗複誘港バ失下村較続神ぇ尤強秀膝兒来績十書済化服破新廠1紹您情半式產系好教暑早め樂地休協良な哪常要揮周かエ麗境働避護ンツ香夜太見設非改広聲他検求危清彼經未在起葉控靴所差內造寄南望尺換向展備眠點完約ぎ裡分説申童優伝島机須塊日立拉,鉄軽單気信很転識支布数紙此迎受心輸坊モ處「訳三曇兄野顔戰增ナ伊列又髪両有取左毛至困吧昔赤狀相夠整別士経頼然簡ホ会發隨営需脱ヨば接永居冬迫圍甘醫誰部充消連弱宇會咲覚姉麼的増首统帶糖朋術商担移景功育庫曲總劃牛程駅犬報ロ學責因パ嚴八世後平負公げ曜陸專午之閉ぬ談ご災昨冷職悪謝對它近射敢意運船臉局難什産頗!球真記ま但蔵究制機案湖臺ひ害券男留内木驗雨施種特復句末濟キ色訴依せ百型る石牠討呢时任執飯歐宅組傳配小活ゆべ暖ズ漸站素らボ束価チ浅回女片独妹英目從認生違策僕楚ペ米こ掛む爸六状落漢プ投カ校做啊洗声探あ割体項履触々訓技ハ低工映是標速善点人デ口次可".encode('utf-8'))
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_simsun_16_cjk.c')

print("\nGenerating 16 px Hebrew, Persian")
os.system("./built_in_font_gen.py --size 16 -o lv_font_dejavu_16_persian_hebrew.c --bpp 4 --font DejaVuSans.ttf -r 0x20-0x7f,0x5d0-0x5ea,0x600-0x6FF,0xFB50-0xFDFF,0xFE70-0xFEFF")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_dejavu_16_persian_hebrew.c')

print("\nGenerating 8 px unscii")
os.system("lv_font_conv --no-compress --no-prefilter --bpp 1 --size 8 --font unscii-8.ttf -r 0x20-0x7F --format lvgl -o lv_font_unscii_8.c --force-fast-kern-format")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_unscii_8.c')

print("\nGenerating 16 px unscii")
os.system("lv_font_conv --no-compress --no-prefilter --bpp 1 --size 16 --font unscii-8.ttf -r 0x20-0x7F --format lvgl -o lv_font_unscii_16.c --force-fast-kern-format")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_unscii_16.c')


