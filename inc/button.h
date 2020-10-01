#ifndef BUTTON_H_
#define BUTTON_H_

#include "irq.hpp"

class Button: public Interruptable {
public:
    Button(){button_ini();}
    bool ButtonFlag{false};
    uint8_t Button_case{0};
    void InterruptHandle() override {ButtonFlag=true;}
private:
    void button_ini()    
    {
        RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
        GPIOA->CRL &= ~(GPIO_CRL_MODE0 | GPIO_CRL_CNF0);
        GPIOA->CRL|=GPIO_CRL_CNF0_1; // CNF0[1:0] 1:0 - input mode (pull-up, pull-down)
        GPIOA->ODR |= GPIO_ODR_ODR0; //set to 1,   Подтяжка вверх
        //AFIO->EXTICR[0] &= ~(AFIO_EXTICR1_EXTI0); //Нулевой канал EXTI подключен к порту PA0
        EXTI->RTSR=0; //rising trigger disabled
        //EXTI->RTSR &=~ EXTI_RTSR_TR0; //Прерывание по нарастанию импульса отключено
        EXTI->FTSR |= EXTI_FTSR_TR0; //Прерывание по спаду импульса
        EXTI->PR = EXTI_PR_PR0;      //Сбрасываем флаг прерывания 
                                     //перед включением самого прерывания
        EXTI->IMR |= EXTI_IMR_MR0;   //Включаем прерывание 0-го канала EXTI
        NVIC_EnableIRQ(EXTI0_IRQn);  //Разрешаем прерывание в контроллере прерываний
    }
};

extern "C" void EXTI0_IRQHandler(void)
{
    for(int i=0;i<720000;i++) {}  //ждем ~10 мс  от дребезга
    InterruptManager::Call(EXTI0_IRQn);
    EXTI->PR = EXTI_PR_PR0; //Сбрасываем флаг прерывания
}

#endif //BUTTON_H_