const username = 'admin';
const password = 'esp8266';
const headers = {
    'Authorization': 'Basic ' + btoa(username + ":" + password)
};
const uptimeContainer = document.querySelector('.uptime');
const stateContainer = document.querySelector('.state');
const buttons = document.querySelectorAll('.toggle-state');

document.addEventListener('DOMContentLoaded', function() {
    (async () => {
        try {
            const {state} = await getData('/gpio');
            updateState(state);
        } catch (e) {
            console.error(e);
        }
    })();

    setInterval(async () => {
        try {
            const {uptime: {hours, minutes, seconds}} = await getData('/health');
            const hh = `${hours < 10 ? '0' + hours : hours}`;
            const mm = `${minutes < 10 ? '0' + minutes : minutes}`;
            const ss = `${seconds < 10 ? '0' + seconds : seconds}`;

            uptimeContainer.innerHTML = `${hh}:${mm}:${ss}`;
        } catch (e) {
            console.error(e);
        }
    }, 5000);

    buttons.forEach(button => {
        button.addEventListener('click', async function() {
            const value = Number(button.dataset.state);
            try {
                const {state} = await sendData('/gpio', {value});
                updateState(state);
            } catch (e) {
                console.error(e);
            }
        });
    })
});

function updateState(state) {
    const className = state ? 'green' : 'red';
    stateContainer.innerHTML = state;
    stateContainer.classList.remove('green');
    stateContainer.classList.remove('red');
    stateContainer.classList.add(className);
}

function getData(url) {
    return fetch(url, {headers}).then(response => response.json());
}

function sendData(url, data = {}) {
    const searchParams = new URLSearchParams();

    for (const prop in data) {
        searchParams.set(prop, data[prop]);
    }

    return fetch(url, {
        method: 'POST',
        headers: {
            ...headers,
            'Content-Type': 'application/x-www-form-urlencoded',
        },
        body: searchParams,
      })
      .then(response => response.json());
}