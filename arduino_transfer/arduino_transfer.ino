/*
  Software serial multple serial test

 Receives from the two software serial ports,
 sends to the hardware serial port.

 In order to listen on a software port, you call port.listen().
 When using two software serial ports, you have to switch ports
 by listen()ing on each one in turn. Pick a logical time to switch
 ports, like the end of an expected transmission, or when the
 buffer is empty. This example switches ports when there is nothing
 more to read from a port

 The circuit:
 Two devices which communicate serially are needed.
 * First serial device's TX attached to digital pin 10(RX), RX to pin 11(TX)
 * Second serial device's TX attached to digital pin 8(RX), RX to pin 9(TX)

 Note:
 Not all pins on the Mega and Mega 2560 support change interrupts,
 so only the following can be used for RX:
 10, 11, 12, 13, 50, 51, 52, 53, 62, 63, 64, 65, 66, 67, 68, 69

 Not all pins on the Leonardo support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 created 18 Apr. 2011
 modified 19 March 2016
 by Tom Igoe
 based on Mikal Hart's twoPortRXExample

 This example code is in the public domain.

 */

#include <SoftwareSerial.h>

uint8_t Rev_Buf[15] = {0};
const unsigned long aaEvery = 20;
unsigned long aaUp;
// software serial #1: RX = digital pin 10, TX = digital pin 11
SoftwareSerial portOne(10, 11);

uint8_t Sum_Verify (uint8_t *PBuf, uint8_t Len)
{
    uint8_t Verify1 = 0;
    while (Len--)
    {
        Verify1 = Verify1 + (*PBuf);
        PBuf ++;
    }
    return (Verify1 == 0);
}

void setup()
{
    // Open serial communications and wait for port to open:
    Serial.begin(115200);

    while (!Serial)
    {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    aaUp = millis( );

    // Start each software serial port
    portOne.begin(1000);
}

void loop()
{
    uint8_t verify = 0, Rev_SBUF_Buf = 0;
    static uint8_t flag_frame_head_h = 0, Rev_Data_Count = 0, flag_Uart_Rev_Work = 0;
    static uint8_t Recv_Timeout = 11;
    static uint8_t flag_Rev_End = 0, Rev_Frame_Len = 0;

    // By default, the last intialized port is listening.
    // when you want to listen on a port, explicitly select it:
    portOne.listen();
    // while there is data coming in, read it
    // and send to the hardware serial port:
    while (portOne.available() > 0)
    {
        Rev_SBUF_Buf = portOne.read();
        flag_Uart_Rev_Work = 1;
        if(!flag_frame_head_h)
        {
            if(Rev_SBUF_Buf == 0xA5)
            {
                flag_frame_head_h = 1;
                Rev_Buf[0] = 0xA5;
                Recv_Timeout = 11;
            }
        }
        else
        {
            Rev_Data_Count++;
            Rev_Buf[Rev_Data_Count] = Rev_SBUF_Buf;
            Rev_Frame_Len = 9;
            if(Rev_Data_Count >= 9)
            {
                Rev_Data_Count = 0;
                flag_frame_head_h = 0;
                flag_Uart_Rev_Work = 0;
                flag_Rev_End = 1;
            }
        }
        //Serial.print(flag_frame_head_h);
        //Serial.write(inByte);
    }


    if(millis( ) - aaUp > aaEvery)
    {
        aaUp = millis( );
        if(Recv_Timeout > 0) Recv_Timeout--;
        else flag_frame_head_h = 0;
    }


    if(flag_Rev_End)
    {
        flag_Rev_End = 0;

        verify = Sum_Verify(Rev_Buf, Rev_Frame_Len + 1);
        if(verify)
        {
            for(int i = 0; i < 10; i++)
            {
                Serial.print(Rev_Buf[i]);
                Serial.print(",");
                if(Rev_Buf[i] > 99)
                {
                    Serial.print("  ");
                }
                else if(Rev_Buf[i] > 9)
                {
                    Serial.print("    ");
                }
                else
                {
                    Serial.print("      ");
                }


            }
            Serial.println("");
        }
    }
}
