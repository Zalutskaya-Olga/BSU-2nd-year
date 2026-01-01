from fastapi import HTTPException, status, Depends
from sqlalchemy.orm import Session
from app.database import get_db_dependency
from app.crud import task_crud_instance
from app.models import TaskModel


def get_task_by_id_dependency(
    task_id: int,
    database_session: Session = Depends(get_db_dependency)
) -> TaskModel:
    task_instance = task_crud_instance.get_task_by_id(database_session, task_id)
    if not task_instance:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail=f"Задача с ID {task_id} не найдена"
        )
    return task_instance
