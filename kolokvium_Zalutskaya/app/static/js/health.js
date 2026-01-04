let autoRefreshInterval = setInterval(() => {
    if (!document.querySelector('.refresh-button').disabled) {
        refreshHealth();
    }
}, 10000);

async function refreshHealth() {
    try {
        const button = document.querySelector('.refresh-button');
        const loading = document.getElementById('refresh-loading');

        if (!button || button.disabled) return;

        button.disabled = true;
        if (loading) loading.style.display = 'inline-block';

        button.style.opacity = '0.8';
        button.style.transform = 'scale(0.95)';

        const response = await fetch('/health?refresh=true');

        if (response.ok) {
            if (response.headers.get('content-type')?.includes('application/json')) {
                const data = await response.json();
                updateHealthDisplay(data);
            } else {
                location.reload();
            }
        } else {
            showHealthError('Ошибка при обновлении статуса');
        }

    } catch (error) {
        console.error('Ошибка обновления:', error);
        showHealthError('Не удалось обновить статус');
    } finally {
        setTimeout(() => {
            const button = document.querySelector('.refresh-button');
            const loading = document.getElementById('refresh-loading');

            if (button) {
                button.disabled = false;
                button.style.opacity = '1';
                button.style.transform = 'scale(1)';
            }

            if (loading) loading.style.display = 'none';
        }, 1000);
    }
}

function updateHealthDisplay(data) {
    const systemStatus = document.querySelector('.system-status');
    if (systemStatus && data.status) {
        systemStatus.className = `system-status ${getStatusClass(data.status)}`;
        systemStatus.querySelector('h2').innerHTML = `
            <span class="status-indicator-large ${getStatusClass(data.status)}"></span>
            Статус системы: ${data.status}
        `;

        if (data.kitty_message) {
            const messageElement = systemStatus.querySelector('p');
            if (messageElement) {
                messageElement.textContent = data.kitty_message;
            }
        }
    }

    updateServiceCard('redis', data.services?.redis);
    updateServiceCard('database', data.services?.database);

    const timestampElement = document.querySelector('.check-time');
    if (timestampElement) {
        const now = new Date();
        timestampElement.textContent = now.toLocaleTimeString('ru-RU');
    }

    showHealthSuccess('Статус обновлен!');
}

function updateServiceCard(serviceName, serviceData) {
    if (!serviceData) return;

    const card = document.querySelector(`.health-card .overview-card:contains("${getServiceTitle(serviceName)}")`);
    if (!card) return;

    const statusClass = getStatusClass(serviceData.status);
    card.className = `health-card ${statusClass}`;

    const emojiElement = card.querySelector('.status-emoji, .overview-icon');
    if (emojiElement && serviceData.emoji) {
        emojiElement.textContent = serviceData.emoji;
    }

    const statusElement = card.querySelector('.status-indicator');
    if (statusElement) {
        statusElement.className = `status-indicator ${statusClass}`;
    }

    const statusText = card.querySelector('p:first-of-type');
    if (statusText && serviceData.status) {
        statusText.innerHTML = `
            <span class="status-indicator ${statusClass}"></span>
            ${serviceData.status.toUpperCase()}
        `;
    }

    const messageElement = card.querySelector('p:nth-of-type(2)');
    if (messageElement && serviceData.message) {
        messageElement.textContent = serviceData.message;
    }

    card.style.animation = 'none';
    setTimeout(() => {
        card.style.animation = 'pulse 0.5s ease-in-out';
    }, 10);
}

function getStatusClass(status) {
    if (!status) return 'unknown';

    if (status.includes('healthy') || status.includes('connected')) {
        return 'healthy';
    } else if (status.includes('warning') || status.includes('degraded')) {
        return 'warning';
    } else if (status.includes('error') || status.includes('disconnected')) {
        return 'error';
    }
    return 'unknown';
}

function getServiceTitle(serviceName) {
    const titles = {
        'redis': 'Redis',
        'database': 'База данных',
        'api': 'API Сервер',
        'network': 'Сеть'
    };
    return titles[serviceName] || serviceName;
}

function showHealthSuccess(message) {
    showNotification(message, 'success', '🎀');
}

function showHealthError(message) {
    showNotification(message, 'error', '😿');
}

function showNotification(message, type, emoji = 'ℹ️') {
    const existingNotification = document.querySelector('.health-notification');
    if (existingNotification) {
        existingNotification.remove();
    }

    const notification = document.createElement('div');
    notification.className = `health-notification ${type}`;
    notification.innerHTML = `
        <div class="notification-content">
            <span class="notification-emoji">${emoji}</span>
            <span class="notification-text">${message}</span>
            <button class="notification-close" onclick="this.parentElement.parentElement.remove()">
                <i class="fas fa-times"></i>
            </button>
        </div>
    `;

    document.body.appendChild(notification);

    setTimeout(() => notification.classList.add('show'), 10);

    setTimeout(() => {
        notification.classList.remove('show');
        setTimeout(() => notification.remove(), 300);
    }, 3000);
}

const notificationStyle = document.createElement('style');
notificationStyle.textContent = `
    .health-notification {
        position: fixed;
        top: 20px;
        right: 20px;
        background: white;
        padding: 20px 25px;
        border-radius: 20px;
        box-shadow: 0 10px 30px rgba(255, 105, 180, 0.3);
        border: 3px solid;
        transform: translateX(120%);
        transition: transform 0.3s ease-out;
        z-index: 10000;
        font-family: 'Comic Sans MS', cursive;
        max-width: 400px;
        min-width: 300px;
    }

    .health-notification.show {
        transform: translateX(0);
    }

    .health-notification.success {
        border-color: #1DD1A1;
        background: linear-gradient(135deg, #E6F7EF, #D1F0E6);
    }

    .health-notification.error {
        border-color: #FF6B6B;
        background: linear-gradient(135deg, #FFE6EB, #FFD1DC);
    }

    .health-notification.warning {
        border-color: #FF9F43;
        background: linear-gradient(135deg, #FFF5E6, #FFEED1);
    }

    .notification-content {
        display: flex;
        align-items: center;
        gap: 15px;
    }

    .notification-emoji {
        font-size: 1.8rem;
        flex-shrink: 0;
    }

    .notification-text {
        font-weight: bold;
        color: #333;
        flex: 1;
        font-size: 1.1rem;
    }

    .notification-close {
        background: none;
        border: none;
        color: #999;
        cursor: pointer;
        font-size: 1.2rem;
        padding: 5px;
        border-radius: 50%;
        width: 30px;
        height: 30px;
        display: flex;
        align-items: center;
        justify-content: center;
        transition: all 0.2s;
    }

    .notification-close:hover {
        background: rgba(0, 0, 0, 0.1);
        color: #FF1493;
    }
`;
document.head.appendChild(notificationStyle);

document.addEventListener('DOMContentLoaded', function() {
    const cards = document.querySelectorAll('.health-card, .overview-card');
    cards.forEach((card, index) => {
        card.style.opacity = '0';
        card.style.transform = 'translateY(20px)';

        setTimeout(() => {
            card.style.transition = 'opacity 0.5s ease, transform 0.5s ease';
            card.style.opacity = '1';
            card.style.transform = 'translateY(0)';
        }, index * 100);
    });

    const systemStatus = document.querySelector('.system-status');
    if (systemStatus && systemStatus.classList.contains('healthy')) {
        systemStatus.style.animation = 'pulse 2s infinite';
    }
});

window.refreshHealth = refreshHealth;
window.showHealthSuccess = showHealthSuccess;
window.showHealthError = showHealthError;