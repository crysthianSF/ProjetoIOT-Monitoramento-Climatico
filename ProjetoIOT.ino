#include <WiFi.h>
#include <WebServer.h>
#include "DHT.h"

#define PINO_DHT 4 
#define TIPO_DHT DHT11
DHT dht(PINO_DHT, TIPO_DHT);

#define SENSOR_AGUA_PIN 32

#define SENSOR_SOLO_PIN 34

const char* wifi_ssid = "iPhone crysthian";
const char* wifi_senha = "qwe12345";

WebServer servidor(80);

String classificarUmidade(int valor) {
  if (valor > 2500) return "Seco";
  if (valor > 1200) return "Levemente úmido";
  return "Molhado";
}

void paginaPrincipal() {

  String html =
    "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
    "<title>Dashboard ESP32</title>"

    "<style>"
    "body { font-family: Arial; background:#f3f4f6; margin:0; padding:0; }"
    "h1 { text-align:center; padding:20px; background:#2563eb; color:white; margin:0; "
    "font-size:26px; font-weight:600; }"

    ".container { max-width:900px; margin:30px auto; display:grid; "
    "grid-template-columns: repeat(auto-fit, minmax(250px, 1fr)); gap:20px; padding:20px; }"

    ".card { background:white; padding:20px; border-radius:14px; "
    "box-shadow:0 4px 10px rgba(0,0,0,0.12); transition:0.3s; }"

    ".card:hover { transform:scale(1.02); }"

    ".titulo { font-size:18px; font-weight:bold; color:#555; }"
    ".valor { font-size:34px; margin-top:12px; color:#111; font-weight:600; }"

    ".atualizando { text-align:center; padding:10px; color:#666; font-size:14px; }"
    "</style>"

    "<script>"
    "function atualizarDados() {"
      "fetch('/dados').then(r => r.json()).then(d => {"
        "document.getElementById('temp').innerHTML = d.temperatura + ' °C';"
        "document.getElementById('umid').innerHTML = d.umidade + ' %';"
        "document.getElementById('agua').innerHTML = d.nivel_agua;"
        "document.getElementById('solo').innerHTML = d.umidade_solo;"
        "document.getElementById('solo_status').innerHTML = d.status_solo;"
      "});"
    "}"
    "setInterval(atualizarDados, 2000);"  
    "window.onload = atualizarDados;"
    "</script>"

    "</head><body>"

    "<h1>Monitoramento ESP32</h1>"

    "<div class='container'>"

      "<div class='card'>"
        "<div class='titulo'>🌡️ Temperatura</div>"
        "<div class='valor' id='temp'>--</div>"
      "</div>"

      "<div class='card'>"
        "<div class='titulo'>💧 Umidade do Ar</div>"
        "<div class='valor' id='umid'>--</div>"
      "</div>"

      "<div class='card'>"
        "<div class='titulo'>🌱 Umidade do Solo</div>"
        "<div class='valor' id='solo'>--</div>"
        "<div class='titulo' style='margin-top:10px;'>Estado:</div>"
        "<div class='valor' style='font-size:22px;' id='solo_status'>--</div>"
      "</div>"

      "<div class='card'>"
        "<div class='titulo'>🚰 Nível de Água</div>"
        "<div class='valor' id='agua'>--</div>"
      "</div>"

    "</div>"

    "<div class='atualizando'>Atualizando automaticamente a cada 2 segundos...</div>"

    "</body></html>";

  servidor.send(200, "text/html", html);
}

void paginaDados() {
  float temperatura = dht.readTemperature();
  float umidade = dht.readHumidity();
  int nivelAgua = analogRead(SENSOR_AGUA_PIN);
  int umidadeSolo = analogRead(SENSOR_SOLO_PIN);

  String statusSolo = classificarUmidade(umidadeSolo);

  String json = "{";
  json += "\"temperatura\": " + String(temperatura) + ",";
  json += "\"umidade\": " + String(umidade) + ",";
  json += "\"umidade_solo\": " + String(umidadeSolo) + ",";
  json += "\"status_solo\": \"" + statusSolo + "\",";
  json += "\"nivel_agua\": " + String(nivelAgua);
  json += "}";

  servidor.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Iniciando sensor");
  dht.begin();
  delay(800);

  Serial.println("Conectando ao WiFi");
  WiFi.begin(wifi_ssid, wifi_senha);

  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado");
  Serial.print("IP do ESP32: ");
  Serial.println(WiFi.localIP());

  servidor.on("/", paginaPrincipal);
  servidor.on("/dados", paginaDados);

  servidor.begin();
  Serial.println("Servidor web iniciado");
}

void loop() {
  servidor.handleClient();
}
