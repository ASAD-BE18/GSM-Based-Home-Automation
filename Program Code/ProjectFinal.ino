#include<SoftwareSerial.h>
#include <PCD8544.h>
#include <DHT.h>
#define DHTPIN 13
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino
PCD8544 lcd;



SoftwareSerial GSM(8,7);

char inchar; // variable to store the incoming character



//Variables
float hum;  //Stores humidity value
float temp; //Stores temperature value
String Network="";
bool sensorConnected=false;
bool gsmConnected=false;
const int Relays[]={9,10,11,12}; //relays
String Relaystat[4];
  
void setup() {
   GSM.begin(9600);
   Serial.begin(9600);
   lcd.begin(84, 48);
   lcd.clear();
   lcd.println("GSM BASED HOME");
   lcd.setCursor(0,1);
   lcd.println("  Automation");
   lcd.setCursor(0,2);
   lcd.println("    Asad");
   lcd.setCursor(0,3);
   lcd.println(" Shafqatullah");
   dht.begin();
   
   for(int i=0;i<4;i++){
    digitalWrite(Relays[i],HIGH);
    pinMode(Relays[i],OUTPUT);
    Relaystat[i]="R"+String(i)+" OFF\n";
    }
   delay(1000);
   lcd.clear();

   lcd.setCursor(0,0);
   lcd.println("Reading from");
   lcd.setCursor(0,1);
   lcd.println("DHT Senosr");
   //lcd.println("  Please wait   GSM Module    is starting");
   //delay(8000);

  delay(100);
  lcd.clear();
  lcd.setCursor(0,3);
  for(int i=0;i<4;i++){
    
    lcd.print(Relaystat[i]);
    
    }
   
}
void loop() {
   //lcd.setCursor(0,0);
   //lcd.println("Carrier: ");
   //lcd.println(Network);
   if(gsmConnected){
     checkSMS();
    }
    else{
      gsmStart();
      }
   readSensor();
  
   
   delay(500);
 }


String getNetworkCarrier()
{
  String modemResponse="";
  GSM.println("AT+COPS?");
  delay(2000);
   while(GSM.available()){
          inchar=GSM.read();
          modemResponse+=inchar;
    }
  
  // Parse and check response
  char res_to_split[modemResponse.length()];
  modemResponse.toCharArray(res_to_split, modemResponse.length());
  if(strstr(res_to_split,"ERROR") == NULL){
    // Tokenizer
    char *ptr_token;
    ptr_token = strtok(res_to_split, "\"");
    ptr_token = strtok(NULL, "\"");
    String final_result = ptr_token;
    return final_result;
  }else{
    return String(NULL);
  }
}

String getSignalStrength()
{
  String modemResponse="";
  GSM.println("AT+CSQ");
   delay(2000);
   while(GSM.available()){
          inchar=GSM.read();
          modemResponse+=inchar;
    }
  char res_to_split[modemResponse.length()];
  modemResponse.toCharArray(res_to_split, modemResponse.length());
  if((strstr(res_to_split,"ERROR") == NULL) | (strstr(res_to_split,"99") == NULL)){
    // Tokenizer
    char *ptr_token;
    ptr_token = strtok(res_to_split, ":");
    ptr_token = strtok(NULL, ":");
    ptr_token = strtok(ptr_token, ",");
    String final_result = ptr_token;
    final_result.trim();
    return final_result;
  }else{
    return String(NULL);
  }
}

void readSensor(){
  // lcd.clear();
   hum = dht.readHumidity();
   temp = dht.readTemperature();         //Reading the temperature in degrees
  
    if (isnan(hum) || isnan(temp)) {      //Checking if the arduino have recieved the values or not
      lcd.setCursor(0,0);
      lcd.println("Failed to read from DHT sensor!");
      delay(500);
      sensorConnected=false;
       }
    else{
      sensorConnected=true;
      }
  if(sensorConnected){
  lcd.setCursor(0,0);
  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.print(" *C ");
 
  lcd.setCursor(0, 1);
  lcd.print("Humid: ");
  lcd.print(hum);
  lcd.print(" %\t");
  }
 }

