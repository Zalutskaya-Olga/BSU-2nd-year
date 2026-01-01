cd /Users/olga_zalutskaya/Documents/oc/kolokvium_Zalutskay
python -m pytest tests/test_tasks.py -v



в первом терминале 
brew services start redis
redis-cli CONFIG SET requirepass "kitty_password"

в 2 терменале 
docker-compose build --no-cache

docker-compose up -d

docker-compose ps

docker-compose logs -f api

docker-compose down





 🌐 Доступ к сервисам
API и главная страница: http://localhost:8888
