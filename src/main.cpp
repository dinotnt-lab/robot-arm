#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <AccelStepper.h>

// =========================
// Wi-Fi
// =========================

const char* WIFI_SSID = "Cassidy-fi";
const char* WIFI_PASSWORD = "OreoKnows";

WebServer server(80);

// =========================
// Motors
// =========================

AccelStepper base(
    AccelStepper::HALF4WIRE,
    4, 18, 5, 19
);

AccelStepper elb1(
    AccelStepper::HALF4WIRE,
    21, 23, 22, 25
);

AccelStepper elb2(
    AccelStepper::HALF4WIRE,
    27, 33, 26, 32
);

AccelStepper* motors[] = {
    &base,
    &elb1,
    &elb2
};

// 4096 half-steps = 360 degrees
const float STEPS_PER_DEGREE = 4096.0 / 360.0;

// =========================
// Dashboard
// =========================

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">

    <title>Robot Arm Control</title>

    <style>
        * {
            box-sizing: border-box;
        }

        body {
            margin: 0;
            background: #101114;
            color: #eeeeee;
            font-family: Arial, sans-serif;
        }

        header {
            padding: 20px;
            background: #181a1f;
            border-bottom: 1px solid #30333a;
        }

        h1 {
            margin: 0;
            font-size: 24px;
        }

        .status {
            margin-top: 6px;
            color: #8f949e;
            font-size: 14px;
        }

        .container {
            max-width: 900px;
            margin: auto;
            padding: 20px;
        }

        .motor {
            background: #181a1f;
            border: 1px solid #30333a;
            border-radius: 12px;
            padding: 20px;
            margin-bottom: 15px;
        }

        .motor-header {
            display: flex;
            justify-content: space-between;
            align-items: center;
        }

        .motor-name {
            font-size: 20px;
            font-weight: bold;
        }

        .angle {
            font-size: 24px;
            font-weight: bold;
        }

        input[type="range"] {
            width: 100%;
            margin: 25px 0;
        }

        .controls {
            display: flex;
            gap: 10px;
        }

        input[type="number"] {
            flex: 1;
            min-width: 0;
            padding: 12px;
            background: #0e0f12;
            color: white;
            border: 1px solid #383b43;
            border-radius: 7px;
            font-size: 16px;
        }

        button {
            padding: 12px 18px;
            background: #292c33;
            color: white;
            border: 1px solid #41454e;
            border-radius: 7px;
            font-size: 16px;
            cursor: pointer;
        }

        button:hover {
            background: #343841;
        }

        .zero {
            width: 100%;
            margin-top: 10px;
        }

        .all-controls {
            display: flex;
            gap: 10px;
            margin-top: 20px;
        }

        .all-controls button {
            flex: 1;
        }
    </style>
</head>

<body>

<header>
    <h1>Robot Arm Control</h1>
    <div class="status" id="status">Connected</div>
</header>

<div class="container">

    <div class="motor">
        <div class="motor-header">
            <div class="motor-name">Base</div>
            <div class="angle" id="angle1">0°</div>
        </div>

        <input
            type="range"
            id="slider1"
            min="-180"
            max="180"
            step="0.1"
            value="0"
            oninput="sliderChanged(1)"
        >

        <div class="controls">
            <input
                type="number"
                id="input1"
                min="-180"
                max="180"
                step="0.1"
                value="0"
            >

            <button onclick="setMotor(1)">Move</button>
        </div>

        <button class="zero" onclick="zeroMotor(1)">
            Set Current Position = 0°
        </button>
    </div>


    <div class="motor">
        <div class="motor-header">
            <div class="motor-name">Elbow 1</div>
            <div class="angle" id="angle2">0°</div>
        </div>

        <input
            type="range"
            id="slider2"
            min="-180"
            max="180"
            step="0.1"
            value="0"
            oninput="sliderChanged(2)"
        >

        <div class="controls">
            <input
                type="number"
                id="input2"
                min="-180"
                max="180"
                step="0.1"
                value="0"
            >

            <button onclick="setMotor(2)">Move</button>
        </div>

        <button class="zero" onclick="zeroMotor(2)">
            Set Current Position = 0°
        </button>
    </div>


    <div class="motor">
        <div class="motor-header">
            <div class="motor-name">Elbow 2</div>
            <div class="angle" id="angle3">0°</div>
        </div>

        <input
            type="range"
            id="slider3"
            min="-180"
            max="180"
            step="0.1"
            value="0"
            oninput="sliderChanged(3)"
        >

        <div class="controls">
            <input
                type="number"
                id="input3"
                min="-180"
                max="180"
                step="0.1"
                value="0"
            >

            <button onclick="setMotor(3)">Move</button>
        </div>

        <button class="zero" onclick="zeroMotor(3)">
            Set Current Position = 0°
        </button>
    </div>


    <div class="all-controls">
        <button onclick="stopAll()">STOP ALL</button>
        <button onclick="homeAll()">HOME ALL</button>
    </div>

