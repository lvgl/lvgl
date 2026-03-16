(function() {
"use strict";

/* --- Cross-reference helpers --- */
const XREF_TARGET = {
  parent_addr: "obj", group_addr: "group", display_addr: "disp",
  read_timer_addr: "timer", focused_addr: "obj", var_addr: "obj",
  user_data_addr: "obj", subject_addr: "subject", target_addr: "obj",
  decoded_addr: "imgcache"
};

/* Section registry: key, icon, title, panelClass, color accent */
const SECTIONS = [
  { key: "displays",           icon: "🖥", title: "Displays & Objects",  cls: "panel-disp-trees" },
  { key: "animations",         icon: "🎬", title: "Animations",         cls: "panel-animations" },
  { key: "timers",             icon: "⏱",  title: "Timers",             cls: "panel-timers" },
  { key: "image_cache",        icon: "🖼", title: "Image Cache",        cls: "panel-img-cache" },
  { key: "image_header_cache", icon: "📋", title: "Header Cache",       cls: "panel-hdr-cache" },
  { key: "indevs",             icon: "🕹", title: "Input Devices",      cls: "panel-indevs" },
  { key: "groups",             icon: "👥", title: "Groups",             cls: "panel-groups" },
  { key: "draw_units",         icon: "🎨", title: "Draw Units",         cls: "panel-draw-units" },
  { key: "draw_tasks",         icon: "📝", title: "Draw Tasks",         cls: "panel-draw-tasks" },
  { key: "subjects",           icon: "📡", title: "Subjects",           cls: "panel-subjects" },
  { key: "image_decoders",     icon: "🔓", title: "Decoders",           cls: "panel-decoders" },
  { key: "fs_drivers",         icon: "💾", title: "FS Drivers",         cls: "panel-fs-drivers" },
];

/* Stat card definitions: label, dataKey, icon, colorClass, sectionId */
const STAT_DEFS = [
  { label: "Displays",   key: "displays",     icon: "🖥", color: "blue",  section: "disp-trees" },
  { label: "Objects",    key: "_objects",      icon: "🌳", color: "green", section: "disp-trees" },
  { label: "Animations", key: "animations",    icon: "🎬", color: "mauve", section: "animations" },
  { label: "Timers",     key: "timers",        icon: "⏱",  color: "peach", section: "timers" },
  { label: "Img Cache",  key: "image_cache",   icon: "🖼", color: "teal",  section: "img-cache" },
  { label: "Input Devs", key: "indevs",        icon: "🕹", color: "pink",  section: "indevs" },
];

/* Named constants replacing magic numbers */
const CONSTANTS = {
  INFINITE_REPEAT: 0xFFFFFFFF,     /* animation infinite repeat sentinel */
  VIEWPORT_SIZE: 520,               /* 3D scene viewport fitting size */
  PERSPECTIVE_DISTANCE: 1200,       /* CSS perspective distance (px) */
  DEFAULT_ROT_X: -30,               /* default X rotation (degrees) */
  DEFAULT_ROT_Y: 30,                /* default Y rotation (degrees) */
  MIN_ZOOM: 0.2,                    /* minimum zoom level */
  MAX_ZOOM: 10,                     /* maximum zoom level */
  ZOOM_SENSITIVITY: 0.01,           /* wheel zoom factor */
  ROTATION_SENSITIVITY: 0.4,        /* mouse drag rotation factor */
  SCREEN_GAP: 2,                    /* gap between screen layers in depth */
  ANIM_DURATION: 500,               /* 3D toggle transition duration (ms) */
};

/* Unified dashboard state object */
const DashState = {
  hl: {
    addr: null,          /* currently highlighted address */
    registry: {},        /* addr -> [element, ...] */
    overlays: {},        /* display_addr -> { canvas, w, h, objs } */
  },
  detail: {
    objDataMap: {},      /* addr -> obj data dict */
    selectedAddr: null,  /* currently selected object address */
    panel: null,         /* detail panel DOM element */
  },
};

function el(tag, cls, text) {
  const e = document.createElement(tag);
  if (cls) e.className = cls;
  if (text !== undefined) e.textContent = text;
  return e;
}

function xref(addr, prefix) {
  if (!addr || addr === "None" || addr === "0x0") return document.createTextNode(addr || "-");
  const a = el("a", "xref", addr);
  a.href = "#" + prefix + "-" + addr;
  return a;
}

function xrefCell(td, key, val) {
  const prefix = XREF_TARGET[key];
  if (prefix && val && val !== "None" && val !== "0x0") {
    td.appendChild(xref(val, prefix));
  } else {
    td.textContent = val != null ? String(val) : "-";
  }
}

function countObjects(trees) {
  let n = 0;
  function walk(obj) { n++; if (obj.children) obj.children.forEach(walk); }
  trees.forEach(t => (t.screens || []).forEach(walk));
  return n;
}

/* --- Linked highlight system --- */
function registerHL(addr, element) {
  if (!addr) return;
  if (!DashState.hl.registry[addr]) DashState.hl.registry[addr] = [];
  DashState.hl.registry[addr].push(element);
}

function highlightObj(addr) {
  if (DashState.hl.addr === addr) return;
  clearHighlight();
  DashState.hl.addr = addr;
  if (!addr) return;
  const els = DashState.hl.registry[addr];
  if (els) els.forEach(e => e.classList.add("hl-active"));
  /* Draw overlay rectangles on display buffers */
  Object.values(DashState.hl.overlays).forEach(ov => {
    const ctx = ov.canvas.getContext("2d");
    ctx.clearRect(0, 0, ov.canvas.width, ov.canvas.height);
    const obj = ov.objs.find(o => o.addr === addr);
    if (obj) {
      const sx = ov.canvas.width / ov.w;
      const sy = ov.canvas.height / ov.h;
      const x = obj.x1 * sx, y = obj.y1 * sy;
      const w = (obj.x2 - obj.x1) * sx, h = (obj.y2 - obj.y1) * sy;
      ctx.strokeStyle = "rgba(137, 180, 250, 0.9)";
      ctx.lineWidth = 2;
      ctx.setLineDash([4, 2]);
      ctx.strokeRect(x, y, w, h);
      ctx.fillStyle = "rgba(137, 180, 250, 0.15)";
      ctx.fillRect(x, y, w, h);
    }
  });
}

function clearHighlight() {
  if (!DashState.hl.addr) return;
  const els = DashState.hl.registry[DashState.hl.addr];
  if (els) els.forEach(e => e.classList.remove("hl-active"));
  Object.values(DashState.hl.overlays).forEach(ov => {
    const ctx = ov.canvas.getContext("2d");
    ctx.clearRect(0, 0, ov.canvas.width, ov.canvas.height);
  });
  DashState.hl.addr = null;
}

/* --- Generic table builder --- */
function makeTable(headers, rows, anchorPrefix) {
  if (!rows || rows.length === 0) return el("p", "empty", "No entries.");
  const wrap = el("div", "table-wrap");
  const tbl = document.createElement("table");
  const thead = tbl.createTHead();
  const hr = thead.insertRow();
  headers.forEach(h => {
    const th = document.createElement("th");
    th.textContent = h;
    hr.appendChild(th);
  });
  const tbody = tbl.createTBody();
  rows.forEach(row => {
    const tr = tbody.insertRow();
    if (anchorPrefix && row.addr) tr.id = anchorPrefix + "-" + row.addr;
    headers.forEach(h => {
      const key = h.toLowerCase().replace(/ /g, "_");
      const td = tr.insertCell();
      const val = row[key];
      if (key === "area" && typeof val === "object") {
        td.textContent = "(" + val.x1 + "," + val.y1 + "," + val.x2 + "," + val.y2 + ")";
      } else if (key === "member_addrs" && Array.isArray(val)) {
        val.forEach((a, i) => {
          if (i > 0) td.appendChild(document.createTextNode(", "));
          td.appendChild(xref(a, "obj"));
        });
      } else if (key === "observer_addrs" && Array.isArray(val)) {
        val.forEach((a, i) => {
          if (i > 0) td.appendChild(document.createTextNode(", "));
          td.appendChild(xref(a, "observer"));
        });
      } else {
        xrefCell(td, key, val != null ? String(val) : "-");
      }
    });
  });
  wrap.appendChild(tbl);
  return wrap;
}

/* --- Object tree rendering (pure hierarchy, no styles) --- */
function renderObjTree(obj, depth) {
  if (depth === undefined) depth = 0;
  const det = document.createElement("details");
  det.className = "obj-node";
  if (obj.addr) det.id = "obj-" + obj.addr;
  const sum = document.createElement("summary");
  sum.style.setProperty("--depth-color", DEPTH_COLORS[depth % DEPTH_COLORS.length]);
  sum.textContent = obj.class_name || "obj";
  det.appendChild(sum);

  /* Store full data for detail panel */
  if (obj.addr) DashState.detail.objDataMap[obj.addr] = obj;

  /* Register for linked highlight */
  if (obj.addr) {
    registerHL(obj.addr, det);
    sum.addEventListener("mouseenter", () => highlightObj(obj.addr));
    sum.addEventListener("mouseleave", () => clearHighlight());
    sum.addEventListener("click", e => {
      e.stopPropagation();
      selectObj(obj.addr);
    });
  }

  if (obj.children) obj.children.forEach(ch => det.appendChild(renderObjTree(ch, depth + 1)));
  return det;
}

/* --- Select object and show detail panel --- */
function selectObj(addr) {
  /* Deselect previous */
  if (DashState.detail.selectedAddr) {
    const prev = document.getElementById("obj-" + DashState.detail.selectedAddr);
    if (prev) prev.classList.remove("obj-selected");
  }
  DashState.detail.selectedAddr = addr;
  const node = document.getElementById("obj-" + addr);
  if (node) node.classList.add("obj-selected");
  if (DashState.detail.panel) renderObjDetail(addr);
}

function renderObjDetail(addr) {
  DashState.detail.panel.innerHTML = "";
  const obj = DashState.detail.objDataMap[addr];
  if (!obj) {
    DashState.detail.panel.appendChild(el("p", "empty", "Select an object to inspect."));
    return;
  }

  /* Header */
  const hdr = el("div", "detail-header");
  hdr.appendChild(el("span", "detail-class", obj.class_name || "obj"));
  hdr.appendChild(el("span", "mono-addr", obj.addr));
  DashState.detail.panel.appendChild(hdr);

  /* Coordinates */
  const c = obj.coords || {};
  const coordSec = el("div", "detail-section");
  coordSec.appendChild(el("div", "detail-section-title", "Coordinates"));
  const coordGrid = el("div", "detail-coord-grid");
  ["x1","y1","x2","y2"].forEach(k => {
    coordGrid.appendChild(el("span", "detail-coord-label", k));
    coordGrid.appendChild(el("span", "detail-coord-val", String(c[k] || 0)));
  });
  const w = (c.x2||0) - (c.x1||0), h = (c.y2||0) - (c.y1||0);
  coordGrid.appendChild(el("span", "detail-coord-label", "w"));
  coordGrid.appendChild(el("span", "detail-coord-val", String(w)));
  coordGrid.appendChild(el("span", "detail-coord-label", "h"));
  coordGrid.appendChild(el("span", "detail-coord-val", String(h)));
  coordSec.appendChild(coordGrid);
  DashState.detail.panel.appendChild(coordSec);

  /* References */
  const refSec = el("div", "detail-section");
  refSec.appendChild(el("div", "detail-section-title", "References"));
  if (obj.parent_addr) {
    const row = el("div", "kv-row");
    row.appendChild(el("span", "kv-label", "parent"));
    row.appendChild(xref(obj.parent_addr, "obj"));
    refSec.appendChild(row);
  }
  if (obj.group_addr) {
    const row = el("div", "kv-row");
    row.appendChild(el("span", "kv-label", "group"));
    row.appendChild(xref(obj.group_addr, "group"));
    refSec.appendChild(row);
  }
  refSec.appendChild(kvPair("children", String(obj.child_count || 0)));
  refSec.appendChild(kvPair("styles", String(obj.style_count || 0)));
  DashState.detail.panel.appendChild(refSec);

  /* Styles */
  if (obj.styles && obj.styles.length > 0) {
    const styleSec = el("div", "detail-section");
    styleSec.appendChild(el("div", "detail-section-title", "Styles (" + obj.styles.length + ")"));
    obj.styles.forEach(s => {
      const card = el("div", "detail-style-card");
      card.appendChild(el("div", "detail-style-hdr",
        "[" + s.index + "] " + s.selector_str + "  " + s.flags_str));
      if (s.properties && s.properties.length > 0) {
        const tbl = document.createElement("table");
        tbl.className = "detail-style-table";
        const thead = tbl.createTHead().insertRow();
        ["prop", "value"].forEach(h => {
          const th = document.createElement("th"); th.textContent = h; thead.appendChild(th);
        });
        const tbody = tbl.createTBody();
        s.properties.forEach(p => {
          const r = tbody.insertRow();
          r.insertCell().textContent = p.prop_name;
          r.insertCell().textContent = p.value_str;
        });
        card.appendChild(tbl);
      }
      styleSec.appendChild(card);
    });
    DashState.detail.panel.appendChild(styleSec);
  }
}

/* --- Depth color palette shared by 3D scene and tree view --- */
const DEPTH_COLORS = [
  "var(--blue)", "var(--green)", "var(--mauve)", "var(--peach)",
  "var(--teal)", "var(--pink)", "var(--yellow)", "var(--red)",
  "var(--sapphire)", "var(--lavender)", "var(--flamingo)",
];

/* --- Panel factory --- */function makePanel(cls, icon, title, count) {
  const panel = el("div", "panel " + cls);
  panel.id = "sec-" + cls.replace("panel-", "");
  const hdr = el("div", "panel-header");
  hdr.appendChild(el("span", "panel-icon", icon));
  hdr.appendChild(el("span", "panel-title", title));
  if (count !== undefined) {
    hdr.appendChild(el("span", "panel-badge", String(count)));
  }
  panel.appendChild(hdr);
  const body = el("div", "panel-body");
  panel.appendChild(body);
  return { panel, body };
}

function emptyMsg() { return el("p", "empty", "No entries."); }

/* --- Stat card factory --- */
function makeStatPanel(icon, label, value, colorClass, sectionId) {
  const panel = el("div", "panel panel-stat");
  if (sectionId) {
    panel.style.cursor = "pointer";
    panel.addEventListener("click", () => {
      const target = document.getElementById("sec-" + sectionId);
      if (target) target.scrollIntoView({ behavior: "smooth", block: "start" });
    });
  }
  const body = el("div", "stat-mini");
  const iconWrap = el("div", "stat-icon-wrap " + colorClass, icon);
  body.appendChild(iconWrap);
  const info = el("div", "stat-info");
  info.appendChild(el("div", "stat-value", String(value)));
  info.appendChild(el("div", "stat-label", label));
  body.appendChild(info);
  panel.appendChild(body);
  return panel;
}

/* --- Section builders --- */
function buildDisplayAndTrees(data) {
  const displays = data.displays || [];
  const trees = data.object_trees || [];
  const objCount = countObjects(trees);
  const { panel, body } = makePanel("panel-disp-trees", "🖥", "Displays & Objects",
    displays.length + " disp / " + objCount + " obj");

  if (displays.length === 0 && trees.length === 0) {
    body.appendChild(emptyMsg());
    return panel;
  }

  /* Build per-display data */
  const dispEntries = displays.map((d, i) => {
    const tree = trees.find(t => t.display_addr === d.addr) || { display_addr: d.addr, screens: [] };
    const objs = [];
    function walk(obj) {
      const c = obj.coords || {};
      objs.push({ addr: obj.addr, x1: c.x1||0, y1: c.y1||0, x2: c.x2||0, y2: c.y2||0 });
      if (obj.children) obj.children.forEach(walk);
    }
    (tree.screens || []).forEach(walk);
    return { disp: d, tree, dispObjs: { [d.addr]: objs }, idx: i };
  });

  /* Display selector tab bar */
  const tabBar = el("div", "disp-tab-bar");
  const contentArea = el("div", "disp-content-area");
  const tabBtns = [];

  function showDisplay(idx) {
    /* Update tab active state */
    tabBtns.forEach((b, j) => b.classList.toggle("active", j === idx));
    /* Clear highlight state from previous display */
    clearHighlight();
    DashState.hl.registry = {};
    DashState.hl.overlays = {};
    /* Rebuild content for selected display */
    contentArea.innerHTML = "";
    DashState.detail.objDataMap = {};
    DashState.detail.panel = null;

    const entry = dispEntries[idx];
    const d = entry.disp;
    const tree = entry.tree;

    /* Display info chips */
    const infoBar = el("div", "disp-info-bar");
    const chip = el("div", "disp-chip");
    chip.appendChild(el("span", "disp-addr", d.addr || ""));
    chip.appendChild(el("span", "disp-res", d.hor_res + " × " + d.ver_res));
    chip.appendChild(el("span", "disp-screens", d.screen_count + " screens"));
    ["buf_1", "buf_2"].forEach(bk => {
      const b = d[bk]; if (!b) return;
      chip.appendChild(badge(bk.replace("_", " ") + " " + b.width + "×" + b.height + " " + b.color_format, "blue"));
    });
    infoBar.appendChild(chip);
    contentArea.appendChild(infoBar);

    /* Three-column split: tree | 3D | detail */
    const split = el("div", "obj-split");

    const treeView = el("div", "obj-tree-view");
    const treeHeader = el("div", "obj-tree-header", "🌳 " + entry.dispObjs[d.addr].length + " objects");
    treeView.appendChild(treeHeader);
    tree.screens.forEach(s => treeView.appendChild(renderObjTree(s)));
    split.appendChild(treeView);

    const view3d = el("div", "obj-3d-view");
    build3DScene(view3d, [tree], [d], entry.dispObjs);
    split.appendChild(view3d);

    const detailView = el("div", "obj-detail-view");
    detailView.appendChild(el("p", "empty", "Select an object to inspect."));
    DashState.detail.panel = detailView;
    split.appendChild(detailView);

    contentArea.appendChild(split);
  }

  dispEntries.forEach((entry, i) => {
    const d = entry.disp;
    const btn = el("button", "disp-tab-btn");
    /* Buffer preview thumbnail */
    const bufData = (d.buf_1 && d.buf_1.image_base64) || (d.buf_2 && d.buf_2.image_base64);
    if (bufData) {
      const thumb = document.createElement("img");
      thumb.className = "disp-tab-thumb";
      thumb.src = "data:image/png;base64," + bufData;
      thumb.draggable = false;
      btn.appendChild(thumb);
    }
    btn.appendChild(document.createTextNode(d.hor_res + "×" + d.ver_res));
    btn.addEventListener("click", () => showDisplay(i));
    tabBtns.push(btn);
    tabBar.appendChild(btn);
  });

  body.appendChild(tabBar);
  body.appendChild(contentArea);

  /* Show first display by default */
  if (dispEntries.length > 0) showDisplay(0);

  return panel;
}

/* --- 3D scene sub-functions --- */

/**
 * Flatten object trees into a layer array with global depth tracking.
 * @param {Array} trees - object_trees array (each with .screens)
 * @returns {{ layers: Array, screenNames: string[], screenMaxLocal: Object }}
 */
function flattenObjectLayers(trees) {
  const layers = [];
  const screenNames = [];
  let globalDepthOffset = 0;
  let screenIdx = 0;
  trees.forEach(t => (t.screens || []).forEach(s => {
    const idx = screenIdx++;
    const layerName = s.layer_name || s.class_name || "screen_" + idx;
    screenNames.push(layerName);
    function flatten(obj, localDepth) {
      const c = obj.coords || {};
      layers.push({
        addr: obj.addr, class_name: obj.class_name || "obj",
        x1: c.x1 || 0, y1: c.y1 || 0, x2: c.x2 || 0, y2: c.y2 || 0,
        depth: globalDepthOffset + localDepth,
        localDepth: localDepth,
        child_count: obj.child_count || 0,
        style_count: obj.style_count || 0,
        screenIdx: idx,
      });
      if (obj.children) obj.children.forEach(ch => flatten(ch, localDepth + 1));
    }
    flatten(s, 0);
    /* Find max local depth in this screen subtree */
    let maxLocal = 0;
    function findMax(obj, d) { maxLocal = Math.max(maxLocal, d); if (obj.children) obj.children.forEach(ch => findMax(ch, d + 1)); }
    findMax(s, 0);
    globalDepthOffset += maxLocal + CONSTANTS.SCREEN_GAP;
  }));

  /* Compute max local depth per screen for relayout */
  const screenMaxLocal = {};
  layers.forEach(l => {
    if (!(l.screenIdx in screenMaxLocal) || l.localDepth > screenMaxLocal[l.screenIdx]) {
      screenMaxLocal[l.screenIdx] = l.localDepth;
    }
  });

  return { layers, screenNames, screenMaxLocal };
}

/**
 * Build the scene control bar (toggle buttons, spread slider, reset button).
 * @param {Object} options - { maxDepth, defaultSpread (optional), bufBase64 }
 * @returns {{ controls: HTMLElement, toggle3d, toggleBorders, toggleBuf, toggleOrtho, spreadSlider, resetBtn, defaultSpread: number }}
 */
function buildSceneControls(options) {
  const { maxDepth, bufBase64 } = options;
  const controls = el("div", "scene-controls");

  const make3dToggle = (label, defaultOn) => {
    const btn = el("button", "scene-toggle-btn" + (defaultOn ? " active" : ""), label);
    btn.dataset.on = defaultOn ? "1" : "0";
    btn.addEventListener("click", () => {
      const on = btn.dataset.on === "1";
      btn.dataset.on = on ? "0" : "1";
      btn.classList.toggle("active", !on);
    });
    return btn;
  };

  const toggle3d = make3dToggle("3D", true);
  const toggleBorders = make3dToggle("Borders", true);
  const toggleBuf = make3dToggle("Buffer", !!bufBase64);
  const toggleOrtho = make3dToggle("Ortho", false);
  controls.appendChild(toggle3d);
  controls.appendChild(toggleBorders);
  if (bufBase64) controls.appendChild(toggleBuf);
  controls.appendChild(toggleOrtho);

  /* Dynamic spread range based on total depth levels */
  const defaultSpread = maxDepth > 0 ? Math.round(300 / maxDepth) : 30;
  const maxSpread = Math.max(200, defaultSpread * 5);

  const spreadLabel = el("label", "scene-label", "Z Spread");
  const spreadSlider = document.createElement("input");
  spreadSlider.type = "range"; spreadSlider.min = "0"; spreadSlider.max = String(maxSpread);
  spreadSlider.value = String(defaultSpread); spreadSlider.className = "scene-slider";
  spreadSlider.setAttribute("aria-label", "Z axis spread");
  controls.appendChild(spreadLabel);
  controls.appendChild(spreadSlider);
  const resetBtn = el("button", "scene-reset-btn", "Reset");
  controls.appendChild(resetBtn);

  return { controls, toggle3d, toggleBorders, toggleBuf, toggleOrtho, spreadSlider, resetBtn, defaultSpread };
}

/**
 * Build the buffer image layer with highlight overlay canvas.
 * @param {Array} displays - display data array
 * @param {number} scale - scene scale factor
 * @param {number} sceneW - unscaled scene width
 * @param {number} sceneH - unscaled scene height
 * @param {Object} dispObjs - per-display object coordinate maps
 * @returns {{ bufLayer: HTMLElement, bufCanvas: HTMLCanvasElement } | null}
 */
function buildBufferLayer(displays, scale, sceneW, sceneH, dispObjs) {
  /* Find first display buffer image */
  let bufBase64 = null;
  (displays || []).forEach(d => {
    if (bufBase64) return;
    ["buf_1", "buf_2"].forEach(bk => {
      if (bufBase64) return;
      const b = d[bk];
      if (b && b.image_base64) bufBase64 = b.image_base64;
    });
  });
  if (!bufBase64) return null;

  const sceneScaledW = sceneW * scale;
  const sceneScaledH = sceneH * scale;

  const bufLayer = el("div", "scene-buf-layer");
  bufLayer.style.width = sceneScaledW + "px";
  bufLayer.style.height = sceneScaledH + "px";
  bufLayer.style.left = "0px";
  bufLayer.style.top = "0px";
  const img = document.createElement("img");
  img.src = "data:image/png;base64," + bufBase64;
  img.style.width = "100%"; img.style.height = "100%";
  img.style.objectFit = "fill";
  img.draggable = false;
  bufLayer.appendChild(img);

  /* Overlay canvas for highlight rectangles on buffer */
  const bufCanvas = document.createElement("canvas");
  bufCanvas.className = "scene-buf-overlay";
  bufCanvas.width = sceneScaledW; bufCanvas.height = sceneScaledH;
  bufLayer.appendChild(bufCanvas);

  /* Register overlay for linked highlight system */
  if (dispObjs) {
    Object.keys(dispObjs).forEach(dAddr => {
      DashState.hl.overlays[dAddr + "-3d"] = {
        canvas: bufCanvas, w: sceneW / scale * scale, h: sceneH / scale * scale,
        objs: dispObjs[dAddr],
      };
    });
    /* Normalize: overlay uses scene pixel coords */
    Object.keys(dispObjs).forEach(dAddr => {
      const disp = (displays || []).find(dd => dd.addr === dAddr);
      if (disp) {
        DashState.hl.overlays[dAddr + "-3d"] = {
          canvas: bufCanvas,
          w: disp.hor_res, h: disp.ver_res,
          objs: dispObjs[dAddr],
        };
        /* Scale canvas to match scene viewport */
        bufCanvas.width = sceneScaledW;
        bufCanvas.height = sceneScaledH;
      }
    });
  }

  return { bufLayer, bufCanvas, bufBase64 };
}

/**
 * Register mouse/wheel event handlers for 3D scene interaction.
 * @param {HTMLElement} viewport - the scene viewport element
 * @param {HTMLElement} scene - the inner scene element
 * @param {Object} callbacks - { getState, applyRotation }
 */
function setupSceneInteraction(viewport, scene, callbacks) {
  const { getState, applyRotation } = callbacks;

  viewport.addEventListener("mousedown", e => {
    const st = getState();
    if (e.button === 1 || (e.button === 0 && e.shiftKey)) {
      /* Middle-click or shift+left-click: pan */
      e.preventDefault();
      st.dragging = "pan"; st.lastX = e.clientX; st.lastY = e.clientY;
      viewport.style.cursor = "move";
    } else if (e.button === 0 && st.is3d) {
      /* Left-click: rotate (3D only) */
      st.dragging = "rotate"; st.lastX = e.clientX; st.lastY = e.clientY;
      viewport.style.cursor = "grabbing";
    }
  });

  window.addEventListener("mousemove", e => {
    const st = getState();
    if (!st.dragging) return;
    const dx = e.clientX - st.lastX, dy = e.clientY - st.lastY;
    st.lastX = e.clientX; st.lastY = e.clientY;
    if (st.dragging === "rotate") {
      st.rotY += dx * CONSTANTS.ROTATION_SENSITIVITY;
      st.rotX -= dy * CONSTANTS.ROTATION_SENSITIVITY;
      st.rotX = Math.max(-90, Math.min(90, st.rotX));
    } else if (st.dragging === "pan") {
      st.panX += dx / st.zoom;
      st.panY += dy / st.zoom;
    }
    applyRotation();
  });

  window.addEventListener("mouseup", () => {
    const st = getState();
    st.dragging = false; viewport.style.cursor = "grab";
  });

  /* Wheel: pinch-to-zoom (ctrl+wheel / trackpad pinch) and pan (two-finger scroll) */
  viewport.addEventListener("wheel", e => {
    e.preventDefault();
    const st = getState();
    if (e.ctrlKey) {
      /* Pinch zoom (trackpad) or ctrl+scroll (mouse) */
      const factor = 1 - e.deltaY * CONSTANTS.ZOOM_SENSITIVITY;
      st.zoom = Math.max(CONSTANTS.MIN_ZOOM, Math.min(CONSTANTS.MAX_ZOOM, st.zoom * factor));
    } else {
      /* Two-finger pan (trackpad) or scroll wheel pan */
      st.panX -= e.deltaX / st.zoom;
      st.panY -= e.deltaY / st.zoom;
    }
    applyRotation();
  }, { passive: false });
}

/* --- 3D Exploded Object Tree View (coordinator) --- */
function build3DScene(container, trees, displays, dispObjs) {
  /* 1. Flatten object trees into layers */
  const { layers, screenNames, screenMaxLocal } = flattenObjectLayers(trees);

  if (layers.length === 0) { container.appendChild(emptyMsg()); return; }

  /* Scene bounds fixed to display resolution */
  let minX = 0, minY = 0, maxX = 0, maxY = 0, maxDepth = 0;
  (displays || []).forEach(d => {
    maxX = Math.max(maxX, d.hor_res || 0);
    maxY = Math.max(maxY, d.ver_res || 0);
  });
  layers.forEach(l => { maxDepth = Math.max(maxDepth, l.depth); });
  const sceneW = maxX - minX || 1;
  const sceneH = maxY - minY || 1;

  /* Scale factor: fit scene into viewport */
  const scale = CONSTANTS.VIEWPORT_SIZE / Math.max(sceneW, sceneH);
  const sceneScaledW = sceneW * scale;
  const sceneScaledH = sceneH * scale;

  /* 2. Build buffer layer */
  const bufResult = buildBufferLayer(displays, scale, sceneW, sceneH, dispObjs);
  const bufBase64 = bufResult ? bufResult.bufBase64 : null;
  const bufLayer = bufResult ? bufResult.bufLayer : null;

  /* 3. Build controls */
  const ctrl = buildSceneControls({ maxDepth, bufBase64 });
  container.appendChild(ctrl.controls);
  const { toggle3d, toggleBorders, toggleBuf, toggleOrtho, spreadSlider, resetBtn, defaultSpread } = ctrl;

  /* Layer filter controls */
  const layerBar = el("div", "scene-layer-bar");
  const layerBtns = [];
  const layerVisible = [];
  screenNames.forEach((name, i) => {
    /* Default: only act_scr visible */
    const defaultOn = name === "act_scr" || screenNames.length === 1;
    layerVisible.push(defaultOn);
    const btn = el("button", "scene-layer-btn" + (defaultOn ? " active" : ""), name);
    btn.style.borderBottomColor = DEPTH_COLORS[i % DEPTH_COLORS.length];
    btn.dataset.idx = String(i);

    /* Single click: toggle visibility */
    btn.addEventListener("click", () => {
      layerVisible[i] = !layerVisible[i];
      btn.classList.toggle("active", layerVisible[i]);
      applyLayerVisibility();
    });

    /* Double click: solo mode (show only this layer) */
    btn.addEventListener("dblclick", e => {
      e.preventDefault();
      const allVisible = layerVisible.every((v, j) => j === i ? v : !v);
      if (allVisible) {
        /* Already solo — restore all */
        layerVisible.fill(true);
      } else {
        layerVisible.fill(false);
        layerVisible[i] = true;
      }
      layerBtns.forEach((b, j) => b.classList.toggle("active", layerVisible[j]));
      applyLayerVisibility();
    });

    layerBtns.push(btn);
    layerBar.appendChild(btn);
  });
  if (screenNames.length > 0) container.appendChild(layerBar);

  /* Tooltip */
  const tooltip = el("div", "scene-tooltip");
  container.appendChild(tooltip);

  /* Viewport and scene */
  const viewport = el("div", "scene-viewport");
  const scene = el("div", "scene-3d");
  viewport.appendChild(scene);
  container.appendChild(viewport);

  scene.style.width = sceneScaledW + "px";
  scene.style.height = sceneScaledH + "px";

  /* Append buffer layer to scene */
  if (bufLayer) scene.appendChild(bufLayer);

  /* Build border layer divs */
  const layerEls = [];
  layers.forEach(l => {
    const div = el("div", "scene-layer");
    const w = (l.x2 - l.x1) * scale;
    const h = (l.y2 - l.y1) * scale;
    const x = (l.x1 - minX) * scale;
    const y = (l.y1 - minY) * scale;
    div.style.width = Math.max(2, w) + "px";
    div.style.height = Math.max(2, h) + "px";
    div.style.left = x + "px";
    div.style.top = y + "px";
    div.style.borderColor = DEPTH_COLORS[l.depth % DEPTH_COLORS.length];
    div.dataset.depth = l.depth;
    div.dataset.addr = l.addr || "";
    div.dataset.screenIdx = l.screenIdx;
    div.dataset.info = l.class_name + "@" + (l.addr || "?") +
      " [" + l.x1 + "," + l.y1 + "," + l.x2 + "," + l.y2 + "]" +
      " children=" + l.child_count + " styles=" + l.style_count;
    if (l.addr) registerHL(l.addr, div);
    layerEls.push({ el: div, depth: l.depth, localDepth: l.localDepth, screenIdx: l.screenIdx });
    scene.appendChild(div);
  });

  /* Interaction state */
  const interactionState = {
    rotX: CONSTANTS.DEFAULT_ROT_X, rotY: CONSTANTS.DEFAULT_ROT_Y,
    dragging: false, lastX: 0, lastY: 0,
    is3d: true, zoom: 1, panX: 0, panY: 0,
  };

  /* Apply layer visibility filter and recompute Z positions.
   * @param {number} [spreadOverride] - if provided, use this spread instead of slider value
   */
  function applyLayerVisibility(spreadOverride) {
    const bordersOn = toggleBorders.dataset.on === "1";

    /* Recompute global depth offsets for visible screens only */
    const visibleScreens = [];
    for (let i = 0; i < screenNames.length; i++) {
      if (layerVisible[i]) visibleScreens.push(i);
    }
    const screenOffset = {};
    let offset = 0;
    visibleScreens.forEach(idx => {
      screenOffset[idx] = offset;
      offset += (screenMaxLocal[idx] || 0) + CONSTANTS.SCREEN_GAP;
    });

    const spread = spreadOverride !== undefined ? spreadOverride
      : (interactionState.is3d ? Number(spreadSlider.value) : 0);
    layerEls.forEach(le => {
      if (!bordersOn || !layerVisible[le.screenIdx]) {
        le.el.style.display = "none";
        return;
      }
      le.el.style.display = "";
      const newDepth = screenOffset[le.screenIdx] + le.localDepth;
      le.el.style.transform = "translateZ(" + (newDepth * spread) + "px)";
    });
    if (bufLayer) bufLayer.style.transform = "translateZ(" + (-spread * 1.5) + "px)";
  }

  /**
   * Apply scene rotation transform.
   * @param {Object} [rotOverride] - { rotX, rotY } to override interactionState
   */
  function applyRotation(rotOverride) {
    /* Toggle perspective vs orthographic projection */
    const ortho = toggleOrtho.dataset.on === "1";
    viewport.style.perspective = ortho ? "none" : CONSTANTS.PERSPECTIVE_DISTANCE + "px";
    /* Zoom and pan applied on scene so viewport clips overflow */
    const st = interactionState;
    const rx = rotOverride ? rotOverride.rotX : st.rotX;
    const ry = rotOverride ? rotOverride.rotY : st.rotY;
    const base = "translate(-50%, -50%) scale(" + st.zoom + ") translate(" + (st.panX / st.zoom) + "px," + (st.panY / st.zoom) + "px)";
    if (st.is3d || rotOverride) {
      scene.style.transform = base + " rotateX(" + rx + "deg) rotateY(" + ry + "deg)";
    } else {
      scene.style.transform = base;
    }
  }

  function applySpread() {
    applyLayerVisibility();
  }

  /* Easing: cubic-bezier(0.4, 0, 0.2, 1) approximation */
  function easeInOutCubic(t) {
    return t < 0.5 ? 4 * t * t * t : 1 - Math.pow(-2 * t + 2, 3) / 2;
  }

  let animFrameId = null;

  /* Saved rotation for 3D toggle memory */
  let savedRotX = CONSTANTS.DEFAULT_ROT_X;
  let savedRotY = CONSTANTS.DEFAULT_ROT_Y;

  /**
   * Animate 3D toggle transition: rotation and z-spread.
   * Entering 3D: animate from (0,0) to saved rotation.
   * Leaving 3D: save current rotation, animate to (0,0).
   * @param {boolean} entering3d - true = entering 3D, false = leaving 3D
   */
  function animateTransition(entering3d) {
    if (animFrameId) { cancelAnimationFrame(animFrameId); animFrameId = null; }
    const duration = CONSTANTS.ANIM_DURATION;
    const targetSpread = Number(spreadSlider.value);

    if (!entering3d) {
      /* Save current rotation before leaving 3D */
      savedRotX = interactionState.rotX;
      savedRotY = interactionState.rotY;
    }

    const toRotX = entering3d ? savedRotX : 0;
    const toRotY = entering3d ? savedRotY : 0;
    const fromRotX = entering3d ? 0 : savedRotX;
    const fromRotY = entering3d ? 0 : savedRotY;
    const startTime = performance.now();

    function tick(now) {
      const elapsed = now - startTime;
      const rawT = Math.min(elapsed / duration, 1);
      const t = easeInOutCubic(rawT);

      const curRotX = fromRotX + (toRotX - fromRotX) * t;
      const curRotY = fromRotY + (toRotY - fromRotY) * t;
      const curSpread = entering3d ? targetSpread * t : targetSpread * (1 - t);

      applyRotation({ rotX: curRotX, rotY: curRotY });
      applyLayerVisibility(curSpread);

      if (rawT < 1) {
        animFrameId = requestAnimationFrame(tick);
      } else {
        animFrameId = null;
        interactionState.rotX = toRotX;
        interactionState.rotY = toRotY;
        applyRotation();
        applyLayerVisibility();
      }
    }
    animFrameId = requestAnimationFrame(tick);
  }

  applyLayerVisibility();
  applyRotation();

  spreadSlider.addEventListener("input", () => { applySpread(); });

  /* Toggle handlers */
  toggle3d.addEventListener("click", () => {
    interactionState.is3d = toggle3d.dataset.on === "1";
    spreadSlider.disabled = !interactionState.is3d;
    animateTransition(interactionState.is3d);
  });

  toggleOrtho.addEventListener("click", () => { applyRotation(); });

  toggleBorders.addEventListener("click", () => { applyLayerVisibility(); });

  if (bufBase64) {
    toggleBuf.addEventListener("click", () => {
      const show = toggleBuf.dataset.on === "1";
      if (bufLayer) bufLayer.style.display = show ? "" : "none";
    });
  }

  /* 4. Setup mouse/wheel interaction */
  setupSceneInteraction(viewport, scene, {
    getState: () => interactionState,
    applyRotation,
  });

  /* Hover: linked highlight + tooltip */
  scene.addEventListener("mouseover", e => {
    const t = e.target.closest(".scene-layer");
    if (t) {
      tooltip.textContent = t.dataset.info;
      tooltip.style.display = "block";
      if (t.dataset.addr) highlightObj(t.dataset.addr);
    }
  });
  scene.addEventListener("mouseout", e => {
    const t = e.target.closest(".scene-layer");
    if (t) {
      tooltip.style.display = "none";
      clearHighlight();
    }
  });
  scene.addEventListener("mousemove", e => {
    if (tooltip.style.display === "block") {
      const rect = container.getBoundingClientRect();
      tooltip.style.left = (e.clientX - rect.left + 12) + "px";
      tooltip.style.top = (e.clientY - rect.top - 8) + "px";
    }
  });

  /* Click to highlight, select and scroll to tree node */
  scene.addEventListener("click", e => {
    const t = e.target.closest(".scene-layer");
    if (t && t.dataset.addr) {
      selectObj(t.dataset.addr);
      const target = document.getElementById("obj-" + t.dataset.addr);
      if (target) {
        let p = target.parentElement;
        while (p) {
          if (p.tagName === "DETAILS") p.open = true;
          p = p.parentElement;
        }
        target.open = true;
        target.scrollIntoView({ behavior: "smooth", block: "nearest" });
      }
    }
  });

  /* Reset button */
  resetBtn.addEventListener("click", () => {
    interactionState.rotX = CONSTANTS.DEFAULT_ROT_X;
    interactionState.rotY = CONSTANTS.DEFAULT_ROT_Y;
    interactionState.zoom = 1; interactionState.panX = 0; interactionState.panY = 0;
    interactionState.is3d = true;
    spreadSlider.value = String(defaultSpread);
    spreadSlider.disabled = false;
    toggle3d.dataset.on = "1"; toggle3d.classList.add("active");
    toggleBorders.dataset.on = "1"; toggleBorders.classList.add("active");
    if (toggleBuf) { toggleBuf.dataset.on = bufBase64 ? "1" : "0"; toggleBuf.classList.toggle("active", !!bufBase64); }
    toggleOrtho.dataset.on = "0"; toggleOrtho.classList.remove("active");
    layerEls.forEach(le => { le.el.style.display = ""; });
    screenNames.forEach((name, i) => {
      layerVisible[i] = name === "act_scr" || screenNames.length === 1;
    });
    layerBtns.forEach((b, i) => b.classList.toggle("active", layerVisible[i]));
    if (bufLayer) bufLayer.style.display = bufBase64 ? "" : "none";
    applyRotation();
    applySpread();
  });
}

function buildSimpleTable(data, key, cls, icon, title, headers, anchorPrefix) {
  const items = data[key] || [];
  const { panel, body } = makePanel(cls, icon, title, items.length);
  body.appendChild(makeTable(headers, items, anchorPrefix));
  return panel;
}

/* --- Status badge helper --- */
function badge(text, color) {
  const b = el("span", "badge badge-" + color, text);
  return b;
}

/* --- Key-value pair helper --- */
function kvPair(label, value) {
  const row = el("div", "kv-row");
  row.appendChild(el("span", "kv-label", label));
  row.appendChild(el("span", "kv-value", String(value)));
  return row;
}

/* --- Progress bar helper --- */
function progressBar(ratio, color) {
  const wrap = el("div", "progress-bar");
  const fill = el("div", "progress-fill " + color);
  fill.style.width = Math.max(0, Math.min(100, ratio * 100)) + "%";
  wrap.appendChild(fill);
  return wrap;
}

/* --- Generic card builder --- */
function buildCard(item, config) {
  const card = el("div", config.cardClass);
  if (config.anchorPrefix && item.addr)
    card.id = config.anchorPrefix + "-" + item.addr;

  const hdr = el("div", config.cardClass.replace("-card", "-header"));
  hdr.appendChild(el("span", "mono-addr", item.addr));
  (config.badges || []).forEach(b => hdr.appendChild(badge(b.text, b.color)));
  card.appendChild(hdr);

  const info = el("div", config.cardClass.replace("-card", "-info"));
  if (config.content) config.content(info, item);
  card.appendChild(info);
  return card;
}

/* --- Animations: card-based with progress bar --- */
function buildAnimations(data) {
  const items = data.animations || [];
  const { panel, body } = makePanel("panel-animations", "🎬", "Animations", items.length);
  if (items.length === 0) { body.appendChild(emptyMsg()); return panel; }
  items.forEach(a => {
    const statusColor = a.status === "paused" ? "yellow" : a.status === "reverse" ? "mauve" : "green";
    body.appendChild(buildCard(a, {
      cardClass: "anim-card",
      anchorPrefix: "anim",
      badges: [{ text: a.status || "running", color: statusColor }],
      content: (info, a) => {
        info.appendChild(kvPair("callback", a.exec_cb || "-"));
        info.appendChild(kvPair("duration", a.duration + "ms"));

        const range = a.end_value - a.start_value;
        const ratio = range !== 0 ? (a.current_value - a.start_value) / range : 0;
        const valRow = el("div", "anim-value-row");
        valRow.appendChild(el("span", "anim-val-label", String(a.start_value)));
        valRow.appendChild(progressBar(ratio, "blue"));
        valRow.appendChild(el("span", "anim-val-label", String(a.end_value)));
        info.appendChild(valRow);
        info.appendChild(el("div", "anim-cur-val", "current: " + a.current_value +
          "  (" + Math.round(ratio * 100) + "%)"));

        const repeat = a.repeat_cnt === CONSTANTS.INFINITE_REPEAT ? "∞" : String(a.repeat_cnt);
        info.appendChild(kvPair("repeat", repeat));
        info.appendChild(kvPair("act_time", a.act_time + "ms"));
      }
    }));
  });
  return panel;
}

/* --- Timers: card-based with frequency and status --- */
function buildTimers(data) {
  const items = data.timers || [];
  const { panel, body } = makePanel("panel-timers", "⏱", "Timers", items.length);
  if (items.length === 0) { body.appendChild(emptyMsg()); return panel; }
  items.forEach(t => {
    body.appendChild(buildCard(t, {
      cardClass: "timer-card",
      anchorPrefix: "timer",
      badges: [{ text: t.paused ? "paused" : "active", color: t.paused ? "yellow" : "green" }],
      content: (info, t) => {
        info.classList.add("timer-info-row");
        info.appendChild(kvPair("callback", t.timer_cb || "-"));
        info.appendChild(kvPair("period", t.period + "ms"));
        info.appendChild(kvPair("frequency", t.frequency || "-"));
        const repeat = t.repeat_count === -1 ? "∞" : String(t.repeat_count);
        info.appendChild(kvPair("repeat", repeat));
        info.appendChild(kvPair("last_run", String(t.last_run)));
      }
    }));
  });
  return panel;
}

/* --- Input Devices: card with type icon and status --- */
function buildIndevs(data) {
  const items = data.indevs || [];
  const { panel, body } = makePanel("panel-indevs", "🕹", "Input Devices", items.length);
  if (items.length === 0) { body.appendChild(emptyMsg()); return panel; }
  const INDEV_ICONS = {
    pointer: "👆", keypad: "⌨️", button: "🔘", encoder: "🎛️", none: "❓"
  };
  items.forEach(d => {
    const typeIcon = INDEV_ICONS[d.type_name] || "🕹";
    const card = buildCard(d, {
      cardClass: "indev-card",
      anchorPrefix: "indev",
      badges: [{ text: d.enabled ? "enabled" : "disabled", color: d.enabled ? "green" : "red" }],
      content: (info, d) => {
        info.appendChild(kvPair("read_cb", d.read_cb || "-"));
        info.appendChild(kvPair("long_press", d.long_press_time + "ms"));
        info.appendChild(kvPair("scroll_limit", String(d.scroll_limit)));
        if (d.display_addr) {
          const row = el("div", "kv-row");
          row.appendChild(el("span", "kv-label", "display"));
          row.appendChild(xref(d.display_addr, "disp"));
          info.appendChild(row);
        }
        if (d.group_addr) {
          const row = el("div", "kv-row");
          row.appendChild(el("span", "kv-label", "group"));
          row.appendChild(xref(d.group_addr, "group"));
          info.appendChild(row);
        }
      }
    });
    /* Prepend type icon and name before mono-addr in header */
    const hdr = card.querySelector(".indev-header");
    const addrSpan = hdr.firstChild;
    hdr.insertBefore(el("span", "indev-type-name", d.type_name || "unknown"), addrSpan);
    hdr.insertBefore(el("span", "indev-type-icon", typeIcon), hdr.firstChild);
    body.appendChild(card);
  });
  return panel;
}

function buildImageCache(data) {
  const entries = data.image_cache || [];
  const { panel, body } = makePanel("panel-img-cache", "🖼", "Image Cache", entries.length);
  if (entries.length === 0) { body.appendChild(emptyMsg()); return panel; }
  const grid = el("div", "cache-grid");
  entries.forEach(e => {
    const card = el("div", "cache-entry");
    if (e.entry_addr) card.id = "imgcache-" + e.entry_addr;
    if (e.preview_base64) {
      const img = document.createElement("img");
      img.className = "cache-thumb";
      img.src = "data:image/png;base64," + e.preview_base64;
      img.alt = "decoded preview";
      card.appendChild(img);
    }
    const info = el("div", "cache-info");
    info.appendChild(el("div", "cache-src", e.src || "-"));
    const meta = el("div", "cache-meta-row");
    meta.appendChild(badge(e.cf || "?", "blue"));
    meta.appendChild(el("span", "cache-size-label", e.size || ""));
    meta.appendChild(badge("rc=" + e.ref_count, "teal"));
    info.appendChild(meta);
    if (e.decoder_name) info.appendChild(el("div", "cache-decoder-label", e.decoder_name));
    card.appendChild(info);
    grid.appendChild(card);
  });
  body.appendChild(grid);
  return panel;
}

function buildSubjects(data) {
  const subjects = data.subjects || [];
  const { panel, body } = makePanel("panel-subjects", "📡", "Subjects", subjects.length);
  if (subjects.length === 0) { body.appendChild(emptyMsg()); return panel; }
  subjects.forEach(s => {
    const card = el("div", "subject-card");
    if (s.addr) card.id = "subject-" + s.addr;
    const hdr = el("div", "subject-header");
    hdr.appendChild(el("span", "subject-addr", s.addr || ""));
    hdr.appendChild(el("span", "subject-type", s.type_name));
    hdr.appendChild(document.createTextNode("  " + (s.observers||[]).length + " observers"));
    card.appendChild(hdr);
    if (s.observers && s.observers.length > 0) {
      const t = document.createElement("table");
      const th = t.createTHead().insertRow();
      ["addr","cb","target_addr","for_obj"].forEach(h => {
        const c = document.createElement("th"); c.textContent = h; th.appendChild(c);
      });
      const tb = t.createTBody();
      s.observers.forEach(o => {
        const r = tb.insertRow();
        if (o.addr) r.id = "observer-" + o.addr;
        r.insertCell().textContent = o.addr || "-";
        r.insertCell().textContent = o.cb || "-";
        const tc = r.insertCell();
        xrefCell(tc, "target_addr", o.target_addr);
        r.insertCell().textContent = String(o.for_obj);
      });
      card.appendChild(t);
    }
    body.appendChild(card);
  });
  return panel;
}

/* --- Groups: card with member count bar and focused highlight --- */
function buildGroups(data) {
  const items = data.groups || [];
  const { panel, body } = makePanel("panel-groups", "👥", "Groups", items.length);
  if (items.length === 0) { body.appendChild(emptyMsg()); return panel; }
  items.forEach(g => {
    const badges = [{ text: g.obj_count + " objects", color: "blue" }];
    if (g.frozen) badges.push({ text: "frozen", color: "yellow" });
    if (g.editing) badges.push({ text: "editing", color: "peach" });
    body.appendChild(buildCard(g, {
      cardClass: "group-card",
      anchorPrefix: "group",
      badges: badges,
      content: (info, g) => {
        info.appendChild(kvPair("wrap", String(g.wrap)));
        if (g.focused_addr) {
          const row = el("div", "kv-row");
          row.appendChild(el("span", "kv-label", "focused"));
          row.appendChild(xref(g.focused_addr, "obj"));
          info.appendChild(row);
        }
        if (g.member_addrs && g.member_addrs.length > 0) {
          const mRow = el("div", "group-members");
          mRow.appendChild(el("span", "kv-label", "members"));
          const mList = el("div", "member-list");
          g.member_addrs.forEach(a => {
            mList.appendChild(xref(a, "obj"));
          });
          mRow.appendChild(mList);
          info.appendChild(mRow);
        }
      }
    }));
  });
  return panel;
}

/* --- Draw Tasks: card with area visualization --- */
function buildDrawTasks(data) {
  const items = data.draw_tasks || [];
  const { panel, body } = makePanel("panel-draw-tasks", "📝", "Draw Tasks", items.length);
  if (items.length === 0) { body.appendChild(emptyMsg()); return panel; }
  items.forEach(t => {
    const stateColor = t.state_name === "ready" ? "green" : t.state_name === "queued" ? "yellow" : "mauve";
    body.appendChild(buildCard(t, {
      cardClass: "dtask-card",
      anchorPrefix: "drawtask",
      badges: [
        { text: t.type_name || "?", color: "blue" },
        { text: t.state_name || "?", color: stateColor },
      ],
      content: (info, t) => {
        if (t.area) {
          const a = t.area;
          const w = a.x2 - a.x1; const h = a.y2 - a.y1;
          info.appendChild(kvPair("area", "(" + a.x1 + "," + a.y1 + ") → (" + a.x2 + "," + a.y2 + ")"));
          info.appendChild(kvPair("size", w + " × " + h));
        }
        info.appendChild(kvPair("opacity", t.opa));
        if (t.preferred_draw_unit_id !== undefined) {
          info.appendChild(kvPair("unit_id", t.preferred_draw_unit_id));
        }
      }
    }));
  });
  return panel;
}

/* --- Draw Units: simple card --- */
function buildDrawUnits(data) {
  const items = data.draw_units || [];
  const { panel, body } = makePanel("panel-draw-units", "🎨", "Draw Units", items.length);
  if (items.length === 0) { body.appendChild(emptyMsg()); return panel; }
  const grid = el("div", "unit-grid");
  items.forEach(u => {
    const card = el("div", "unit-card");
    if (u.addr) card.id = "drawunit-" + u.addr;
    card.appendChild(el("div", "unit-name", u.name || "(unnamed)"));
    card.appendChild(el("div", "unit-idx", "#" + u.idx));
    card.appendChild(el("div", "mono-addr", u.addr));
    grid.appendChild(card);
  });
  body.appendChild(grid);
  return panel;
}

/* --- Image Decoders: card grid --- */
function buildDecoders(data) {
  const items = data.image_decoders || [];
  const { panel, body } = makePanel("panel-decoders", "🔓", "Decoders", items.length);
  if (items.length === 0) { body.appendChild(emptyMsg()); return panel; }
  const grid = el("div", "decoder-grid");
  items.forEach(d => {
    const card = el("div", "decoder-card");
    if (d.addr) card.id = "decoder-" + d.addr;
    card.appendChild(el("div", "decoder-name", d.name || "(unnamed)"));
    const cbs = el("div", "decoder-cbs");
    if (d.info_cb && d.info_cb !== "-") cbs.appendChild(badge("info", "blue"));
    if (d.open_cb && d.open_cb !== "-") cbs.appendChild(badge("open", "green"));
    if (d.close_cb && d.close_cb !== "-") cbs.appendChild(badge("close", "peach"));
    card.appendChild(cbs);
    card.appendChild(el("div", "mono-addr", d.addr));
    grid.appendChild(card);
  });
  body.appendChild(grid);
  return panel;
}

/* --- FS Drivers: card with drive letter badge --- */
function buildFsDrivers(data) {
  const items = data.fs_drivers || [];
  const { panel, body } = makePanel("panel-fs-drivers", "💾", "FS Drivers", items.length);
  if (items.length === 0) { body.appendChild(emptyMsg()); return panel; }
  const grid = el("div", "fs-grid");
  items.forEach(d => {
    const card = el("div", "fs-card");
    if (d.addr) card.id = "fsdrv-" + d.addr;
    card.appendChild(el("div", "fs-letter", d.letter + ":"));
    card.appendChild(el("div", "fs-driver-name", d.driver_name || "(unnamed)"));
    const info = el("div", "fs-info");
    info.appendChild(kvPair("cache", d.cache_size));
    const cbs = el("div", "fs-cbs");
    if (d.open_cb && d.open_cb !== "-") cbs.appendChild(badge("open", "green"));
    if (d.read_cb && d.read_cb !== "-") cbs.appendChild(badge("read", "blue"));
    if (d.write_cb && d.write_cb !== "-") cbs.appendChild(badge("write", "peach"));
    if (d.close_cb && d.close_cb !== "-") cbs.appendChild(badge("close", "mauve"));
    info.appendChild(cbs);
    card.appendChild(info);
    grid.appendChild(card);
  });
  body.appendChild(grid);
  return panel;
}

/* --- Top bar nav builder --- */
function buildTopNav(data) {
  const nav = document.getElementById("topbar-nav");
  nav.innerHTML = "";
  SECTIONS.forEach(s => {
    const count = s.key === "displays"
      ? (data.displays||[]).length
      : s.key === "object_trees"
        ? (data[s.key]||[]).reduce((n,t) => n + (t.screens?t.screens.length:0), 0)
        : (data[s.key]||[]).length;
    if (count === 0) return;
    const a = el("a", "", s.icon + " " + count);
    const targetId = "sec-" + s.cls.replace("panel-", "");
    a.href = "#" + targetId;
    a.title = s.title + " (" + count + ")";
    a.addEventListener("click", e => {
      e.preventDefault();
      const target = document.getElementById(targetId);
      if (target) target.scrollIntoView({ behavior: "smooth", block: "start" });
    });
    nav.appendChild(a);
  });
}

/* --- Main render --- */
function renderDashboard(data) {
  const meta = document.getElementById("header-meta");
  meta.innerHTML = "";
  meta.appendChild(document.createTextNode(data.meta?.timestamp || ""));
  if (data.meta?.lvgl_version) {
    meta.appendChild(el("span", "version-tag", "LVGL " + data.meta.lvgl_version));
  }

  buildTopNav(data);

  const grid = document.getElementById("bento-grid");
  grid.innerHTML = "";

  /* Stat cards row */
  const objCount = countObjects(data.object_trees || []);
  STAT_DEFS.forEach(s => {
    const val = s.key === "_objects" ? objCount : (data[s.key]||[]).length;
    grid.appendChild(makeStatPanel(s.icon, s.label, val, s.color, s.section));
  });

  /* Main panels in bento layout order */
  grid.appendChild(buildDisplayAndTrees(data));

  grid.appendChild(buildImageCache(data));
  grid.appendChild(buildSimpleTable(data, "image_header_cache",
    "panel-hdr-cache", "📋", "Header Cache",
    ["entry_addr","src","size","cf","ref_count","src_type","decoder_name"], "imghdr"));
  grid.appendChild(buildDecoders(data));

  grid.appendChild(buildAnimations(data));
  grid.appendChild(buildTimers(data));
  grid.appendChild(buildIndevs(data));

  grid.appendChild(buildGroups(data));
  grid.appendChild(buildDrawUnits(data));
  grid.appendChild(buildDrawTasks(data));
  grid.appendChild(buildSubjects(data));
  grid.appendChild(buildFsDrivers(data));
}

/* --- Boot logic --- */
const jsonEl = document.getElementById("lvgl-data");
const raw = jsonEl ? jsonEl.textContent.trim() : "";

if (raw) {
  try {
    renderDashboard(JSON.parse(raw));
  } catch(e) {
    document.getElementById("bento-grid").textContent =
      "Failed to parse embedded JSON: " + e.message;
  }
} else {
  const dropZone = document.getElementById("drop-zone");
  dropZone.style.display = "flex";
  const fileInput = document.getElementById("file-input");

  dropZone.addEventListener("dragover", e => {
    e.preventDefault(); dropZone.classList.add("dragover");
  });
  dropZone.addEventListener("dragleave", () => dropZone.classList.remove("dragover"));
  dropZone.addEventListener("drop", e => {
    e.preventDefault(); dropZone.classList.remove("dragover");
    if (e.dataTransfer.files[0]) loadFile(e.dataTransfer.files[0]);
  });
  dropZone.addEventListener("click", () => fileInput.click());
  fileInput.addEventListener("change", () => {
    if (fileInput.files[0]) loadFile(fileInput.files[0]);
  });

  function loadFile(file) {
    const reader = new FileReader();
    reader.onload = () => {
      try {
        const data = JSON.parse(reader.result);
        dropZone.style.display = "none";
        renderDashboard(data);
      } catch(e) { alert("Invalid JSON file: " + e.message); }
    };
    reader.readAsText(file);
  }
}

/* --- Search / filter --- */
document.getElementById("search").addEventListener("input", function() {
  const q = this.value.toLowerCase();
  document.querySelectorAll(".panel").forEach(p => {
    if (p.classList.contains("panel-stat")) return;
    if (!q) { p.classList.remove("hidden"); return; }
    p.classList.toggle("hidden", !p.textContent.toLowerCase().includes(q));
  });
});

/* --- Active nav tracking via IntersectionObserver --- */
const observer = new IntersectionObserver(entries => {
  entries.forEach(entry => {
    const id = entry.target.id;
    if (!id) return;
    const link = document.querySelector('.topbar-nav a[href="#' + id + '"]');
    if (link) link.classList.toggle("active", entry.isIntersecting);
  });
}, { rootMargin: "-20% 0px -70% 0px" });

new MutationObserver(() => {
  document.querySelectorAll(".panel[id]").forEach(p => observer.observe(p));
}).observe(document.getElementById("bento-grid"), { childList: true });

/* --- Theme toggle (dark/light/cyber) --- */
(function initTheme() {
  const btn = document.getElementById("theme-toggle");
  const root = document.documentElement;
  const STORAGE_KEY = "lvgl-dash-theme";
  const THEMES = ["dark", "light", "cyber"];
  const ICONS = { dark: "🌙", light: "☀️", cyber: "⚡" };
  const TITLES = { dark: "Switch to light theme", light: "Switch to cyber theme", cyber: "Switch to dark theme" };

  function applyTheme(theme) {
    root.setAttribute("data-theme", theme);
    btn.textContent = ICONS[theme] || "🌙";
    btn.title = TITLES[theme] || "";
  }

  /* Restore from localStorage, or follow system preference */
  const saved = localStorage.getItem(STORAGE_KEY);
  if (saved && THEMES.includes(saved)) {
    applyTheme(saved);
  } else if (window.matchMedia("(prefers-color-scheme: light)").matches) {
    applyTheme("light");
  }

  btn.addEventListener("click", () => {
    const cur = root.getAttribute("data-theme") || "dark";
    const idx = THEMES.indexOf(cur);
    const next = THEMES[(idx + 1) % THEMES.length];
    applyTheme(next);
    localStorage.setItem(STORAGE_KEY, next);
  });

  /* Follow system changes when no manual override */
  window.matchMedia("(prefers-color-scheme: light)").addEventListener("change", e => {
    if (!localStorage.getItem(STORAGE_KEY)) {
      applyTheme(e.matches ? "light" : "dark");
    }
  });
})();

})();
