import pytest
import sys
import os

sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from fastapi.testclient import TestClient
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker
from app.database import BaseModel

TEST_DATABASE_URL = "sqlite:///./test_todo.db"
test_engine = create_engine(TEST_DATABASE_URL, connect_args={"check_same_thread": False})
TestingSessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=test_engine)

from app.main import app_instance

def override_get_db():
    try:
        db = TestingSessionLocal()
        yield db
    finally:
        db.close()

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
    'database_url': TEST_DATABASE_URL,
    'redis_host': "localhost",
    'redis_port': 6379,
    'redis_db': 0,
    'redis_password': None,
    'enable_kitty_sounds': True,
    'kitty_emoji': "🐱🎀🌸",
    'default_bow': "pink",
})()

from app.database import get_db_dependency
app_instance.dependency_overrides[get_db_dependency] = override_get_db

test_client = TestClient(app_instance)


@pytest.fixture(scope="function", autouse=True)
def setup_test_database():
    BaseModel.metadata.create_all(bind=test_engine)
    yield
    BaseModel.metadata.drop_all(bind=test_engine)


def test_read_root():
    response = test_client.get("/")
    assert response.status_code == 200
    assert "message" in response.json()


def test_create_and_read_task(setup_test_database):
    task_data = {
        "title": "Тестовая задача",
        "description": "Описание тестовой задачи",
        "status": "todo"
    }

    response = test_client.post("/tasks", json=task_data)
    assert response.status_code == 201
    created_task = response.json()
    assert task_data["title"] in created_task["title"]
    assert created_task["status"] == task_data["status"]

    task_id = created_task["id"]
    response = test_client.get(f"/tasks/{task_id}")
    assert response.status_code == 200
    retrieved_task = response.json()
    assert retrieved_task["id"] == task_id
    assert task_data["title"] in retrieved_task["title"]


def test_get_all_tasks(setup_test_database):
    for i in range(3):
        task_data = {
            "title": f"Задача {i}",
            "description": f"Описание {i}",
            "status": "todo"
        }
        test_client.post("/tasks", json=task_data)

    response = test_client.get("/tasks")
    assert response.status_code == 200
    tasks_data = response.json()
    assert "tasks" in tasks_data
    assert len(tasks_data["tasks"]) == 3


def test_update_task(setup_test_database):
    task_data = {
        "title": "Задача для обновления",
        "description": "Старое описание",
        "status": "todo"
    }
    response = test_client.post("/tasks", json=task_data)
    task_id = response.json()["id"]

    update_data = {
        "title": "Обновленное название",
        "status": "in_progress"
    }
    response = test_client.put(f"/tasks/{task_id}", json=update_data)
    assert response.status_code == 200
    updated_task = response.json()
    assert updated_task["title"] == update_data["title"]
    assert updated_task["status"] == update_data["status"]


def test_partial_update_task(setup_test_database):
    task_data = {
        "title": "Задача для частичного обновления",
        "description": "Описание",
        "status": "todo"
    }
    response = test_client.post("/tasks", json=task_data)
    task_id = response.json()["id"]

    update_data = {"status": "done"}
    response = test_client.patch(f"/tasks/{task_id}", json=update_data)
    assert response.status_code == 200
    updated_task = response.json()
    assert updated_task["status"] == "done"
    assert task_data["title"] in updated_task["title"]


def test_delete_task(setup_test_database):
    task_data = {
        "title": "Задача для удаления",
        "description": "Будет удалена",
        "status": "todo"
    }
    response = test_client.post("/tasks", json=task_data)
    task_id = response.json()["id"]

    response = test_client.delete(f"/tasks/{task_id}")
    assert response.status_code == 204

    response = test_client.get(f"/tasks/{task_id}")
    assert response.status_code == 404


def test_task_not_found():
    response = test_client.get("/tasks/999999")
    assert response.status_code == 404


def test_invalid_task_data():
    invalid_data = {
        "title": "",
        "status": "invalid_status"
    }
    response = test_client.post("/tasks", json=invalid_data)
    assert response.status_code == 422


def test_health_check():
    response = test_client.get("/health")
    assert response.status_code == 200
    assert "status" in response.json()
