// Include required libraries
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>

// Define WiFi credentials
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

// Define Firebase API Key, Project ID, and user credentials
#define API_KEY ""
#define FIREBASE_PROJECT_ID ""
#define USER_EMAIL ""
#define USER_PASSWORD ""

// Define Firebase Data object, Firebase authentication, and configuration
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Initialize the DHT sensor
  dht.begin();

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Print Firebase client version
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  // Assign the API key
  config.api_key = API_KEY;

  // Assign the user sign-in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the callback function for the long-running token generation task
  config.token_status_callback = tokenStatusCallback;  // see addons/TokenHelper.h

  // Begin Firebase with configuration and authentication
  Firebase.begin(&config, &auth);

  // Reconnect to Wi-Fi if necessary
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Define the path to the Firestore document
  String documentPath = "EspData/DHT11";

  // Create a FirebaseJson object for storing data
  FirebaseJson content;

  // Read temperature and humidity from the DHT sensor
  float temperature = random(20, 40);
  float humidity = random(0, 100);

  // Print temperature and humidity values
  Serial.println(temperature);
  Serial.println(humidity);

  // Check if the values are valid (not NaN)
  if (!isnan(temperature) && !isnan(humidity)) {
    // Set the 'Temperature' and 'Humidity' fields in the FirebaseJson object
    content.set("fields/Temperature/stringValue", String(temperature, 2));
    content.set("fields/Humidity/stringValue", String(humidity, 2));

    Serial.print("Update/Add DHT Data... ");

    // Use the patchDocument method to update the Temperature and Humidity Firestore document
    if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "Temperature") && Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "Humidity")) {
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      } else {
            Serial.println(fbdo.errorReason());
          }
        } else {
          Serial.println("Failed to read DHT data.");
        }

  // Delay before the next reading
  delay(10000);
}
