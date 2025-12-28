#include <Arduino.h>

extern "C" void print_from_ui(const char * msg) {
  if (msg) {
    Serial.println(msg);
  } else {
    Serial.println("print_from_ui: (null)");
  }
}
