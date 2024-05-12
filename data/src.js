var gateway = `ws://${window.location.hostname}/ws`;
var websocket;

// Init web socket when the page loads
window.addEventListener('load', function () {
    initWebSocket();
    $('.b_e').each(function (i) {
        $(this).on('click', function (e) {
            console.log('mama', i);
            

            websocket.send("getReadings"+toString(i));
        });
    });

    $('.d_t_v').each(function (i) {
        let $el = $(this);
        $el.text(Math.random() * 10000000);
        $el.addClass('d' + i);
    });
});


function onload(event) {
    initWebSocket();
}

function getReadings() {
    websocket.send("getReadings");
}

function initWebSocket() {
    console.log('Trying to open a WebSocket connection…');
    websocket = new WebSocket(gateway);
    websocket.onopen = onOpen;
    websocket.onclose = onClose;
    websocket.onmessage = onMessage;
}

// When websocket is established, call the getReadings() function
function onOpen(event) {
    console.log('Connection opened');
    getReadings();
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

// Function that receives the message from the ESP32 with the readings
function onMessage(event) {
    console.log("recieved:");
    console.log(event.data);
    var keys = 'temperature';
    document.getElementById(keys).innerHTML = event.data;
}






