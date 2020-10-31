# 1 "C:\\Users\\2013b\\AppData\\Local\\Temp\\tmp9dpr4qc5"
#include <Arduino.h>
# 1 "C:/Users/2013b/Documents/Arduino/FancyWatchOS/src/main.ino"





#include "config.h"
#include "display.h"
#include "kernel.h"
#include "utils.h"


Kernel* kernel;


QueueHandle_t events;
bool powerIRQ = false;
void setup();
void InitInterrupts(TTGOClass* device);
void loop();
#line 18 "C:/Users/2013b/Documents/Arduino/FancyWatchOS/src/main.ino"
void setup()
{

#ifdef LOG_SERIAL
    Serial.begin(9600);
#endif



    events = xQueueCreate(256, sizeof(Event));


    kernel = new Kernel(TTGOClass::getWatch(), events);

    InitInterrupts(kernel->GetDriver());


    kernel->GetDisplay()->Enable();
    kernel->GetDisplay()->GetTFT()->setTextColor(TFT_GREEN);

}

void InitInterrupts(TTGOClass* device)
{



    pinMode(AXP202_INT, INPUT_PULLUP);
    attachInterrupt(AXP202_INT, [] { powerIRQ = true; }, FALLING);
    device->power->enableIRQ(AXP202_PEK_SHORTPRESS_IRQ | AXP202_VBUS_REMOVED_IRQ | AXP202_VBUS_CONNECT_IRQ | AXP202_CHARGING_IRQ, true);
    device->power->clearIRQ();
}

void loop()
{





    if (powerIRQ)
    {
        AXP20X_Class* power = kernel->GetDriver()->power;
        power->readIRQ();

        Event e;
        if (power->isVbusPlugInIRQ())
        {
            e.type = EVENT_POWER_CONNECT;
            Log("Power connect!");
        }
        else if (power->isVbusRemoveIRQ())
        {
            e.type = EVENT_POWER_DISCONNECT;
            Log("Power disconnect!");
        }
        else if (power->isPEKShortPressIRQ())
        {
            e.type = EVENT_POWER_BUTTON;
            Log("Power button!");
        }
        else
        {
            e.type = EVENT_POWER_CHARGE;
            Log("Power charge!");
        }
        power->clearIRQ();

        xQueueSend(events, &e, portMAX_DELAY);

        powerIRQ = false;
    }


    kernel->Update();
}