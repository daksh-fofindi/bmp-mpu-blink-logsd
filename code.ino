#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// --- Pin Definitions ---
#define BUZZER_PIN 27
#define LED_PIN 13
#define SD_CS 5 // Chip Select pin for SD card

// --- Sensor Objects and File ---
Adafruit_BMP280 bmp;   // I2C address 0x76 (standard)
Adafruit_MPU6050 mpu; // I2C address 0x68 (standard)

// Single File Name
const char* LOG_FILE_NAME = "/FLIGHT_LOG.csv";

// --- Timing Variables ---
// **MODIFIED:** Log data every 100ms (10 times per second)
const long LOG_INTERVAL_MS = 100; 
unsigned long lastLogTime = 0;

// LED/Buzzer blink variables (2 blinks per second, 150ms ON)
const int BLINK_DURATION_MS = 150; 
const int BLINK_INTERVAL_MS = 500; 
unsigned long lastBlinkTime = 0;
bool isBlinkOn = false;

// --- Helper Functions ---

// Function to initialize the file with a combined header
void initializeFileHeader(const char* fileName) {
  // Define the comprehensive CSV header
  const char* header = "Time_ms,Temp_C,Pressure_hPa,Altitude_m,AccX_g,AccY_g,AccZ_g,GyroX_dps,GyroY_dps,GyroZ_dps";

  File dataFile = SD.open(fileName, FILE_WRITE);
  if (dataFile) {
    // Write header only if the file is new/empty
    if (dataFile.size() == 0) {
      dataFile.println(header);
      Serial.println("Wrote combined file header.");
    }
    dataFile.close();
  } else {
    Serial.print("Error creating/opening file for header: ");
    Serial.println(fileName);
  }
}

// Function to log ALL sensor data to the single file
void readAndLogAllSensors() {
  unsigned long currentMillis = millis();
  
  // 1. Read BMP280 Data
  // Note: BMP280 readings can be slower than MPU6050. For 100ms logging, ensure the BMP is
  // configured for faster acquisition (done by default with Adafruit_BMP280 library).
  float temp = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0F; // Convert to hPa
  float altitude = bmp.readAltitude(1013.25);
  
  // 2. Read MPU6050 Data
  sensors_event_t a, g, mpu_temp;
  mpu.getEvent(&a, &g, &mpu_temp);
  
  // 3. Prepare the combined data string
  // Format: Time, Temp, Pressure, Altitude, AccX, AccY, AccZ, GyroX, GyroY, GyroZ
  String dataString = "";
  dataString += String(currentMillis);
  dataString += ",";
  dataString += String(temp, 2);      // BMP280 Temp
  dataString += ",";
  dataString += String(pressure, 2);  // BMP280 Pressure
  dataString += ",";
  dataString += String(altitude, 2);  // BMP280 Altitude
  dataString += ",";
  dataString += String(a.acceleration.x, 3); // MPU6050 Accel X
  dataString += ",";
  dataString += String(a.acceleration.y, 3); // MPU6050 Accel Y
  dataString += ",";
  dataString += String(a.acceleration.z, 3); // MPU6050 Accel Z
  dataString += ",";
  dataString += String(g.gyro.x, 3);         // MPU6050 Gyro X
  dataString += ",";
  dataString += String(g.gyro.y, 3);         // MPU6050 Gyro Y
  dataString += ",";
  dataString += String(g.gyro.z, 3);         // MPU6050 Gyro Z

  // 4. Log to the Single SD Card File
  File dataFile = SD.open(LOG_FILE_NAME, FILE_APPEND);
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    Serial.print("Logged: ");
    Serial.println(dataString);
    Serial.println("------------------------------------");
  } else {
    Serial.println("Error opening FLIGHT_LOG.csv for writing! Check SD card.");
  }
}

// --- Setup ---

void setup() {
  // Pin setup
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  
  Serial.begin(115200);
  Serial.println("\n--- Combined Data Logger Initializing ---");

  // I2C Initialization for both sensors (SDA=21, SCL=22 for ESP32)
  Wire.begin(21, 22);

  // 1. BMP280 Initialization
  if (!bmp.begin(0x76)) {
    Serial.println("Could not find BMP280 sensor! Check wiring.");
    while (1); // Halt if sensor is not found
  } else {
    Serial.println("BMP280 sensor initialized.");
  }

  // 2. MPU6050 Initialization
  if (!mpu.begin(0x68)) {
    Serial.println("Failed to find MPU6050 chip! Check wiring.");
    while (1); // Halt if sensor is not found
  } else {
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    Serial.println("MPU6050 sensor initialized.");
  }

  // 3. SD Card Initialization
  Serial.print("Initializing SD card...");
  // Using higher SPI clock speed for faster writing
  if (!SD.begin(SD_CS, SPI, 4000000)) { 
    Serial.println("SD card initialization failed!");
    while (1); // Halt if card fails
  }
  Serial.println("SD card initialized.");

  // 4. Initialize Single File Header
  initializeFileHeader(LOG_FILE_NAME);
  
  Serial.println("--- Setup Complete. Starting 100ms Logging ---");
}

// --- Loop ---

void loop() {
  unsigned long currentMillis = millis();

  // --- Task 1: Non-blocking LED & Buzzer Blink (Runs frequently) ---
  // This logic manages the 150ms HIGH state and the 350ms LOW state.
  if (!isBlinkOn && (currentMillis - lastBlinkTime >= (BLINK_INTERVAL_MS - BLINK_DURATION_MS))) {
    // Condition met to turn ON (350ms has passed since last OFF)
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(BUZZER_PIN, HIGH);
    isBlinkOn = true;
    lastBlinkTime = currentMillis; // Mark the time we turned ON

  } else if (isBlinkOn && (currentMillis - lastBlinkTime >= BLINK_DURATION_MS)) {
    // Condition met to turn OFF (150ms ON time has passed)
    digitalWrite(LED_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    isBlinkOn = false;
    // lastBlinkTime is NOT reset here; it remains the time the pulse started.
  }
  
  // --- Task 2: Sensor Data Logging (Runs only every 100ms) ---
  if (currentMillis - lastLogTime >= LOG_INTERVAL_MS) {
    lastLogTime = currentMillis;
    readAndLogAllSensors();
  }
}