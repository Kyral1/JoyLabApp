from pydantic import BaseModel
from datetime import datetime

class Stat(BaseModel):
    user_id: int
    type: str   # "LED" or "Sound"
    success: bool
    timestamp: datetime