void gsmStart(){
    char check="";
    GSM.println("AT");
    delay(500);
    
    check=GSM.read();
    Serial.println(check);
   if(check=='O'){ 
   Network=getNetworkCarrier(); //get network carrier
   lcd.clear();
   lcd.println("GSM Module");
   lcd.setCursor(0,1);
   lcd.println(" is now ");
   lcd.setCursor(0,2);
   lcd.println("Connected");
   delay(1000);
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.println("Carrier: ");
   lcd.setCursor(0,1);
   lcd.println(Network);
   Network=getSignalStrength(); //get signal strength
   lcd.setCursor(0,2);
   lcd.println("Signal: ");
   lcd.println(Network);
   delay(1000);
   GSM.println("AT+CMGF=1"); // set mode to text
    delay(100);
  GSM.println("AT+CNMI=2,2,0,0,0"); // just to get a notification when SMS arrives &direct out SMS upon receipt to the GSM serial out
  gsmConnected=true;
  lcd.clear();
  lcd.setCursor(0,3);
  for(int i=0;i<4;i++){
    
    lcd.print(Relaystat[i]);
    
        }
     } 
   
 }

  void checkSMS(){

     String num="";
     int i=0;
     while(GSM.available()){
  
    inchar=GSM.read();
    Serial.println(inchar);
    if(isdigit(inchar) && ++i<13){
    num+=inchar;
    }
    if (inchar=='$'){

        delay(10);

        inchar=GSM.read();
        Serial.println(inchar);
        
        if(isdigit(inchar)){
          int x=0;
          if(inchar=='0')
          x=0;
          else
           x = inchar - '0';
          if( (x>=0) && (x<4)){

              inchar=GSM.read();
              Serial.println(inchar);
              if (inchar=='0'){

              digitalWrite(Relays[x], HIGH);
              Relaystat[x]="R"+String(x)+" OFF\n";
              lcd.setCursor(0,3);
              for(int j=0;j<4;j++){
    
                           lcd.print(Relaystat[j]);
    
                         }
              sendMessage("Success",num);

               }

           else if (inchar=='1'){
    
               digitalWrite(Relays[x], LOW);
               Relaystat[x]="R"+String(x)+" ON\n";
                lcd.setCursor(0,3);
                for(int j=0;j<4;j++){
    
                          lcd.print(Relaystat[j]);
    
                           }
               sendMessage("Success",num);
                              }
                }
                else if(x==4){
               inchar=GSM.read();   
               if (inchar=='0'){
              for(int j=0;j<4;j++){
              digitalWrite(Relays[j], HIGH);
              Relaystat[j]="R"+String(j)+" OFF\n";
              delay(500);
              }
              lcd.setCursor(0,3);
              for(int j=0;j<4;j++){
    
                           lcd.print(Relaystat[j]);
    
                         }
              sendMessage("Success",num);

               }

           else if (inchar=='1'){

                
               for(int j=0;j<4;j++){
               digitalWrite(Relays[j], LOW);
               Relaystat[j]="R"+String(j)+" ON\n";
               delay(500);
               }
                lcd.setCursor(0,3);
                for(int j=0;j<4;j++){
    
                          lcd.print(Relaystat[j]);
    
                           }
               sendMessage("Success",num);
                              }
                  
                  }
            
            }
            else if(inchar=='a'){
              String msg="";
              if(sensorConnected){
                
                msg="Temperature: "+String(temp)+"C\nHumidity: "+String(hum)+"%\n";
               // sendMessage(msg,num);
                }
                else{
                  msg="Failed to read from Sensor\n";
                 }
                 
                 for(int j=0;j<4;j++){
                 msg+=Relaystat[j];
                 Serial.println(msg);
                 }
                 sendMessage(msg,num);
               }
     delay(100);

    GSM.println("AT+CMGD=1,4"); // delete all SMS

    delay(1000);
          }
      }
      Serial.println(num);
         
 }
 void sendMessage(String message,String number){

  String command="";
    
  
  command="AT+CMGS=\"+"+number+"\"";
  GSM.println(command);
  delay(100);
  Serial.println(GSM.readString());
  GSM.println(message);
  delay(100);
  Serial.println(GSM.readString());
  GSM.println((char)26);
  delay(100);
  Serial.println(GSM.readString());
  GSM.println();
  delay(1000);
  GSM.println("AT+CNMI=2,2,0,0,0");
  delay(100);
  Serial.println(GSM.readString());    
  }
