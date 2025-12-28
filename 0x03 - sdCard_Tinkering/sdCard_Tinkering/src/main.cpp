#include "pins_config.h"
#include "LovyanGFX_Driver.h"
#include <Wire.h>
#include <SPI.h>
#include <FS.h>
#include <SD.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <TCA9534.h>

TCA9534 ioex;

#define SD_MOSI 6
#define SD_MISO 4
#define SD_SCK 5
#define SD_CS 0 //The chip selector pin is not connected to IO

SPIClass SD_SPI = SPIClass(HSPI);
LGFX gfx;

// Function to write a simple text file to SD card
void writeTextFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);
  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written successfully");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// SD card initialization
int SD_init() {
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  SD_SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
  if (!SD.begin(SD_CS, SD_SPI, 80000000)) {
    Serial.println(F("ERROR: File system mount failed!"));
    SD_SPI.end();
    return 1;
  } else {
    Serial.println("Card Mount Successed");
    Serial.printf("SD Size: %lluMB \n", SD.cardSize() / (1024 * 1024));
  }
  Serial.println("**** TF Card init finished ****.");
  return 0;
}

void setup() {
  // Initialize serial communication for debugging
  Serial.begin(115200);

  // Initialize the display
  gfx.init();            // Initialize display driver
  gfx.initDMA();         // Initialize DMA for fast graphics
  gfx.startWrite();      // Start writing to the display
  gfx.fillScreen(TFT_BLACK); // Clear the display (black background)
  delay(500);            // Short delay for stability

  
 
  /**
   * [ Initialize I2C for IO expander (TCA9534) ]
   * TCA9534: Used to control the backlight of your display by configuring
   * one of its pins as an output and setting it high (turning the backlight on).
  */
  Wire.begin(15, 16);    // Set I2C SDA to GPIO 15, SCL to GPIO 16
  delay(50);             // Wait for I2C bus to stabilize
  ioex.attach(Wire);     // Attach IO expander to I2C bus
  ioex.setDeviceAddress(0x18); // Set IO expander I2C address
  ioex.config(1, TCA9534::Config::OUT); // Set pin 1 as output
  ioex.output(1, TCA9534::Level::H);    // Set pin 1 high (turn on backlight)

  Serial.println("----- Setup started -----");

  // Initialize SD card and write a test file
  if (SD_init() == 0) {
    Serial.println("TF_Card initialization succeeded");
    writeTextFile(SD, "/test.txt", "Hello from ESP32!\n"); // Write a simple text file
  } else {
    Serial.println("TF card initialization failed");
  }

  // Set display rotation and clear screen again
  gfx.setRotation(2);
  gfx.fillScreen(TFT_BLACK);
  Serial.println("----- Setup done -----");
}

void loop() {
  // ...existing code for your main loop...
}