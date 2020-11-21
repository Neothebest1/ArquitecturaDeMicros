#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

const char* ssid = "Totalplay-ABA2";
const char* password = "ABA2FBDAAxkMTpM8";

const int gLeftFw = 5;
const int gRigthFw = 18;
const int gLeftBack = 19;
const int gRigthBack = 21;
String carState;


#define STOP_STATE 0
#define FW_STATE 1
#define BACK_STATE 2
#define RT_STATE 3
#define LT_STATE 4

int gCurrentState;

void setCurrentState(int pState){
    gCurrentState = pState;
}

int getCurrentState(void){
    return gCurrentState;
}

AsyncWebServer server(80);

// Replaces placeholder with LED state value
String processor(const String& var){
  Serial.println(var);
  int lCurrentState;
  if(var == "STATE"){
    lCurrentState = getCurrentState();
    switch(lCurrentState){
      case STOP_STATE:
        carState = "STOPPED";
        break;
      case FW_STATE:
        carState = "FW_STATE";
        break;
      case BACK_STATE:
        carState = "BACK_STATE";
        break;
      case RT_STATE:
        carState = "RT_STATE";
        break;
      case LT_STATE:
        carState = "LT_STATE";
        break;
      default:
        carState = "INVALID";
        break;
    }
    Serial.print(carState);
    return carState;
  }
  return String();
}
 
void setup(){
  
  setCurrentState(STOP_STATE);
  
  Serial.begin(115200);
  pinMode(gLeftFw, OUTPUT);
  pinMode(gRigthFw, OUTPUT);
  pinMode(gLeftBack, OUTPUT);
  pinMode(gRigthBack, OUTPUT);

  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP32 Local IP Address
  Serial.println(WiFi.localIP());

  digitalWrite(gLeftFw, LOW);
  digitalWrite(gRigthFw, LOW);
  digitalWrite(gLeftBack, LOW);
  digitalWrite(gRigthBack, LOW);

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to load style.css file
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Route to set GPIO to HIGH
  server.on("/parar", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(gLeftFw, LOW);
    digitalWrite(gRigthFw, LOW);
    digitalWrite(gLeftBack, LOW);
    digitalWrite(gRigthBack, LOW);
    setCurrentState(STOP_STATE);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });
  
  // Route to set GPIO to LOW
  server.on("/adelante", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(gLeftFw, HIGH);
    digitalWrite(gRigthFw, HIGH);
    digitalWrite(gLeftBack, LOW);
    digitalWrite(gRigthBack, LOW);
    setCurrentState(FW_STATE);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/atras", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(gLeftFw, LOW);
    digitalWrite(gRigthFw, LOW);
    digitalWrite(gLeftBack, HIGH);
    digitalWrite(gRigthBack, HIGH);
    setCurrentState(BACK_STATE);     
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/izquierda", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(gLeftFw, LOW);
    digitalWrite(gRigthFw, HIGH);
    digitalWrite(gLeftBack, LOW);
    digitalWrite(gRigthBack, LOW);
    setCurrentState(LT_STATE);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.on("/derecha", HTTP_GET, [](AsyncWebServerRequest *request){
    digitalWrite(gLeftFw, HIGH);
    digitalWrite(gRigthFw, LOW);
    digitalWrite(gLeftBack, LOW);
    digitalWrite(gRigthBack, LOW);
    setCurrentState(RT_STATE);    
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Start server
  server.begin();
}
 
void loop(){
  
}
