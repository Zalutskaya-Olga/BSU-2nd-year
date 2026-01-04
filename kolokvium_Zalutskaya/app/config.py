from pydantic_settings import BaseSettings
from typing import Optional, List
from enum import Enum


class KittyTheme(str, Enum):
    HELLO_KITTY = "hello_kitty"
    MY_MELODY = "my_melody"
    CINNAMOROLL = "cinnamoroll"
    KUROMI = "kuromi"


class AppSettings(BaseSettings):
    app_name: str = "Kawaii Todo List 🎀"
    debug_mode: bool = True
    api_version: str = "v1.0.0"

    cors_origins: List[str] = ["*"]

    theme: KittyTheme = KittyTheme.HELLO_KITTY
    primary_color: str = "#FF69B4"
    secondary_color: str = "#FFFFFF"
    accent_color: str = "#FFB6C1"
    ribbon_color: str = "#FF1493"

    database_url: str = "sqlite:///./data/kitty_tasks.db"

    redis_host: str = "redis"
    redis_port: int = 6379
    redis_db: int = 0
    redis_password: Optional[str] = "kitty_password"

    enable_kitty_sounds: bool = True
    kitty_emoji: str = "🐱🎀🌸"
    default_bow: str = "pink"

    model_config = {
        "env_file": ".env",
        "env_file_encoding": "utf-8",
        "case_sensitive": False,
        "env_prefix": "",
        "extra": "allow"
    }


app_settings = AppSettings()
