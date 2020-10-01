#ifndef SPI2_H_
#define SPI2_H_

#include "main.h"

class Spi2_master
{
public:
    Spi2_master()
    {
        spi2_ini();
    }
    void cs_set()
    {
        GPIOB->BSRR|=GPIO_BSRR_BR12;
        GPIOC->BSRR|=GPIO_BSRR_BR13;//led on
    }
    void cs_idle()
    {
        GPIOB->BSRR|=GPIO_BSRR_BS12;
        GPIOC->BSRR|=GPIO_BSRR_BS13;//led off
    }
    void ld_set()
    {
        GPIOB->BSRR|=GPIO_BSRR_BS10;
    }
    void ld_reset()
    {
        GPIOB->BSRR|=GPIO_BSRR_BR10;
    }    
    void sendByte(uint8_t data)
    {
        while(!(SPI2->SR&SPI_SR_TXE)){}        
        SPI2->DR=data;
        //*(uint8_t*)SPI1->DR=data;
        while((SPI2->SR&SPI_SR_BSY)){}
    }
    void sendBytes(volatile uint8_t* data)
    {
        for(uint8_t i=0;i<7;i++)
        {
            while(!(SPI2->SR&SPI_SR_TXE)){}        
            SPI2->DR=*(data+i);
            //*(uint8_t*)SPI1->DR=data;
            while((SPI2->SR&SPI_SR_BSY)){}            
        }
    }
    void sendWord(uint16_t data)
    {
        while(!(SPI2->SR&SPI_SR_TXE)){}        
        SPI2->DR=data;
        //*(uint8_t*)SPI1->DR=data;
        while((SPI2->SR&SPI_SR_BSY)){}
        ld_set();ld_reset();
    }
private:
    void spi2_ini()
    {
        //************ Настройка SPI - master ***************************
        RCC->APB1ENR|=RCC_APB1ENR_SPI2EN;
        SPI2->CR1|=(SPI_CR1_BR_2|SPI_CR1_BR_1|SPI_CR1_BR_0); // 0:0:0  psk=2 => 36MHz/2 = 18MHz  => spi rate
        SPI2->CR1&=~SPI_CR1_BIDIMODE; // двухпроводной режим работы
        SPI2->CR1|=SPI_CR1_BIDIOE; //1: Output enabled (transmit-only mode)
        SPI2->CR1&=~SPI_CR1_CRCEN; // 0: CRC calculation disabled
        SPI2->CR1&=~SPI_CR1_CRCNEXT; //0: Data phase (no CRC phase)
        //SPI2->CR1&=~SPI_CR1_DFF; //0: 8-bit data frame format is selected for transmission/reception
        SPI2->CR1|=SPI_CR1_DFF; //1: 16-bit data frame format is selected for transmission/reception
        SPI2->CR1&=~SPI_CR1_RXONLY; //0: Full duplex (Transmit and receive)
        SPI2->CR1|=SPI_CR1_SSM; // программное управление ведомым включено 
        SPI2->CR1|=SPI_CR1_SSI; // нужно чтобы эти два бита были в 1 для правильной инициализации мастера
        SPI2->CR1&=~SPI_CR1_LSBFIRST; //1: LSB first //0: MSB transmitted first
        SPI2->CR1|=SPI_CR1_MSTR; //1: Master configuration
        SPI2->CR1&=~SPI_CR1_CPOL; //1: CK to 1 when idle (смотреть в datasheet slave) 
        SPI2->CR1&=~SPI_CR1_CPHA; //1: The second clock transition is the first data capture edge (тоже)
        //SPI1->CR1|=SPI_CR1_SPE;
        
        
        SPI2->CR1|=SPI_CR1_SPE; // 1 - spi2 enable

        //****** Настройка ножек: b12-CS, b13-SCK, b14-MISO, b15-MOSI
        //-------------------------- b12 hardware CS ----------------------------------------
        RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
        //RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;
        GPIOB->CRH&=~GPIO_CRH_CNF12;
        GPIOB->CRH|=GPIO_CRH_MODE12;  //CS 1:1 output max speed CNF-0:0 push-pull

        
        GPIOB->CRH|=GPIO_CRH_CNF13_1;
        GPIOB->CRH&=~GPIO_CRH_CNF13_0; //   //1:0 alt func push pull max speed
        GPIOB->CRH|=GPIO_CRH_MODE13;  //SCK  1:1 

        
        //GPIOB->CRH|=GPIO_CRH_CNF14_1;
        //GPIOB->CRH&=~GPIO_CRH_CNF14_0; //0:1 floating input  // 1:0 alt func push pull max speed
        //GPIOB->CRH&=~GPIO_CRH_MODE14;  //MISO  0:0  internal pullup

        
        GPIOB->CRH|=GPIO_CRH_CNF15_1;
        GPIOB->CRH&=~GPIO_CRH_CNF15_0; //1:0 alt func push pull max speed
        GPIOB->CRH|=GPIO_CRH_MODE15;  //MOSI  1:1 

        //***************  PB11 - LD for External DAC **********************************
        GPIOB->CRH&=~GPIO_CRH_CNF10;
        GPIOB->CRH|=GPIO_CRH_MODE10;  //CS 1:1 output max speed CNF-0:0 push-pull
    }
};



#endif //SPI2_H