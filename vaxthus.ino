// Include the DHT sensor library
#include "DHT.h"

// Motor3 pins
int motorUppNedPin1 = 9;   // Blue - 28BYJ48 pin 1
int motorUppNedPin2 = 10;  // Pink - 28BYJ48 pin 2
int motorUppNedPin3 = 11;  // Yellow - 28BYJ48 pin 3
int motorUppNedPin4 = 12;  // Orange - 28BYJ48 pin 4

int motorSpeed = 1200;  // Variable to set stepper motor speed
int lookup[8] = { B01000, B01100, B00100, B00110, B00010, B00011, B00001, B01001 };

// Define sensor and control pins
const int soilPin = A0;  // Soil moisture sensor pin
const int DHT11_PIN = 2; // DHT11 sensor data pin

// Create DHT sensor object
DHT dht11(DHT11_PIN, DHT11);

// Define threshold values for control
const int thresSoil = 226; // Soil moisture threshold
const int thresHumid = 80; // Humidity threshold
const int thresTemp = 29;  // Temperature threshold

void setup() {
  // Initialize motor pins as outputs
  pinMode(motorUppNedPin1, OUTPUT);
  pinMode(motorUppNedPin2, OUTPUT);
  pinMode(motorUppNedPin3, OUTPUT);
  pinMode(motorUppNedPin4, OUTPUT);
  
  // Start serial communication
  Serial.begin(9600);
  
  // Initialize DHT sensor
  dht11.begin();
  
  // Initialize pin 3 as output for controlling pump motor
  pinMode(3, OUTPUT);
}

void loop() {
  // Read temperature and humidity from DHT11 sensor
  float humi = readHumid();
  float tempC = readTemp();

  // Check if any reads failed and print error message if so
  if (isnan(humi) || isnan(tempC)) {
    Serial.println("Failed to read from DHT11 sensor!");
  } else {
    // Print humidity and temperature values
    Serial.print("DHT11# Humidity: ");
    Serial.print(humi);
    Serial.print("%");

    Serial.print("  |  "); 

    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.print("°C ~ ");
    Serial.print(dht11.readTemperature(true)); // Convert Celsius to Fahrenheit
    Serial.println("°F");
  }

  // Read soil moisture value
  int value = analogRead(soilPin);
  Serial.print("Moisture: ");
  Serial.println(value);
  delay(50);

  // Check soil moisture level and control pump motor accordingly
  if (value < thresSoil) {
    Serial.println("The soil is DRY");
    digitalWrite(3, HIGH); // Turn on pump motor
  } else {
    Serial.println("The soil is WET");
    digitalWrite(3, LOW); // Turn off pump motor
  }

  // Check temperature and humidity thresholds to control stepper motor
  if (tempC > thresTemp || humi > thresHumid) {
    // Move stepper motor upwards
    uppat();
  } else {
    // Move stepper motor downwards
    nerat();
  }
}

// Function to read temperature from DHT11 sensor
float readTemp() {
  return dht11.readTemperature();
}

// Function to read humidity from DHT11 sensor
float readHumid() {
  return dht11.readHumidity();
}

// Function to move stepper motor downwards
void nerat() {
  for (int i = 0; i < 8; i++) {
    setOutputUppNed(i);
    delayMicroseconds(motorSpeed);
    Serial.print("Ned ");
  }
}

// Function to move stepper motor upwards
void uppat() {
  for (int i = 7; i >= 0; i--) {
    setOutputUppNed(i);
    delayMicroseconds(motorSpeed);
    Serial.print("Upp ");
  }
}

// Function to set the output pins for stepper motor
void setOutputUppNed(int out) {
  digitalWrite(motorUppNedPin1, bitRead(lookup[out], 0));
  digitalWrite(motorUppNedPin2, bitRead(lookup[out], 1));
  digitalWrite(motorUppNedPin3, bitRead(lookup[out], 2));
  digitalWrite(motorUppNedPin4, bitRead(lookup[out], 3));
}
