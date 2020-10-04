#include "main.h"

void (*InterruptManager::IsrVectors[41])(void)={InterruptManager::DefaultHandler};
//extern "C" void __cxa_pure_virtual(void);
//void __cxa_pure_virtual(void) {};

int main()
{
	RCCini rcc;
	LED13 led;
		
	led.ledOn();
	Spi2_master spi2;
	Timer1PWM pwm;
	Button but;	
	Uart uart1(1); //sets automatically interrupt handler in constructor
	InterruptSubject<EXTI0_IRQn> interruptButton;	
	interruptButton.setInterrupt(&but);
	interruptButton.SetVector();

	Usb usb;
	__enable_irq();	
	while(1) {	
		if (usb.endpoints[0].setup_flag && usb.endpoints[0].rx_flag ) {
			usb.EnumerateSetup(0);
			usb.set_Rx_VALID(0);
			usb.endpoints[0].setup_flag=false;			
		} else if (usb.endpoints[0].rx_flag) {			
			Uart::pThis->sendStr("Status");
			if(usb.AddressFlag) {				
				usb.setAddress();
				usb.AddressFlag = false;
			}
			usb.epWaitNull(0);
		} else if (usb.endpoints[1].rx_flag) {

		} else if (usb.endpoints[2].rx_flag) {

		} else if (usb.endpoints[3].rx_flag) {

		}
		if(uart1.wasInterruptedby1){
			if(uart1.receivedArr[0] == 0xff) {
				led.toggle();
				uart1.sendStr("AGA");
			}
			uart1.sendByte(uart1.receivedArr[0]);
			uart1.wasInterruptedby1=false;
		}
		if(but.ButtonFlag==true) {
			switch (but.Button_case) {
				case 0: but.Button_case=1;spi2.sendWord(0);pwm.setDuty(0);led.toggle();break;
				case 1: but.Button_case=2;spi2.sendWord(500);pwm.setDuty(4);led.toggle();break;
				case 2: but.Button_case=3;spi2.sendWord(1000);pwm.setDuty(6);led.toggle();break;
				case 3: but.Button_case=4;spi2.sendWord(1500);pwm.setDuty(8);led.toggle();break;
				case 4: but.Button_case=5;spi2.sendWord(2000);pwm.setDuty(9);led.toggle();break;
				case 5: but.Button_case=6;spi2.sendWord(2500);pwm.setDuty(10);led.toggle();break;
				case 6: but.Button_case=0;spi2.sendWord(3000);pwm.setDuty(10);led.toggle();break;
				default:break;
			}
			but.ButtonFlag=false;
		}		
	}
	return 0;
}
