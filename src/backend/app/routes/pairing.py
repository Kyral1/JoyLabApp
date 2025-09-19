from fastapi import APIRouter

router = APIRouter(prefix="/pair", tags=["Pairing"])

#pair toy and wirstband function
@router.post("/toy")
def pair_toy():
    return {"status": "ok", "message": "Toy paired successfully"}

@router.post("/wristband")
def pair_wristband():
    return {"status": "ok", "message": "Wristband paired successfully"}
