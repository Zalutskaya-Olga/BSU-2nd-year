document.addEventListener('DOMContentLoaded', function() {
    loadStats();
    setupTaskForm();
    addAnimations();
    setInterval(loadStats, 30000);
});

async function loadStats() {
    try {
        const statsResponse = await fetch('/stats');
        if (statsResponse.ok) {
            const stats = await statsResponse.json();
            updateStatsDisplay(stats);
        }

        const tasksResponse = await fetch('/tasks');
        if (tasksResponse.ok) {
            const data = await tasksResponse.json();
            updateTasksDisplay(data);
        }
    } catch (error) {
        console.error('Ошибка загрузки статистики:', error);
        showError('😿 Не удалось загрузить статистику');
    }
}

function updateStatsDisplay(stats) {
    const totalElement = document.getElementById('total-tasks');
    const completedElement = document.getElementById('completed-tasks');
    const pendingElement = document.getElementById('pending-tasks');

    if (totalElement) totalElement.textContent = stats.total || '0';
    if (completedElement) completedElement.textContent = stats.completed || '0';
    if (pendingElement) pendingElement.textContent = stats.pending || '0';

    animateCounter(totalElement, stats.total);
    animateCounter(completedElement, stats.completed);
    animateCounter(pendingElement, stats.pending);
}

function updateTasksDisplay(data) {
    const taskList = document.getElementById('recent-tasks');
    if (!taskList) return;

    const recentTasks = data.tasks.slice(0, 3);
    taskList.innerHTML = '';

    recentTasks.forEach(task => {
        const taskElement = document.createElement('div');
        taskElement.className = 'mini-task';
        taskElement.innerHTML = `
            <div class="mini-task-title">${task.title}</div>
            <div class="mini-task-status ${task.status}">${getStatusText(task.status)}</div>
        `;
        taskList.appendChild(taskElement);
    });
}

function animateCounter(element, target) {
    if (!element) return;

    const current = parseInt(element.textContent) || 0;
    const diff = target - current;

    if (diff === 0) return;

    const duration = 500;
    const steps = 20;
    const increment = diff / steps;
    let currentStep = 0;

    const timer = setInterval(() => {
        currentStep++;
        const newValue = Math.round(current + (increment * currentStep));
        element.textContent = newValue;

        if (currentStep >= steps) {
            clearInterval(timer);
            element.textContent = target;
        }
    }, duration / steps);
}

function getStatusText(status) {
    const statusMap = {
        'todo': '📝 Сделать',
        'in_progress': '🏃‍♀️ В процессе',
        'done': '✅ Выполнено'
    };
    return statusMap[status] || '📝 Сделать';
}

function setupTaskForm() {
    const form = document.getElementById('create-task-form');
    if (!form) return;

    form.addEventListener('submit', async function(e) {
        e.preventDefault();

        const title = document.getElementById('task-title').value.trim();
        const description = document.getElementById('task-description').value.trim();
        const completed = document.getElementById('task-completed').checked;

        if (!title) {
            showError('🎀 Введите название задачи!');
            return;
        }

        try {
            const response = await fetch('/tasks', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({
                    title: title,
                    description: description,
                    status: completed ? 'done' : 'todo',
                    category: 'fun',
                    priority: 3
                })
            });

            if (response.ok) {
                const result = await response.json();
                showSuccess('✨ Задача создана успешно!');
                form.reset();
                loadStats();
                createConfetti();
            } else {
                const error = await response.json();
                showError('😿 Ошибка: ' + (error.detail || 'Не удалось создать задачу'));
            }
        } catch (error) {
            console.error('Ошибка:', error);
            showError('💔 Не удалось создать задачу');
        }
    });
}

function showSuccess(message) {
    showNotification(message, 'success');
}

function showError(message) {
    showNotification(message, 'error');
}

function showNotification(message, type) {
    const existingNotification = document.querySelector('.kitty-notification');
    if (existingNotification) {
        existingNotification.remove();
    }

    const notification = document.createElement('div');
    notification.className = `kitty-notification ${type}`;
    notification.innerHTML = `
        <div class="notification-content">
            <span class="notification-emoji">${type === 'success' ? '🎀' : '😿'}</span>
            <span class="notification-text">${message}</span>
        </div>
    `;

    document.body.appendChild(notification);

    setTimeout(() => notification.classList.add('show'), 10);

    setTimeout(() => {
        notification.classList.remove('show');
        setTimeout(() => notification.remove(), 300);
    }, 3000);
}

