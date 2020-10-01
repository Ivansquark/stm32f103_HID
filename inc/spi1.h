#ifndef SPI1_H_
#define SPI1_H_

#include "main.h"

class Spi1_master
{
public:
    Spi1_master()
    {
        spi1_ini();
    }
    void cs_set()
    {
        GPIOB->BSRR|=GPIO_BSRR_BR4;
        GPIOC->BSRR|=GPIO_BSRR_BR13;
    }
    void cs_idle()
    {
        GPIOB->BSRR|=GPIO_BSRR_BS4;
        GPIOC->BSRR|=GPIO_BSRR_BS13;
    }    
    void sendByte(uint8_t data)
    {
        while(!(SPI1->SR&SPI_SR_TXE)){}        
        SPI1->DR=data;
        //*(uint8_t*)SPI1->DR=data;
        while((SPI1->SR&SPI_SR_BSY)){}
    }
private:
    void spi1_ini()
    {
        //****** Настройка ножек: b12-CS, b13-SCK, b14-MISO, b15-MOSI
        RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
        //RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;
        GPIOA->CRL&=~GPIO_CRL_CNF4;
        GPIOA->CRL|=GPIO_CRL_MODE4;  //CS 1:1 output max speed CNF-0:0 push-pull

        
        GPIOA->CRL|=GPIO_CRL_CNF5_1;
        GPIOA->CRL&=~GPIO_CRL_CNF5_0; //   //1:0 alt func push pull max speed
        GPIOA->CRL|=GPIO_CRL_MODE5;  //SCK  1:1 

        
        //GPIOB->CRH|=GPIO_CRH_CNF14_1;
        //GPIOB->CRH&=~GPIO_CRH_CNF14_0; //0:1 floating input  // 1:0 alt func push pull max speed
        //GPIOB->CRH&=~GPIO_CRH_MODE14;  //MISO  0:0  internal pullup

        
        GPIOA->CRL|=GPIO_CRL_CNF7_1;
        GPIOA->CRL&=~GPIO_CRL_CNF7_0; //1:0 alt func push pull max speed
        GPIOA->CRL|=GPIO_CRL_MODE7;  //MOSI  1:1 

        //************ Настройка SPI - master ***************************
        RCC->APB2ENR|=RCC_APB2ENR_SPI1EN;
        SPI1->CR1|=SPI_CR1_BR_2|SPI_CR1_BR_1|SPI_CR1_BR_0; // 0:0:0  psk=2 => 48MHz/256  => spi rate
        SPI1->CR1&=~SPI_CR1_BIDIMODE; // двухпроводной режим работы
        SPI1->CR1|=SPI_CR1_BIDIOE; //1: Output enabled (transmit-only mode)
        SPI1->CR1&=~SPI_CR1_CRCEN; // 0: CRC calculation disabled
        SPI1->CR1&=~SPI_CR1_CRCNEXT; //0: Data phase (no CRC phase)
        SPI1->CR1&=~SPI_CR1_DFF; //0: 8-bit data frame format is selected for transmission/reception
        SPI1->CR1&=~SPI_CR1_RXONLY; //0: Full duplex (Transmit and receive)
        SPI1->CR1|=SPI_CR1_SSM; // программное управление ведомым включено 
        SPI1->CR1|=SPI_CR1_SSI; // нужно чтобы эти два бита были в 1 для правильной инициализации мастера
        SPI1->CR1&=~SPI_CR1_LSBFIRST; //1: LSB first //0: MSB transmitted first
        SPI1->CR1|=SPI_CR1_MSTR; //1: Master configuration
        SPI1->CR1&=~SPI_CR1_CPOL; //1: CK to 1 when idle (смотреть в datasheet slave) 
        SPI1->CR1&=~SPI_CR1_CPHA; //1: The second clock transition is the first data capture edge (тоже)
        SPI1->CR1|=SPI_CR1_SPE;
    }
};



#endif //SPI1_H