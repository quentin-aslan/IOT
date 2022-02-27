#include <WiFi.h>
#include <HTTPClient.h>

# Ne pas oublier les id wifi
const char* ssid = "";
const char* password = "";

// ENDPOINTS
const char* serverName = "http://192.168.1.50:31830/dht22";

// DHT22
#include "DHT.h"
#define DHTPIN 25 // PIN OU EST CONNECTER LES DATAS
#define DHTTYPE DHT22
#define DHT22_LOCATION "SALON" // 1 : SALON, 2 : CHAMBRE (BUREAU)
DHT dht(DHTPIN, DHTTYPE);

// Set timer to 30secondes
unsigned long timerDelay = 30000;

void setup() {
    Serial.begin(115200);

    // CONNECTION AU REASEAU WIFI
    Serial.println(ssid);
    Serial.println(password);
    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while (WiFi.status() != WL_CONNECTED) { // ON NE CONTINUE PAS TANT QUE L'ESP EST PAS CONNECTÉ A LA WIFI
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());

    // Lancement du DHT22
    dht.begin();
}

void loop() {
    //Check WiFi connection status
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("On est connecté à la wifi");

        // ON RECUPERE LES VALEURS DU CAPTEURS
        float humidityFloat = dht.readHumidity();
        float temperatureFloat = dht.readTemperature();

        // On vérifie qu'elles ne sont pas null
        if (isnan(humidityFloat) || isnan(temperatureFloat)) {
            Serial.println(F("Failed to read from DHT sensor!"));
            delay(timerDelay);
            return;
        }

        // Calcule du ressentie
        float realFeelFloat = dht.computeHeatIndex(temperatureFloat, humidityFloat, false);

        Serial.println("Temp : ");
        Serial.print(temperatureFloat);

        Serial.println("Humidity : ");
        Serial.print(humidityFloat);

        Serial.println("realFeel : ");
        Serial.print(realFeelFloat);

        // ON ENVOIE LES DATAS AU SERVEUR
        HTTPClient http;

        // ALL
        char datas[150];
        sprintf(datas, "{\"humidity\":\"%f\",\"temperature\":\"%f\",\"realFeel\":\"%f\",\"location\":\"%s\",\"location\":\"%s\"}", humidityFloat, temperatureFloat, realFeelFloat, DHT22_LOCATION);

        http.begin(serverName);
        http.addHeader("Content-Type", "application/json");
        int datasStatus = http.POST(datas);
        Serial.print("HTTP Response code : ");
        Serial.println(datasStatus);
        http.end();

    } else {
        Serial.println("WiFi Disconnected");
    }
    delay(timerDelay);
}
