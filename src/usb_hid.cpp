#include "usb_hid.hpp"

Usb* Usb::pThis = nullptr;

Usb::Usb() {
    usb_init();
    ep0_init();
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
    /*!< таблица для ep 0>*/
    /*!< параметры задаются в адресах USB (реальные отличаются в два раза) >*/
    USB_BTABLE->EP[0].USB_ADDR_TX = 0x40; //реальный адрес 0х40006080
    USB_BTABLE->EP[0].USB_COUNT_TX = 0;
    USB_BTABLE->EP[0].USB_ADDR_RX = 0x80; //реальный адрес 0х40006100
    USB_BTABLE->EP[0].USB_COUNT_RX = (1<<15) | (1<<10); //64 байта    
    /*!< таблица для ep 1>*/
    USB_BTABLE->EP[1].USB_ADDR_TX = 0xC0; //реальный адрес 0х40006180  
    USB_BTABLE->EP[1].USB_COUNT_TX = 0;
    USB_BTABLE->EP[1].USB_ADDR_RX = 0x100; //реальный адрес 0х40006200
    USB_BTABLE->EP[1].USB_COUNT_RX = (1<<15) | (1<<10); //64 байта
    /*!< таблица для ep 2>*/
    USB_BTABLE->EP[2].USB_ADDR_TX = 0x140; //реальный адрес 0х40006280  
    USB_BTABLE->EP[2].USB_COUNT_TX = 0;
    USB_BTABLE->EP[2].USB_ADDR_RX = 0x180; //реальный адрес 0х40006300
    USB_BTABLE->EP[2].USB_COUNT_RX = (1<<15) | (1<<10); //64 байта
    /*!< таблица для ep 3>*/
    USB_BTABLE->EP[3].USB_ADDR_TX = 0x1C0; //реальный адрес 0х40006380  
    USB_BTABLE->EP[3].USB_COUNT_TX = 0;
    USB_BTABLE->EP[3].USB_ADDR_RX = 0x200; //реальный адрес 0х40006400
    USB_BTABLE->EP[3].USB_COUNT_RX = (1<<15) | (1<<10); //64 байта

    for(uint8_t n=0;n<4;n++) {
        /*!< указываем на массив принятых и передаваемых данных в памяти >*/
        endpoints[n].r_buf = (rx_buf*)((uint8_t*)0x40006100 + n*0x100);
        endpoints[n].t_buf = (tx_buf*)((uint8_t*)0x40006080 + n*0x100);
        //Usb::pThis->endpoints[n].r_buf = (Usb::rx_buf*)((uint8_t*)0x40006100 + n*0x100); 
        //Usb::pThis->endpoints[n].t_buf = (Usb::tx_buf*)((uint8_t*)0x40006080 + n*0x100);
    }    
    USB_CR -> DADDR = USB_DADDR_EF;  //enable function
}

