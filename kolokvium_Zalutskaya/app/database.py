from sqlalchemy import create_engine
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker, Session
from app.config import app_settings
from contextlib import contextmanager
import logging

logger_instance = logging.getLogger(__name__)

database_engine = create_engine(
    app_settings.database_url,
    connect_args={"check_same_thread": False} if "sqlite" in app_settings.database_url else {}
)

SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=database_engine)

BaseModel = declarative_base()


def initialize_database():
    try:
        BaseModel.metadata.create_all(bind=database_engine)
        logger_instance.info("База данных успешно инициализирована")
    except Exception as error:
        logger_instance.error(f"Ошибка инициализации базы данных: {error}")
        raise


@contextmanager
def get_database_session():
    database_session = SessionLocal()
    try:
        yield database_session
        database_session.commit()
    except Exception as error:
        database_session.rollback()
        logger_instance.error(f"Ошибка в сессии БД: {error}")
        raise
    finally:
        database_session.close()


def get_db_dependency():
    with get_database_session() as session:
        yield session
