
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

int mode = 1;
int temp_mode=0;

int cap_resistorValue = 8070;
int res_resistorValue = 1000;
const double voltage_sensor_multiplication_factor = 5.2 / 1023.0;
double res_voltage = 0;
double resistance = 0;

double voltage;
double current;

unsigned long startTime;
unsigned long elapsedTime;
float microFarads;                
float nanoFarads;

void setup() {
  lcd.begin(16, 2);
  lcd.backlight();
  pinMode(2, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(13, INPUT);
  Serial.begin(9600);
  lcd.print("Hello, World!");
  pinMode(6,OUTPUT);  
  mode=1;
}

void loop() {
  temp_mode = see_mode();
  lcd.clear();
  if (mode == 0) {
    Serial.println("Invalid pin combination. Please review choices.");
    lcd.print("Invalid choice");
    lcd.setCursor(0, 1);
    lcd.print("Review pins");
  } 
  if(temp_mode==5){
  }
  else{
    mode=temp_mode;
  }
    switch (mode) {
      case 1:
        measureResistance();
        break;
        
      case 2:
        measureCapacitance();
        break;

      case 3:
        measureVoltage();
        break;

      case 4:
        measureCurrent();
        break;
    }
    Serial.print(mode);
  delay(1000); // Delay between measurements
}

void measureResistance() {
  Serial.println("Resistance mode");
  lcd.print("Resistance mode");
  digitalWrite(6, HIGH);
  digitalWrite(7, LOW);
  delay(1000);
  
  res_voltage = voltage_sensor_multiplication_factor * analogRead(A1);
  Serial.println(analogRead(A1));
  Serial.print("Res_voltage= ");Serial.println(res_voltage);
  resistance = (res_voltage / (5.2 - res_voltage)) * res_resistorValue;
  
  Serial.print("Resistance: ");
  if (resistance >= 1000) {
    Serial.print(resistance / 1000.0);
    Serial.println(" kOhms");
    lcd.setCursor(0, 1);
    lcd.print("Res: ");
    lcd.print(resistance / 1000.0);
    lcd.print(" kOhm");
  } else {
    Serial.print(resistance);
    Serial.println(" Ohms");
    lcd.setCursor(0, 1);
    lcd.print("Res: ");
    lcd.print(resistance);
    lcd.print(" Ohm");
  }
}

void measureCapacitance() {
  Serial.println("Capacitance mode");
  lcd.print("Capacitance mode");
  digitalWrite(6,LOW);
  while(analogRead(A0)>5){
    Serial.println("Press  Button to discharge");
    Serial.println(analogRead(A0));2
  digitalWrite(6, HIGH);

  startTime = millis(); // Begin the timer
  while (analogRead(A0) < 648) {Serial.println(analogRead(A0));} // 63.2% of 1023 (5V)
  elapsedTime = millis() - startTime;
  microFarads = ((float)elapsedTime / cap_resistorValue) * 1000;
  
  Serial.print(elapsedTime);       
  Serial.print(" mS    ");         

  if (microFarads > 1) {
    Serial.print((long)microFarads);       
    Serial.println(" microFarads");
    lcd.setCursor(0, 1);
    lcd.print("Cap: ");
    lcd.print((long)microFarads);
    lcd.print(" uF");
  } else {
    nanoFarads = microFarads * 1000.0;      
    Serial.print((long)nanoFarads);         
    Serial.println(" nanoFarads");          
    lcd.setCursor(0, 1);
    lcd.print("Cap: ");
    lcd.print((long)nanoFarads);
    lcd.print(" nF");
  }
}

void measureVoltage() {
  Serial.println("Voltage mode");
  lcd.print("Voltage mode");
  digitalWrite(7, LOW);
  digitalWrite(6, LOW);
  delay(1000);
  Serial.println(analogRead(A2));
  voltage = voltage_sensor_multiplication_factor * analogRead(A2) *9.170;
  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.println(" V");
  lcd.setCursor(0, 1);
  lcd.print("Volt: ");
  lcd.print(voltage);
  lcd.print(" V");
}

void measureCurrent() {
  Serial.println("Current mode");
  lcd.print("Current mode");
  digitalWrite(7, LOW);
  digitalWrite(6, LOW);

  current = voltage_sensor_multiplication_factor * (analogRead(A4) - analogRead(A3)) / 3.3;
  
  Serial.print("Current: ");
  if (current >= 1) {
    Serial.print(current);
    Serial.println(" A");
    lcd.setCursor(0, 1);
    lcd.print("Curr: ");
    lcd.print(current);
    lcd.print(" A");
  } else {
    Serial.print(current * 1000);
    Serial.println(" mA");
    lcd.setCursor(0, 1);
    lcd.print("Curr: ");
    lcd.print(current * 1000);
    lcd.print(" mA");
  }
}

int see_mode() {
  int pin8 = digitalRead(2);
  int pin9 = digitalRead(9);
  int pin10 = digitalRead(10);
  int pin13 = digitalRead(13);
  int highCount = pin8 + pin9 + pin10 + pin13;
  Serial.print("Count=");Serial.println(highCount);

  if (highCount != 1) {
    if (highCount!=4){
      return 5;//be in state u are
    }
    else{
      return 0; // Invalid combination
    }
  }
  if (pin13 == HIGH) {
    return 1; // Mode 1: Resistance
  } else if (pin8 == HIGH) {
    return 2; // Mode 2: Capacitance
  } else if (pin9 == HIGH) {
    return 3; // Mode 3: Voltage
  } else if (pin10 == HIGH) {
    return 4; // Mode 4: Current
  }

  return 0; // Should not reach here
}
