from fastapi import FastAPI, Depends, HTTPException, status, Request
from fastapi.middleware.cors import CORSMiddleware
from contextlib import asynccontextmanager
import logging
from loguru import logger
import redis
from prometheus_client import make_asgi_app, Counter, Histogram
from fastapi.responses import HTMLResponse, JSONResponse, RedirectResponse
from fastapi.staticfiles import StaticFiles
import os
from datetime import datetime
import sqlite3

from app.config import app_settings
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

        try:
            if "sqlite" in app_settings.database_url:
                db_path = app_settings.database_url.replace("sqlite:///", "")
                if not os.path.exists(db_path):
                    with open(db_path, 'w') as f:
                        pass
                    logger.info(f"Создан файл базы данных: {db_path}")

                conn = sqlite3.connect(db_path)
                conn.execute("SELECT 1")
                conn.close()
                logger.info("SQLite база данных подключена успешно")
        except Exception as db_error:
            logger.error(f"Ошибка подключения к базе данных: {db_error}")

        global redis_client_instance
        redis_client_instance = redis.Redis(
            host=app_settings.redis_host,
            port=app_settings.redis_port,
            db=app_settings.redis_db,
            decode_responses=True,
            socket_connect_timeout=3,
            socket_timeout=3
        )

        try:
            redis_client_instance.ping()
            logger.info("Redis подключен успешно")
        except redis.ConnectionError:
            logger.warning("Redis не доступен, работаем без кэша")
            redis_client_instance = None
            app_instance.dependency_overrides[get_db_dependency] = get_db_dependency
    except Exception as error:
        logger.error(f"Ошибка запуска: {error}")
        raise

    yield

    if redis_client_instance:
        redis_client_instance.close()
    logger.info("Приложение остановлено")


app_instance = FastAPI(
    title=app_settings.app_name,
    version=app_settings.api_version,
    debug=app_settings.debug_mode,
    lifespan=app_lifespan,
    swagger_ui_parameters={
        "defaultModelsExpandDepth": -1,
        "docExpansion": "none",
        "filter": True,
        "showExtensions": True,
        "showCommonExtensions": True,
        "displayRequestDuration": True,
        "tryItOutEnabled": True,
    }
)

os.makedirs("static", exist_ok=True)
os.makedirs("static/css", exist_ok=True)

app_instance.mount("/static", StaticFiles(directory="static"), name="static")

metrics_app = make_asgi_app()
app_instance.mount("/metrics", metrics_app)

