<p align="right">
  <a href="./README.md">English</a> | <a href="./README_zh.md">中文</a> | <b>Português do Brasil</b> | <a href="./README_jp.md">日本語</a> | <a href="./README_he.md">עברית</a>
</p>

# Documentação

A documentação do LVGL é escrita em **MDX**. O site que renderiza essas páginas é mantido separadamente pela equipe do LVGL — os contribuidores aqui escrevem e revisam o conteúdo `.mdx` em `./src/`.

> **Imagem Docker de pré-visualização local — *em breve*.** Publicaremos uma imagem no GHCR (`ghcr.io/lvgl/lvgl-docs` — tag exata a definir) que inclui o site da documentação e serve o conteúdo de `./src/` em `http://localhost:3000`.


## Tudo Deve Estar Documentado

Nossa política firme: ***TUDO DEVE ESTAR DOCUMENTADO***. APIs públicas, widgets, funcionalidades e opções de configuração precisam de uma página ou seção em MDX. Na dúvida, adicione.


## Onde o Conteúdo Fica

Todas as páginas MDX ficam em `./src/`. A estrutura de diretórios define a URL, e a ordem da barra lateral é controlada pelos arquivos `meta.json` dentro de cada diretório.

Uma página é um arquivo `.mdx` com frontmatter YAML:

```mdx
---
title: Animations
description: Animate widget properties over time with the LVGL animation engine.
---

# Animations

Animations change a property's value over a period of time...

<LvglExample name="lv_example_anim_1" path="anim/lv_example_anim_1" />
```


## Componentes Comuns

Um pequeno conjunto de componentes cobre a maior parte das necessidades de autoria:

- `<Callout type="info|warn|error" title="...">` — notas, avisos, dicas.
- `<LvglExample name="..." path="..." />` — incorpora um exemplo de código executável.
- `<ApiLink name="lv_label_create" />` — link inline para um símbolo da API.
- `<ApiLinkList items={["lv_label"]} />` — link para páginas relacionadas da API no final de uma página.
- `<Figure src="/_static/images/..." alt="..." caption="..." />` — imagens com legenda.
- `<DirectoryIndex />` — lista gerada automaticamente de páginas filhas em páginas de índice.

O **catálogo completo de componentes**, as regras de formatação, os exemplos de `meta.json` e a lista de ícones estão em [Writing Docs](./src/contributing/writing_docs.mdx). Comece por lá para qualquer coisa além dos componentes acima.


## Mais Sobre Contribuição

Para estilo de código, DCO, pull requests e outras diretrizes de contribuição, veja as páginas em [`./src/contributing/`](./src/contributing).
