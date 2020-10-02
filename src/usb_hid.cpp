#include "usb_hid.hpp"

Usb::Usb() {
    usb_init();
    pThis = this;
}

void Usb::usb_init() {
    /*!< предделитель для USB >*/
    RCC->CFGR &=~ RCC_CFGR_USBPRE; //0: делим на 1.5 72/1.5=48 MHz   (1: не делим)
    RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
    GPIOA->CRH |= (GPIO_CRH_CNF12_1 | GPIO_CRH_CNF13_1);  // alt func (push-pull)
    GPIOA->CRH |= (GPIO_CRH_MODE12 | GPIO_CRH_MODE13);  // max speed
    NVIC_DisableIRQ(USB_LP_CAN1_RX0_IRQn);
    {    /*!< тактирование на USB >*/
        RCC->APB1ENR |= RCC_APB1ENR_USBEN;
        /*!< force reset >*/
        USB_CR->CNTR = USB_CNTR_FRES;
        /*!< Таблица адресов располагается начиная с 0х40006000 >*/
        USB_CR->BTABLE = 0;
        /*!< конфигурируется конечная точка 0 >*/
        USB_CR->DADDR =0; //|= USB_DADDR_EF;
        /*!< Сбрасываем флаги >*/
        USB_CR->ISTR =0;
        /*!< Разрешаем прерывания по RESET и CTRM >*/    
        USB_CR->CNTR =USB_CNTR_RESETM | USB_CNTR_CTRM;  //correct transfer interrupt mask
    }
    NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);    
}

void Usb::ep0_init() {
    //Записываем в USB_EPnR тип и номер конечной точки. Для упрощения номер конечной точки
    //устанавливается равным  номеру USB_EPnR
    USB_EP -> EP0R |= USB_EP0R_EP_TYPE_0;
    USB_EP -> EP0R &=~ USB_EP0R_EP_TYPE_1; // 0:1 - control Ep
    USB_EP -> EP0R ^= (USB_EP0R_STAT_RX | USB_EP0R_STAT_TX_1); //Rx=1:1 - разрешена на прием(ACK) Tx=1:0 - NACK 
    /*!< таблица для ep 0>*/
    USB_TABLE->EP[0]->USB_ADDR_TX = 0x40; //смещен на 64 байта от начала таблицы  
    USB_TABLE->EP[0]->USB_COUNT_TX = 0;
    USB_TABLE->EP[0]->USB_ADDR_RX = 0x80; //
    USB_TABLE->EP[0]->USB_COUNT_RX = (1<<15) | (1<<10); //64 байта
    /*!< таблица для ep 1>*/
    USB_TABLE->EP[0]->USB_ADDR_TX = 0xC0; //смещен на 64 байта от начала таблицы  
    USB_TABLE->EP[0]->USB_COUNT_TX = 0;
    USB_TABLE->EP[0]->USB_ADDR_RX = 0x100; //
    USB_TABLE->EP[0]->USB_COUNT_RX = (1<<15) | (1<<10); //64 байта
    /*!< таблица для ep 2>*/
    USB_TABLE->EP[0]->USB_ADDR_TX = 0x140; //смещен на 64 байта от начала таблицы  
    USB_TABLE->EP[0]->USB_COUNT_TX = 0;
    USB_TABLE->EP[0]->USB_ADDR_RX = 0x180; //
    USB_TABLE->EP[0]->USB_COUNT_RX = (1<<15) | (1<<10); //64 байта
    /*!< таблица для ep 3>*/
    USB_TABLE->EP[0]->USB_ADDR_TX = 0x1C0; //смещен на 64 байта от начала таблицы  
    USB_TABLE->EP[0]->USB_COUNT_TX = 0;
    USB_TABLE->EP[0]->USB_ADDR_RX = 0x200; //
    USB_TABLE->EP[0]->USB_COUNT_RX = (1<<15) | (1<<10); //64 байта
}

