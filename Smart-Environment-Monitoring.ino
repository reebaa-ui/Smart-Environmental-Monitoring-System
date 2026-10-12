#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// =====================================================
// PIN DEFINITIONS
// =====================================================

#define DHTPIN 4          // DHT11 data pin
#define DHTTYPE DHT11     // Sensor type
#define MQ9_PIN 34        // MQ-9 analog output connected to GPIO 34


// =====================================================
// WIFI CREDENTIALS
// =====================================================

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";


// =====================================================
// OBJECTS
// =====================================================

DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);


// =====================================================
// GLOBAL VARIABLES
// =====================================================

float temperature = 0.0;
float humidity = 0.0;
int gasValue = 0;

unsigned long previousMillis = 0;

const unsigned long sensorInterval = 2000;   // 2 seconds


// =====================================================
// WEB DASHBOARD HTML
// =====================================================

const char MAIN_page[] PROGMEM = R"rawliteral(

<!DOCTYPE html>

<html>

<head>

<meta name="viewport" content="width=device-width, initial-scale=1">

<title>Smart Environmental Monitoring</title>

<style>

body {
    font-family: Arial, sans-serif;
    background: #f2f2f2;
    margin: 0;
    padding: 20px;
    text-align: center;
}

h1 {
    margin-bottom: 5px;
}

.subtitle {
    color: #666;
    margin-bottom: 25px;
}

.container {
    display: flex;
    flex-wrap: wrap;
    justify-content: center;
    gap: 20px;
}

.card {
    background: white;
    width: 250px;
    padding: 25px;
    border-radius: 15px;
    box-shadow: 0 4px 10px rgba(0,0,0,0.15);
}

.card h2 {
    margin-top: 0;
}

.value {
    font-size: 40px;
    font-weight: bold;
    margin: 20px 0;
}

.unit {
    color: #555;
}

.status {
    margin-top: 25px;
    font-weight: bold;
}

.footer {
    margin-top: 30px;
    color: #777;
    font-size: 14px;
}

</style>

</head>


<body>

<h1>Smart Environmental Monitoring System</h1>

<div class="subtitle">
ESP32 Based Real-Time Monitoring
</div>


<div class="container">


<div class="card">

<h2>Temperature</h2>

<div class="value" id="temperature">
-- 
</div>

<div class="unit">
°C
</div>

</div>


<div class="card">

<h2>Humidity</h2>

<div class="value" id="humidity">
--
</div>

<div class="unit">
%
</div>

</div>


<div class="card">

<h2>Air Quality</h2>

<div class="value" id="gas">
--
</div>

<div class="unit">
MQ-9 Analog Value
</div>

</div>


</div>


<div class="status">

System Status:
<span id="status">Connecting...</span>

</div>


<div class="footer">

Data updates automatically every 2 seconds

</div>


<script>

function updateData() {

    fetch('/data')

    .then(response => response.json())

    .then(data => {

        document.getElementById("temperature").innerHTML =
            data.temperature.toFixed(1);

        document.getElementById("humidity").innerHTML =
            data.humidity.toFixed(1);

        document.getElementById("gas").innerHTML =
            data.aqi;

        document.getElementById("status").innerHTML =
            "Connected";

    })

    .catch(error => {

        document.getElementById("status").innerHTML =
            "Connection Lost";

    });

}


// Update sensor information every 2 seconds

setInterval(updateData, 2000);


// Get data immediately when page loads

updateData();

</script>


</body>

</html>

)rawliteral";


// =====================================================
// READ SENSOR DATA
// =====================================================

void readSensors()
{

    // Read temperature

    float newTemperature = dht.readTemperature();


    // Read humidity

    float newHumidity = dht.readHumidity();


    // Read MQ-9 analog value

    int newGasValue = analogRead(MQ9_PIN);


    // -------------------------------------------------
    // Validate DHT11 readings
    // -------------------------------------------------

    if (!isnan(newTemperature) && !isnan(newHumidity))
    {

        temperature = newTemperature;

        humidity = newHumidity;

    }

    else
    {

        Serial.println("Error: Failed to read DHT11 sensor!");

    }


    // -------------------------------------------------
    // Update MQ-9 value
    // -------------------------------------------------

    gasValue = newGasValue;


    // -------------------------------------------------
    // Display sensor values on Serial Monitor
    // -------------------------------------------------

    Serial.println("--------------------------------");

    Serial.print("Temperature: ");

    Serial.print(temperature);

    Serial.println(" °C");


    Serial.print("Humidity: ");

    Serial.print(humidity);

    Serial.println(" %");


    Serial.print("MQ-9 Gas Sensor Value: ");

    Serial.println(gasValue);

    Serial.println("--------------------------------");

}


