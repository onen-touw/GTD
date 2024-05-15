var gateway = `ws://${window.location.hostname}/ws`;
var websocket;
var serverStatus = [0, $()];
var devMod = true;


window.addEventListener('load', function () {
    initWebSocket();
    $('.b_e').each(function (i) {
        $(this).on('click', function (e) {
            console.log('mama', i);
        });
        $(this).addClass(('d' + i));
    });

    $('.d_t_v').each(function (i) {
        $(this).text(Math.random() * 10000000);
    });
    serverStatus[1] = $("#sttus");
    serverStatus[0] = 0;
    serverStatus[1].addClass("c_R");
});

const formElement = document.getElementById('form1'); // извлекаем элемент формы
formElement.addEventListener('submit', (e) => {
    e.preventDefault();
    const formData = new FormData(formElement); // создаём объект FormData, передаём в него элемент формы
    const val = String(formData.get('name'));
    var dataToSend = val + ';!';
    websocket.send(dataToSend);
    console.log(dataToSend);
    formElement.reset();

});

function initWebSocket() {
    if (!devMod) {

        console.log('Trying to open a WebSocket connection…');
        websocket = new WebSocket(gateway);
        websocket.onopen = onOpen;
        websocket.onclose = onClose;
        websocket.onmessage = onMessage;
    }
}

function onOpen(event) {
    console.log('Connection opened');
    serverStatus[0] = 1;
    serverStatus[1].addClass("c_G");
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
    serverStatus[0] = 0;
    serverStatus[1].addClass("c_R");
}

function onMessage(event) {
    console.log("recieved:");
    console.log(event.data);
    var dataArray = String(event.data).split(';');
    console.log(dataArray);
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
            case 'V':
                key = 'volID';
                break;
            case 'I':
                key = 'curID';
                break;
            case 'v':
                key = 'sunvID';
                break;
            case 'W':
                key = 'pwrID';
                break;

            default:
                break;
        }
        var hash = '#' + key;
        if (hash.length > 1) {
            $(hash).text(item.substring(1, item.length));
        }
    });


}






