from fastapi import APIRouter
from app.models.stats import Stat
from typing import List

router = APIRouter(prefix="/stats", tags=["Stats"])

stats_db: List[Stat] = []

#frontend sends result, backend stores it
@router.post("/upload")
def upload_stat(stat: Stat):
    stats_db.append(stat)
    return {"status": "ok", "received": stat.dict()}

#frontend asks for stats and gets sent back a list
@router.get("/{user_id}")
def get_stats(user_id: int, period: str = "weekly"):
    filtered = [s.dict() for s in stats_db if s.user_id == user_id]
    return {"user_id": user_id, "period": period, "stats": filtered}