void Usb::EnumerateSetup(uint8_t num){
    uint16_t len=0;
    uint8_t *pbuf;
    setupP* setupPack = (setupP*)(endpoints[num].r_buf);
    //Uart::pThis->sendByte((setupPack->wRequest)>>8);
    //Uart::pThis->sendByte((setupPack->wRequest));
    //Uart::pThis->sendStr("\n");  
    switch(swap16(setupPack->wRequest)) {            
        case GET_DESCRIPTOR_DEVICE:        
        //Uart::pThis->sendByte((setupPack->setup.wValue)>>8);
        //Uart::pThis->sendByte((setupPack->setup.wValue));
        switch(swap16(setupPack->setup.wValue)) {      
            case USB_DESC_TYPE_DEVICE:   //Запрос дескриптора устройства
            //Uart::pThis->sendStr("D_D\n");            
            len = sizeof(Device_Descriptor);
            pbuf = (uint8_t *)Device_Descriptor; // выставляем в буфер адрес массива с дескриптором устройства.
            break;
            case USB_DESC_TYPE_CONFIGURATION:   //Запрос дескриптора конфигурации
            //Uart::pThis->sendStr("C_D\n");
            len = sizeof(confDescr);
            pbuf = (uint8_t *)&confDescr;
            break;	   
            case swap16(USBD_IDX_LANGID_STR): //Запрос строкового дескриптора
            //Uart::pThis->sendStr("LAN\n");
            len = sizeof(LANG_ID_Descriptor);
            pbuf = (uint8_t *)LANG_ID_Descriptor;                   
            break;
            case swap16(USBD_strManufacturer): //Запрос строкового дескриптора
            //Uart::pThis->sendStr("USBD_strManufacturer\n");
            len = sizeof(Man_String);
            pbuf = (uint8_t *)Man_String;                             
            break;
            case swap16(USBD_strProduct): //Запрос строкового дескриптора
            //Uart::pThis->sendStr("USBD_strProduct\n");
            len = sizeof(Prod_String);
            pbuf = (uint8_t *)Prod_String;         
            break;                     
            case swap16(USBD_IDX_SERIAL_STR): //Запрос строкового дескриптора
            //Uart::pThis->sendStr("USBD_IDX_SERIAL_STR\n");
            len = sizeof(SN_String);
            pbuf = (uint8_t *)SN_String;    
            break;        
        }
        break;
        case SET_ADDRESS:  // Установка адреса устройства
        //Uart::pThis->sendStr("ADr\n");
        address = setupPack->setup.wValue;
        //Uart::pThis->sendByte(address);
        AddressFlag = true;   
        return;         
        break;        
	    case GET_CONFIGURATION:
		/*Устройство передает один байт, содержащий код конфигурации устройства*/
		pbuf=(uint8_t*)&confDescr+5; //номер конфигурации (единственной)
		len=1;//WriteINEP(0x00,pbuf,MIN(len , uSetReq.wLength));
        break;
        case SET_CONFIGURATION: // Установка конфигурации устройства
        setConfiguration();
	    break;       // len-0 -> ZLP
        case GET_DESCRIPTOR_INTERFACE: 
            //Uart::pThis->sendStr("GET_DESCRIPTOR_INTERFACE\n");
            //Uart::pThis->sendByte((setupPack->setup.wValue)>>8);
            //Uart::pThis->sendByte((setupPack->setup.wValue));            
            switch(swap16(setupPack->setup.wValue)) {      
               case GET_REPORT:   //Запрос дескриптора устройства                       
               len = sizeof(HID_Report);
               pbuf = (uint8_t *)HID_Report; // выставляем в буфер адрес массива с дескриптором устройства.
               Uart::pThis->sendStr("G_R\n");    
               break;
        }
            
        break;
	    case SET_INTERFACE: // Установка конфигурации устройства
	    /*<здесь выбирается интерфейс (в данном случае не должен выбираться, т.к. разные конечные точки)>*/
        Uart::pThis->sendStr("SET_INTERFACE\n");
        break;	         
	    /* CDC Specific requests */
        case SET_LINE_CODING: //устанавливает параметры линии передач
        setLineCodingFlag=true;	
        Uart::pThis->sendStr("SET_LINE_CODING\n");
        //cdc_set_line_coding();           
        break;
        case GET_LINE_CODING:
        //cdc_get_line_coding();           
        break;
        case SET_CONTROL_LINE_STATE:
        Uart::pThis->sendStr("SET_CONTROL_LINE_STATE\n");
        //cdc_set_control_line_state();    
        break;
        case SEND_BREAK:
        //cdc_send_break();                
        break;
        case SEND_ENCAPSULATED_COMMAND:
        //cdc_send_encapsulated_command(); 
        break;
        case CLEAR_FEATURE_ENDP:
        Uart::pThis->sendStr("CLEAR_FEATURE_ENDP\n");
		break;	
        case SET_IDLE:
        Uart::pThis->sendStr("SET_IDLE\n");
        break;
	    default: 
        Uart::pThis->sendStr("default\n");
        break;
        //stall();
    }   
    uint16_t size = MIN(len,setupPack->setup.wLength);
    if(size<=64) {
        EP_Write(0x00,pbuf, size);   // записываем в конечную точку адрес дескриптора и его размер (а также запрошенный размер)
    } else {        
        EP_Write(0x00,pbuf, 64);
        bigSize = true;
        //set_Tx_VALID(0);
        EP_Write(0x00,pbuf, size-64);
        bigSize = false;
    }    
}

