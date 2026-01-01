
import pytest
from fastapi.testclient import TestClient
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

os.environ['IGNORE_ENV_FILE'] = 'true'

import app.config

app.config.app_settings = type('TestSettings', (), {
    'app_name': "Kawaii Todo List 🎀",
    'debug_mode': True,
    'api_version': "v1.0.0",
    'cors_origins': ["*"],
    'theme': "hello_kitty",
    'primary_color': "#FF69B4",
    'secondary_color': "#FFFFFF",
    'accent_color': "#FFB6C1",
    'ribbon_color': "#FF1493",
    'database_url': "sqlite:///./test_todo.db",
    'redis_host': "localhost",
    'redis_port': 6379,
    'redis_db': 0,
    'redis_password': None,
    'enable_kitty_sounds': True,
    'kitty_emoji': "🐱🎀🌸",
    'default_bow': "pink",
})()

from app.main import app_instance


@pytest.fixture
def test_client():
    with TestClient(app_instance) as client:
        yield client


@pytest.fixture
def sample_task_data():
    return {
        "title": "Тестовая задача",
        "description": "Тестовое описание",
        "status": "todo"
    }
