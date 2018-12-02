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
  char* type;
  int dimensions[3];
} enviro;

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

char* condList[3] = {"No threat      ", "Mild threat    ", "Critical threat"};

double getPressure(){
  // Reads air pressure in Pa and converts to mm Hg
  return 0.00750062 * myPressure.readPressure();
}

double getTemp(){
  // Reads air temperature in degrees Celsius
  return myPressure.readTemp();
}

double calc_y(gas pres_Gas, double pressure, double temp){
  //Function calculates y
  
  return (1/pressure) * pow(10, pres_Gas.A - (pres_Gas.B / (temp + pres_Gas.C)));
}

//double calc_z(gas pres_Gas, double pressure, double temp){
//  //Function calculates z
//  
//  return
//    -0.101 -
//    0.36* temp / pres_Gas.Tc +
//    1.3868 * pow(temp/pres_Gas.Tc - 0.919, 0.5) +
//    (0.021 + (0.04275/((temp/pres_Gas.Tc)-0.65)))*(pressure/pres_Gas.Pc) +
//    (1 - (-0.101 - 0.36*temp/pres_Gas.Tc + 1.3868 * pow((temp/pres_Gas.Tc) - 0.919, 0.5) * exp(-pressure/pres_Gas.Pc*(0.6222 - 0.224*(temp/pres_Gas.Tc) +
//    ((0.0657/((temp/pres_Gas.Tc)-0.85))-0.037)*(pressure/pres_Gas.Pc) + 
//    0.32*exp(-19.53*((temp/pres_Gas.Tc)-1))*(pow(pressure/pres_Gas.Pc,4))-
//    (0.122*(exp(-11.3*((temp/pres_Gas.Tc)-1))))*pow((pressure/pres_Gas.Pc)/10, 4)))));
//    
//}

double calc_yLow(gas pres_Gas, double volume, double pressure, double temp){
  return pres_Gas.lowerExpLim * 100  / volume;
}

double calc_yHigh(gas pres_Gas, double volume, double pressure, double temp){
  return pres_Gas.upperExpLim * 100 / volume;
}

int dangerDet(double Y, double yLow, double yHigh){
  //Based off current conditions within the room, determines whether gas toxic/flammable or not
  
  if (Y < yLow) return 0;
  else if(Y < yHigh) return 1;
  else return 2;
}

int dangerDet_LCD(double Y, double yLow, double yHigh){
  if (Y < yLow){
    lcd.setCursor(0,1);
    lcd.print("No threat     ");
    return 0;
  }
  else if(Y < yHigh){
    lcd.setCursor(0,1);
    lcd.print("Mild threat    ");
    return 1;
  }
  else{
    lcd.setCursor(0,1);
    lcd.print("Critical threat");
    return 2;
  }
}

void dangerPrint(int dangerLvl){
  //Function which prints on LCD whether gas is toxic/flammable or not
  lcd.clear();
  lcd.print("Danger lvl:    ");
  lcd.setCursor(0,1);
  
//  switch(dangerLvl){
//    case 0:
//      lcd.print("No threat");
//      break;
//
//    case 1:
//      lcd.print("Mild threat");
//      break;
//
//    case 2:
//      lcd.print("Critical threat");
//      break;
//  }

  if(dangerLvl == 0) lcd.print("No threat");
  else if(dangerLvl == 1) lcd.print("Mild threat");
  else if(dangerLvl == 2) lcd.print("Critical threat");

  lcd.clear();
}

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  Wire.begin(); 

  myPressure.setModeBarometer(); // Measure pressure in Pascals from 20 to 110 kPa
  myPressure.setOversampleRate(7); // Set Oversample to the recommended 128
  myPressure.enableEventFlags(); // Enable all three pressure and temp event flags

}

void loop() {
  lcd.setCursor(0,0);
  lcd.print("Pressure (mm Hg)");
  lcd.setCursor(0,1);
  lcd.print(getPressure());
  delay(1000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Temp. (degC):   ");
  lcd.setCursor(0,1);
  lcd.print(getTemp());
  delay(2000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Mole Fraction:  ");
  lcd.setCursor(0,1);
  lcd.print(calc_y(gasesList[3], getPressure(), getTemp()));
  delay(2000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Low. M Fraction:");
  lcd.setCursor(0,1);
  lcd.print(calc_yLow(gasesList[0], 5*5, getPressure(), getTemp()));
  delay(2000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Upp. M Fraction:");
  lcd.setCursor(0,1);
  lcd.print(calc_yHigh(gasesList[0], 5*5, getPressure(), getTemp()));
  delay(2000);
  lcd.clear();

  int dangerLvl = dangerDet(
    calc_y(gasesList[3], getPressure(), getTemp()),
    calc_yLow(gasesList[0], 5*5, getPressure(), getTemp()),
    calc_yHigh(gasesList[0], 5*5, getPressure(), getTemp())
    );

  Serial.println(condList[dangerLvl]);

  lcd.clear();

}
