const categoryEmojis = {
    'school': '📚',
    'home': '🏠',
    'work': '💼',
    'fun': '🎮',
    'shopping': '🛍️'
};

const categoryNames = {
    'school': 'Учеба',
    'home': 'Дом',
    'work': 'Работа',
    'fun': 'Развлечения',
    'shopping': 'Покупки'
};

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
            const categoryEmoji = categoryEmojis[task.category] || '🐱';
            const categoryName = categoryNames[task.category] || 'Без категории';

            taskItem.innerHTML = `
                <div class="task-info">
                    <h3 class="task-title">${task.title || 'Без названия'}</h3>
                    <p class="task-description">${task.description || 'Без описания'}</p>
                    <div>
                        <span class="task-status ${statusClass}">${statusText}</span>
                        <span class="priority-stars">${starsHTML}</span>
                        <span class="task-category">${categoryEmoji} ${categoryName}</span>
                    </div>
                </div>
                <div class="task-actions">
                    <button onclick="updateTask(${task.id}, 'in_progress')" class="kitty-button">🏃‍♀️</button>
                    <button onclick="updateTask(${task.id}, 'done')" class="kitty-button">✅</button>
                    <button onclick="deleteTask(${task.id})" class="kitty-button delete-btn">🗑️</button>
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
                <button onclick="loadTasks()" class="kitty-button">Повторить</button>
            </div>
        `;
    }
}

async function createTask() {
    const title = document.getElementById('taskTitle').value.trim();
    const description = document.getElementById('taskDescription').value.trim();
    const status = document.getElementById('taskStatus').value;
    const category = document.getElementById('taskCategory').value;
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
                title: title,
                description: description,
                status: status,
                category: category,
                priority: priority
            })
        });

        if (response.ok) {
            document.getElementById('taskTitle').value = '';
            document.getElementById('taskDescription').value = '';
            document.getElementById('taskPriority').value = 3;
            document.getElementById('priorityStars').textContent = renderStars(3);

            alert('✨ Задача создана!');
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

document.addEventListener('DOMContentLoaded', function() {
    document.getElementById('taskPriority').addEventListener('input', function() {
        const value = parseInt(this.value);
        document.getElementById('priorityStars').textContent = renderStars(value);
    });

    const prioritySlider = document.getElementById('taskPriority');
    const priorityStars = document.getElementById('priorityStars');
    priorityStars.textContent = renderStars(prioritySlider.value);

    loadTasks();
});

const deleteBtnStyle = document.createElement('style');
deleteBtnStyle.textContent = `
    .delete-btn {
        background: #FF6B6B !important;
    }
    .delete-btn:hover {
        background: #FF4F4F !important;
    }
`;
document.head.appendChild(deleteBtnStyle);

setInterval(loadTasks, 30000);