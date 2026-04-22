<p align="right">
  <a href="./README.md">English</a> | <a href="./README_zh.md">中文</a> | <a href="./README_pt_BR.md">Português do Brasil</a> | <b>日本語</b> | <a href="./README_he.md">עברית</a>
</p>

# ドキュメント

LVGL のドキュメントは **MDX** で記述されています。これらのページを配信するサイトは LVGL チームが別途メンテナンスしており、本ディレクトリの貢献者は `./src/` 配下の `.mdx` コンテンツの執筆とレビューを担当します。

> **ローカルプレビュー用 Docker イメージ — *近日公開*。** GHCR にイメージ（`ghcr.io/lvgl/lvgl-docs` — タグは調整中）を公開する予定です。このイメージはドキュメントサイトを同梱しており、`http://localhost:3000` でこの `./src/` のコンテンツを配信します。


## すべてを文書化すること

LVGL の揺るぎない方針：***すべてを文書化すること***。公開 API、ウィジェット、機能、設定オプションにはすべて MDX のページまたはセクションが必要です。迷ったら追加してください。


## コンテンツの配置場所

すべての MDX ページは `./src/` 配下にあります。ディレクトリ構造が URL を決定し、サイドバーの順序は各ディレクトリ内の `meta.json` ファイルで制御します。

ページは YAML frontmatter 付きの `.mdx` ファイルです：

```mdx
---
title: Animations
description: Animate widget properties over time with the LVGL animation engine.
---

# Animations

Animations change a property's value over a period of time...

<LvglExample name="lv_example_anim_1" path="anim/lv_example_anim_1" />
```


## よく使うコンポーネント

以下の少数のコンポーネントで、執筆に必要なほとんどのケースに対応できます：

- `<Callout type="info|warn|error" title="...">` — 注記、警告、ヒント。
- `<LvglExample name="..." path="..." />` — 実行可能なコード例を埋め込みます。
- `<ApiLink name="lv_label_create" />` — API シンボルへのインラインリンク。
- `<ApiLinkList items={["lv_label"]} />` — ページ末尾で関連 API ページへリンクします。
- `<Figure src="/_static/images/..." alt="..." caption="..." />` — キャプション付きの画像。
- `<DirectoryIndex />` — インデックスページで子ページの一覧を自動生成します。

**コンポーネントの完全な一覧**、書式ルール、`meta.json` の例、アイコン一覧は [Writing Docs](./src/contributing/writing_docs.mdx) を参照してください。上記以外のことはそこから始めてください。


## 貢献に関するその他の情報

コーディングスタイル、DCO、プルリクエスト、その他の貢献ガイドラインについては [`./src/contributing/`](./src/contributing) 配下のページを参照してください。
