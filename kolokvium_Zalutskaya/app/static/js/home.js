async function loadStats() {
    try {
        const response = await fetch('/tasks');
        const data = await response.json();

        if (data.tasks) {
            document.getElementById('total-tasks').textContent = data.total || data.tasks.length;

            const done = data.tasks.filter(task => task.status === 'done').length;
            document.getElementById('done-tasks').textContent = done;
        }
    } catch (error) {
        console.log('Не удалось загрузить статистику:', error);
    }
}

function changeTheme() {
    alert('🎀 Функция смены темы в разработке!');
}

const style = document.createElement('style');
style.textContent = `
    .text-center {
        text-align: center;
        margin: 40px 0;
    }
`;
document.head.appendChild(style);

document.addEventListener('DOMContentLoaded', loadStats);
setInterval(loadStats, 30000);