#include <WiFi.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

int ldr  = 35;
int led = 2;
int buzzer = 2;
bool alert = false;

// Insert Firebase project API Key
#define API_KEY "AIzaSyAhzvBhSNftA8rqOQzZaHjM7rHyd8yCzkk"
#define DATABASE_URL "https://firstoffer-e4d08-default-rtdb.firebaseio.com/"
#define USER_EMAIL "aayushsolanki40@gmail.com"
#define USER_PASSWORD "Aayush@02112001"

//Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Variables to save database paths
String listenerPath = "board1/outputs/digital/";

const char* ssid = "Mr.";
const char* password = "12131214";

const String title = "Security System Alert";
const String body = "Aayush, Object is detected betweeen your sensor";

const String device = "c3SM1LCxTjuMJvX-1velE3:APA91bG6q7GETj4RmhqsXv-kYSJiQzO2DXfuzNagLgFxmr0yPd_WxfwOBekBOJwShLQDIGC1oNgs1Je9RJP8z-yuvo0jXwAYyOnBpekh5BUwvLu4zffYC_FB47dmq0hZtBToO1B0eFdz"; //device token key
const String key = "key=AAAAqRUg5Po:APA91bHfEL0u7GtHer4IfLzBbJVnrCoa1kIVYvvY7PGhoIPxBHSJL5uPrkDeeriNXL1LHrM1iExJgd8V6pfAcDGmrK76rtUw9oeVBf1EtHy9QrpmZaBqkvMLw3hAUCgwvhCorTTVSdL9"; //you should create youself

unsigned long sendDataPrevMillis = 0;
int intValue;
float floatValue;
bool signupOK = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(ldr, INPUT);
  pinMode(led, OUTPUT);
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); //Optional
  WiFi.begin(ssid, password);
  Serial.println("\nConnecting");

  while(WiFi.status() != WL_CONNECTED){
      Serial.print(".");
      delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");
  Serial.println(WiFi.localIP());

  // Assign the api key (required)
  config.api_key = API_KEY;

  // Assign the user sign in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the RTDB URL (required)
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase Signup ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  config.max_token_generation_retry = 5;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  delay(2000);
}



int sendNotification()
{
  HTTPClient http;        
  http.begin("https://fcm.googleapis.com/fcm/send");
  http.addHeader("Authorization",key);
  http.addHeader("Content-Type","application/json");
  Serial.print("Sending notification ! ==> ");  
  String data = String("{\"to\":\""+device+"\",\"notification\":{\"body\":\""+body+"\",\"OrganizationId\":\"2\",\"content_available\":true,\"priority\":\"high\",\"title\":\""+title+"\"},\"data\":{\"priority\":\"high\",\"sound\":\"app_sound.wav\",\"content_available\":true,\"bodyText\":\"NewAnnouncementassigned\"}}");
  int httpCode = http.POST(data);
//  String payload = http.getString();
//  Serial.println(payload);
  if(httpCode == HTTP_CODE_OK){
    Serial.println("OK");
  }else{
    Serial.println("Failed");
  }
  http.end();
  return httpCode;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){

    sendDataPrevMillis = millis();
    if (Firebase.RTDB.getInt(&fbdo, "/test/int")) {
      if (fbdo.dataType() == "int") {
        intValue = fbdo.intData();
        Serial.println(intValue);
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }
    
    if (Firebase.RTDB.getFloat(&fbdo, "/test/float")) {
      if (fbdo.dataType() == "float") {
        floatValue = fbdo.floatData();
        Serial.println(floatValue);
      }
    }
    else {
      Serial.println(fbdo.errorReason());
    }
  
  if(alert == false){
    int x = analogRead(ldr);
    Serial.println(x);
    if(x == 0){
      digitalWrite(led, HIGH);
      digitalWrite(buzzer, HIGH);
      if(WiFi.status()== WL_CONNECTED){
        sendNotification();
        alert = true;
      }
      else {
        Serial.println("WiFi Disconnected");
      }
    }
    else{
      digitalWrite(led, LOW);
      digitalWrite(buzzer, LOW);
    }
  }
  }
}
