
class TaskModel(BaseModel):
    __tablename__ = "kitty_tasks"

    id = Column(Integer, primary_key=True, index=True)
    title = Column(String(255), nullable=False, index=True)
    description = Column(String(1000), nullable=True)
    status = Column(Enum(TaskStatus), default=TaskStatus.TODO, nullable=False)
    category = Column(Enum(KittyCategory), default=KittyCategory.FUN, nullable=False)
    priority = Column(Integer, default=1, nullable=False)
    created_at = Column(DateTime, default=func.now(), nullable=False)
    completed_at = Column(DateTime, nullable=True)

    def to_dict(self):
        return {
            "id": self.id,
            "title": self.title,
            "description": self.description,
            "status": self.status.value,
            "status_emoji": self.get_status_emoji(),
            "category": self.category.value,
            "category_emoji": self.get_category_emoji(),
            "priority": self.priority,
            "priority_stars": "⭐" * min(self.priority, 5),
            "created_at": self.created_at.isoformat() if self.created_at else None,
            "completed_at": self.completed_at.isoformat() if self.completed_at else None,
            "kitty_style": {
                "ribbon": self.get_ribbon_color(),
                "theme": "hello_kitty"
            }
        }

    def get_status_emoji(self):
        emojis = {
            TaskStatus.TODO: "📝",
            TaskStatus.IN_PROGRESS: "🏃‍♀️",
            TaskStatus.DONE: "✅🎀"
        }
        return emojis.get(self.status, "🎀")

    def get_category_emoji(self):
        emojis = {
            KittyCategory.SCHOOL: "📚",
            KittyCategory.HOME: "🏠",
            KittyCategory.WORK: "💼",
            KittyCategory.FUN: "🎮",
            KittyCategory.SHOPPING: "🛍️"
        }
        return emojis.get(self.category, "🐱")

    def get_ribbon_color(self):
        colors = {
            1: "#FFB6C1",
            2: "#FF69B4",
            3: "#FF1493",
            4: "#DB7093",
            5: "#C71585"
        }
        return colors.get(self.priority, "#FF69B4")