app_instance.add_middleware(
    CORSMiddleware,
    allow_origins=app_settings.cors_origins,
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
async def read_root(request: Request):
    accept = request.headers.get("accept", "")

    if "text/html" in accept:
        html_content = """
        <!DOCTYPE html>
        <html lang="ru">
        <head>
            <meta charset="UTF-8">
            <meta name="viewport" content="width=device-width, initial-scale=1.0">
            <title>🐱 Hello Kitty Todo List 🎀</title>
            <link rel="stylesheet" href="/static/css/kitty-theme.css">
            <style>
                .hero-section {
                    text-align: center;
                    padding: 80px 20px;
                    background: linear-gradient(135deg, #FFB6C1, #FF69B4);
                    border-radius: 30px;
                    margin: 40px 0;
                    color: white;
                    min-height: 300px;
                    display: flex;
                    flex-direction: column;
                    justify-content: center;
                    align-items: center;
                }

                .main-button {
                    display: inline-block;
                    background: white;
                    color: #FF1493;
                    font-size: 28px;
                    padding: 20px 50px;
                    border-radius: 50px;
                    text-decoration: none;
                    font-weight: bold;
                    margin-top: 30px;
                    border: 5px solid #FF1493;
                    box-shadow: 0 10px 30px rgba(255, 20, 147, 0.3);
                    transition: all 0.3s;
                    font-family: 'Comic Sans MS', cursive;
                }

                .main-button:hover {
                    transform: scale(1.1);
                    background: #FF1493;
                    color: white;
                    text-decoration: none;
                }

                .small-links {
                    text-align: center;
                    margin-top: 30px;
                    padding-top: 20px;
                    border-top: 2px dotted #FFB6C1;
                }

                .small-links a {
                    color: #FF69B4;
                    text-decoration: none;
                    margin: 0 15px;
                    font-size: 16px;
                    transition: all 0.3s;
                }

                .small-links a:hover {
                    color: #FF1493;
                    text-decoration: underline;
                }

                .kitty-emoji-large {
                    font-size: 60px;
                    margin: 20px 0;
                }

                .welcome-text {
                    font-size: 24px;
                    margin-bottom: 30px;
                    max-width: 600px;
                }
            </style>
        </head>
        <body class="kitty-body">
            <div class="kitty-container">
                <!-- Заголовок -->
                <h1 class="kitty-title">🐱 Hello Kitty Todo List 🎀</h1>

                <div class="kitty-emoji-large">🌸🎀🐱🎀🌸</div>

                <!-- Главная секция -->
                <div class="hero-section">
                    <div class="welcome-text">
                        Добро пожаловать в самый кавайный менеджер задач!
                    </div>

                    <!-- Главная большая кнопка -->
                    <a href="/kitty/tasks-ui" class="main-button">
                        📝 Мои задачи 🎀
                    </a>
                </div>

                <!-- Маленькие ссылки внизу -->
                <div class="small-links">
                    <a href="/docs">📚 Полная документация</a>
                    |
                    <a href="/health">❤️ Мониторинг</a>
                </div>

                <!-- Футер -->
                <div class="kitty-footer">
                    <p>🌸 Сделано с любовью Hello Kitty 🎀</p>
                    <p>Версия 1.0.0 • Все права защищены бантиками 🐱</p>
                </div>
            </div>

            <script>
                // Простая анимация для кнопки
                document.addEventListener('DOMContentLoaded', function() {
                    const button = document.querySelector('.main-button');

                    // Пульсация кнопки
                    setInterval(() => {
                        button.style.transform = button.style.transform === 'scale(1.05)' ? 'scale(1)' : 'scale(1.05)';
                    }, 2000);
                });
            </script>
        </body>
        </html>
        """
        return HTMLResponse(content=html_content)

    return {
        "message": "🐱 Добро пожаловать в Hello Kitty Todo API! 🎀",
        "version": app_settings.api_version,
        "docs": "/docs",
        "health": "/health",
        "kitty_tip": "Будь милым и продуктивным! 🌸",
        "emoji": "🐱🎀🌸"
    }


@app_instance.get("/health")
async def health_check(request: Request):

    health_status = {
        "status": "healthy ❤️",
        "emoji": "🐱🎀🌸",
        "services": {},
        "kitty_message": "Всё работает отлично! 🎀",
        "timestamp": datetime.now().isoformat(),
        "version": app_settings.api_version
    }

    try:
        if redis_client_instance:
            redis_client_instance.ping()
            health_status["services"]["redis"] = {
                "status": "connected",
                "emoji": "🎀",
                "message": "Redis готов к работе!",
                "details": f"{app_settings.redis_host}:{app_settings.redis_port}"
            }
        else:
            health_status["services"]["redis"] = {
                "status": "not_configured",
                "emoji": "⚙️",
                "message": "Redis не настроен"
            }
    except Exception as e:
        health_status["services"]["redis"] = {
            "status": "error",
            "emoji": "💔",
            "message": str(e)
        }
        health_status["status"] = "degraded ⚠️"

    try:
        if "sqlite" in app_settings.database_url:
            db_path = app_settings.database_url.replace("sqlite:///", "")

            if os.path.exists(db_path):
                conn = sqlite3.connect(db_path)
                cursor = conn.cursor()
                cursor.execute("SELECT 1")
                conn.close()

                health_status["services"]["database"] = {
                    "status": "connected",
                    "emoji": "💾",
                    "message": "База данных работает",
                    "details": f"SQLite: {db_path}"
                }
            else:
                health_status["services"]["database"] = {
                    "status": "file_not_found",
                    "emoji": "📁",
                    "message": f"Файл базы данных не найден: {db_path}",
                    "action": "Будет создан при первой записи"
                }
                health_status["status"] = "degraded ⚠️"
    except Exception as e:
        health_status["services"]["database"] = {
            "status": "error",
            "emoji": "💾💔",
            "message": str(e)
        }
        health_status["status"] = "unhealthy 💔"

    accept = request.headers.get("accept", "")
    if "text/html" in accept:
        redis_status = health_status["services"]["redis"]["status"]
        db_status = health_status["services"]["database"]["status"]

        html_content = f"""
        <!DOCTYPE html>
        <html lang="ru">
        <head>
            <meta charset="UTF-8">
            <title>❤️ Hello Kitty Health Check 🎀</title>
            <link rel="stylesheet" href="/static/css/kitty-theme.css">
            <style>
                .health-grid {{
                    display: grid;
                    grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
                    gap: 20px;
                    margin: 30px 0;
                }}

                .health-card {{
                    background: linear-gradient(135deg, #FFF5F7, #FFE6EB);
                    border-radius: 20px;
                    padding: 25px;
                    border: 3px solid;
                    text-align: center;
                    transition: all 0.3s;
                }}

                .health-card.healthy {{
                    border-color: #1DD1A1;
                }}

                .health-card.warning {{
                    border-color: #FF9F43;
                }}

                .health-card.error {{
                    border-color: #FF6B6B;
                }}

                .status-emoji {{
                    font-size: 50px;
                    margin-bottom: 15px;
                }}

                .status-indicator {{
                    display: inline-block;
                    width: 20px;
                    height: 20px;
                    border-radius: 50%;
                    margin-right: 10px;
                }}

                .status-indicator.healthy {{
                    background: #1DD1A1;
                }}

                .status-indicator.warning {{
                    background: #FF9F43;
                }}

                .status-indicator.error {{
                    background: #FF6B6B;
                }}

                .system-status {{
                    font-size: 24px;
                    font-weight: bold;
                    padding: 15px;
                    border-radius: 20px;
                    margin: 20px 0;
                    text-align: center;
                }}

                .system-status.healthy {{
                    background: linear-gradient(135deg, #E6F7EF, #D1F0E6);
                    border: 3px solid #1DD1A1;
                    color: #1DD1A1;
                }}

                .system-status.warning {{
                    background: linear-gradient(135deg, #FFF5E6, #FFEED1);
                    border: 3px solid #FF9F43;
                    color: #FF9F43;
                }}

                .system-status.error {{
                    background: linear-gradient(135deg, #FFE6EB, #FFD1DC);
                    border: 3px solid #FF6B6B;
                    color: #FF6B6B;
                }}
            </style>
        </head>
        <body class="kitty-body">
            <div class="kitty-container">
                <h1 class="kitty-title">❤️ Проверка здоровья системы 🎀</h1>
                <div class="kitty-emoji">🐱❤️🌸</div>

                <div class="system-status {'healthy' if health_status['status'].startswith('healthy') else 'error'}">
                    <h2>Статус системы: {health_status["status"]}</h2>
                    <p>{health_status["kitty_message"]}</p>
                </div>

                <div class="health-grid">
                    <div class="health-card {'healthy' if redis_status == 'connected' else 'warning' if redis_status == 'not_configured' else 'error'}">
                        <div class="status-emoji">{health_status['services']['redis']['emoji']}</div>
                        <h3>Redis 🎀</h3>
                        <p><span class="status-indicator {'healthy' if redis_status == 'connected' else 'warning' if redis_status == 'not_configured' else 'error'}"></span>
                        {redis_status.upper()}</p>
                        <p>{health_status['services']['redis']['message']}</p>
                        <p><small>{health_status['services']['redis'].get('details', '')}</small></p>
                    </div>

                    <div class="health-card {'healthy' if db_status == 'connected' else 'warning' if db_status == 'file_not_found' else 'error'}">
                        <div class="status-emoji">{health_status['services']['database']['emoji']}</div>
                        <h3>База данных 💾</h3>
                        <p><span class="status-indicator {'healthy' if db_status == 'connected' else 'warning' if db_status == 'file_not_found' else 'error'}"></span>
                        {db_status.upper()}</p>
                        <p>{health_status['services']['database']['message']}</p>
                        <p><small>{health_status['services']['database'].get('details', '')}</small></p>
                    </div>

                    <div class="health-card healthy">
                        <div class="status-emoji">🚀</div>
                        <h3>FastAPI Сервер</h3>
                        <p><span class="status-indicator healthy"></span>РАБОТАЕТ</p>
                        <p>Сервер запущен и отвечает на запросы</p>
                        <p><small>Версия: {app_settings.api_version}</small></p>
                    </div>
                </div>

                <div class="kitty-nav" style="margin-top: 40px;">
                    <a href="/">🏠 На главную</a>
                    <a href="/docs">📚 Документация</a>
                    <a href="/kitty/tasks-ui">🎀 Интерфейс задач</a>
                </div>

                <div class="kitty-footer">
                    <p>Проверено: {datetime.now().strftime("%Y-%m-%d %H:%M:%S")}</p>
                    <p>Версия: {app_settings.api_version}</p>
                </div>
            </div>

            <script>
                // Авто-обновление каждые 10 секунд
                setTimeout(() => location.reload(), 10000);

                // Добавляем анимацию
                document.querySelectorAll('.health-card').forEach(card => {{
                    card.addEventListener('mouseenter', () => {{
                        card.style.transform = 'translateY(-10px)';
                    }});

                    card.addEventListener('mouseleave', () => {{
                        card.style.transform = 'translateY(0)';
                    }});
                }});
            </script>
        </body>
        </html>
        """
        return HTMLResponse(content=html_content)

    return JSONResponse(content=health_status)


@app_instance.get("/kitty")
async def hello_kitty_page():
    return RedirectResponse(url="/")


@app_instance.get("/kitty/tasks-ui")
async def kitty_tasks_ui():
    html_content = """
    <!DOCTYPE html>
    <html lang="ru">
    <head>
        <meta charset="UTF-8">
        <meta name="viewport" content="width=device-width, initial-scale=1.0">
        <title>📝 Hello Kitty Tasks 🎀</title>
        <link rel="stylesheet" href="/static/css/kitty-theme.css">
        <style>
            .tasks-container {
                max-width: 800px;
                margin: 0 auto;
            }

            .task-form {
                background: linear-gradient(135deg, #FFF5F7, #FFE6EB);
                border-radius: 25px;
                padding: 30px;
                margin-bottom: 30px;
                border: 3px solid #FFB6C1;
            }

            .form-group {
                margin-bottom: 20px;
            }

            .form-group label {
                display: block;
                margin-bottom: 8px;
                color: #FF1493;
                font-weight: bold;
            }

            .task-list {
                margin-top: 30px;
            }

            .task-item {
                display: flex;
                justify-content: space-between;
                align-items: center;
                padding: 20px;
                margin: 15px 0;
                background: white;
                border-radius: 20px;
                border: 3px solid #FFB6C1;
                transition: all 0.3s;
            }

            .task-item:hover {
                border-color: #FF69B4;
                transform: translateX(10px);
            }

            .task-info {
                flex: 1;
            }

            .task-actions {
                display: flex;
                gap: 10px;
            }

            .task-status {
                display: inline-block;
                padding: 5px 15px;
                border-radius: 15px;
                font-size: 14px;
                font-weight: bold;
                margin-left: 10px;
            }

            .status-todo { background: #FFE6EB; color: #FF6B6B; }
            .status-in-progress { background: #E6F7F6; color: #4ECDC4; }
            .status-done { background: #E6F7EF; color: #1DD1A1; }

            .priority-stars {
                display: inline-block;
                margin-left: 10px;
                font-size: 18px;
            }

            .priority-star {
                color: #FFD700;
            }

            .priority-star-empty {
                color: #FFB6C1;
            }

            .loading {
                text-align: center;
                padding: 40px;
                font-size: 20px;
                color: #FF69B4;
            }

            .error-message {
                background: linear-gradient(135deg, #FFD1DC, #FFB6C1);
                border: 3px solid #FF6B6B;
                border-radius: 20px;
                padding: 20px;
                text-align: center;
                margin: 20px 0;
            }

            .priority-slider {
                width: 100%;
                margin: 10px 0;
            }

            .stars-display {
                font-size: 24px;
                text-align: center;
                margin: 10px 0;
                min-height: 30px;
            }
        </style>
    </head>
    <body class="kitty-body">
        <div class="kitty-container">
            <h1 class="kitty-title">📝 Мои задачи 🎀</h1>
            <div class="kitty-emoji">🐱📝🌸</div>

            <nav class="kitty-nav">
                <a href="/">🏠 На главную</a>
                <a href="/docs">📚 API Документация</a>
                <a href="/health">❤️ Здоровье</a>
            </nav>

            <div class="task-form">
                <h2 class="kitty-subtitle">✨ Создать новую задачу</h2>
                <div class="form-group">
                    <label for="taskTitle">Название задачи:</label>
                    <input type="text" id="taskTitle" class="kitty-input" placeholder="Что нужно сделать? 🎀">
                </div>

                <div class="form-group">
                    <label for="taskDescription">Описание:</label>
                    <textarea id="taskDescription" class="kitty-input" rows="3" placeholder="Подробности... 🌸"></textarea>
                </div>

                <div class="form-group">
                    <label for="taskStatus">Статус:</label>
                    <select id="taskStatus" class="kitty-select">
                        <option value="todo">📝 Сделать</option>
                        <option value="in_progress">🏃‍♀️ В процессе</option>
                        <option value="done">✅ Выполнено</option>
                    </select>
                </div>

                <div class="form-group">
                    <label for="taskPriority">Приоритет (1-5):</label>
                    <input type="range" id="taskPriority" min="1" max="5" value="3" class="priority-slider">
                    <div id="priorityStars" class="stars-display">
                        ⭐⭐⭐⭐⭐
                    </div>
                </div>

                <button onclick="createTask()" class="kitty-button">🎀 Создать задачу</button>
            </div>

            <div class="tasks-container">
                <h2 class="kitty-subtitle">🌸 Мои задачи</h2>
                <div id="taskList" class="task-list">
                    <div class="loading">
                        <p>Загрузка задач... 🐱</p>
                        <div class="kitty-emoji">⏳</div>
                    </div>
                </div>
            </div>
        </div>

        <script>
            // Функция для отображения звезд
            function renderStars(priority) {
                priority = Math.min(5, Math.max(1, parseInt(priority) || 3));
                let stars = '';
                for (let i = 1; i <= 5; i++) {
                    if (i <= priority) {
                        stars += '⭐';
                    } else {
                        stars += '☆';
                    }
                }
                return stars;
            }

            // Обновляем звездочки приоритета
            document.getElementById('taskPriority').addEventListener('input', function() {
                const value = parseInt(this.value);
                document.getElementById('priorityStars').textContent = renderStars(value);
            });

            // Инициализируем звезды
            document.addEventListener('DOMContentLoaded', function() {
                const prioritySlider = document.getElementById('taskPriority');
                const priorityStars = document.getElementById('priorityStars');
                priorityStars.textContent = renderStars(prioritySlider.value);
            });

            // Загружаем задачи
            async function loadTasks() {
                try {
                    const response = await fetch('/tasks');
                    if (!response.ok) throw new Error('Ошибка сервера');

                    const data = await response.json();
                    const taskList = document.getElementById('taskList');

                    if (!data.tasks || data.tasks.length === 0) {
                        taskList.innerHTML = `
                            <div class="kitty-message">
                                <p>У вас пока нет задач! 🎀</p>
                                <p>Создайте первую задачу выше! ✨</p>
                            </div>
                        `;
                        return;
                    }

                    taskList.innerHTML = '';

                    data.tasks.forEach(task => {
                        const taskItem = document.createElement('div');
                        taskItem.className = 'task-item';

                        const statusClass = {
                            'todo': 'status-todo',
                            'in_progress': 'status-in-progress',
                            'done': 'status-done'
                        }[task.status] || 'status-todo';

                        const statusText = {
                            'todo': '📝 Сделать',
                            'in_progress': '🏃‍♀️ В процессе',
                            'done': '✅ Выполнено'
                        }[task.status] || '📝 Сделать';

                        const starsHTML = renderStars(task.priority || 3);

                        taskItem.innerHTML = `
                            <div class="task-info">
                                <h3 style="margin: 0;">${task.title || 'Без названия'}</h3>
                                <p style="margin: 5px 0; color: #666;">${task.description || 'Без описания'}</p>
                                <div>
                                    <span class="task-status ${statusClass}">${statusText}</span>
                                    <span class="priority-stars">${starsHTML}</span>
                                </div>
                            </div>
                            <div class="task-actions">
                                <button onclick="updateTask(${task.id}, 'in_progress')" class="kitty-button" style="padding: 8px 16px;">🏃‍♀️</button>
                                <button onclick="updateTask(${task.id}, 'done')" class="kitty-button" style="padding: 8px 16px;">✅</button>
                                <button onclick="deleteTask(${task.id})" class="kitty-button" style="padding: 8px 16px; background: #FF6B6B;">🗑️</button>
                            </div>
                        `;

                        taskList.appendChild(taskItem);
                    });

                } catch (error) {
                    console.error('Ошибка загрузки задач:', error);
                    document.getElementById('taskList').innerHTML = `
                        <div class="error-message">
                            <p>😿 Не удалось загрузить задачи</p>
                            <p>Проверьте подключение к серверу</p>
                            <button onclick="loadTasks()" class="kitty-button" style="margin-top: 10px;">Повторить</button>
                        </div>
                    `;
                }
            }

            // Создаем задачу
            async function createTask() {
                const title = document.getElementById('taskTitle').value.trim();
                const description = document.getElementById('taskDescription').value.trim();
                const status = document.getElementById('taskStatus').value;
                const priority = parseInt(document.getElementById('taskPriority').value);

                if (!title) {
                    alert('🎀 Введите название задачи!');
                    return;
                }

                try {
                    const response = await fetch('/tasks', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({
                            title: title + (title.includes('🎀') ? '' : ' 🎀'),
                            description: description,
                            status: status,
                            priority: priority,
                            category: 'fun'
                        })
                    });

                    if (response.ok) {
                        // Очищаем форму
                        document.getElementById('taskTitle').value = '';
                        document.getElementById('taskDescription').value = '';
                        document.getElementById('taskPriority').value = 3;
                        document.getElementById('priorityStars').textContent = renderStars(3);

                        // Показываем сообщение
                        alert('✨ Задача создана!');

                        // Обновляем список
                        loadTasks();
                    } else {
                        const error = await response.json();
                        alert('😿 Ошибка: ' + (error.detail || 'Не удалось создать задачу'));
                    }
                } catch (error) {
                    console.error('Ошибка:', error);
                    alert('💔 Не удалось создать задачу');
                }
            }

            // Обновляем задачу
            async function updateTask(taskId, newStatus) {
                try {
                    const response = await fetch(`/tasks/${taskId}`, {
                        method: 'PATCH',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify({ status: newStatus })
                    });

                    if (response.ok) {
                        alert('✅ Задача обновлена!');
                        loadTasks();
                    }
                } catch (error) {
                    console.error('Ошибка:', error);
                    alert('😿 Не удалось обновить задачу');
                }
            }

            // Удаляем задачу
            async function deleteTask(taskId) {
                if (!confirm('🎀 Удалить задачу?')) return;

                try {
                    const response = await fetch(`/tasks/${taskId}`, {
                        method: 'DELETE'
                    });

                    if (response.ok) {
                        alert('🗑️ Задача удалена!');
                        loadTasks();
                    }
                } catch (error) {
                    console.error('Ошибка:', error);
                    alert('😿 Не удалось удалить задачу');
                }
            }

            // Загружаем задачи при загрузке страницы
            document.addEventListener('DOMContentLoaded', loadTasks);

            // Авто-обновление каждые 30 секунд
            setInterval(loadTasks, 30000);
        </script>
    </body>
    </html>
    """
    return HTMLResponse(content=html_content)


@app_instance.get("/tasks", response_model=schemas.TasksListResponseSchema)
def read_tasks_list(
        skip_param: int = 0,
        limit_param: int = 100,
        database_session: Session = Depends(get_db_dependency)
):
    try:
        cache_key = f"tasks:{skip_param}:{limit_param}"
        if redis_client_instance:
            try:
                cached_data = redis_client_instance.get(cache_key)
                if cached_data:
                    logger.info("Данные получены из кэша")
                    return eval(cached_data)
            except:
                pass

        tasks_list = crud.task_crud_instance.get_all_tasks(database_session, skip_param, limit_param)
        total_count = crud.task_crud_instance.count_tasks(database_session)

        converted_tasks = []
        for task in tasks_list:
            converted_tasks.append(schemas.TaskResponseSchema.from_orm(task))

        response_data = {
            "tasks": converted_tasks,
            "total": total_count,
            "emoji": "🐱🎀🌸",
            "theme": app_settings.theme.value if hasattr(app_settings.theme, 'value') else app_settings.theme,
            "message": "Вот твои кавайные задачи!"
        }

        if redis_client_instance:
            try:
                redis_client_instance.setex(cache_key, 300, str(response_data))
            except:
                pass

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
    return schemas.TaskResponseSchema.from_orm(task_instance)


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
        task_dict = task_data.model_dump()
        created_task = crud.task_crud_instance.create_task(database_session, task_dict)

        if redis_client_instance:
            try:
                redis_client_instance.delete("tasks:*")
            except:
                pass

        return schemas.TaskResponseSchema.from_orm(created_task)
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
        update_dict = task_data.model_dump(exclude_unset=True)
        updated_task = crud.task_crud_instance.update_task(database_session, existing_task, update_dict)

        if redis_client_instance:
            try:
                redis_client_instance.delete(f"task:{task_id}")
                redis_client_instance.delete("tasks:*")
            except:
                pass

        return schemas.TaskResponseSchema.from_orm(updated_task)
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
        update_dict = task_data.model_dump(exclude_unset=True)
        updated_task = crud.task_crud_instance.update_task(database_session, existing_task, update_dict)

        if redis_client_instance:
            try:
                redis_client_instance.delete(f"task:{task_id}")
                redis_client_instance.delete("tasks:*")
            except:
                pass

        return schemas.TaskResponseSchema.from_orm(updated_task)
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
            try:
                redis_client_instance.delete(f"task:{task_id}")
                redis_client_instance.delete("tasks:*")
            except:
                pass

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
        reload=app_settings.debug_mode
    )
