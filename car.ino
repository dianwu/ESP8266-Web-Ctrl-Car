#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

/* Set these to your desired credentials. */
const char *ssid = "ESP-web-remote-car";
const char *password = "20111101";

ESP8266WebServer server(80);

/* Input for motorA:
   IN1      IN2      Action
   LOW      LOW      Motor Stop
   HIGH     LOW      Motor moves forward
   LOW      HIGH     Motor moves backward
   HIGH     HIGH     Motor Stop
*/
int motorIn1 = 4;
int motorIn2 = 5;
int motorIn3 = 12;
int motorIn4 = 13;

int DELAY = 2000;

/**
   car move direction
   0 - stop
             [front]
           7 | 8 | 9
          -----------
   [left]  4 | 5 | 6  [right]
          -----------
           1 | 2 | 3
             [back]
   5 - stop
*/
int dir = 5;
void setup()
{
  Serial.begin(115200);
  delay(1000);
  pinMode(motorIn1, OUTPUT);
  pinMode(motorIn2, OUTPUT);
  pinMode(motorIn3, OUTPUT);
  pinMode(motorIn4, OUTPUT);
  motorstop();
  setupWiFiAP();
}

void loop()
{
  server.handleClient();
  delay(1000);
//  doAction();
//   selfTest();
}

void doAction()
{
  switch (dir) {
    case 2:
      backward();
      break;
    case 4:
      right();
      break;
    case 6:
      left();
      break;
    case 8:
      forward();
      break;
    default:
      motorstop();
      break;
  }
}

void selfTest()
{
  forward();
  delay(DELAY);
  motorstop(); delay(DELAY);

  backward();
  delay(DELAY);
  motorstop(); delay(DELAY);

  right();
  delay(DELAY);
  motorstop(); delay(DELAY);

  left();
  delay(DELAY);
  motorstop(); delay(DELAY);
}

void motorstop()
{
  Serial.println("motorstop\n");
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
  digitalWrite(motorIn3, LOW);
  digitalWrite(motorIn4, LOW);
}

void forward()
{
  Serial.println("forward\n");
  digitalWrite(motorIn1, HIGH);
  digitalWrite(motorIn2, LOW);
  digitalWrite(motorIn3, HIGH);
  digitalWrite(motorIn4, LOW);
}

void backward()
{
  Serial.println("backward\n");
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, HIGH);
  digitalWrite(motorIn3, LOW);
  digitalWrite(motorIn4, HIGH);
}

// Let right motor keep running, but stop left motor
void right()
{
  Serial.println("right\n");
  digitalWrite(motorIn1, HIGH);
  digitalWrite(motorIn2, LOW);
  digitalWrite(motorIn3, LOW);
  digitalWrite(motorIn4, LOW);
}

// Let left motor keep running, but stop right motor
void left()
{
  Serial.println("left\n");
  digitalWrite(motorIn1, LOW);
  digitalWrite(motorIn2, LOW);
  digitalWrite(motorIn3, HIGH);
  digitalWrite(motorIn4, LOW);
}

