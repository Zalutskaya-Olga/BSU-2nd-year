from pydantic import BaseModel, Field, ConfigDict
from typing import Optional
from enum import Enum


class TaskStatusEnum(str, Enum):
    TODO = "todo"
    IN_PROGRESS = "in_progress"
    DONE = "done"


class TaskCreateSchema(BaseModel):
    title: str = Field(..., min_length=1, max_length=255, description="Название задачи")
    description: Optional[str] = Field(None, max_length=1000, description="Описание задачи")
    status: TaskStatusEnum = Field(TaskStatusEnum.TODO, description="Статус задачи")

    model_config = ConfigDict(json_schema_extra={
        "example": {
            "title": "Купить молоко",
            "description": "Обязательно 3.2% жирности",
            "status": "todo"
        }
    })


class TaskUpdateSchema(BaseModel):
    title: Optional[str] = Field(None, min_length=1, max_length=255, description="Название задачи")
    description: Optional[str] = Field(None, max_length=1000, description="Описание задачи")
    status: Optional[TaskStatusEnum] = Field(None, description="Статус задачи")

    model_config = ConfigDict(json_schema_extra={
        "example": {
            "status": "done"
        }
    })


class TaskResponseSchema(BaseModel):
    id: int
    title: str
    description: Optional[str]
    status: TaskStatusEnum

    model_config = ConfigDict(from_attributes=True)


class TasksListResponseSchema(BaseModel):
    tasks: list[TaskResponseSchema]
    total_count: int