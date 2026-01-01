#!/usr/bin/env python3
import uvicorn
import os
import sys
from pathlib import Path

# Hello Kitty ASCII art
HELLO_KITTY_ART = """
🌸🎀🌸🎀🌸🎀🌸🎀🌸🎀🌸🎀🌸🎀🌸
   🐱 Hello Kitty Todo API 🎀
🌸🎀🌸🎀🌸🎀🌸🎀🌸🎀🌸🎀🌸🎀🌸
"""

if __name__ == "__main__":
    print(HELLO_KITTY_ART)
    print("🚀 Запуск Kawaii Todo List...")
    print("🎀 Главная страница: http://localhost:8000")
    print("📚 Документация: http://localhost:8000/docs")
    print("❤️  Здоровье: http://localhost:8000/health")
    print("🌸 Наслаждайтесь кавайностью!")
    print("-" * 40)

    # Проверяем и создаем необходимые директории
    Path("./data").mkdir(exist_ok=True)
    Path("./logs").mkdir(exist_ok=True)
    Path("./app/static/css").mkdir(exist_ok=True, parents=True)

    uvicorn.run(
        "app.main:app_instance",
        host="0.0.0.0",
        port=8000,
        reload=True,
        log_level="info"
    )