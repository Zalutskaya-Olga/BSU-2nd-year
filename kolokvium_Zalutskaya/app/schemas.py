from pydantic import BaseModel, Field, ConfigDict, validator
from typing import Optional, List
from enum import Enum
import random


class TaskStatusEnum(str, Enum):
    TODO = "todo"
    IN_PROGRESS = "in_progress"
    DONE = "done"

    @property
    def emoji(self):
        emojis = {
            "todo": "📝",
            "in_progress": "🏃‍♀️",
            "done": "✅"
        }
        return emojis.get(self.value, "🎀")


class KittyCategory(str, Enum):
    SCHOOL = "school"
    HOME = "home"
    WORK = "work"
    FUN = "fun"
    SHOPPING = "shopping"


class TaskCreateSchema(BaseModel):
    title: str = Field(..., min_length=1, max_length=255)
    description: Optional[str] = Field(None, max_length=1000)
    status: TaskStatusEnum = Field(default=TaskStatusEnum.TODO)
    category: Optional[KittyCategory] = Field(default=KittyCategory.FUN)
    priority: int = Field(default=1, ge=1, le=5)

    @validator('title')
    def add_kitty_charm(cls, v):
        kitty_suffixes = [" 🎀", " 🐱", " 🌸", " ❤️", " ✨"]
        if not any(suffix in v for suffix in kitty_suffixes):
            v += random.choice(kitty_suffixes)
        return v

    model_config = ConfigDict(json_schema_extra={
        "example": {
            "title": "Купить молоко для печенья 🎀",
            "description": "Обязательно с розовой упаковкой! 🌸",
            "status": "todo",
            "category": "shopping",
            "priority": 3
        }
    })


class TaskUpdateSchema(BaseModel):
    title: Optional[str] = Field(None, min_length=1, max_length=255)
    description: Optional[str] = Field(None, max_length=1000)
    status: Optional[TaskStatusEnum] = Field(None)
    category: Optional[KittyCategory] = Field(None)
    priority: Optional[int] = Field(None, ge=1, le=5)

    model_config = ConfigDict(json_schema_extra={
        "example": {
            "status": "done",
            "priority": 5,
            "category": "fun"
        }
    })


class TaskResponseSchema(BaseModel):
    id: int
    title: str
    description: Optional[str]
    status: TaskStatusEnum
    category: KittyCategory
    priority: int
    created_at: Optional[str] = None
    completed_at: Optional[str] = None

    @classmethod
    def from_orm(cls, obj):
        data = {
            "id": obj.id,
            "title": obj.title,
            "description": obj.description,
            "status": obj.status.value if hasattr(obj.status, 'value') else obj.status,
            "category": obj.category.value if hasattr(obj.category, 'value') else obj.category,
            "priority": obj.priority,
            "created_at": obj.created_at.isoformat() if obj.created_at else None,
            "completed_at": obj.completed_at.isoformat() if obj.completed_at else None
        }
        return cls(**data)

    @property
    def decorated_title(self):
        decorations = {
            "school": "📚",
            "home": "🏠",
            "work": "💼",
            "fun": "🎮",
            "shopping": "🛍️"
        }
        return f"{decorations.get(self.category, '🎀')} {self.title}"

    model_config = ConfigDict(from_attributes=True)


class TasksListResponseSchema(BaseModel):
    emoji: str = "🐱🎀🌸"
    theme: str
    tasks: List[TaskResponseSchema]
    total: int
    message: str = "Вот твои кавайные задачи!"

    model_config = ConfigDict(from_attributes=True)