</div>


<script>

function updateDisplay(motor, angle) {
    document.getElementById("angle" + motor).innerText =
        Number(angle).toFixed(1) + "°";

    document.getElementById("slider" + motor).value = angle;
    document.getElementById("input" + motor).value = angle;
}


function sliderChanged(motor) {
    let value = document.getElementById("slider" + motor).value;

    document.getElementById("input" + motor).value = value;
    document.getElementById("angle" + motor).innerText =
        Number(value).toFixed(1) + "°";
}


function setMotor(motor) {
    let angle = document.getElementById("input" + motor).value;

    fetch("/move?motor=" + motor + "&angle=" + angle)
        .then(response => response.text())
        .then(data => {
            console.log(data);
            updateDisplay(motor, angle);
        });
}


function zeroMotor(motor) {
    fetch("/zero?motor=" + motor)
        .then(response => response.text())
        .then(data => {
            console.log(data);

            updateDisplay(motor, 0);
        });
}


function stopAll() {
    fetch("/stop")
        .then(response => response.text())
        .then(data => console.log(data));
}


function homeAll() {
    fetch("/home")
        .then(response => response.text())
        .then(data => {
            console.log(data);

            updateDisplay(1, 0);
            updateDisplay(2, 0);
            updateDisplay(3, 0);
        });
}

</script>

</body>
</html>
)rawliteral";


// =========================
// Web handlers
// =========================

void handleRoot() {
    server.send_P(200, "text/html", INDEX_HTML);
}


void handleMove() {

    if (!server.hasArg("motor") || !server.hasArg("angle")) {
        server.send(400, "text/plain", "Missing motor or angle");
        return;
    }

    int motor = server.arg("motor").toInt();
    float angle = server.arg("angle").toFloat();

    if (motor < 1 || motor > 3) {
        server.send(400, "text/plain", "Invalid motor");
        return;
    }

    long steps = (long)(angle * STEPS_PER_DEGREE);

    motors[motor - 1]->moveTo(steps);

    Serial.print("MOTOR ");
    Serial.print(motor);
    Serial.print(" -> ");
    Serial.print(angle);
    Serial.print(" degrees / ");
    Serial.print(steps);
    Serial.println(" steps");

    server.send(200, "text/plain", "OK");
}


void handleZero() {

    if (!server.hasArg("motor")) {
        server.send(400, "text/plain", "Missing motor");
        return;
    }

    int motor = server.arg("motor").toInt();

    if (motor < 1 || motor > 3) {
        server.send(400, "text/plain", "Invalid motor");
        return;
    }

    // Make the motor's CURRENT physical position 0.
    motors[motor - 1]->setCurrentPosition(0);

    Serial.print("ZERO MOTOR ");
    Serial.println(motor);

    server.send(200, "text/plain", "OK");
}


void handleStop() {

    for (int i = 0; i < 3; i++) {
        motors[i]->stop();
    }

    Serial.println("STOP ALL");

    server.send(200, "text/plain", "STOPPED");
}


void handleHome() {

    for (int i = 0; i < 3; i++) {
        motors[i]->moveTo(0);
    }

    Serial.println("HOME ALL");

    server.send(200, "text/plain", "HOMING");
}


// =========================
// Setup
// =========================

void setup() {

    Serial.begin(115200);

    // Motor configuration
    for (int i = 0; i < 3; i++) {

        motors[i]->setMaxSpeed(100);
        motors[i]->setAcceleration(10);

        // Current position becomes 0 degrees
        motors[i]->setCurrentPosition(0);
    }

    Serial.println();
    Serial.println("Connecting to Wi-Fi...");

    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {

        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("Wi-Fi connected");

    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Web routes
    server.on("/", handleRoot);
    server.on("/move", handleMove);
    server.on("/zero", handleZero);
    server.on("/stop", handleStop);
    server.on("/home", handleHome);

    server.begin();

    Serial.println("Web server started");
    Serial.println("READY");
}


// =========================
// Main loop
// =========================

void loop() {

    // Keep all motors moving
    for (int i = 0; i < 3; i++) {
        motors[i]->run();
    }

    // Handle dashboard requests
    server.handleClient();
}