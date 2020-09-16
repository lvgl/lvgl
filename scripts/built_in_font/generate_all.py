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
os.system(u"./built_in_font_gen.py --size 16 -o lv_font_simsun_16_cjk.c --bpp 4 --font SimSun.woff -r 0x20-0x7f --symbols 一在有個我不這了他也就人都說而我們你了要會對及和與以很種的大能著她那上但年還可以最自己為來所他們兩各可為或好等又將因為於由從更被才已者每次把三什麼問題其讓此做再所以只與則台灣卻並位想去呢學生表示到公司將如果社會看小天因此新但是它中使工作全覺得使用這些裡並由於時候知道這樣一認為時間事過向可能中國美國到和幾系統政府大家國家許多生活跟已經大學研究因本二活動該世界應四希望方式內項啊下環境一些必須文化高孩子沒有不能如開始元不同仍網路日本用中心來對雖然重要地方進行關係市場太老師提供學校應該指出經濟其他家發展教育成為多非常便方面很多吃然後未發現電腦一樣而且心不過無法企業正服務較不會臺灣曾嗎空間看到五如何國內們無對於以及之後可是當人員比先產品資訊資料比較先生地除了大陸需要像在給歲請月些名另若亦地區技術至特別其實國際不要發生參加一定其中問台北包括講造成看像常即喜歡去沒出現政治話走單位一直吧是否當然整處理歷史了解那怎麼機會家聽所有只要朋友令甚至用真六呀情況還是錢方法點任何經驗藝術你們十主要媽媽增加提出為什麼以您計畫作利用東西在條設計找之間成長能夠決定學習誰見半時代完成帶相當同學件能力別人生命下來場會議容易開發民眾事情書事實有關自組織言多愛建立相關均產生多業者解決完全的話接受知約一般推動過程管理功能手打水要求小朋友教授難我國告訴內容結果調查家庭成立選擇經營然而父母寫人類至於買尤其配合進入例如得討論依作品情形資源原因啦妳運動觀念給軟體品質經過如此嗯精神影響之過好像成參與以後於是部分另外公園透過訓練努力研究具有共同所謂下行為合作經合作目標起來考慮長意見辦法音樂連受廠商隻受到一切或是中央某女性教學極獲得真的路來快國小部份工程女人舉行句只是段根據現象人民土地面對注意這裡新聞繼續相信政策變成計劃強調學人士前前存在制度意義代表課程該沒至需求人生那些成功爸爸產業負責民間雖影響直接幾乎分實際團體價值使得類形成科技這麼當七不但往本身標準似乎應用或者動物電話態度建設事業老那麼常常字坐舉辦自我有的具目的塊條件即使好十分多少放又電影科學執行邊委員會溝通開一起張針對員工引起自然那麼安全總統此外擁有並且事件設計研究所語言嚴重故事學術片設備之外車基本實在久套達到改善死結構住皆改變拿小組支持座醫院既僅值得學者八交通階段就是申請主管申請同感覺電視母親嘛香港記者壓力快樂喝敢院也許人們談生產怕就身體規定程度積極知識作為機構而是鼓勵角色狀況專家據清楚不僅比賽玩效果越保護共開放附近上父親專業經費曾經工作願意分別重視不少歡迎小孩小時中國人顯示中共出男人避免屬於實施聲音主義行動不可只有校園興趣山表現得回來主任裡面經常不再電子受思想頭終於謝謝協助除當地正式真正低性份因素推出上價格去認識方向責任說明工業大量做逐漸心理一點供須簡單運用觀察往往規劃減少重新業務報導仍然感到開放領域有效女要從事發揮人才反而行政銀行公共媒體提高代自然社區力量啊教育部愈超過維持家長結合校長通常缺乏委員特色結果有時教師之前遠控制本否則法少原則要臉通過建議工具作業達節目智慧來自而變化同樣形式站以為健康擔任人口規劃剛特殊原來道分傳統總是前往投資加強不斷對象追求加上比思考製作台北市取得出來加入台安排兒童國中範圍老人雙方牠北京年輕結束教程式婦女找到彼此全球成本回到部而已之下等變期間非小姐整體採用根本叫歐洲正在加以充滿系列隨著早等等頗不足總分析深報告不錯在於旁笑故消費者意識公尺民族為主大眾到底願度大概對方官員發表進一步自由正確豐富國民黨戰爭怎麼樣只好明顯改革表達肯定強高興哪樹適合茶別國外關心蘇聯成績人物聽到創造不必不論尚居民不管美麗伊拉克帶來有般永遠感情兒子這樣子起全部".encode('utf-8'))
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_simsun_16_cjk.c')

print("\nGenerating 16 px Hebrew, Persian")
os.system("./built_in_font_gen.py --size 16 -o lv_font_dejavu_16_persian_hebrew.c --bpp 4 --font DejaVuSans.ttf -r 0x20-0x7f,0x5d0-0x5ea,0x600-0x6FF,0xFB50-0xFDFF,0xFE70-0xFEFF")
os.system('sed -i \'s|#include "lvgl/lvgl.h"|#include "../../lvgl.h"|\' lv_font_dejavu_16_persian_hebrew.c')