// =====================================================
// HANDLE HOME PAGE REQUEST
// =====================================================

void handleRoot()
{

    server.send_P(
        200,
        "text/html",
        MAIN_page
    );

}


// =====================================================
// HANDLE SENSOR DATA REQUEST
// =====================================================

void handleData()
{

    String json = "{";

    json += "\"temperature\":";
    json += String(temperature, 1);

    json += ",";

    json += "\"humidity\":";
    json += String(humidity, 1);

    json += ",";

    json += "\"aqi\":";
    json += String(gasValue);

    json += "}";


    server.send(
        200,
        "application/json",
        json
    );

}


// =====================================================
// CONNECT TO WIFI
// =====================================================

void connectToWiFi()
{

    Serial.println();

    Serial.println("Connecting to Wi-Fi...");


    WiFi.mode(WIFI_STA);

    WiFi.begin(
        ssid,
        password
    );


    // Keep attempting until connection is established

    while (WiFi.status() != WL_CONNECTED)
    {

        delay(500);

        Serial.print(".");

    }


    Serial.println();

    Serial.println("Wi-Fi connected!");

    Serial.print("IP Address: ");

    Serial.println(WiFi.localIP());

}


// =====================================================
// AUTOMATIC WIFI RECONNECTION
// =====================================================

void checkWiFiConnection()
{

    if (WiFi.status() != WL_CONNECTED)
    {

        Serial.println("Wi-Fi connection lost!");

        Serial.println("Attempting reconnection...");


        WiFi.disconnect();

        WiFi.begin(
            ssid,
            password
        );


        unsigned long startAttemptTime = millis();


        // Try reconnecting for up to 10 seconds

        while (
            WiFi.status() != WL_CONNECTED &&
            millis() - startAttemptTime < 10000
        )
        {

            delay(500);

            Serial.print(".");

        }


        Serial.println();


        if (WiFi.status() == WL_CONNECTED)
        {

            Serial.println("Wi-Fi reconnected!");

            Serial.print("IP Address: ");

            Serial.println(WiFi.localIP());

        }

        else
        {

            Serial.println(
                "Wi-Fi reconnection failed."
            );

        }

    }

}


// =====================================================
// SETUP
// =====================================================

void setup()
{

    // -------------------------------------------------
    // Initialize Serial Communication
    // -------------------------------------------------

    Serial.begin(115200);

    delay(1000);


    Serial.println();

    Serial.println(
        "======================================"
    );

    Serial.println(
        " Smart Environmental Monitoring System"
    );

    Serial.println(
        "======================================"
    );


    // -------------------------------------------------
    // Initialize DHT11
    // -------------------------------------------------

    dht.begin();

    Serial.println("DHT11 initialized.");


    // -------------------------------------------------
    // Configure MQ-9
    // -------------------------------------------------

    pinMode(
        MQ9_PIN,
        INPUT
    );

    Serial.println(
        "MQ-9 sensor initialized on GPIO 34."
    );


    // -------------------------------------------------
    // Connect to Wi-Fi
    // -------------------------------------------------

    connectToWiFi();


    // -------------------------------------------------
    // Configure HTTP Web Server
    // -------------------------------------------------

    server.on(
        "/",
        handleRoot
    );


    server.on(
        "/data",
        handleData
    );


    // -------------------------------------------------
    // Start Web Server
    // -------------------------------------------------

    server.begin();


    Serial.println(
        "HTTP web server started."
    );


    Serial.print(
        "Open this address in your browser: http://"
    );

    Serial.println(
        WiFi.localIP()
    );


    // -------------------------------------------------
    // Initialize timer
    // -------------------------------------------------

    previousMillis = millis();

}


// =====================================================
// MAIN LOOP
// =====================================================

void loop()
{

    // -------------------------------------------------
    // Handle incoming HTTP requests
    // -------------------------------------------------

    server.handleClient();


    // -------------------------------------------------
    // Monitor Wi-Fi connection
    // -------------------------------------------------

    checkWiFiConnection();


    // -------------------------------------------------
    // Read sensors every 2 seconds
    // -------------------------------------------------

    unsigned long currentMillis = millis();


    if (
        currentMillis - previousMillis >=
        sensorInterval
    )
    {

        previousMillis = currentMillis;


        // Read temperature, humidity and gas value

        readSensors();

    }

}
