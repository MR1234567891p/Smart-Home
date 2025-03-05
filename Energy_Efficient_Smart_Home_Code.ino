#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Initialize the LCD with I2C address 0x27 and 16x2 characters
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define in A0           // IR sensor for entry on analog pin A0
#define out A1          // IR sensor for exit on analog pin A1
#define led 13          // LED indicator on digital pin 13
#define in1 3           // L298N motor driver IN1 pin
#define in2 4           // L298N motor driver IN2 pin
#define enA 5           // L298N motor driver ENA pin (PWM control)

int count = 0;
int debounceDelay = 50;  // 50ms debounce delay
bool lastStateIn = LOW;  // Store the last state of the "in" sensor
bool lastStateOut = LOW; // Store the last state of the "out" sensor

void IN() {
    count++;
    lcd.clear();
    lcd.print("Person In Room:");
    lcd.setCursor(0, 1);
    lcd.print(count);
    delay(1000);
}

void OUT() {
    count--;
    lcd.clear();
    lcd.print("Person In Room:");
    lcd.setCursor(0, 1);
    lcd.print(count);
    delay(1000);
}

void setup() {
    lcd.begin(16, 2);    // Initialize the LCD with 16 columns and 2 rows
    lcd.backlight();     // Turn on the LCD backlight
    lcd.print("Visitor Counter");
    delay(2000);

    pinMode(in, INPUT);
    pinMode(out, INPUT);
    pinMode(led, OUTPUT);

    // Set motor driver pins
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(enA, OUTPUT);

    lcd.clear();
    lcd.print("Person In Room:");
    lcd.setCursor(0, 1);
    lcd.print(count);

    // Initialize motor to OFF state
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(enA, 0);  // PWM at 0 (motor off)
}

void loop() {
    bool currentStateIn = digitalRead(in);
    bool currentStateOut = digitalRead(out);

    // Debounce logic for "in" sensor
    if (currentStateIn == HIGH && lastStateIn == LOW) {
        delay(debounceDelay); // Wait to ensure stable signal
        if (digitalRead(in) == HIGH) {
            IN();
        }
    }
    lastStateIn = currentStateIn;

    // Debounce logic for "out" sensor
    if (currentStateOut == HIGH && lastStateOut == LOW) {
        delay(debounceDelay); // Wait to ensure stable signal
        if (digitalRead(out) == HIGH) {
            OUT();
        }
    }
    lastStateOut = currentStateOut;

    // LED and motor behavior
    if (count <= 0) {
        lcd.clear();
        digitalWrite(led, LOW);        // Turn off LED
        lcd.print("Nobody In Room");
        lcd.setCursor(0, 1);
        lcd.print("Light Is Off");

        // Turn off motor (fan)
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
        analogWrite(enA, 0);          // PWM at 0 (motor off)
        count = 0; // Ensure count doesn't go below zero
        delay(200);
    } else {
        digitalWrite(led, HIGH);      // Turn on LED

        // Turn on motor (fan)
        digitalWrite(in1, HIGH);     // Set motor to forward direction
        digitalWrite(in2, LOW);
        analogWrite(enA, 255);       // Set full PWM for maximum speed
    }
}
