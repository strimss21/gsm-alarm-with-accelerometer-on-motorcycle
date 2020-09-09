#include <SoftwareSerial.h>
#include<Wire.h>
SoftwareSerial mySerial(10,11);  // (Rx,Tx  > Tx,Rx)

String numberForSms = "+71111111111";
boolean status = false;
boolean smsSent = false;

char incomingByte; 
String inputString;

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX;
int gyroValue;

void setup(){
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // PWR_MGMT_1 register
  Wire.write(0);     // set to zero (wakes up the MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
     
  delay(2000);                            
  mySerial.begin(9600);
  mySerial.println("AT+CMGF=1");        //For text format        
  delay(1000);                             
  mySerial.println("AT+CSCS=\"GSM\"");  // Encoding in ASCII
  delay(1000); 
  mySerial.println("AT+GSMBUSY=0");     // Disable incoming calls
  delay(1000);
  mySerial.println("AT+CSCB=1");       // Disable receiving special messages
  delay(1000);
  mySerial.println("AT+CNMI=1,2,0,0,0"); // Processing new messages  
  mySerial.println("AT+CMGL=\"REC UNREAD\""); // Read unread messages
}

void loop(){
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  
  Serial.print("AcX = "); Serial.println(AcX);
  gyroValue = AcX;
  delay(1000);
  readSms();
  if(status == true){
   if(gyroValue > 1000 && smsSent==false){
    sendSms("Alarm", "+71111111111");
    smsSent = true;}}}

/* Send SMS */
void sendSms(String text, String phone) {
  mySerial.println("AT+CMGS=\"" + phone + "\""); 
  delay(1000);
  mySerial.print(text);                         
  delay(1000);
  mySerial.print((char)26);       
  delay(3000);  
  Serial.print("SMS sent");}
  
/* Read SMS */
void readSms(){
 if(mySerial.available()){
  delay(300);
  while(mySerial.available()){
  incomingByte = mySerial.read();
  inputString += incomingByte;}
  
  delay(10);      
  Serial.println(inputString);
  if(inputString.indexOf("true") > -1){
   status = true; 
   smsSent = false;
   sendSms("The motorcycle is under protection!", "+71111111111");} 
   
  if (inputString.indexOf("false") > -1){
    status = false;
    sendSms("Not protected", "+71111111111");}}
  mySerial.println("AT+CMGDA=\"DEL ALL\"");
}
