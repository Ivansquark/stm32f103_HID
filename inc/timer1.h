#ifndef TIMER1_H_
#define TIMER1_H_

class Timer1PWM
{
public:
    Timer1PWM(){pwm_ini();}
    void setDuty(uint8_t procent)
    {
          TIM1->CCR4=procent*10; //duty cycle in 100%  
    }
private:
    void pwm_ini()
    {
        RCC->APB2ENR|=RCC_APB2ENR_TIM1EN;
        RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
        RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;
        GPIOA->CRH&=~GPIO_CRH_CNF11;
        GPIOA->CRH|=GPIO_CRH_CNF11_1;
        GPIOA->CRH&=~GPIO_CRH_CNF11_0; // 1:0 - output push pull
        GPIOA->CRH&=~GPIO_CRH_MODE11;
        GPIOA->CRH|=GPIO_CRH_MODE11;   // 1:1 - output 50MHz

        TIM1->CCMR2 |= TIM_CCMR2_OC4PE; // Включаем регистр предварительной загрузки компатратора канала 4
        TIM1->CCMR2&=~TIM_CCMR2_OC4M_0;
        TIM1->CCMR2|=TIM_CCMR2_OC4M_1;
        TIM1->CCMR2|=TIM_CCMR2_OC4M_2; //1:1:0 pwm straight polarity
        TIM1->CCMR2&=~TIM_CCMR2_CC4S;
        TIM1->CCER|=TIM_CCER_CC4E ;//| TIM_CCER_CC4NP; //signal on correponding pin
        TIM1->CCER&=~TIM_CCER_CC4P; //no inverting
        TIM1->BDTR=TIM_BDTR_MOE; // Main output enable 
        //OC and OCN outputs are enabled if their respective enable bits are set (CCxE, CCxNE in TIMx_CCER register).

        TIM1->PSC = 72-1;// TIM clk = 72000000  (clk APB2) => 1 MHz
        TIM1->ARR = 1000-1; // - auto reload register 1000000/1000 = 1000 Hz frequency
        //TIM4->DIER|=TIM_DIER_UIE; //interrupt at overload  Update interrupt enable
        TIM1->CR1 |= TIM_CR1_ARPE;//Включен режим предварительной записи регистра автоперезагрузки
        TIM1->DIER &= ~TIM_DIER_UIE;
        
        TIM1->CCR4=500; //duty cycle 50%
        //TIM1->EGR|=TIM_EGR_CC4G;
        //TIM1->EGR|=TIM_EGR_UG;
        
        //TIM1->BDTR|=TIM_BDTR_MOE;
        //TIM1->BDTR|=TIM_BDTR_OSSR;
        TIM1->CR1|=TIM_CR1_CEN;
        //NVIC_EnableIRQ(TIM4_IRQn); //irq enable 
    }
};

#endif //TIMER1_H_