<p align="right">
  <a href="./README.md">English</a>  |  <b>中文</b> | <a href="./README_pt_BR.md">Português do Brasil</a> | <a href="./README_jp.md">日本語</a> | <a href="./README_he.md">עברית</a>
</p>

# 文档

LVGL 文档以 **MDX** 编写。用于渲染这些页面的站点由 LVGL 团队单独维护——本目录下的贡献者负责在 `./src/` 中编写和审阅 `.mdx` 内容。

> **本地预览 Docker 镜像——*即将推出*。** 我们将在 GHCR 上发布一个镜像（`ghcr.io/lvgl/lvgl-docs`，具体标签待定），该镜像打包了文档站点，并在 `http://localhost:3000` 提供对 `./src/` 内容的访问。


## 所有内容都必须文档化

我们的明确方针：***所有内容都必须文档化***。公开 API、控件、功能和配置选项都需要对应的 MDX 页面或章节。如有疑问，请补充文档。


## 内容存放位置

所有 MDX 页面都位于 `./src/` 下。目录结构决定 URL，侧边栏顺序由各目录内的 `meta.json` 文件控制。

每个页面都是带有 YAML frontmatter 的 `.mdx` 文件：

```mdx
---
title: Animations
description: Animate widget properties over time with the LVGL animation engine.
---

# Animations

Animations change a property's value over a period of time...

<LvglExample name="lv_example_anim_1" path="anim/lv_example_anim_1" />
```


## 常用组件

以下一小组组件可满足大多数编写需求：

- `<Callout type="info|warn|error" title="...">` —— 注释、警告、提示。
- `<LvglExample name="..." path="..." />` —— 嵌入可运行的代码示例。
- `<ApiLink name="lv_label_create" />` —— 指向 API 符号的行内链接。
- `<ApiLinkList items={["lv_label"]} />` —— 在页面末尾链接到相关 API 页面。
- `<Figure src="/_static/images/..." alt="..." caption="..." />` —— 带标题的图片。
- `<DirectoryIndex />` —— 在索引页上自动生成子页面列表。

**完整的组件目录**、格式化规则、`meta.json` 示例以及图标列表请参见 [Writing Docs](./src/contributing/writing_docs.mdx)。除上述组件以外的所有内容都从那里开始查阅。


## 更多贡献指南

关于代码风格、DCO、Pull Request 及其他贡献规范，请参阅 [`./src/contributing/`](./src/contributing) 下的页面。
