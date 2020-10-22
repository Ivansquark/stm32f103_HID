#ifndef USB_HID_HPP
#define USB_HID_HPP

#include "stm32f10x.h"
#include "usb_descriptors.hpp"
//#include "stdint.h"
//#include "string.h"
#include "uart.hpp"

extern "C" void USB_LP_CAN_RX0_IRQHandler();

class Usb {
public:
    Usb();
    /*!< структура описывающая конечную точку >*/
    struct rx_buf{
        uint16_t rx;
        uint16_t reserved;
    };
    struct tx_buf{
        uint16_t tx;
        uint16_t reserved;
    };
    struct ep_t {
        //Адрес передающего буфера
        rx_buf* r_buf{nullptr};
        //Адрес приемного буфера
        tx_buf* t_buf{nullptr};
        //Состояние регистра USB_EPnR
        uint16_t status{0};
        //Количество принятых байт
        uint8_t rx_cnt{0};
        //Флаг события успешной передачи
        volatile bool tx_flag{0};
        //Флаг события успешного приема
        volatile bool rx_flag{0};
        //Флаг-маркер управляющей транзакции
        volatile bool setup_flag{0};
    };
    ep_t endpoints[4];

    #pragma pack(push, 1)
	typedef struct setup_request
    {
    	uint8_t bmRequestType; // D7 направление передачи фазы данных 0 = хост передает в устройство 1 = устройство передает на хост...
    	uint8_t bRequest;	   // Запрос (2-OUT…6-SETUP)
    	uint16_t reserved0;
        uint16_t wValue;	   // (Коды Запросов дескрипторов)
    	uint16_t reserved1;
        uint16_t wIndex;	   //
    	uint16_t reserved2;
        uint16_t wLength;	   //
        uint16_t reserved3;
    }USB_SETUP_req;		
	#pragma pack(pop)
	
	/*! <накладываем на структуру объединение, чтобы обращаться к различным полям> */
	#pragma pack(push, 1)
	typedef union
	{
		USB_SETUP_req setup; //!< размер структуры		
		uint16_t wRequest;	 //!< Слово объединяющее первые два байта структуры	
	} setupP;    	
	#pragma pack(pop)
	//setupP setupPack{0};
    bool AddressFlag{false};
    bool setLineCodingFlag{false};
    bool bigSize{false};
    void process();
    void EnumerateSetup(uint8_t num);
    void EP_Write(uint8_t number, uint8_t *buf, uint16_t size);
    void EP_Read(uint8_t number, uint8_t *buf);
    void ep0_init();
    static Usb* pThis;
    void setAddress();
    void setConfiguration();
    void epWaitNull(uint8_t number);

    void set_Rx_VALID(uint8_t num);
    void set_Tx_VALID(uint8_t num);
    void clear_Rx (uint8_t num);
    void clear_Tx (uint8_t num);
    void change_DTOG_Rx(uint8_t num);
    void change_DTOG_Tx(uint8_t num);
    void set_DTOG_Tx(uint8_t num);
    void set_DTOG_Rx(uint8_t num);
    
    uint16_t MIN(uint16_t len, uint16_t wLength);

    bool connected{false};
    bool LedOn{false};

private:
    uint8_t receiveBuf[64]={0};

    void usb_init();
    /*! Инициализация конечной точки
    *   number - номер (0...7)
    *   type - тип конечной точки (EP_TYPE_BULK, EP_TYPE_CONTROL, EP_TYPE_ISO, EP_TYPE_INTERRUPT)
    *   addr_tx - адрес передающего буфера в периферии USB
    *   addr_rx - адрес приемного буфера в периферии USB
    *   Размер приемного буфера - фиксированный 64 байта
    */    
    void usb_receive(uint8_t epID);
    //void cdc_get_line_coding();
    //void cdc_set_control_line_state();
    //void cdc_send_break();
    //void cdc_send_encapsulated_command();
    //void cdc_get_encapsulated_command();
    uint8_t address=0;
    static constexpr uint16_t swap16(uint16_t val) {return ((val&0xFF)<<8)|((val>>8)&0xFF);}
    
};


#endif //USB_HID_HDD
