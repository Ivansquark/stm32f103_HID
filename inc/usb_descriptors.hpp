#ifndef USB_DESCRIPTORS_H
#define USB_DESCRIPTORS_H

constexpr uint8_t Device_Descriptor[18] =
    {
        /* Standard USB device descriptor for the CDC serial driver */
        18, // size
        1, // USBGenericDescriptor_DEVICE
        0x01,0x01, // USBDeviceDescriptor_USB 1_00
        0, // description in interface
        0, // 
        0, // 
        64, // BOARD_USB_ENDPOINTS_MAXPACKETSIZE
        0x83,0x04, // VENDORID
        0x50,0x57, // PRODUCTID
        0x00,0x01, //   bcdDevice 2.0 (00 01)
        1, // Index of manufacturer description //0
        2, // Index of product description //0
        3, // Index of serial number description //0
        1 // One possible configuration
    };

    constexpr struct
    {
        const uint8_t Config_Descriptor[9];
        const uint8_t Interface_Descriptor1[9]; 
        const uint8_t Hid_Descriptor[9];       
        const uint8_t EP1_In_Descriptor[7];
        const uint8_t EP1_OUT_Descriptor[7];
    }  confDescr =    
    {        /*Configuration Descriptor*/
        {
            0x09, /* bLength: Configuration Descriptor size */
            0x02, /* bDescriptorType: Configuration */
            41,   /* wTotalLength:no of retuinturned bytes */
            0x00,
            0x01, /* bNumInterfaces: 1 interface */
            0x01, /* bConfigurationValue: Configuration value */
            0x00, /* iConfiguration: Index of string descriptor describing the configuration */
            0xE0, /* bmAttributes - C0 -self pow Bus powered 00 [6] 0-bus powered 1-no bus power [5] 0-no wakeup 1- wakeup */
            0x32 /* MaxPower 100 mA 50*2 */
        },
        {
            /*Interface Descriptor slave interface*/
            0x09, /* bLength: Interface Descriptor size */
            0x04, /* bDescriptorType: Interface */
            0x00, /* bInterfaceNumber: Number of Interface */
            0x00, /* bAlternateSetting: Alternate setting */
            0x01, /* bNumEndpoints: One endpoints used */
            0x03, /* bInterfaceClass: HID Interface Class */
            0x00, /* bInterfaceSubClass: 1:BOOT 0:NO boot */
            0x00, /* bInterfaceProtocol: No special */
            0x00 /* iInterface: */
        },      
        {
            /*! < HID descriptor > */		
            0x09, //Length
            0x21, //HID_DESCRIPTOR_TYPE             
            0x01, //HID 1.1
            0x01, 
            0x00, //country code
            0x01, //count of report descriptors
            0x22, //report descriptor type
            38,   //report descriptor length
            0x00
        },  
			{
				/*Endpoint 1 Descriptor*/
				0x07, /* bLength: Endpoint Descriptor size */
				0x05, /* bDescriptorType: Endpoint */
				0x81, /* bEndpointAddress IN2  8-IN 1-endpoint 1*/
				0x03, /* bmAttributes: Interrupt */
				64, /* wMaxPacketSize LO: */
				0x00, /* wMaxPacketSize HI: */
				0xFF /* bInterval: 1ms*/
			},
			/*EP1_OUT_Descriptor[7]*/
			{
				0x07,   /*Endpoint descriptor length = 7 */
				0x05,   /*Endpoint descriptor type */
				0x01,   /*Endpoint address (0-OUT 1-endpoint1) */
				0x03,   /*Interrupt endpoint type Interrupt, 0x02 -BULK*/
				64,
				0x00,
				0xFF     /*Polling interval in milliseconds*/
			}
    };		
     
    const uint8_t HID_Report[] {
        0x06, 0x00, 0xff,              // USAGE_PAGE (Generic Desktop)
            0x09, 0x01,                    // USAGE (Vendor Usage 1)
            0xa1, 0x01,                    // COLLECTION (Application)

            0x85, 0x01,                    //   REPORT_ID (1)
            0x09, 0x01,                    //   USAGE (Vendor Usage 1)
            0x75, 0x08,                    //   REPORT_SIZE (8)
            0x95, 0x01,                    //   REPORT_COUNT (1)
            0x91, 0x82,                    //   OUTPUT (Data,Var,Abs,Vol)

            0x85, 0x02,                    //   REPORT_ID (2)
            0x09, 0x02,                    //   USAGE (Vendor Usage 2)
            0x75, 0x08,                    //   REPORT_SIZE (8)
            0x95, 0x03,                    //   REPORT_COUNT (4)
            0x81, 0x82,                    //   INPUT (Data,Var,Abs,Vol)

            0x85, 0x03,                    //   REPORT_ID (3)
            0x09, 0x03,                    //   USAGE (Vendor Usage 3)
            0x75, 0x08,                    //   REPORT_SIZE (8)  размер элемента в битах
            0x95, 0x04,                    //   REPORT_COUNT (4)  число полей данных в репорте   4 элемента по 8 бит
            0x81, 0x82,                    //   INPUT (Data,Var,Abs,Vol) данные, по одному биту, абсолютные значения (не мышь)
        0xC0						 //   END_COLLECTION	             
    };
