var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

window.addEventListener('load', function () {
    initWebSocket();
    $('.b_e').each(function (i) {
        $(this).on('click', function (e) {
            console.log('mama', i);
        });
    });

    $('.d_t_v').each(function (i) {
        let $el = $(this);
        $el.text(Math.random() * 10000000);
        $el.addClass('d' + i);
    });
});

const formElement = document.getElementById('form1'); // извлекаем элемент формы
formElement.addEventListener('submit', (e) => {
    e.preventDefault();
    const formData = new FormData(formElement); // создаём объект FormData, передаём в него элемент формы
    // теперь можно извлечь данные
    const val = String(formData.get('name'));
    // const siz = String(val.length);
    var dataToSend = val + ';!';
    websocket.send(dataToSend);
    console.log(dataToSend);
    formElement.reset();

});

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    console.log("recieved:");
    console.log(event.data);
    var dataArray = String(event.data).split(';');
    dataArray.forEach(function (item) {
        var key = '';
        switch (item[0]) {
            case 'T':
                key = 'tempID';
                break;
            case 'P':
                key = 'presID';
                break;
            case 'H':
                key = 'humID';
                break;
            default:
                break;
        }
        document.getElementById(key).innerHTML = item.substring(1, item.length);
        var hash = '#' + key;
        $(hash).text(item.substring(1, item.length));
    });


}