void setupWiFiAP()
{
  WiFi.softAP(ssid, password);
  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", HTTP_GET, handleRoot);
  server.on("/direction", HTTP_POST, []() {
    returnOK();
  }, handlePost);
  server.on("/direction", HTTP_GET, handleGet);
  
  server.begin();
}
void handleRoot() {
  server.send(200, "text/html", "<!DOCTYPE html><html><head><meta name=viewport content=\"width=device-width, initial-scale=1, user-scalable=0;\"><title>car</title><style type=text/css>html{font-size:16px}.direction-btn{user-select: none;display:inline-block;height:20vh;width:29vw;box-shadow:inset 0 0 1px #111;font-size:1.5rem;text-align:center;vertical-align:middle;line-height:20vh}svg:active{background:#2868c7}.direction-svg{width:100%;height:100%;fill:#3a3a3a}.direction-area{padding:0 3vw;text-align:center}.moniter{height:35vh;width:100vw}</style></head><body><svg id=Layer_1 xmlns=http://www.w3.org/2000/svg><defs><g id=g1><style>.st0{fill:#3a3a3a}path:hover{fill:#000}</style><path class=st0 d=\"M68.6 167.9l.5-.5c26.7-26 55.4-50 85.8-71.7l11.9-8.5c9.5-7.8 23.8-1 23.8 11.3V135c.4 0 .8.1 1.3.1.8.1 1.5.1 2.7.3 2.2.3 4.7.6 7.3.9 2.6.3 5.5.7 8.4 1.1 2.8.5 5.7 1 8.8 1.6 1.5.3 3.1.6 4.8.9 1.6.3 3.3.7 5 1.1 3.4.8 6.9 1.6 10.6 2.5 3.6 1 7.4 2.1 11.3 3.3 3.9 1.2 7.9 2.6 12 4 16.4 5.9 34.3 13.9 52.4 24.9 18.1 10.9 36.3 25 53.3 41.8 8.4 8.5 16.7 17.6 24.2 27.5 3.9 4.9 7.4 10.1 11.1 15.3 1.8 2.6 3.4 5.4 5.1 8l2.6 4.1 2.4 4.2c12.9 22.3 22.9 47 29.1 72.9 3.2 12.9 5.2 26.2 6.5 39.5.3 3.4.6 6.7.9 10.1.1 1.1.2 2.1.2 3.2 0 14.3-11.6 25.8-25.8 25.8-12 0-22-8.1-24.9-19.2 0-.2-.1-.5-.2-.8-.1-.3-.1-.6-.2-.8-.8-3.1-2.3-9.1-2.6-10.6-2.4-10.5-5.3-20.7-9.1-30.5-7.5-19.6-17.6-37.6-29.6-53.2l-2.2-2.9-2.3-2.8c-1.6-1.9-3-3.8-4.6-5.6-3.3-3.5-6.4-7.1-9.8-10.3-6.5-6.7-13.5-12.6-20.5-18-14.1-10.7-28.4-19.1-42.3-25.1-13.9-6.1-27.1-10.1-38.9-12.6-3-.6-5.8-1.2-8.6-1.7-2.8-.4-5.5-.8-8-1.2-2.6-.3-5.1-.5-7.4-.8-1.2-.1-2.3-.3-3.5-.3-1.1-.1-2.2-.1-3.3-.2-2.1-.1-4.2-.2-6.1-.3h-9.9v36.6c0 12.3-14.3 19-23.8 11.3l-11.5-8.2c-30.7-21.9-59.7-46.1-86.7-72.5-9.8-8-9.8-22.6-.2-30.5z\" id=\"XMLID_374_\"/></g><g id=g2><style>.st0{fill:#3a3a3a}</style><path id=XMLID_1_ d=\"M349.3 177.2l-8-11.2c-21.3-29.9-44.9-58.2-70.5-84.4-7.6-9.3-21.9-9.3-29.5 0l-.5.5c-25.4 26-48.7 53.9-69.8 83.5l-8.3 11.6c-7.6 9.2-1 23.2 11 23.2H223c-.3 7.1-.6 14.3-1 21.4-1.9 34.1-3.9 68.1-5.8 102.2-1.3 21.8-2.5 43.6-3.8 65.4-.5 7.6.5 15 3.7 22 9 19.7 30 29.8 50.9 24.6 20.5-5.1 34.1-24.6 32.7-46.7-.9-14.1-1.9-28.3-2.6-42.4-2.7-48.8-5.3-97.7-7.9-146.5h49.2c11.9 0 18.5-13.9 10.9-23.2z\" class=\"st0\"/></g></defs></svg></div><div class=direction-area><div class=\"top-left direction-btn\"><svg class=direction-svg viewBox=\"0 0 512 512\"><use xlink:href=#g1></use></svg></div><div class=\"top direction-btn\"><svg class=direction-svg viewBox=\"0 0 512 512\"><use xlink:href=#g2></use></svg></div><div class=\"top-right direction-btn\"><svg class=direction-svg viewBox=\"0 0 512 512\"><use xlink:href=#g1 transform=\" translate(512) scale(-1,1)\"></use></svg></div><div class=\"left direction-btn\"><svg class=direction-svg viewBox=\"0 0 512 512\"><use xlink:href=#g2 transform=\"rotate(-90 256 256)\"></use></svg></div><div class=\"stop-toggle direction-btn\">STOP</div><div class=\"right direction-btn\"><svg class=direction-svg viewBox=\"0 0 512 512\"><use xlink:href=#g2 transform=\"rotate(90 256 256)\"></use></svg></div><div class=\"down-left direction-btn\"><svg class=direction-svg viewBox=\"0 0 512 512\"><use xlink:href=#g1 transform=\"rotate(-90 256 256)\"></use></svg></div><div class=\"down direction-btn\"><svg class=direction-svg viewBox=\"0 0 512 512\"><use xlink:href=#g2 transform=\"rotate(180 256 256)\"></use></svg></div><div class=\"down-right direction-btn\"><svg class=direction-svg viewBox=\"0 0 512 512\"><use xlink:href=#g1 transform=\"translate(512) scale(-1,1) rotate(-90 256 256)\"></use></svg></div></div><div class=moniter><script type=text/javascript>function postAct(b){var a=0;switch(b){case\"top\":a=8;break;case\"left\":a=4;break;case\"right\":a=6;break;case\"down\":a=2;break}var c=new XMLHttpRequest();c.open(\"GET\",\"direction?dir=\"+a,true);c.send()}function onmousedown(b){var a;if(b.target.tagName===\"svg\"){a=b.target.parentNode.className.replace(/((top|down)?-?(right|left)?).*/,\"$1\");postAct(a)}}document.onmousedown=onmousedown;</script></body></html>");
}
void returnOK() {
  server.send(200, "application/json", "");
  server.sendContent("b{status:0}");
}
void handlePost() {
  
}
void handleGet() {
  Serial.println(":::handlePost::::");
  Serial.print("uri::::");
  Serial.println(server.uri());
  Serial.print("hasArg::::");
  Serial.println(server.hasArg("dir"));
  Serial.print("arg::::");
  Serial.println(server.arg("dir"));
  WiFiClient client = server.client();
  dir = atoi(server.arg("dir").c_str());
  doAction();
  delay(2000);
  dir = 0;
  server.send(200, "text/json", "{status:0}");
}
