#include <lorawan.h>
#define SENSOR 33 // define pin 2 for sensor
#define ACTION 25 // define pin 9 as for ACTION
#define ACTION2 12

//ABP Credentials
const char *devAddr = "a425af65";
const char *nwkSKey = "9bee5e21c02b63e10000000000000000";
const char *appSKey = "0000000000000000a2b0191571086872";


const unsigned long interval = 10000;    // 10 s interval to send message
unsigned long previousMillis = 0;  // will store last time message sent
unsigned int counter = 0;     // message counter
String dataSend = "";

char myStr[50];
byte outStr[255];
byte recvStatus = 0;
int port, channel, freq;
bool newmessage = false;
int objectCount = 0;
bool lampStatus = false; // false: OFF, true: ON


const sRFM_pins RFM_pins = {
  .CS = 5,
  .RST = 0,
  .DIO0 = 27,
  .DIO1 = 2,
};

void setup() {
  // Setup loraid access
  Serial.begin(9600);
  delay(2000);
  if (!lora.init()) {
    Serial.println("RFM95 not detected");
    delay(5000);
    return;
  }

  // Set LoRaWAN Class change CLASS_A or CLASS_C
  lora.setDeviceClass(CLASS_A);

  // Set Data Rate
  lora.setDataRate(SF10BW125);

  // Set FramePort Tx
  lora.setFramePortTx(5);

  // set channel to random
  lora.setChannel(MULTI);

  // Set TxPower to 15 dBi (max)
  lora.setTxPower(15);

  // Put ABP Key and DevAddress here
  lora.setNwkSKey(nwkSKey);
  lora.setAppSKey(appSKey);
  lora.setDevAddr(devAddr);
}

void loop() {
  lampu();
  // Check interval overflow
  if (millis() - previousMillis > interval) {
    previousMillis = millis(); 
    int l;
    l = objectCount;
    dataSend = "{\"Lampu\": " + String(lampStatus ? "ON" : "OFF") + "}";
    dataSend.toCharArray(myStr, 50);

    Serial.print("Sending: ");
    Serial.println(dataSend);
    lora.sendUplink(myStr, strlen(myStr), 0);
    port = lora.getFramePortTx();
    channel = lora.getChannel();
    freq = lora.getChannelFreq(channel);
    Serial.print(F("fport: "));    Serial.print(port);Serial.print(" ");
    Serial.print(F("Ch: "));    Serial.print(channel);Serial.print(" ");
    Serial.print(F("Freq: "));    Serial.print(freq);Serial.println(" ");
  }


  // Check Lora RX
  lora.update();

  recvStatus = lora.readDataByte(outStr);
  if (recvStatus) {
    newmessage = true;
    int counter = 0;
    port = lora.getFramePortRx();
    channel = lora.getChannelRx();
    freq = lora.getChannelRxFreq(channel);

    for (int i = 0; i < recvStatus; i++)
    {
      if (((outStr[i] >= 32) && (outStr[i] <= 126)) || (outStr[i] == 10) || (outStr[i] == 13))
        counter++;
    }
    if (port != 0)
    {
      if (counter == recvStatus)
      {
        Serial.print(F("Received String : "));
        for (int i = 0; i < recvStatus; i++)
        {
          Serial.print(char(outStr[i]));
        }
      }
      else
      {
        Serial.print(F("Received Hex: "));
        for (int i = 0; i < recvStatus; i++)
        {
          Serial.print(outStr[i], HEX); Serial.print(" ");
        }
      }
      Serial.println();
      Serial.print(F("fport: "));    Serial.print(port);Serial.print(" ");
      Serial.print(F("Ch: "));    Serial.print(channel);Serial.print(" ");
      Serial.print(F("Freq: "));    Serial.println(freq);Serial.println(" ");
    }
    else
    {
      Serial.print(F("Received Mac Cmd : "));
      for (int i = 0; i < recvStatus; i++)
      {
        Serial.print(outStr[i], HEX); Serial.print(" ");
      }
      Serial.println();
      Serial.print(F("fport: "));    Serial.print(port);Serial.print(" ");
      Serial.print(F("Ch: "));    Serial.print(channel);Serial.print(" ");
      Serial.print(F("Freq: "));    Serial.println(freq);Serial.println(" ");
    }
  }

}


void lampu() {
  int L = digitalRead(SENSOR); // read the sensor

  if (L == 0) {
    objectCount++; // increment the object count
    Serial.print("Object detected. Count: ");
    Serial.println(objectCount);

    // Check if the object count is odd or even
    if (objectCount % 2 == 1) {
      digitalWrite(ACTION, HIGH); // turn on the action
      lampStatus = true;
      Serial.println("Action: ON");
      
    } else {
      digitalWrite(ACTION, LOW); // turn off the action
      Serial.println("Action: OFF");
      lampStatus = false;
    }

    delay(2000); // delay to avoid counting multiple objects

    // Reset object count to 0 after reaching 20
    if (objectCount == 20) {
      objectCount = 0;
    }
  }

  delay(100); // add a small delay before the next iteration
}
