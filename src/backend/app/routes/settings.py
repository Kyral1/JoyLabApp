from fastapi import APIRouter
from app.models.settings import Settings

router = APIRouter(prefix="/settings", tags=["Settings"])

#default for now --> will keep track of the outputs
current_settings = Settings(volume=50, brightness=75, vibration=True, music_choice="classical")

@router.get("/")
def get_settings():
    return current_settings

@router.post("/update")
def update_settings(new_settings: Settings):
    global current_settings
    current_settings = new_settings
    return {"status": "ok", "updated": new_settings.dict()}
