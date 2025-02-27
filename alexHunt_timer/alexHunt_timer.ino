// Alex Eugene Hunt
// Completed July 2022
// (SELF DESIGNED & CODED) 
// Remote Controlled Timer w/ Musical Sound, LCD Display, & RGB Lights.

// Arduino 1.8.19

#include <IRremote.h> // (Version 3.7.1) w/ the timer changed to 1 in the [IRremote-3.7.1/src/private/IRTimer.hpp] file, line 73. https://www.arduinolibraries.info/libraries/i-rremote
#include <LiquidCrystal.h>

// to initialize the ir and lcd (based from my setup of connected pins) 
const int IRpin  = 5;
const int rs  = 7;
const int en  = 8;
const int d4  = 9;
const int d5  = 10;
const int d6  = 11;
const int d7  = 12;
IRrecv IR(IRpin);
LiquidCrystal lcd(rs,en,d4,d5,d6,d7);
// pin initialization
const int redPin  = 2;
const int bluPin  = 4;
const int buzzPin = 3;
const int con     = 6;  // contrast
const int bac     = 13; // backlight
// global variables
int h;
int m;
int s;
int hAmt;
int mAmt;
int sAmt;
int userCol     = 1;
int userVolume  = 5;
int fillAmount;
int NOTE_B3   = 247;
int NOTE_C4   = 262;
int NOTE_D4   = 294;
int NOTE_E4   = 330;
int NOTE_G4   = 392;
int NOTE_A4   = 440;
int NOTE_AS4  = 466;
int NOTE_C5   = 523;
int NOTE_CS5  = 554;
bool isOn       = false;
bool stopBut    = false;
bool alarmOn    = false;
bool isPaused   = true;
bool isCounting = false;
String button;
String runningValue;
String repeatValue;
String zeroFill;
String hFin;
String mFin;
String sFin;
String none = "";
unsigned long t_end;
unsigned long t_start;
unsigned long t_total;
unsigned long t_sPause;
unsigned long t_ePause;
unsigned long t_sElaps;

void setup() {
  // color & sound
  pinMode(redPin, OUTPUT);
  pinMode(bluPin, OUTPUT);
  pinMode(buzzPin, OUTPUT);
  // lcd
  lcd.begin(16,2);
  pinMode(con,OUTPUT);
  pinMode(bac,OUTPUT);
  digitalWrite(con,LOW);
  digitalWrite(bac,LOW);
  // ir pin
  IR.enableIRIn();
}

