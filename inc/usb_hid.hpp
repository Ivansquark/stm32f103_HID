#ifndef USB_HID_HPP
#define USB_HID_HPP

#include "stm32f10x.h"

extern "C" void USB_LP_CAN_RX0_IRQHandler() {
    //pThis->
    
}

class Usb {
public:
    Usb();

private:
    void usb_init();
    static Usb* pThis;
};


#endif //USB_HID_HDD
