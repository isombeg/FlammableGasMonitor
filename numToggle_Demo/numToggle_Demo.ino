#include <SparkFunMPL3115A2.h>
#include <LiquidCrystal.h>
#include <Wire.h>

LiquidCrystal lcd(2,3,4,5,6,7); //LCD object

typedef struct{
  char* function;
  int pinNum;
} push_button;

push_button buttons[3] = {
   {"Up", 13},
   {"Down", 12},
   {"Selection" , 11}
};


int buttonDet(){
  //Function determines what button is being pressed
  Serial.println("Beginning buttonDet");
  
  if(digitalRead(buttons[0].pinNum) == HIGH) return 1; //Up button is pressed
  else if(digitalRead(buttons[1].pinNum) == HIGH) return -1; //Down button is pressed
  else if(digitalRead(buttons[2].pinNum) == HIGH) return 0; //Select button is pressed

  buttonDet();
}

double numToggle(const char* mssg, int digAmt, int decPlace){
  //Function for user to toggle across number on LCD screen while incrementing and decrementing digits
  //FIX ME: Number which appears on LCD Screen differs from number which function returns
  
  Serial.println("Beginning numToggle");
  
  lcd.setCursor(0,0);
  lcd.print(mssg);

  int location;
  for(location = 0; location < digAmt + (decPlace > 0 ? 1 : 0); location++){
    lcd.setCursor(location, 1);
    if(location == digAmt - decPlace) lcd.print(".");
    else lcd.print(0);
  }
  
  double rslt = pow(10, -decPlace);
  lcd.setCursor(location - 1, 1);
  Serial.print("\nrslt: "); Serial.println(rslt);
  Serial.print("location: "); Serial.println(location);
  
  int digit = 0;
  while(location > 0){
    delay(500);
    int pressedNum = buttonDet();
    lcd.setCursor(location + decPlace - digAmt - 1 > 0 ? location - 1 : location - 2, 1);

    Serial.print("buttonDet returned: "); Serial.println(pressedNum);
        
    switch (pressedNum){
      case 0:
        rslt += 1.0/pow(10, (--location) - decPlace) * digit;
        Serial.print("rslt: "); Serial.println(rslt);
        digit = 0;
        break;
  
      case -1:
        digit = digit - 1 < 0 ? 9 : (digit - 1) % 10;
        lcd.print(digit);
        break;
      
      case 1:
        digit = digit == 10 ? 0 : digit + 1;
        lcd.print(digit);
        break;
    }

    Serial.print("digit: "); Serial.println(digit);
    Serial.print("location: "); Serial.println(location);
  }

  lcd.print(rslt);
  Serial.println("End of numToggle \n\n\n\n\n\n\n\n\n\n\n\n\n .");
  delay(2000);
  return rslt;
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);

  pinMode(buttons[0].pinNum, INPUT);
  pinMode(buttons[1].pinNum, INPUT);
  pinMode(buttons[2].pinNum, INPUT);
}

void loop() {
  numToggle("Tank height (m)", 6, 3);

}