//---------------------------------------------------------------------------------------------------	
	/*! <Всего 67 байт>*/	
	constexpr uint8_t LANG_ID_Descriptor[4]
    {
        /* Index 0x00: LANGID Codes */
        0x04,   /*descriptor length = 7 */
        0x03,   /*STRING descriptor type */
        0x09,	/*LANG_US uint16_t*/
		0x04      /*! !!! все что больше байта необходимо переворачивать*/  
    };   
	constexpr uint8_t Man_String[13]
	{
		/* Index 0x01:  */
		0x0a,   /*descriptor length = 13 */
        0x03,   /*STRING descriptor type */
        'I',0,
		'v',0,
		'a',0,
		'n',0
	};
	constexpr uint8_t Prod_String[14] 
	{
		/* Index 0x02:  */
		0x08,   /*descriptor length = 14 */
        0x03,   /*STRING descriptor type */
        'O',0,
		'p',0,
		'A',0		
	};
	constexpr uint8_t SN_String[14]
	{
		/* Index 0x03:  */
		0x08,   /*descriptor length = 14 */
        0x03,   /*STRING descriptor type */
        '1',0,
		'2',0,
		'3',0		
	};
	/*! <wRequest> */
	static constexpr uint16_t GET_STATUS_DEVICE = 0x8000;
	static constexpr uint16_t GET_STATUS_INTERFACE = 0x8100;
	static constexpr uint16_t GET_STATUS_ENDP = 0x8200;
    static constexpr uint16_t CLEAR_FEATURE_DEVICE = 0x0001;
	static constexpr uint16_t CLEAR_FEATURE_INTERFACE = 0x0101;
	static constexpr uint16_t CLEAR_FEATURE_ENDP = 0x0201;
    static constexpr uint16_t SET_FEATURE_DEVICE = 0x0003;
	static constexpr uint16_t SET_FEATURE_INTERFACE = 0x0103;
	static constexpr uint16_t SET_FEATURE_ENDP = 0x0203;
    static constexpr uint16_t SET_ADDRESS = 0x0005;
    static constexpr uint16_t GET_DESCRIPTOR_DEVICE = 0x8006;
	static constexpr uint16_t GET_DESCRIPTOR_INTERFACE = 0x8106;
	static constexpr uint16_t GET_DESCRIPTOR_ENDP = 0x8206;
    static constexpr uint16_t SET_DESCRIPTOR = 0x0007;
    static constexpr uint16_t GET_CONFIGURATION = 0x8008;
    static constexpr uint16_t SET_CONFIGURATION = 0x0009;
    static constexpr uint16_t GET_INTERFACE = 0x810A;
    static constexpr uint16_t SET_INTERFACE = 0x010B;
    static constexpr uint16_t SYNCH_FRAME = 0x820C;
    static constexpr uint16_t SET_IDLE = 0x210A;
	//static constexpr uint16_t GET_REPORT = 0xA101;
	/*! <bmRequestType> */    
    static constexpr uint8_t STD_GET_STATUS = 0x00;
    static constexpr uint8_t STD_CLEAR_FEATURE = 0x01;
    static constexpr uint8_t STD_SET_FEATURE = 0x03;
    static constexpr uint8_t STD_SET_ADDRESS = 0x05;
    static constexpr uint8_t STD_GET_DESCRIPTOR = 0x06;
    static constexpr uint8_t STD_SET_DESCRIPTOR = 0x07;
    static constexpr uint8_t STD_GET_CONFIGURATION = 0x08;
	/*!<Драйвер в операционной системе по окончанию энумерации посылает такой запрос>*/
    static constexpr uint8_t STD_SET_CONFIGURATION = 0x09; //в Value задается конфигурация в младшем байте
    static constexpr uint8_t STD_GET_INTERFACE = 0xA;
    static constexpr uint8_t STD_SET_INTERFACE = 0x11;
    static constexpr uint8_t STD_SYNCH_FRAME = 0x12;
    /*! <mValue> */
    static constexpr uint16_t USB_DESC_TYPE_DEVICE = 0x0001;
    static constexpr uint16_t USB_DESC_TYPE_CONFIGURATION = 0x0002;
    static constexpr uint16_t USB_DESC_TYPE_DEVICE_QUALIFIER = 0x0600;
	/*<В запросах на передачу дескриптора Value содержит в старшем байте тип дескриптора, а в младшем индекс>*/
    static constexpr uint16_t USBD_IDX_LANGID_STR = 0x0300;
    static constexpr uint16_t USBD_strManufacturer = 0x0301;
    static constexpr uint16_t USBD_strProduct = 0x0302;
    static constexpr uint16_t USBD_IDX_SERIAL_STR = 0x0303;
    //static constexpr uint16_t USBD_IDX_CONFIG_STR = 0x0304;
    
    /*!< HID CLASS >*/
    static constexpr uint16_t GET_DESCRIPTOR_HID = 0x0021;
	static constexpr uint16_t GET_REPORT = 0x0022;
    static constexpr uint16_t SET_REPORT = 0x0023;

	//<(bRequest<<8)|(bmRequestType)>
	static constexpr uint16_t SEND_ENCAPSULATED_COMMAND = 0x2100; //посылка команды
	//static constexpr uint16_t GET_ENCAPSULATED_RESPONSE = 0xA101; // прием команды
	static constexpr uint16_t SET_LINE_CODING = 0x2120; //хост устанавливает параметры передачи данных
	static constexpr uint16_t GET_LINE_CODING = 0xA121;	// хост узнает текущие настройки линии передачи
	static constexpr uint16_t SET_CONTROL_LINE_STATE = 0x2122; // устанавливает состояние линии передачи (RTS[1] DTR[0])
	static constexpr uint16_t SEND_BREAK = 0x2123; //замораживает передачу данных
	
	constexpr uint8_t line_coding[7]={0x00,0xC2,0x01,0x00,0x00,0x00,0x08}; //uint32_t baudRate 115200; стоп бит, четность, размер посылки
	
	
#endif //USB_DESCRIPTORS_H