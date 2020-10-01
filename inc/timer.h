#ifndef TIMER_H
#define TIMER_H

#include "irq.h"
//#include "main.h"
//#include "stm32f10x.h"
//---------------------------------------------------------------
extern "C" void TIM2_IRQHandler(void);
extern "C" void TIM3_IRQHandler(void);
extern "C" void TIM4_IRQHandler(void);
//----------------------------------------------------------------------
extern "C" void TIM4_IRQHandler(void)
{
    TIM4->SR&=~TIM_SR_UIF; //drop Update interrupt flag
    //GPIOC->ODR ^= GPIO_ODR_ODR13;    
    InterruptManager::IsrVectors[TIM4_IRQn]();
}
extern "C" void TIM3_IRQHandler(void)
{
    TIM3->SR&=~TIM_SR_UIF; //drop Update interrupt flag
    //GPIOC->ODR ^= GPIO_ODR_ODR13;    
    InterruptManager::IsrVectors[TIM3_IRQn]();
}
extern "C" void TIM2_IRQHandler(void)
{
    TIM2->SR&=~TIM_SR_UIF; //drop Update interrupt flag    
    InterruptManager::IsrVectors[TIM2_IRQn]();
}
//---------------- General purpose timers 2-3-4---------------
class Timer
{
public:
    Timer(uint8_t timerNum)
    {
        timer_ini(timerNum);
    }    
private:
    void timer_ini(uint8_t timerNum)
    {
        switch(timerNum)
        {
            case 2:
            RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;
            TIM2->PSC = 72;// TIM clk = 72000000  (clk APB1*2) => 1 MHz
            TIM2->ARR = 50-1; //50 us
            TIM2->DIER|=TIM_DIER_UIE; //interrupt at overload 
            TIM2->CR1|=TIM_CR1_CEN;
            NVIC_EnableIRQ(TIM2_IRQn); //irq enable
            break;
            case 3:
            RCC->APB1ENR|=RCC_APB1ENR_TIM3EN;
            TIM3->PSC = 7200;// TIM clk = 72000000  (clk APB1*2) => 10 kHz
            TIM3->ARR = 2000-1; //200 ms
            TIM3->DIER|=TIM_DIER_UIE; //interrupt at overload 
            TIM3->CR1|=TIM_CR1_CEN;
            NVIC_EnableIRQ(TIM3_IRQn); //irq enable
            break;
            case 4:
            RCC->APB1ENR|=RCC_APB1ENR_TIM4EN;
            TIM4->PSC = 7200;// TIM clk = 72000000  (clk APB1*2) => 10 kHz
            TIM4->ARR = 5000-1; //500 ms
            TIM4->DIER|=TIM_DIER_UIE; //interrupt at overload  Update interrupt enable
            TIM4->CR1|=TIM_CR1_CEN;
            NVIC_EnableIRQ(TIM4_IRQn); //irq enable 
            break;
        }        
    }
};

#endif //TIMER_H