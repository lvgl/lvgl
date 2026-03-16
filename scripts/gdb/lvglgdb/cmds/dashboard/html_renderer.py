import json
from pathlib import Path

_STATIC_DIR = Path(__file__).parent / "static"


def render(data: dict, output_path: str) -> None:
    """Generate self-contained HTML with JSON data embedded."""
    json_str = _safe_json_encode(data)
    html = _build_html(json_str)
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(html)


def render_viewer(output_path: str) -> None:
    """Generate empty shell HTML viewer (no embedded data)."""
    html = _build_html("")
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(html)


def _safe_json_encode(data: dict) -> str:
    """Serialize dict to JSON with HTML-safe escaping."""
    raw = json.dumps(data, ensure_ascii=False, indent=None)
    # Escape HTML-special chars to prevent injection in <script> block
    return raw.replace("&", "\\u0026").replace("<", "\\u003c").replace(">", "\\u003e")


def _read_static(filename: str) -> str:
    """Read a static asset file from the static/ directory."""
    return (_STATIC_DIR / filename).read_text(encoding="utf-8")


def _build_html(json_content: str) -> str:
    """Build the complete HTML page with embedded or empty JSON."""
    template = _read_static("template.html")
    css = _read_static("style.css")
    js = _read_static("dashboard.js")
    # Deterministic single-replacement order: CSS → JS → JSON_DATA.
    # Each placeholder is replaced exactly once (count=1) so that content
    # injected in an earlier step cannot be mis-interpreted as a later placeholder.
    return (
        template
        .replace("{{CSS}}", css, 1)
        .replace("{{JS}}", js, 1)
        .replace("{{JSON_DATA}}", json_content, 1)
    )
