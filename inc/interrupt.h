#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include "main.h" 

class InterruptManager
{
public:
    static void (*IsrV[88])();
    static void addHandler(void (*Fptr)(), IRQn irq){IsrV[irq] = Fptr;}
    static void call(IRQn irq){IsrV[irq]();}
};
class Interruptable
{
public:
    virtual ~Interruptable(){}
    virtual void run()=0;
};

template<IRQn irq>
class InterruptableClass
{
public:
    static void setVector(){InterruptManager::addHandler(irqHandlers,irq);}
    static void irqHandlers()
    {
        for(uint8_t i=0;i<5;i++)
        {
            if(arrInterruptableClasses[i]!=nullptr)
            {
                arrInterruptableClasses[i]->run();
            }
        }
    }
    static void addClass(Interruptable* cl)
    {
        for(uint8_t i=0;i<5;i++)
        {
            if(arrInterruptableClasses[i]==nullptr)
            {
                arrInterruptableClasses[i]=cl;break;
            }
        }
    }
    static Interruptable* arrInterruptableClasses[5];
};
template<IRQn irq>
Interruptable* InterruptableClass<irq>::arrInterruptableClasses[5]={nullptr};

#endif //INTERRUPT_H_