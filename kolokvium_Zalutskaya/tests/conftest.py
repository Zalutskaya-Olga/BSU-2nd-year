import pytest
from fastapi.testclient import TestClient
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