void loop() {
  // if the alarm is now on, sound the alarm!
  if (alarmOn == true){ 
    alarm();
  }
  // this chunk of code will turn off the system after 7.5 seconds has passed if no button has been pressed and the system is on
  t_end = 7500;
  t_start = millis();
  while(!IR.decode()){
    if (millis() -  t_start >= t_end && isOn == true && isCounting == false){
      powerButton();
    }
  }
  // if a button is pressed, get the value of the button and call the appropriate function
  button = getButton(IR.decodedIRData.command);
  if (button == "POWER") { // to turn the system on and off
    IR.resume();
    powerButton();
  }
  if (isOn){
    if (button == "EQ") { // this is the "clear" button
      eqButton();
    }
    else if (button == "UP" || button == "DOWN") { // this will change the color of the LED
      colorButton(button, true, true);
      delay(75);
      colorOff();
    }
    else if (button == "ST/REPT") { // this will repeat the previous alarm
      repeatButton();
    }
    else if (button == "FUNC/STOP") { // this stops the triggered alarm
      stopButton();
    }
    else if (button == "VOLUME UP" || button == "VOLUME DOWN") { // this will turn the volume up or down
      volumeButton(button);
    }
    else if (button == "PAUSE/PLAY") { // this will start the timer
      playButton();
    }
    else if (button == "0" || button == "1" || button == "2" || button == "3" || button == "4" || button == "5" || button == "6" || button == "7" || button == "8" || button == "9") { // the numbers pressed on the remote
      displayNumbers(button, false);
    }
  }
  delay(175);
  IR.resume();
}
// this funcion will turn on or off the arduino
void powerButton() {
  if (!isOn) {
    isOn = true;
    volumeButton(none);
    colorButton(none, true, false);
    displayNumbers("", false);
    analogWrite(con,100);
    digitalWrite(bac,HIGH);
  }
  else {
    isOn = false;
    runningValue = "";
    lcd.clear();
    digitalWrite(bac,LOW);
    digitalWrite(con,LOW);
  }
}
// this function will clear the user input
void eqButton() {
  repeatValue = runningValue;
  runningValue = "";
  displayNumbers("", false);
}
// this function will display the color selected and turn on the coresponding LED
void colorButton(String upOrDown, bool colorName, bool colorBlink) {
  if (upOrDown == "UP"){
    userCol++;
  }
  else if (upOrDown == "DOWN"){
    userCol--;
  }
  lcd.setCursor(8, 1);
  if (userCol % 3 == 0){
    if (colorName)
      lcd.print("LCD RED ");
    if (colorBlink)
      digitalWrite(redPin,HIGH);
  }
  else if (userCol % 3 == 1 || userCol % 3 == -1){
    if (colorName)
      lcd.print("LCD BLUE");
    if (colorBlink)
      digitalWrite(bluPin,HIGH);
  }
  else if (userCol % 3 == 2 || userCol % 3 == -2){
    if (colorName)
      lcd.print("LCD PRPL");
    if (colorBlink){
      digitalWrite(redPin,HIGH);
      digitalWrite(bluPin,HIGH);
    }
  }
}
// this function will turn off the LED
void colorOff(){
  digitalWrite(redPin, LOW);
  digitalWrite(bluPin, LOW);
}
// this function will repeat the previous time input
void repeatButton() {
  runningValue = repeatValue;
  displayNumbers(repeatValue, true);
}
// this function will stop the alarm and turn off the LED & sound
void stopButton() {
  alarmOn = false;
  stopBut = true;
  noTone(buzzPin);
  colorOff();
}
// this function will change the sound and display the selected sound
void volumeButton(String upOrDownV) {
  if (upOrDownV == "VOLUME UP" && userVolume < 10){
    userVolume++;
  }
  else if (upOrDownV == "VOLUME DOWN" && userVolume > 0){
    userVolume--;
  }
  lcd.setCursor(0, 1);
  if (userVolume == 0){
    lcd.print("MUTE  ");
  }
  else if (userVolume < 8){
    lcd.print("VOL " + String(userVolume) + " ");
  }
  else if (userVolume == 8){
    lcd.print("TUNE 1");
  }
  else if (userVolume == 9){
    lcd.print("TUNE 2");
  }
  else if (userVolume == 10){
    lcd.print("TUNE 3");
  }
}
// this function will handle the pause and play aspects of the timer
void playButton() {
  if (isPaused){
    timer(hAmt, mAmt, sAmt);
  }
  else {
    IR.resume();
    while (!isPaused) {
      if (IrReceiver.decode()){
        if (getButton(IR.decodedIRData.command) == "PAUSE/PLAY"){
          isPaused = true;
        }
      }
    }
  }
}
// this function displays the numbers on the screen
void displayNumbers(String newNumbers, bool repeat) {
  lcd.setCursor(0,0);
  if (!isCounting && !repeat){ // string, false, false
    if ((runningValue + newNumbers).length() <= 6) {
      lcd.print(getTime(runningValue += newNumbers));
    }
  }
  else if (!isCounting && repeat) { // string, null, true
    lcd.print(getTime(newNumbers));
  }
  else if (isCounting) {
    lcd.print(getTime(newNumbers));
  }
}
// this function will sound the unique alarm with the coresponding selection
void alarm() {
  while (alarmOn){
    colorButton(none, false, true);
    if (userVolume == 1){
      analogWrite(buzzPin, 1);
      delay(350);
    }
    else if (userVolume == 2){
      analogWrite(buzzPin, 2);
      delay(350);
    }
    else if (userVolume == 3){
      analogWrite(buzzPin, 3);
      delay(350);
    }
    else if (userVolume == 4){
      analogWrite(buzzPin, 4);
      delay(350);
    }
    else if (userVolume == 5){
      analogWrite(buzzPin, 5);
      delay(350);
    }
    else if (userVolume == 6){
      analogWrite(buzzPin, 6);
      delay(350);
    }
    else if (userVolume == 7){
      analogWrite(buzzPin, 7);
      delay(350);
    }
    else if (userVolume == 8){
      playTune1();
    }
    else if (userVolume == 9){
      playTune2();
    }
    else if (userVolume == 10){
      playTune3();
    }
    colorOff();
    noTone(buzzPin);
    delay(350);
    if (IR.decode()){
      if(getButton(IR.decodedIRData.command) == "FUNC/STOP") {
        stopButton();
      }
    }
    IR.resume();
  }
}
// this is the time of the timer, counting up and subtracting 
void timer(unsigned long numHour, unsigned long numMins, unsigned long numSecs) {
  isCounting = true;
  t_total = (numHour * 3600000) + (numMins * 60000) + (numSecs * 1000);
  t_start = millis();
  while (millis() -  t_start <= t_total){
    if (IR.decode()){
      button = getButton(IR.decodedIRData.command);
      if (button == "PAUSE/PLAY"){
        t_sPause = millis();
        isPaused = false;
        playButton();
        t_ePause = millis() - t_sPause;
        t_total += t_ePause;
      }
      else if (button == "FAST FOWARD" && t_total > 20000){
        t_total -= 5000;
      }
      else if (button == "REWIND" && t_total < 7000000000){
        t_total += 5000;
      }
      else if (button == "VOLUME UP" || button == "VOLUME DOWN"){
        volumeButton(button);
      }
      else if (button == "UP" || button == "DOWN"){
        colorButton(button, true, false);
      }
    }
    IR.resume();
    if ((millis() -  t_start) % 1000 <= 5){
      t_sElaps = (millis() -  t_start) / 1000;
      displayNumbers(String(((t_total / 1000) - t_sElaps)), false);
    }
  }
  eqButton();
  alarmOn = true;
  isCounting = false;
}
// this returns the remote button with the ir return value
String getButton(int input){
  switch(input) {
    case 22:
      return "0";
    case 12:
      return "1";
    case 24:
      return "2";
    case 94:
      return "3";
    case 8:
      return "4";
    case 28:
      return "5";
    case 90:
      return "6";
    case 66:
      return "7";
    case 82:
      return "8";
    case 74:
      return "9";
    case 69:
      return "POWER";
    case 70:
      return "VOLUME UP";
    case 71:
      return "FUNC/STOP";
    case 68:
      return "REWIND";
    case 64:
      return "PAUSE/PLAY";
    case 67:
      return "FAST FOWARD";
    case 7:
      return "DOWN";
    case 21:
      return "VOLUME DOWN";
    case 9:
      return "UP";
    case 25:
      return "EQ";
    case 13:
      return "ST/REPT";
    default:
      return "";
  }
}
// this function formats ms into h, m, s
String getTime(String inputTime) {
  fillAmount = 6 - inputTime.length();
  zeroFill = "";
  for (int i = 0; i < fillAmount; i++){
    zeroFill += "0";
  }
  inputTime = zeroFill + inputTime;
  if (isCounting) {
    hAmt = inputTime.toInt() / 3600;
    mAmt = (inputTime.toInt() % 3600) / 60;
    sAmt = (inputTime.toInt() % 3600) % 60;
  }
  else {
    hAmt = (String(inputTime[0]) + String(inputTime[1])).toInt();
    mAmt = (String(inputTime[2]) + String(inputTime[3])).toInt();
    sAmt = (String(inputTime[4]) + String(inputTime[5])).toInt();
  }
  if (hAmt < 10) {
    hFin = "0" + String(hAmt) + "H ";
  }
  else {
    hFin = String(hAmt) + "H ";
  }
  if (mAmt < 10) {
    mFin = "0" + String(mAmt) + "M ";
  }
  else {
    mFin = String(mAmt) + "M ";
  }
  if (sAmt < 10) {
    sFin = "0" + String(sAmt) + "S";
  }
  else {
    sFin = String(sAmt) + "S";
  }
  return hFin + mFin + sFin;
}
// seven nation army by the white stripes
void playTune1() {
  tone(buzzPin, NOTE_E4);
  delay(372);
  noTone(buzzPin);
  delay(3);
  tone(buzzPin, NOTE_E4);
  delay(125);
  noTone(buzzPin);
  colorOff();
  tone(buzzPin, NOTE_G4);
  delay(187.5);
  noTone(buzzPin);
  tone(buzzPin, NOTE_E4);
  delay(62.5);
  noTone(buzzPin);
  delay(125);
  tone(buzzPin, NOTE_D4);
  delay(125);
  noTone(buzzPin);
  colorButton(none,false,true);
  tone(buzzPin, NOTE_C4);
  delay(500);
  noTone(buzzPin);
  colorOff();
  tone(buzzPin, NOTE_B3);
  delay(150);
  colorButton(none,false,true);
}
// another one bites the dust by queen
void playTune2() {
  tone(buzzPin, NOTE_E4);
  delay(250);
  noTone(buzzPin);
  delay(250);
  colorOff();
  tone(buzzPin, NOTE_E4);
  delay(250);
  noTone(buzzPin);
  delay(250);
  colorButton(none,false,true);
  tone(buzzPin, NOTE_E4);
  delay(250);
  noTone(buzzPin);
  delay(625);
  tone(buzzPin, NOTE_E4);
  delay(122);
  noTone(buzzPin);
  delay(3);
  colorOff();
  tone(buzzPin, NOTE_E4);
  delay(247);
  noTone(buzzPin);
  delay(3);
  tone(buzzPin, NOTE_E4);
  delay(247);
  noTone(buzzPin);
  delay(3);
  colorButton(none,false,true);
  tone(buzzPin, NOTE_G4);
  delay(247);
  noTone(buzzPin);
  delay(3);
  tone(buzzPin, NOTE_E4);
  delay(122);
  noTone(buzzPin);
  delay(3);
  colorOff();
  tone(buzzPin, NOTE_A4);
  delay(125);
  noTone(buzzPin);
  delay(650);
}
// smoke on the water by deep purple
void playTune3() {
  tone(buzzPin, NOTE_G4);
  delay(247);
  noTone(buzzPin);
  delay(3);
  colorOff();
  tone(buzzPin, NOTE_AS4);
  delay(247);
  noTone(buzzPin);
  delay(3);
  colorButton(none,false,true);
  tone(buzzPin, NOTE_C5);
  delay(250);
  noTone(buzzPin);
  delay(125);
  tone(buzzPin, NOTE_G4);
  delay(125);
  noTone(buzzPin);
  delay(125);
  tone(buzzPin, NOTE_AS4);
  delay(125);
  noTone(buzzPin);
  delay(125);
  tone(buzzPin, NOTE_CS5);
  delay(122);
  noTone(buzzPin);
  delay(3);
  colorOff();
  tone(buzzPin, NOTE_C5);
  delay(497);
  noTone(buzzPin);
  delay(3);
  colorButton(none,false,true);
  tone(buzzPin, NOTE_G4);
  delay(247);
  noTone(buzzPin);
  delay(3);
  colorOff();
  tone(buzzPin, NOTE_AS4);
  delay(247);
  noTone(buzzPin);
  delay(3);
  colorButton(none,false,true);
  tone(buzzPin, NOTE_C5);
  delay(250);
  noTone(buzzPin);
  delay(125);
  tone(buzzPin, NOTE_AS4);
  delay(125);
  noTone(buzzPin);
  delay(125);
  colorOff();
  tone(buzzPin, NOTE_G4);
  delay(150);
}
