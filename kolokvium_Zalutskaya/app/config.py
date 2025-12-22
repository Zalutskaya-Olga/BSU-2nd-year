from pydantic_settings import BaseSettings
from typing import Optional


class AppSettings(BaseSettings):
    app_name: str = "Todo List API"
    debug_mode: bool = True
    api_version: str = "v1"

    database_url: str = "sqlite:///./todo.db"

    redis_host: str = "localhost"
    redis_port: int = 6379
    redis_db: int = 0

    log_level: str = "INFO"

    cors_origins: list = ["*"]

    class Config:
        env_file = ".env"
        case_sensitive = True


app_settings_instance = AppSettings()