#include <Arduino.h>

static int latest_temperature = 0;

extern "C" void request_signal() {
  
  
  Serial.println("Requesting new reading...");

  Serial1.println("x");
  // Wait for response (blocking, simple)
  unsigned long start = millis();
  while (!Serial1.available() && millis() - start < 1000) {
    delay(10);
  }
  if (Serial1.available()) {
    String response = Serial1.readStringUntil('\n');
    Serial.print("Received response: ");
    Serial.println(response);
    int temp = response.toInt();
    if (temp != 0 || response.startsWith("0")) {
      latest_temperature = temp;
    }
  } else {
    Serial.println("No response received.");
  }
}

extern "C" int get_latest_temperature() {
  return latest_temperature;
}