uint16_t Usb::MIN(uint16_t len, uint16_t wLength)
{
    uint16_t x=0;
    (len<wLength) ? x=len : x=wLength;
    return x;
}

void Usb::setAddress() {
    USB_CR->DADDR |= address;
    //Uart::pThis->sendStr("SA\n");
}

void Usb::setConfiguration() {    
    //Uart::pThis->sendStr("SET_CON\n");    
}

/*uint8_t number – номер конечной точки
uint8_t *buf – указатель на отправляемые данные
uint16_t size – длинна отправляемых данных
*/
void Usb::EP_Write(uint8_t number, uint8_t *buf, uint16_t size) {
    uint8_t i;
    uint32_t timeout = 1000000;    
//Ограничение на отправку данных больше 64 байт
    //if (size > 64) size = 64;
    /*!< передача 16 битных значений (записываем в буфер Tx) >*/
    uint16_t temp = (size & 0x0001) ? (size + 1) / 2 : size / 2;//если нечетный добавляем единицу
    uint16_t* buf16 = nullptr;
    if(bigSize) {
        buf16 = (uint16_t *)((uint8_t*)buf+64);
        Uart::pThis->sendStr("bigSize\n");
        Uart::pThis->sendByte(size);
        Uart::pThis->sendStr("bigSize\n");
    } else {
        buf16 = (uint16_t *)buf;
    }    
    for (i = 0; i < temp; i++){
        endpoints[number].t_buf[i].tx = buf16[i];
    }
    //Количество передаваемых байт в регистр
    //USB_BTABLE -> EP[number].USB_COUNT_TX = 0;
    USB_BTABLE -> EP[number].USB_COUNT_TX = size;
    set_Tx_VALID(number);
    /*!< Ждем пока данные передадутся, в прерывании tx_flag установится в 1 >*/
    endpoints[number].tx_flag = 0;
    while (!endpoints[number].tx_flag){
        if (timeout) timeout--;
        else break;
    } 
}

void Usb::process() {
    if (endpoints[0].setup_flag && endpoints[0].rx_flag ) {
			EnumerateSetup(0);
			set_Rx_VALID(0);
			endpoints[0].setup_flag=false;			
		} else if (endpoints[0].rx_flag) {			
			//Uart::pThis->sendStr("Status");
			if(AddressFlag) {
				uint8_t x=0;
				//usb.set_DTOG_Tx(0);
				EP_Write(0x00,&x,0);	//ZLP			
				setAddress();
				AddressFlag = false;
			}			
			set_Rx_VALID(0);
			endpoints[0].rx_flag=false;
		} else if (endpoints[1].rx_flag) {            
            if(endpoints[1].rx_cnt) {                
                LedOn = *((uint8_t*)(endpoints[1].r_buf + 1));
            }            
            set_Rx_VALID(1); 
            Uart::pThis->sendStr("INTERRUPT arrived\n");
            endpoints[1].rx_flag=false;
            connected = true;
		} else if (endpoints[2].rx_flag) { //IN
            Uart::pThis->sendStr("RX2\n");
            set_Rx_VALID(2); 
		} else if (endpoints[3].rx_flag) { //OUT
            Uart::pThis->sendStr("Bulk arrived\n"); 
            set_Rx_VALID(3); 
            endpoints[3].rx_flag=false;
            connected = true;          
		}
}

