int AssociateState = 0;            // variable for reading the associate status
const int XB_Program = XBEE_MULT4;

void setup(){
    Serial.begin(115200);              // Serial port for XBEE socket
    while(!Serial);
    SerialUSB.begin(115200);           // Native USB port
    while(!SerialUSB);
    pinMode(XBEE_MULT2, INPUT);      // DIGI Associate PIN
    pinMode(DS6, OUTPUT);            // Green LED
    pinMode(XBEE_RST, OUTPUT);
    digitalWrite(XBEE_RST, HIGH);    // Make sure RESET is HIGH
    pinMode(XB_Program, OUTPUT);
    digitalWrite(XB_Program, HIGH);

    digitalWrite(XBEE_RST, LOW);    // Make sure RESET is LOW to reset the XBEE
    delay(500);
    digitalWrite(XBEE_RST, HIGH);    // Make sure RESET is HIGH
                                    /*
                                    ets Jun  8 2016 00:22:57

                                    rst:0x1 (POWERON_RESET), boot : 0x13 (SPI_FAST_FLASH_BOOT)
                                    configsip : 0, SPIWP : 0x00
                                    clk_drv : 0x00, q_drv : 0x00, d_drv : 0x00, cs0_drv : 0x00, hd_drv : 0x00, wp_drv : 0x00
                                    mode : DIO, clock div : 1
                                    load : 0x3fff0008, len : 8
                                    load : 0x3fff0010, len : 160
                                    load : 0x40078000, len : 10632
                                    load : 0x40080000, len : 252
                                    entry 0x40080034
                                    */

    //digitalWrite(XB_Program, LOW);  // Put the XBEE in programming mode
                                    /*
                                    ets Jun  8 2016 00:22:57

                                    rst:0x1 (POWERON_RESET), boot : 0x3 (DOWNLOAD_BOOT(UART0 / UART1 / SDIO_REI_REO_V2))
                                    waiting for download
                                    */

    pinMode(DS2, OUTPUT);
    digitalWrite(DS2, LOW);          // Turns on Red LED
}

void serialEvent(){
//    if (inBufferptr > 255){
//        inBufferptr = 0;
//    }
    if(Serial.available()){        // read from port 0 XBEE, send to port 1 SerialUSB:
        int inByte = Serial.read();
        SerialUSB.write(inByte);
    }
}

void serialUSBEvent(){
    if(SerialUSB.available()){     // read from port 1 SerialUSB, send to port 0 XBEE:
        int inByte = SerialUSB.read();
        Serial.write(inByte);
    }
}


void loop(){
    AssociateState = digitalRead(XBEE_MULT2);

    if(AssociateState == HIGH){
        digitalWrite(DS6, HIGH);       // turn LED on:
    } else{
        digitalWrite(DS6, LOW);        // turn LED off:
    }
}