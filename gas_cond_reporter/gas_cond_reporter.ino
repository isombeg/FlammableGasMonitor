#include <SparkFunMPL3115A2.h>
#include <LiquidCrystal.h>
#include <Wire.h>
#include <math.h>

LiquidCrystal lcd(2,3,4,5,6,7); //LCD object
MPL3115A2 myPressure;

//necessary structs
typedef struct{
  char gasName[17];
  double A;
  double B;
  double C;
  double T1;
  double T2;
  double Tc;
  double Pc;
  double lowerExpLim;
  double upperExpLim;
} gas;

typedef struct{
  double rWidth;
  double rLength;
  double rHeight;
} room;

typedef struct{
  double tRadius;
  double tHeight;
} tank;

typedef struct{
  char* type;
  int dimensions[3];
} enviro;

typedef struct{
  char* function;
  int pinNum;
} push_button;

push_button buttons[3] = {
   {"Up", 13},
   {"Down", 12},
   {"Selection" , 11}
};

enviro Room = {"Room", {0}};
enviro Tank = {"Tank", {0}};

enviro envList[2] = {Room, Tank};

gas Benzene = {"Benzene",4.02232,1206.53,220.291,278.7,377.1,562.6,36935.99,0.0135,0.0665};
gas Butadiene_1_3 = {"Butadiene-1,3",3.97489,930.546,238.854,193.3,287.6,425,32452,0.02,0.12};
gas Butane = {"Butane",3.93266,935.773,238.789,195.5,292.1,425.17,28477.196,0.0186,0.0841};
gas Carbon_Monoxide = {"Carbon Monoxide",3.81912,291.743,267.999,68.2,88.1,133,26220,0.12,0.75};
gas Ethane = {"Ethane",3.95405,663.72,256.681,130.4,198.2,305.4,36631.99,0.03,0.12};

//gases list
gas gasesList[5] = {
  Benzene,
  Butadiene_1_3,
  Butane,
  Carbon_Monoxide,
  Ethane,
};

double getPressure(){
  // Reads air pressure in Pa and converts to mm Hg
  return 0.00750062 * myPressure.readPressure();
}

double getTemp(){
  // Reads air temperature in degrees Celsius
  return myPressure.readTemp();
}

gas userSelection(){
  //Function for user to select gas
}

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

int enviroDet(int key){
  Serial.println("Start: enviroDet");
  Serial.print("key: "); Serial.println(key);
  
  //Function to select on whether environment is a room or tank (on LCD screen)
  lcd.setCursor(0,0);
  lcd.print("Environment?");
  lcd.setCursor(0,1);
  Serial.print("Type for key: "); Serial.println(envList[key % 2].type);
  lcd.print(envList[key % 2].type);

  if(buttonDet() == 0){
    Serial.println("buttonDet returned: 0");
    
    lcd.setCursor(0,0);
    lcd.print("            ");
    lcd.setCursor(0,1);
    lcd.print("            ");
    
    delay(2000);
    return key;
  }

  else if(buttonDet() == 1 || buttonDet() == -1){
    Serial.println("buttonDet returned: non 0 number");
    
    enviroDet(++key);
  }

  enviroDet(key);
  
}

double calc_y(gas pres_Gas, double pressure, double temp){
  //Function calculates y
  
  return (1/pressure) * pow(10, pres_Gas.A - (pres_Gas.B / (temp + pres_Gas.C)));
}

double calc_z(gas pres_Gas, double pressure, double temp){
  //Function calculates z
  
  return
    -0.101 -
    0.36* temp / pres_Gas.Tc +
    1.3868 * pow(temp/pres_Gas.Tc - 0.919, 0.5) +
    (0.021 + (0.04275/((temp/pres_Gas.Tc)-0.65)))*(pressure/pres_Gas.Pc) +
    (1 - (-0.101 - 0.36*temp/pres_Gas.Tc + 1.3868 * pow((temp/pres_Gas.Tc) - 0.919, 0.5) * exp(-pressure/pres_Gas.Pc*(0.6222 - 0.224*(temp/pres_Gas.Tc) +
    ((0.0657/((temp/pres_Gas.Tc)-0.85))-0.037)*(pressure/pres_Gas.Pc) + 
    0.32*exp(-19.53*((temp/pres_Gas.Tc)-1))*(pow(pressure/pres_Gas.Pc,4))-
    (0.122*(exp(-11.3*((temp/pres_Gas.Tc)-1))))*pow((pressure/pres_Gas.Pc)/10, 4)))));
    
}

double calc_yLow(gas pres_Gas, double volume, double pressure, double temp){
  return pres_Gas.lowerExpLim * 100 * calc_z(pres_Gas, pressure, temp)  / volume;
}

double calc_yHigh(gas pres_Gas, double volume, double pressure, double temp){
  return pres_Gas.upperExpLim * 100 * calc_z(pres_Gas, pressure, temp) / volume;
}

int dangerDet(double Y, double yLow, double yHigh){
  //Based off current conditions within the room, determines whether gas toxic/flammable or not
  
  if (Y < yLow) return 0;
  else if(Y < yHigh) return 1;
  else return 2;
}

void dangerPrint(int dangerLvl){
  //Function which prints on LCD whether gas is toxic/flammable or not

  lcd.setCursor(0,0);
  lcd.print("Danger lvl:    ");
  lcd.setCursor(0,1);
  lcd.print("               ");
  lcd.setCursor(0,1);
  
  switch(dangerLvl){
    case 0:
      lcd.print("No threat");
      break;

    case 1:
      lcd.print("Mild threat");
      break;

    case 2:
      lcd.print("Critical threat");
      break;
  }

  lcd.clear();
}

void setup() {
  // set up the LCD's number of columns and rows:
  Serial.begin(9600);
  lcd.begin(16, 2);

  myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags

  pinMode(buttons[0].pinNum, INPUT);
  pinMode(buttons[1].pinNum, INPUT);
  pinMode(buttons[2].pinNum, INPUT);
}

void loop() {
  Serial.println("Working");
  Serial.print("Room Pressure (mm Hg): "); Serial.println(getPressure());
  Serial.print("Room Temperature (deg C)"); Serial.println(getTemp());

}
