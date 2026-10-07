"""Real LVGL smoke coverage; API contracts live in the small fixture."""

from conftest import COLLECTIONS, REPO, generate, objects


def test_full_lvgl_generation(tmp_path):
    before = {p.relative_to(REPO) for p in REPO.rglob("*.json")}
    data = generate(REPO, tmp_path / "output")
    assert set(data) == COLLECTIONS
    assert all(isinstance(data[key], list) for key in COLLECTIONS)
    for key in ("functions", "enums", "structures", "typedefs", "macros"):
        assert data[key], f"full generation unexpectedly lost {key}"
    assert any(isinstance(m["initializer"], str) and m["initializer"] for m in data["macros"])
    assert not [obj for obj in objects(data) if obj.get("json_type") == "unknown_type"]
    assert {p.relative_to(REPO) for p in REPO.rglob("*.json")} == before
