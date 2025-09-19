from pydantic import BaseModel

class Settings(BaseModel):
    volume: int
    brightness: int
    vibration: bool
    music_choice: str