void USB_LP_CAN_RX0_IRQHandler() {
    /*! прерывание по ресету */
    if(USB_CR->ISTR & USB_ISTR_RESET) {
        Uart::pThis->sendStr("reset\n");        
        /*!Переинициализируем регистры*/
        //Usb::pThis->ep0_init();        
        USB_CR->CNTR   = USB_CNTR_CTRM | USB_CNTR_RESETM;// | USB_CNTR_SUSPM;  
        /*!< обнуляем адрес устройства >*/
        USB_CR -> DADDR &=~ 0x7F;
        //TODO: endpoint initialization !!!
        USB_EP -> EPnR[0].value = 0;
        USB_EP -> EPnR[1].value = 0;
        USB_EP -> EPnR[0].value |= USB_EP0R_EP_TYPE_0;
        USB_EP -> EPnR[0].value &=~ USB_EP0R_EP_TYPE_1; // 0:1 - control Ep
        USB_EP -> EPnR[0].value ^= (USB_EP0R_STAT_RX | USB_EP0R_STAT_TX_1); //Rx=1:1 - разрешена на прием(ACK) Tx=1:0 - NACK         
        
        /*!< Ep1 IN and OUT INTERRUPT initialization >*/
        USB_EP -> EPnR[1].value |= USB_EP1R_EP_TYPE_0;
        USB_EP -> EPnR[1].value |= USB_EP1R_EP_TYPE_1 | (1<<0); // 1:1 - INTERRUPT Ep
        USB_EP -> EPnR[1].value ^= (USB_EP0R_STAT_RX | USB_EP0R_STAT_TX_1); //Rx=1:1 - VALID Tx=1:0 - NACK
        /*!< включаем устройство >*/
        USB_CR ->BTABLE=0;		
        //Записываем в USB_EPnR тип и номер конечной точки. Для упрощения номер конечной точки        
        USB_CR->ISTR = 0;//&= ~USB_ISTR_RESET;
    }
    /*!< прерывание по приему >*/
    if (USB_CR -> ISTR & USB_ISTR_CTR) {         
        //usb_receive((uint8_t)(USB_CR -> ISTR)); //номер контрольной точки и направление        
        /*!< номер конечной точки >*/
        int n = USB_CR -> ISTR & USB_ISTR_EP_ID;        
		//Копируем количество принятых байт
		Usb::pThis->endpoints[n].rx_cnt = (USB_BTABLE -> EP[n].USB_COUNT_RX) ;//& 0x1F;	        
        //Копируем содержимое EPnR этой конечной точки
		Usb::pThis->endpoints[n].status = USB_EP ->EPnR[n].value;
		//Обновляем состояние флажков
		Usb::pThis->endpoints[n].rx_flag = (Usb::pThis->endpoints[n].status & USB_EP0R_CTR_RX) ? 1 : 0;
		Usb::pThis->endpoints[n].setup_flag = (Usb::pThis->endpoints[n].status & USB_EP0R_SETUP) ? 1 : 0;
        Usb::pThis->endpoints[n].tx_flag = (Usb::pThis->endpoints[n].status & USB_EP0R_CTR_TX) ? 1 : 0; //конец передачи
		//Очищаем флаги приема и передачи
        Usb::pThis->clear_Rx(n);
        Usb::pThis->clear_Tx(n);
        //USB_CR -> ISTR &=~ USB_ISTR_CTR;
        if(n==1) {
            Uart::pThis->sendStr("INTERRUPT");
            //Uart::pThis->sendByte(n);
        }
        USB_CR -> ISTR =0;
    }    
}

