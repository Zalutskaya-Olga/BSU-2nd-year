from fastapi import FastAPI, Depends, HTTPException, status
from fastapi.middleware.cors import CORSMiddleware
from contextlib import asynccontextmanager
import logging
from loguru import logger
import redis
from prometheus_client import make_asgi_app, Counter, Histogram

from app.config import app_settings_instance
from app.database import initialize_database, get_db_dependency
from app import crud, schemas, dependencies
from sqlalchemy.orm import Session

logger_instance = logging.getLogger(__name__)

REQUEST_COUNTER = Counter('http_requests_total', 'Total HTTP requests', ['method', 'endpoint', 'status'])
REQUEST_DURATION = Histogram('http_request_duration_seconds', 'HTTP request duration', ['method', 'endpoint'])

redis_client_instance = None


@asynccontextmanager
async def app_lifespan(app_instance: FastAPI):
    try:
        initialize_database()
        logger.info("Приложение запущено")

        global redis_client_instance
        redis_client_instance = redis.Redis(
            host=app_settings_instance.redis_host,
            port=app_settings_instance.redis_port,
            db=app_settings_instance.redis_db,
            decode_responses=True
        )
        redis_client_instance.ping()
        logger.info("Redis подключен")
    except Exception as error:
        logger.error(f"Ошибка запуска: {error}")
        raise

    yield

    if redis_client_instance:
        redis_client_instance.close()
    logger.info("Приложение остановлено")


app_instance = FastAPI(
    title=app_settings_instance.app_name,
    version=app_settings_instance.api_version,
    debug=app_settings_instance.debug_mode,
    lifespan=app_lifespan
)

metrics_app = make_asgi_app()
app_instance.mount("/metrics", metrics_app)

app_instance.add_middleware(
    CORSMiddleware,
    allow_origins=app_settings_instance.cors_origins,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)


@app_instance.middleware("http")
async def collect_metrics_middleware(request, call_next):
    import time
    start_time = time.time()
    response = await call_next(request)
    duration = time.time() - start_time

    REQUEST_COUNTER.labels(
        method=request.method,
        endpoint=request.url.path,
        status=response.status_code
    ).inc()

    REQUEST_DURATION.labels(
        method=request.method,
        endpoint=request.url.path
    ).observe(duration)

    return response


@app_instance.get("/")
async def read_root():
    return {
        "message": f"Добро пожаловать в {app_settings_instance.app_name}",
        "version": app_settings_instance.api_version,
        "docs": "/docs",
        "metrics": "/metrics"
    }


@app_instance.get("/health")
async def health_check():
    health_status = {"status": "healthy"}

    try:
        if redis_client_instance and redis_client_instance.ping():
            health_status["redis"] = "connected"
        else:
            health_status["redis"] = "disconnected"
    except:
        health_status["redis"] = "error"

    return health_status


@app_instance.get("/tasks", response_model=schemas.TasksListResponseSchema)
def read_tasks_list(
        skip_param: int = 0,
        limit_param: int = 100,
        database_session: Session = Depends(get_db_dependency)
):
    try:
        cache_key = f"tasks:{skip_param}:{limit_param}"
        if redis_client_instance:
            cached_data = redis_client_instance.get(cache_key)
            if cached_data:
                logger.info("Данные получены из кэша")
                return eval(cached_data)

        tasks_list = crud.task_crud_instance.get_all_tasks(database_session, skip_param, limit_param)
        total_count = crud.task_crud_instance.count_tasks(database_session)

        response_data = {
            "tasks": tasks_list,
            "total_count": total_count
        }

        if redis_client_instance:
            redis_client_instance.setex(cache_key, 300, str(response_data))

        return response_data
    except Exception as error:
        logger.error(f"Ошибка получения списка задач: {error}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Внутренняя ошибка сервера"
        )


@app_instance.get("/tasks/{task_id}", response_model=schemas.TaskResponseSchema)
def read_single_task(
        task_instance: schemas.TaskResponseSchema = Depends(dependencies.get_task_by_id_dependency)
):
    return task_instance


@app_instance.post(
    "/tasks",
    response_model=schemas.TaskResponseSchema,
    status_code=status.HTTP_201_CREATED
)
def create_new_task(
        task_data: schemas.TaskCreateSchema,
        database_session: Session = Depends(get_db_dependency)
):
    try:
        created_task = crud.task_crud_instance.create_task(database_session, task_data)

        if redis_client_instance:
            redis_client_instance.delete("tasks:*")

        return created_task
    except Exception as error:
        logger.error(f"Ошибка создания задачи: {error}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Не удалось создать задачу"
        )


@app_instance.put("/tasks/{task_id}", response_model=schemas.TaskResponseSchema)
def update_task_completely(
        task_id: int,
        task_data: schemas.TaskUpdateSchema,
        database_session: Session = Depends(get_db_dependency),
        existing_task: schemas.TaskResponseSchema = Depends(dependencies.get_task_by_id_dependency)
):
    try:
        updated_task = crud.task_crud_instance.update_task(database_session, existing_task, task_data)

        if redis_client_instance:
            redis_client_instance.delete(f"task:{task_id}")
            redis_client_instance.delete("tasks:*")

        return updated_task
    except Exception as error:
        logger.error(f"Ошибка обновления задачи: {error}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Не удалось обновить задачу"
        )


@app_instance.patch("/tasks/{task_id}", response_model=schemas.TaskResponseSchema)
def partially_update_task(
        task_id: int,
        task_data: schemas.TaskUpdateSchema,
        database_session: Session = Depends(get_db_dependency),
        existing_task: schemas.TaskResponseSchema = Depends(dependencies.get_task_by_id_dependency)
):
    try:
        updated_task = crud.task_crud_instance.update_task(database_session, existing_task, task_data)

        if redis_client_instance:
            redis_client_instance.delete(f"task:{task_id}")
            redis_client_instance.delete("tasks:*")

        return updated_task
    except Exception as error:
        logger.error(f"Ошибка частичного обновления задачи: {error}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Не удалось обновить задачу"
        )


@app_instance.delete("/tasks/{task_id}", status_code=status.HTTP_204_NO_CONTENT)
def delete_existing_task(
        task_id: int,
        database_session: Session = Depends(get_db_dependency)
):
    try:
        delete_successful = crud.task_crud_instance.delete_task(database_session, task_id)
        if not delete_successful:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail=f"Задача с ID {task_id} не найдена"
            )

        if redis_client_instance:
            redis_client_instance.delete(f"task:{task_id}")
            redis_client_instance.delete("tasks:*")

        return None
    except HTTPException:
        raise
    except Exception as error:
        logger.error(f"Ошибка удаления задачи: {error}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Не удалось удалить задачу"
        )


if __name__ == "__main__":
    import uvicorn

    uvicorn.run(
        "app.main:app_instance",
        host="0.0.0.0",
        port=8000,
        reload=app_settings_instance.debug_mode
    )