function addAnimations() {
    const cards = document.querySelectorAll('.stat-card, .feature-card, .action-btn');
    cards.forEach(card => {
        card.addEventListener('mouseenter', () => {
            card.style.transform = 'translateY(-5px)';
        });

        card.addEventListener('mouseleave', () => {
            card.style.transform = 'translateY(0)';
        });
    });

    animateBows();
}

function animateBows() {
    const bows = document.querySelectorAll('.bow, .ribbon');
    bows.forEach((bow, index) => {
        bow.style.animation = `float ${3 + index * 0.5}s ease-in-out infinite`;
    });
}

function createConfetti() {
    const confettiCount = 50;
    const colors = ['#FF69B4', '#FFB6C1', '#FF1493', '#FFD700', '#FF99CC'];

    for (let i = 0; i < confettiCount; i++) {
        const confetti = document.createElement('div');
        confetti.className = 'confetti';
        confetti.style.cssText = `
            position: fixed;
            width: 10px;
            height: 10px;
            background: ${colors[Math.floor(Math.random() * colors.length)]};
            top: -20px;
            left: ${Math.random() * 100}vw;
            border-radius: 50%;
            z-index: 9999;
            pointer-events: none;
            animation: confetti-fall ${1 + Math.random() * 2}s linear forwards;
            transform: rotate(${Math.random() * 360}deg);
        `;

        document.body.appendChild(confetti);

        setTimeout(() => confetti.remove(), 3000);
    }
}

const style = document.createElement('style');
style.textContent = `
    @keyframes float {
        0%, 100% { transform: translateY(0); }
        50% { transform: translateY(-10px); }
    }

    @keyframes confetti-fall {
        0% {
            transform: translateY(0) rotate(0deg);
            opacity: 1;
        }
        100% {
            transform: translateY(100vh) rotate(720deg);
            opacity: 0;
        }
    }

    .kitty-notification {
        position: fixed;
        top: 20px;
        right: 20px;
        background: white;
        padding: 15px 25px;
        border-radius: 20px;
        box-shadow: 0 10px 30px rgba(255, 105, 180, 0.3);
        border: 3px solid;
        transform: translateX(100%);
        transition: transform 0.3s ease-out;
        z-index: 10000;
        font-family: 'Comic Sans MS', cursive;
    }

    .kitty-notification.show {
        transform: translateX(0);
    }

    .kitty-notification.success {
        border-color: #1DD1A1;
        background: linear-gradient(135deg, #E6F7EF, #D1F0E6);
    }

    .kitty-notification.error {
        border-color: #FF6B6B;
        background: linear-gradient(135deg, #FFE6EB, #FFD1DC);
    }

    .notification-content {
        display: flex;
        align-items: center;
        gap: 10px;
    }

    .notification-emoji {
        font-size: 1.5rem;
    }

    .notification-text {
        font-weight: bold;
        color: #333;
    }

    .mini-task {
        display: flex;
        justify-content: space-between;
        align-items: center;
        padding: 10px 15px;
        margin: 5px 0;
        background: white;
        border-radius: 15px;
        border: 2px solid #FFB6C1;
    }

    .mini-task-title {
        color: #FF1493;
        font-weight: bold;
    }

    .mini-task-status {
        font-size: 0.9rem;
        padding: 3px 10px;
        border-radius: 10px;
    }

    .mini-task-status.todo {
        background: #FFE6EB;
        color: #FF6B6B;
    }

    .mini-task-status.in_progress {
        background: #E6F7F6;
        color: #4ECDC4;
    }

    .mini-task-status.done {
        background: #E6F7EF;
        color: #1DD1A1;
    }
`;
document.head.appendChild(style);

function showSampleTasks() {
    const sampleTasks = [
        {
            title: "Купить клубничный торт 🍰",
            description: "Для чаепития с Hello Kitty",
            category: "shopping",
            priority: 4
        },
        {
            title: "Нарисовать бантик 🎀",
            description: "Новый дизайн для Kitty",
            category: "fun",
            priority: 3
        },
        {
            title: "Сделать зарядку 🏃‍♀️",
            description: "Утренняя разминка",
            category: "home",
            priority: 5
        }
    ];

    sampleTasks.forEach((task, index) => {
        setTimeout(() => {
            showSuccess(`Пример задачи: ${task.title}`);
        }, index * 500);
    });
}

window.loadStats = loadStats;
window.showSampleTasks = showSampleTasks;