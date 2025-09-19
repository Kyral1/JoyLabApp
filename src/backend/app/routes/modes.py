from fastapi import APIRouter

router = APIRouter(prefix="/modes", tags=["Modes"])

#available modes
@router.get("/")
def list_modes():
    return {"modes": ["LED Focused", "Sound Focused", "Both"]}

#frontend tells backend which modes to switch to
@router.post("/set")
def set_mode(mode: str):
    return {"status": "ok", "mode": mode}
