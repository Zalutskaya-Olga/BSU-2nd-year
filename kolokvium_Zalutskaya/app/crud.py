from sqlalchemy.orm import Session
from app.models import TaskModel, TaskStatus
from app.schemas import TaskCreateSchema, TaskUpdateSchema
from typing import Optional, List
import logging

logger_instance = logging.getLogger(__name__)


class TaskCRUD:

    @staticmethod
    def create_task(database_session: Session, task_data: TaskCreateSchema) -> TaskModel:
        try:
            db_task_instance = TaskModel(
                title=task_data.title,
                description=task_data.description,
                status=TaskStatus(task_data.status.value)
            )
            database_session.add(db_task_instance)
            database_session.commit()
            database_session.refresh(db_task_instance)
            logger_instance.info(f"Создана задача с ID: {db_task_instance.id}")
            return db_task_instance
        except Exception as error:
            database_session.rollback()
            logger_instance.error(f"Ошибка создания задачи: {error}")
            raise

    @staticmethod
    def get_task_by_id(database_session: Session, task_id: int) -> Optional[TaskModel]:
        try:
            return database_session.query(TaskModel).filter(TaskModel.id == task_id).first()
        except Exception as error:
            logger_instance.error(f"Ошибка получения задачи {task_id}: {error}")
            raise

    @staticmethod
    def get_all_tasks(database_session: Session, skip: int = 0, limit: int = 100) -> List[TaskModel]:
        try:
            return database_session.query(TaskModel).offset(skip).limit(limit).all()
        except Exception as error:
            logger_instance.error(f"Ошибка получения списка задач: {error}")
            raise

    @staticmethod
    def update_task(
            database_session: Session,
            db_task_instance: TaskModel,
            update_data: TaskUpdateSchema
    ) -> TaskModel:
        try:
            update_dict = update_data.model_dump(exclude_unset=True)

            for field_name, field_value in update_dict.items():
                if field_value is not None:
                    if field_name == "status":
                        setattr(db_task_instance, field_name, TaskStatus(field_value))
                    else:
                        setattr(db_task_instance, field_name, field_value)

            database_session.commit()
            database_session.refresh(db_task_instance)
            logger_instance.info(f"Обновлена задача с ID: {db_task_instance.id}")
            return db_task_instance
        except Exception as error:
            database_session.rollback()
            logger_instance.error(f"Ошибка обновления задачи: {error}")
            raise

    @staticmethod
    def delete_task(database_session: Session, task_id: int) -> bool:
        try:
            db_task_instance = database_session.query(TaskModel).filter(TaskModel.id == task_id).first()
            if not db_task_instance:
                return False

            database_session.delete(db_task_instance)
            database_session.commit()
            logger_instance.info(f"Удалена задача с ID: {task_id}")
            return True
        except Exception as error:
            database_session.rollback()
            logger_instance.error(f"Ошибка удаления задачи {task_id}: {error}")
            raise

    @staticmethod
    def count_tasks(database_session: Session) -> int:
        try:
            return database_session.query(TaskModel).count()
        except Exception as error:
            logger_instance.error(f"Ошибка подсчета задач: {error}")
            raise


task_crud_instance = TaskCRUD()