void Enumerate_Setup(uint8_t num){
    uint16_t len=0;
    uint8_t *pbuf; 
    setupPack = (setupPack*)endpoints[number].rx_buf;
    switch(swap(setupPack.wRequest))
    {    
        case GET_DESCRIPTOR_DEVICE:        
        switch(setupPack.setup.wValue) {        
            case USB_DESC_TYPE_DEVICE:   //Запрос дескриптора устройства
            //USART_debug::usart2_sendSTR("DEVICE DESCRIPTER\n");
            //counter++;
            len = sizeof(Device_Descriptor);
            pbuf = (uint8_t *)Device_Descriptor; // выставляем в буфер адрес массива с дескриптором устройства.
            break;
            case USB_DESC_TYPE_CONFIGURATION:   //Запрос дескриптора конфигурации
            //USART_debug::usart2_sendSTR("CONFIGURATION DESCRIPTER\n");
            len = sizeof(confDescr);
            pbuf = (uint8_t *)&confDescr;
            break;		           
            case USBD_IDX_LANGID_STR: //Запрос строкового дескриптора
            //USART_debug::usart2_sendSTR("USBD_IDX_LANGID_STR\n");
            len = sizeof(LANG_ID_Descriptor);
            pbuf = (uint8_t *)LANG_ID_Descriptor;                   
            break;
            case USBD_strManufacturer: //Запрос строкового дескриптора
            //USART_debug::usart2_sendSTR("USBD_strManufacturer\n");
            len = sizeof(Man_String);
            pbuf = (uint8_t *)Man_String;                             
            break;
            case USBD_strProduct: //Запрос строкового дескриптора
            //USART_debug::usart2_sendSTR("USBD_strProduct\n");
            len = sizeof(Prod_String);
            pbuf = (uint8_t *)Prod_String;         
            break;                     
            case USBD_IDX_SERIAL_STR: //Запрос строкового дескриптора
            //USART_debug::usart2_sendSTR("USBD_IDX_SERIAL_STR\n");
            len = sizeof(SN_String);
            pbuf = (uint8_t *)SN_String;    
            break;        
        }
    break;
    case SET_ADDRESS:  // Установка адреса устройства
        AddressFlag = true;            
    break;
	case GET_CONFIGURATION:
		/*Устройство передает один байт, содержащий код конфигурации устройства*/
		pbuf=(uint8_t*)&confDescr+5; //номер конфигурации (единственной)
		len=1;//WriteINEP(0x00,pbuf,MIN(len , uSetReq.wLength));
    //USART_debug::usart2_sendSTR("GET_CONFIGURATION\n");
	break;
    case SET_CONFIGURATION: // Установка конфигурации устройства
	break;       // len-0 -> ZLP
	case SET_INTERFACE: // Установка конфигурации устройства
	/*<здесь выбирается интерфейс (в данном случае не должен выбираться, т.к. разные конечные точки)>*/
    USART_debug::usart2_sendSTR("SET_INTERFACE\n");
    break;	  	  
	/* CDC Specific requests */
    case SET_LINE_CODING: //устанавливает параметры линии передач
    USART_debug::usart2_sendSTR(" SLC \n");
	  setLineCodingFlag=true;	
      //cdc_set_line_coding();           
      break;
    case GET_LINE_CODING:
    USART_debug::usart2_sendSTR(" GLC \n");
      cdc_get_line_coding();           
      break;
    case SET_CONTROL_LINE_STATE:
    USART_debug::usart2_sendSTR(" SCLS \n");
      cdc_set_control_line_state();    
      break;
    case SEND_BREAK:
    USART_debug::usart2_sendSTR(" S_B \n");
      cdc_send_break();                
      break;
    case SEND_ENCAPSULATED_COMMAND:
    USART_debug::usart2_sendSTR("SEND_ENCAPSULATED_COMMAND\n");
      cdc_send_encapsulated_command(); 
      break;
    case GET_ENCAPSULATED_RESPONSE:
    USART_debug::usart2_sendSTR("GET_ENCAPSULATED_RESPONSE\n");
      cdc_get_encapsulated_command();  
      break;
	case CLEAR_FEATURE_ENDP:
			USART_debug::usart2_sendSTR(" CLEAR_FEATURE_ENDP \n");
			//USART_debug::usart2_send(setupPack.b[2]);
		    //USART_debug::usart2_send(setupPack.b[3]);
		//Сбросить все TXFIFO
		USB_OTG_FS->GRSTCTL = USB_OTG_GRSTCTL_TXFFLSH | USB_OTG_GRSTCTL_TXFNUM;
		while (USB_OTG_FS->GRSTCTL & USB_OTG_GRSTCTL_TXFFLSH); //очищаем Tx_FIFO, которое почему то переполняется
		break;
	
	default: 
  //stall();
  USART_debug::usart2_sendSTR(" STALL \n");break;
  }   
  EP_Write(0x00,pbuf,len);   // записываем в конечную точку адрес дескриптора и его размер (а также запрошенный размер)
}

