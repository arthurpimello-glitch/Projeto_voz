#define ENABLE_DATABASE

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <FirebaseClient.h>


// ======================================================
// WIFI
// ======================================================

#define WIFI_SSID "Balaco"
#define WIFI_PASSWORD "04052618"


// ======================================================
// FIREBASE
// ======================================================

#define DATABASE_URL "https://projeto-voz-esp32-default-rtdb.firebaseio.com"


// ======================================================
// PINO DA SAÍDA
// ======================================================

#define SAIDA 2


// ======================================================
// FUNÇÕES
// ======================================================

void processarFirebase(AsyncResult &aResult);


// ======================================================
// FIREBASE
// ======================================================

// Cliente SSL
WiFiClientSecure ssl_client;
WiFiClientSecure stream_ssl_client;


// Clientes assíncronos
using AsyncClient = AsyncClientClass;

AsyncClient aClient(ssl_client);
AsyncClient streamClient(stream_ssl_client);


// Autenticação sem usuário
NoAuth no_auth;


// Aplicação Firebase
FirebaseApp app;


// Realtime Database
RealtimeDatabase Database;


// ======================================================
// SETUP
// ======================================================

void setup()
{
    Serial.begin(115200);


    // ------------------------------------------
    // CONFIGURA SAÍDA
    // ------------------------------------------

    pinMode(SAIDA, OUTPUT);

    // Começa desligada
    digitalWrite(SAIDA, LOW);


    // ------------------------------------------
    // CONECTA AO WIFI
    // ------------------------------------------

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

    Serial.print("Conectando ao Wi-Fi");

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }

    Serial.println();
    Serial.println("Wi-Fi conectado!");

    Serial.print("IP da ESP32: ");
    Serial.println(WiFi.localIP());


    // ------------------------------------------
    // CONFIGURA SSL
    // ------------------------------------------

    ssl_client.setInsecure();
    stream_ssl_client.setInsecure();


    // ------------------------------------------
    // INICIALIZA FIREBASE
    // ------------------------------------------

    Serial.println("Inicializando Firebase...");

    initializeApp(
        aClient,
        app,
        getAuth(no_auth)
    );


    // ------------------------------------------
    // CONECTA AO REALTIME DATABASE
    // ------------------------------------------

    app.getApp<RealtimeDatabase>(Database);

    Database.url(DATABASE_URL);


    // ------------------------------------------
    // COMEÇA A ESCUTAR /dispositivos/esp32/rele
    // ------------------------------------------

    streamClient.setSSEFilters(
        "get,put,patch,keep-alive,cancel,auth_revoked"
    );

    Database.get(
        streamClient,
        "/dispositivos/esp32/rele",
        processarFirebase,
        true,
        "streamComando"
    );


    Serial.println();
    Serial.println("================================");
    Serial.println("ESP32 pronta!");
    Serial.println("Aguardando comandos...");
    Serial.println("================================");
}


// ======================================================
// LOOP
// ======================================================

void loop()
{
    // Mantém Firebase funcionando
    app.loop();
}


// ======================================================
// PROCESSAR DADOS DO FIREBASE
// ======================================================

void processarFirebase(AsyncResult &aResult)
{
    // Não há resultado disponível
    if (!aResult.available())
    {
        return;
    }


    // Verifica se é um resultado do Realtime Database
    RealtimeDatabaseResult &stream =
        aResult.to<RealtimeDatabaseResult>();


    // Verifica se é realmente um stream
    if (stream.isStream())
    {
        Serial.println("--------------------------------");

        Serial.print("Evento: ");
        Serial.println(stream.event());

        Serial.print("Caminho: ");
        Serial.println(stream.dataPath());

        Serial.print("Valor recebido: ");
        Serial.println(stream.to<String>());


        // ------------------------------------------
        // CONVERTE O VALOR PARA BOOLEAN
        // ------------------------------------------

        bool comando = stream.to<bool>();


        // ------------------------------------------
        // LIGA
        // ------------------------------------------

        if (comando == true)
        {
            digitalWrite(SAIDA, HIGH);

            Serial.println(">>> SAIDA LIGADA");
        }


        // ------------------------------------------
        // DESLIGA
        // ------------------------------------------

        else
        {
            digitalWrite(SAIDA, LOW);

            Serial.println(">>> SAIDA DESLIGADA");
        }

        Serial.println("--------------------------------");
    }
}