void Usb::set_Rx_VALID(uint8_t num) {
        uint16_t status = USB_EP -> EPnR[num].value;
        status &=~ ((1<<14)| (7<<4)); // обнуляем toggle
        //uint16_t x = status & USB_EP0R_STAT_RX;
        if((status & USB_EP0R_STAT_RX) == 0x0000) { //if cleared
            status |= USB_EP0R_STAT_RX;
            USB_EP -> EPnR[num].value = status;           
        } else if ((status & USB_EP0R_STAT_RX) == 0x1000) { // if STALL
            status &=~ USB_EP0R_STAT_RX_0;
            status |= USB_EP0R_STAT_RX_1;
            USB_EP -> EPnR[num].value = status;              
        } else if ((status & USB_EP0R_STAT_RX) == 0x2000) { // if NAK
            status &=~ USB_EP0R_STAT_RX_1;
            status |= USB_EP0R_STAT_RX_0;
            USB_EP -> EPnR[num].value = status;
        } else if ((status & USB_EP0R_STAT_RX) == 0x3000) { // if VALID nothing to do
            return;
        }        
    }
    void Usb::set_Tx_VALID(uint8_t num) {
        uint16_t status = USB_EP -> EPnR[num].value;
        status &=~ ((1<<6)| (7<<12));// обнуляем toggle
        if((status & USB_EP0R_STAT_TX) == 0x0000) { //if cleared
            status |= USB_EP0R_STAT_TX;
            USB_EP -> EPnR[num].value = status;           
        } else if ((status & USB_EP0R_STAT_TX) == 0x10) { // if STALL
            status &=~ USB_EP0R_STAT_TX_0;
            status |= USB_EP0R_STAT_TX_1;
            USB_EP -> EPnR[num].value = status;              
        } else if ((status & USB_EP0R_STAT_TX) == 0x20) { // if NAK
            status &=~ USB_EP0R_STAT_TX_1;
            status |= USB_EP0R_STAT_TX_0;
            USB_EP -> EPnR[num].value = status;
        } else if ((status & USB_EP0R_STAT_TX) == 0x30) { // if VALID nothing to do
            return;
        }
    }
    void Usb::clear_Rx (uint8_t num) {
        uint16_t status = USB_EP -> EPnR[num].value;
        status &=~ ((7<<4)| (7<<12)); // обнуляем toggle
        status &=~ (1<<15); //очищаем CTR_Rx
        USB_EP -> EPnR[num].value = status;
    }
    void Usb::clear_Tx (uint8_t num) {
        uint16_t status = USB_EP -> EPnR[num].value;
        status &=~ ((7<<4)| (7<<12)); // обнуляем toggle
        status &=~ (1<<7); //очищаем CTR_Tx
        USB_EP -> EPnR[num].value = status;
    }
    void Usb::change_DTOG_Rx(uint8_t num) {
        uint16_t status = USB_EP -> EPnR[num].value;
        status &=~ ((7<<4)| (7<<12)); // обнуляем toggle
        status |= (1<<14); // записываем 1 в бит для дальнейшего переворота
        USB_EP -> EPnR[num].value = status;
    }
    void Usb::change_DTOG_Tx(uint8_t num) {
        uint16_t status = USB_EP -> EPnR[num].value;
        status &=~ ((7<<4)| (7<<12)); // обнуляем toggle
        status |= (1<<6); // записываем 1 в бит для дальнейшего переворота
        USB_EP -> EPnR[num].value = status;
    }
    void Usb::set_DTOG_Tx(uint8_t num) {
        uint16_t status = USB_EP -> EPnR[num].value;
        status &=~ ((7<<4)| (7<<12)); // обнуляем toggle
        if(status & (USB_EP0R_DTOG_TX)){
            return;
        } else {
            status |= (1<<6); // записываем 1 в бит для дальнейшего переворота
            USB_EP -> EPnR[num].value = status;
        }
    }
    void Usb::set_DTOG_Rx(uint8_t num) {
        uint16_t status = USB_EP -> EPnR[num].value;
        status &=~ ((7<<4)| (7<<12)); // обнуляем toggle
        if(status & (USB_EP0R_DTOG_RX)){
            return;
        } else {
            status |= (1<<14); // записываем 1 в бит для дальнейшего переворота
            USB_EP -> EPnR[num].value = status;
        }
    }