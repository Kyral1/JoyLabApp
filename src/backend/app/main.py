#Entry point of the backend, creates FastAPI App
#FastAPI is the translator between the frontend and the embedded controls
#written in python, but speaks in web languages

from fastapi import FastAPI
from app.routes import settings, stats, pairing, modes, users #imports seperate routes files

app = FastAPI(title="JoyLab Backend")

# Register routers (each page of the app)
app.include_router(settings.router)
app.include_router(stats.router)
app.include_router(pairing.router)
app.include_router(modes.router)
app.include_router(users.router)  # Register the users router

#can get rid off later (just confirmation for now)
@app.get("/")
def root():
    return {"message": "JoyLab backend is running"}