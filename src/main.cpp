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
	//*((uint8_t*)(0x40005C00)) = 0x0820;
	//usb.clear_Rx(0);
	//usb.clear_Tx(0);
	//usb.set_Tx_VALID(0);
	//int y=0;

	__enable_irq();	
	uint8_t HID_buf[4]={0};
	while(1) {	
		/*!< обработка прерываний от USB >*/
		usb.process(); 
		if(uart1.wasInterruptedby1){
			if(uart1.receivedArr[0] == 0xff) {
				led.toggle();
				uart1.sendStr("AGA");
			}
			uart1.sendByte(uart1.receivedArr[0]);
			uart1.wasInterruptedby1=false;
		}
		HID_buf[0]=3;
		if(but.ButtonFlag==true) {
			switch (but.Button_case) {
				case 0: but.Button_case=1;HID_buf[1]=0;//led.toggle();
				break;
				case 1: but.Button_case=2;HID_buf[2]=1;//led.toggle();
				break;
				case 2: but.Button_case=3;HID_buf[3]=2;//led.toggle();
				break;
				case 3: but.Button_case=4;HID_buf[1]=3;//led.toggle();
				break;
				case 4: but.Button_case=5;HID_buf[2]=4;//led.toggle();
				break;
				case 5: but.Button_case=6;HID_buf[3]=5;//led.toggle();
				break;
				case 6: but.Button_case=0;HID_buf[1]=6;//led.toggle();
				break;
				default:break;
			}
			usb.EP_Write(1,HID_buf,4);
			Uart::pThis->sendStr("b");			
			but.ButtonFlag=false;
		}		
		if(usb.connected) {				
			if(usb.LedOn) {
				led.ledOn();
			} else {
				led.ledOff();
			}
		}
	}
	return 0;
}
