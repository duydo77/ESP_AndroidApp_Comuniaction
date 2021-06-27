#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>

StaticJsonBuffer<256> jb;
JsonObject& JObj = jb.createObject();

String Byte;
float nhietdo, doam; 
int stmCnt = 0, mod = 0; 
int guiCnt= 0, guiStt = 0;
uint8_t Tx[4];

void Controls(void);
 
void setup() 
{     
    pinMode(BUILTIN_LED, OUTPUT);
    WiFi.disconnect();
    WiFi.begin("duy", "123456789");
    Serial.begin(115200);
    Serial.setTimeout(10);
    while(WiFi.waitForConnectResult() != WL_CONNECTED)
    {
       digitalWrite(BUILTIN_LED, !digitalRead(2));
       delay(100);
    }
    Firebase.begin("doluongdkmt-a6a4f.firebaseio.com");
    //Firebase.stream("/Controls");
    digitalWrite(BUILTIN_LED, LOW);
}


void loop(){
  Controls();
  delay(100);
  if (Serial.available()>0) 
  {
      Byte = Serial.readString();
      if(Byte[0] == 's')
      {   
          nhietdo = float(Byte[5]) + float(Byte[7]&0x7F)/10;
          doam = float(Byte[1]) + float(Byte[3])/10;
          JObj["Temp"] = String(nhietdo);
          JObj["Humid"] = String(doam);
          //Firebase.setFloat("/Status/Humid", doam);
          JObj["D1"] = (Byte[8] & 0x04)?"1":"0";
          JObj["D2"] = (Byte[8] & 0x08)?"1":"0";
          JObj["D3"] = (Byte[8] & 0x010)?"1":"0";
          JObj["Gas"] = (Byte[8] & 0x01)?"0":"1";
          JObj["Motion"] = (Byte[8] & 0x02)?"1":"0";
          JObj["Fan"] = (int)(Byte[9]);
          JObj["Runing"] = (Firebase.getInt("/Controls/Runing"));
          Firebase.set("Status", JObj);
          guiCnt ++;
          if (guiCnt == 4)
          {
            guiStt = (guiStt)?0:1;
            Firebase.setInt("/Status/Runing",guiStt);
            guiCnt = 0;
          }
          //Firebase.setString("/Status/Fan", Byte[9]);
//          Firebase.setString("/Status/D1", (Byte[8] & 0x04)?"1":"0");
//          Firebase.setString("/Status/D2", (Byte[8] & 0x08)?"1":"0");
//          Firebase.setString("/Status/D3", (Byte[8] & 0x10)?"1":"0");
//          Firebase.setString("/Status/Gas", (Byte[8] & 0x01)?"1":"0");
//          Firebase.setString("/Status/Motion", (Byte[8] & 0x02)?"1":"0");
          
      }
  }
}

void Controls(void)
{ 
  Tx[0] = 'C';
  Tx[1] = 0;  
  FirebaseObject FObj = Firebase.get("/Controls");
  if(FObj.getString("Mode") == "1") Tx[1] = Tx[1] | 0x01;
  if(FObj.getString("D1") == "1") Tx[1] = Tx[1] | 0x04;
  if(FObj.getString("D2") == "1") Tx[1] = Tx[1] | 0x08;
  if(FObj.getString("D3") == "1") Tx[1] = Tx[1] | 0x02;
  Tx[1] = (byte)((uint8_t)Tx[1]);
  Tx[2] = (byte)((FObj.getString("Fan")).toInt());
  Tx[3] = 'E';
  Serial.write(Tx, 4); 
}

  
