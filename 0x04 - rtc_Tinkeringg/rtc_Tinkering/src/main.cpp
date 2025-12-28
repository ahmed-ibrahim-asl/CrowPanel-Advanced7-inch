#include <Arduino.h>
#include "I2C_BM8563.h"



// I2C pin definition for M5Stick & M5Stick Plus & M5Stack Core2
#define BM8563_I2C_SDA 15
#define BM8563_I2C_SCL 16
I2C_BM8563 rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire1);




// Set the RTC time and date
void setRTC(int year, int month, int day_of_the_month, int weekDay, int hour, int minute, int second) {
  I2C_BM8563_DateTypeDef dateStruct;
  I2C_BM8563_TimeTypeDef timeStruct;
  dateStruct.year = year;
  dateStruct.month = month;
  dateStruct.date = day_of_the_month;
  dateStruct.weekDay = weekDay;
  timeStruct.hours = hour;
  timeStruct.minutes = minute;
  timeStruct.seconds = second;
  rtc.setDate(&dateStruct);
  rtc.setTime(&timeStruct);
}

// Get the RTC time and date and print to Serial
void printRTC() {
  I2C_BM8563_DateTypeDef dateStruct;
  I2C_BM8563_TimeTypeDef timeStruct;
  rtc.getDate(&dateStruct);
  rtc.getTime(&timeStruct);
  Serial.printf("%04d/%02d/%02d %02d:%02d:%02d\n",
                dateStruct.year,
                dateStruct.month,
                dateStruct.date,
                timeStruct.hours,
                timeStruct.minutes,
                timeStruct.seconds);
}

void setup() {
  // Init Serial
  Serial.begin(115200);
  Serial.println("Setting RTC to 2023/01/01 00:00:00");
  delay(50);

  // Init I2C
  Wire1.begin(BM8563_I2C_SDA, BM8563_I2C_SCL);

  // Init RTC
  rtc.begin();

  /**
   * [ Break Down of setRTC() parameters ]
   * year: The full year (e.g., 2025)
   * month: The month number (1 = January, 12 = December)
   * date: The day of the month (1–31)
   * weekDay: The day of the week (0 = Sunday, 1 = Monday, ..., 6 = Saturday)
   * hour: The hour (0–23, 24-hour format)
   * minute: The minute (0–59)
   * second: The second (0–59)
   */

  setRTC(2023, 1, 1, 0, 0, 0, 0); 
  
}

void loop() {
  // Print RTC time and date every second
  printRTC();
  delay(1000);
}
