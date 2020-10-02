#ifndef USB_HID_HPP
#define USB_HID_HPP

#include "stm32f10x.h"
#include "usb_descriptors.hpp"
//#include "stdint.h"
//#include "string.h"
#include "uart.hpp"

#define CLEAR_DTOG_RX(R)       			(R & USB_EP0R_DTOG_RX) ? R : (R & (~USB_EP0R_DTOG_RX))
#define SET_DTOG_RX(R)         			(R & USB_EP0R_DTOG_RX) ? (R & (~USB_EP0R_DTOG_RX)) : (R | USB_EP0R_DTOG_RX)
#define TOGGLE_DTOG_RX(R)      			(R | USB_EP0R_DTOG_RX)
#define KEEP_DTOG_RX(R)        			(R & (~USB_EP0R_DTOG_RX))
#define CLEAR_DTOG_TX(R)       			(R & USB_EP0R_DTOG_TX) ? R : (R & (~USB_EP0R_DTOG_TX))
#define SET_DTOG_TX(R)         			(R & USB_EP0R_DTOG_TX) ? (R & (~USB_EP0R_DTOG_TX)) : (R | USB_EP0R_DTOG_TX)
#define TOGGLE_DTOG_TX(R)      			(R | USB_EP0R_DTOG_TX)
#define KEEP_DTOG_TX(R)        			(R & (~USB_EP0R_DTOG_TX))
#define SET_VALID_RX(R)        			((R & USB_EP0R_STAT_RX) ^ USB_EP0R_STAT_RX)   | (R & (~USB_EP0R_STAT_RX))
#define SET_NAK_RX(R)          			((R & USB_EP0R_STAT_RX) ^ USB_EP0R_STAT_RX_1) | (R & (~USB_EP0R_STAT_RX))
#define SET_STALL_RX(R)        			((R & USB_EP0R_STAT_RX) ^ USB_EP0R_STAT_RX_0) | (R & (~USB_EP0R_STAT_RX))
#define KEEP_STAT_RX(R)        			(R & (~USB_EP0R_STAT_RX))
#define SET_VALID_TX(R)        			((R & USB_EP0R_STAT_TX) ^ USB_EP0R_STAT_TX)   | (R & (~USB_EP0R_STAT_TX))
#define SET_NAK_TX(R)          			((R & USB_EP0R_STAT_TX) ^ USB_EP0R_STAT_TX_1) | (R & (~USB_EP0R_STAT_TX))
#define SET_STALL_TX(R)        			((R & USB_EP0R_STAT_TX) ^ USB_EP0R_STAT_TX_0) | (R & (~USB_EP0R_STAT_TX))
#define KEEP_STAT_TX(R)        			(R & (~USB_EP0R_STAT_TX))
#define CLEAR_CTR_RX(R)       		 	(R & (~(USB_EP0R_CTR_RX | USB_EP0R_STAT_RX | USB_EP0R_STAT_TX | USB_EP0R_DTOG_RX | USB_EP0R_DTOG_TX)))
#define CLEAR_CTR_TX(R)        			(R & (~(USB_EP0R_CTR_TX | USB_EP0R_STAT_RX | USB_EP0R_STAT_TX | USB_EP0R_DTOG_RX | USB_EP0R_DTOG_TX)))
#define CLEAR_CTR_RX_TX(R)     			(R & (~(USB_EP0R_CTR_TX | USB_EP0R_CTR_RX | USB_EP0R_STAT_RX | USB_EP0R_STAT_TX | USB_EP0R_DTOG_RX | USB_EP0R_DTOG_TX)))

extern "C" void USB_LP_CAN_RX0_IRQHandler();

class Usb {
public:
    Usb();
    /*!< структура описывающая конечную точку >*/
    struct ep_t {
        //Адрес передающего буфера
        uint16_t *tx_buf{nullptr};
        //Адрес приемного буфера
        uint16_t *rx_buf{nullptr};
        //Состояние регистра USB_EPnR
        uint16_t status{0};
        //Количество принятых байт
        uint8_t rx_cnt{0};
        //Флаг события успешной передачи
        bool tx_flag{0};
        //Флаг события успешного приема
        bool rx_flag{0};
        //Флаг-маркер управляющей транзакции
        bool setup_flag{0};
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
		uint8_t b[8];	 	 //!< массив байтов равный размеру структуры
		uint16_t wRequest;	 //!< Слово объединяющее первые два байта структуры	
	} setupP;    	
	#pragma pack(pop)
	setupP setupPack{0};
    bool AddressFlag{false};
    bool setLineCodingFlag{false};
    void EnumerateSetup(uint8_t num);
    void EP_Write(uint8_t number, uint8_t *buf, uint16_t size);
    void EP_Read(uint8_t number, uint8_t *buf);
    void ep0_init();
    static Usb* pThis;

private:
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
    
    static constexpr uint16_t swap16(uint16_t val) {return ((val&0xFF)<<8)|((val>>8)&0xFF);}
};


#endif //USB_HID_HDD
