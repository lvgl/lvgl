<p align="right">
  <b>עברית</b> | <a href="./README_zh.md">中文</a> | <a href="./README_pt_BR.md">Português do Brasil</a> | <a href="./README_jp.md">日本語</a> | <a href="./README.md">English</a>
</p>

<div dir="rtl">

# תיעוד

תיעוד LVGL נכתב ב‑**MDX**. האתר שמרנדר את הדפים הללו מתוחזק בנפרד על ידי צוות LVGL — התורמים כאן כותבים וסוקרים תוכן `.mdx` תחת `./src/`.

> **דמות Docker לתצוגה מקומית — *בקרוב*.** אנו נפרסם דמות ב‑GHCR (‏`ghcr.io/lvgl/lvgl-docs` — תג מדויק ייקבע בהמשך) שכוללת את אתר התיעוד ומגישה את תוכן `./src/` בכתובת `http://localhost:3000`.


## הכול חייב להיות מתועד

המדיניות הנחרצת שלנו: ***הכול חייב להיות מתועד***. כל ה‑API הציבורי, הוויג'טים, הפיצ'רים ואפשרויות ההגדרה דורשים דף או מקטע ב‑MDX. במקרה של ספק — הוסיפו תיעוד.


## היכן נמצא התוכן

כל דפי ה‑MDX נמצאים תחת `./src/`. מבנה התיקיות קובע את ה‑URL, וסדר סרגל הצד נשלט על ידי קובצי `meta.json` בתוך כל תיקייה.

דף הוא קובץ `.mdx` עם frontmatter בפורמט YAML:

</div>

```mdx
---
title: Animations
description: Animate widget properties over time with the LVGL animation engine.
---

# Animations

Animations change a property's value over a period of time...

<LvglExample name="lv_example_anim_1" path="anim/lv_example_anim_1" />
```

<div dir="rtl">

## רכיבים נפוצים

קבוצה קטנה של רכיבים מכסה את רוב צורכי הכתיבה:

- `<Callout type="info|warn|error" title="...">` — הערות, אזהרות, טיפים.
- `<LvglExample name="..." path="..." />` — מטמיע דוגמת קוד ניתנת להרצה.
- `<ApiLink name="lv_label_create" />` — קישור שורתי לסימבול API.
- `<ApiLinkList items={["lv_label"]} />` — קישורים לדפי API קשורים בתחתית הדף.
- `<Figure src="/_static/images/..." alt="..." caption="..." />` — תמונות עם כיתוב.
- `<DirectoryIndex />` — רשימה שנוצרת אוטומטית של דפי צאצא בדפי אינדקס.

**קטלוג הרכיבים המלא**, כללי העיצוב, דוגמאות `meta.json` ורשימת האייקונים נמצאים ב‑[Writing Docs](./src/contributing/writing_docs.mdx). התחילו שם עבור כל דבר מעבר לרכיבים שלמעלה.


## עוד על תרומה לפרויקט

לסגנון קוד, DCO, Pull Requests והנחיות תרומה נוספות — ראו את הדפים תחת [`./src/contributing/`](./src/contributing).

</div>
