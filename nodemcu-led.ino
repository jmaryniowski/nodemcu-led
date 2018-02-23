#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "secret.h"

#define LED1 12
#define LED2 13
#define LED3 15

ESP8266WebServer server(80);
IPAddress localIP;

void setup() {
  Serial.begin(115200);
  setupLEDs();
  startWiFi();
  startServer();
}

void loop() {
  server.handleClient();
}

void setupLEDs() {
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  digitalWrite(LED1, 0);
  digitalWrite(LED2, 0);
  digitalWrite(LED3, 0);
}

void startWiFi() {
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  localIP = WiFi.localIP();

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(localIP);
}

void startServer() {
  server.on("/", HTTP_GET, handleMainPage);
  server.on("/", HTTP_POST, handlePost);
  server.begin();
  Serial.println("Server started");
}

void handleMainPage() {
  server.send(200, "text/html", getHTMLresponse());
}

String getButtonClass(int pin) {
  switch (pin) {
    case LED1: return digitalRead(pin) ? "btn-danger" : "btn-outline-danger";
    case LED2: return digitalRead(pin) ? "btn-warning" : "btn-outline-warning";
    case LED3: return digitalRead(pin) ? "btn-success" : "btn-outline-success";
    default: return "";
  }
}

String getHTMLresponse() {
  String s = "<html><head><title>LED status page</title>"
             "<script src=\"http://code.jquery.com/jquery-3.3.1.min.js\"></script>"
             "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">"
             "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0/css/bootstrap.min.css\" integrity=\"sha384-Gn5384xqQ1aoWXA+058RXPxPg6fy4IWvTNh0E263XmFcJlSAwiGgFAW/dAiS6JXm\" crossorigin=\"anonymous\"></head>"
             "<body>"
             "<a href=\"#\" class=\"btn btn-lg btn-block " + getButtonClass(LED1) + "\" data-pin=\""+ LED1 +"\">LED 1</a>"
             "<a href=\"#\" class=\"btn btn-lg btn-block " + getButtonClass(LED2) + "\" data-pin=\""+ LED2 +"\">LED 2</a>"
             "<a href=\"#\" class=\"btn btn-lg btn-block " + getButtonClass(LED3) + "\" data-pin=\""+ LED3 +"\">LED 3</a>"
             "<script type=\"text/javascript\">"
             "$(\"a\").click(function(e) {"
             "var $target = $(e.target);"
             "$.post(\"http://" + localIP.toString() + "\", {led: $target.attr(\"data-pin\")}, function (data) {$target.removeClass(\"btn-danger btn-warning btn-success btn-outline-danger btn-outline-warning btn-outline-success\"); $target.toggleClass(data);});"
             "e.preventDefault();"
             "});"
             "</script>"
             "</body></html>";
  return s;
}

void toggleLed(int pin) {
  Serial.print("Toggling LED on pin ");
  Serial.println(pin);
  digitalWrite(pin, !digitalRead(pin));
}

void handlePost() {
  if (server.hasArg("led")) {
    int ledPin = server.arg("led").toInt();
    toggleLed(ledPin);
    server.send(200, "text/plain", getButtonClass(ledPin));
  }
}

