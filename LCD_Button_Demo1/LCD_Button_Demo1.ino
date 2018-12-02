#include <LiquidCrystal.h>
#include <Wire.h>

LiquidCrystal lcd(2,3,4,5,6,7);

typedef struct{
  char* function;
  int pinNum;
} push_button;

push_button buttons[3] = {
   {"Up", 13},
   {"Down", 12},
   {"Selection" , 11}
};

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Button pressed:");

  Wire.begin();        // Join i2c bus
  Serial.begin(9600);  // Start serial for output

  pinMode(buttons[0].pinNum, INPUT);
  pinMode(buttons[1].pinNum, INPUT);
  pinMode(buttons[2].pinNum, INPUT);
}

void loop() {
  lcd.setCursor(0, 1);

  if(digitalRead(buttons[0].pinNum) == HIGH){
    Serial.println(buttons[0].pinNum);
    lcd.print(buttons[0].function);
    delay(2000);
  }
  
  else if(digitalRead(buttons[1].pinNum) == HIGH){
    Serial.println(buttons[1].pinNum);
    lcd.print(buttons[1].function);
    delay(2000);
  }

  else if(digitalRead(buttons[2].pinNum) == HIGH){
    Serial.println(buttons[2].pinNum);
    lcd.print(buttons[2].function);
    delay(2000);
  }

  lcd.print("           ");

}