/*uint8_t number – номер конечной точки
uint8_t *buf – указатель на отправляемые данные
uint16_t size – длинна отправляемых данных
*/
void Usb::EP_Write(uint8_t number, uint8_t *buf, uint16_t size) {
    uint8_t i;
    uint32_t timeout = 100000;
//Читаем EPnR
    uint16_t status = USB_EP -> EPnR[number];
//Ограничение на отправку данных больше 64 байт
    if (size > 64) size = 64;
/* ВНИМАНИЕ КОСТЫЛЬ
 * Из-за ошибки записи в область USB/CAN SRAM с 8-битным доступом
 * пришлось упаковывать массив в 16-бит, собственно размер делить
 * на 2, если он был четный, или делить на 2 + 1 если нечетный
 */
    uint16_t temp = (size & 0x0001) ? (size + 1) / 2 : size / 2;
    uint16_t *buf16 = (uint16_t *)buf;
    for (i = 0; i < temp; i++){
        endpoints[number].tx_buf[i] = buf16[i];
    }
    //Количество передаваемых байт в регистр
    USB_BTABLE -> EP[number].USB_COUNT_TX = size;
    //STAT_RX, DTOG_TX, DTOG_RX – оставляем, STAT_TX=VALID
    status = KEEP_STAT_RX(status);      
    status = SET_VALID_TX(status);      
    status = KEEP_DTOG_TX(status);
    status = KEEP_DTOG_RX(status);
    USB -> EPnR[number] = status;
//Ждем пока данные передадутся
    endpoints[number].tx_flag = 0;
    while (!endpoints[number].tx_flag){
        if (timeout) timeout--;
        else break;
    }
}

void Usb::EP_Read(uint8_t number, uint8_t *buf){
	uint32_t timeout = 100000;
	uint16_t status, i;
	status = USB_EP -> EPnR[number];
	status = SET_VALID_RX(status);
	status = SET_NAK_TX(status);
	status = KEEP_DTOG_TX(status);
	status = KEEP_DTOG_RX(status);
	USB_EP -> EPnR[number] = status;
	endpoints[number].rx_flag = 0;
	while (!endpoints[number].rx_flag){
		if (timeout) timeout--;
		else break;
	}
	for (i = 0; i < endpoints[number].rx_cnt; i++){
		buf[i] = endpoints[number].rx_buf[i]; // чтение из буффер конечной точки
	}
}




void USB_LP_CAN_RX0_IRQHandler() {
    /*! прерывание по ресету */
    if(USB_CR->ISTR & USB_ISTR_RESET) {
        USB_СК->ISTR &= ~USB_ISTR_RESET;
        /*!Переинициализируем регистры*/
        USB_CR -> CNTR = USB_CNTR_RESETM | USB_CNTR_CTRM;
		ep0_init();        
        USB->CNTR   = USB_CNTR_CTRM | USB_CNTR_RESETM | USB_CNTR_SUSPM;         
		USB_CR -> DADDR = USB_DADDR_EF;  //enable function
    }
    /*!< прерывание по приему >*/
    if (USB_CR -> ISTR & USB_ISTR_CTR) { 
        //usb_receive((uint8_t)(USB_CR -> ISTR)); //номер контрольной точки и направление
        
        /*!< номер конечной точки >*/
        uint8_t n = USB_CR -> ISTR & USB_ISTR_EP_ID;
		//Копируем количество принятых байт
		endpoints[n].rx_cnt = (USB_BTABLE -> EP[n].USB_COUNT_RX) & 0x1F;
		//Копируем содержимое EPnR этой конечной точки
		endpoints[n].status = USB_EP ->EPnR[n].value;
		//Обновляем состояние флажков
		endpoints[n].rx_flag = (endpoints[n].status & USB_EP0R_CTR_RX) ? 1 : 0;
		endpoints[n].setup_flag = (endpoints[n].status & USB_EP0R_SETUP) ? 1 : 0;
		endpoints[n].tx_flag = (endpoints[n].status & USB_EP0R_CTR_TX) ? 1 : 0; //конец передачи
		//Очищаем флаги приема и передачи
		endpoints[n].status = CLEAR_CTR_RX_TX(endpoints[n].status);
		USB_EP -> EPnR[n].value = endpoints[n].status;
        USB_CR -> ISTR &=~ USB_ISTR_CTR;
    }    
}