cd /Users/olga_zalutskaya/Documents/oc/kolokvium_Zalutskaya
python -m pytest tests/test_tasks.py -v




brew services start redis
Проверьте, что Redis работает:
redis-cli ping
# Должен ответить: PONG
python run.py
Откройте в браузере:

http://localhost:8000 - главная страница
http://localhost:8000/docs - документация API
http://localhost:8000/health - проверка здоровья (должен показать Redis как "connected")