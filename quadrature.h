#ifndef QUADRATURE_H
#define QUADRATURE_H APR_2022

#include "gpio_irq_handler.h"

#include <pico/time.h>

/*
    An interrupt based quadrature encoder for the RP2040.

    Because the RP2040 can only have one gpio_isr the gpio_irq_handler is used to
    allow one function call to be registered per pin.

    This simple OOP approach to an encoder is enabled by static variables and templates:
    The interrupt service routines (isrs) are called without a reference to their object.
    The static keyword is used to still allow access to object variables.
    Static variables are the same for all instances of a class.
    So we trick the compiler into creating a new class for each encoder using a template.

    Instanciation may look somthing like this,
    note that "Quadrature_encoder<MY_DATA_PIN, MY_CLK_PIN>" is the name of the class:

    Quadrature_encoder<DATA_PIN, CLK_PIN> my_encoder = Quadrature_encoder<DATA_PIN, CLK_PIN>();

    After construction call begin() during setup and count() to retrieve the steps since the beginning.
    end() usually must not be called.

    resolution parameter:
    A_pin:
        _________       _________
        |       |       |       |
        |       |       |       |
    ____|       |_______|       |______
    B_pin:
            _________       _________
            |       |       |       |
            |       |       |       |
    ________|       |_______|       |__
    count +1:
        f   f   f   f   f   f   f   f   full
        h       h       h       h       half
                q               q       quarter
*/

enum class resolution
{
    full,
    half,
    quarter
};

enum class pull_direction
{
    none,
    up,
    down
};

template<int A_pin, int B_pin>
class Quadrature_encoder
{
public:
    //Returns the total count of steps.
    long count() { return cnt; }

    //Resets the count to 0 or a specified value.
    void reset_count(int new_value = 0) { count = new_value; }

    //Initializes the pins, pulls them as specified and attaches the interrupts
    void begin(pull_direction = pull_direction::none, resolution = resolution::quarter);

    //Detattches the interrupts and resets any pulling.
    //This function is also called during destruction.
    void end();

    ~Quadrature_encoder<A_pin, B_pin>();

private:
    static inline volatile int32_t cnt = 0;

    // ISRs
    static void on_A_change(uint gpio, uint32_t events);
	static void on_B_change(uint gpio, uint32_t events);
};



template<int A_pin, int B_pin>
Quadrature_encoder<A_pin, B_pin>::~Quadrature_encoder()
{
    end();
}

template<int A_pin, int B_pin>
void Quadrature_encoder<A_pin, B_pin>::begin(pull_direction pull_dir, resolution res)
{
    // configure the pins
    gpio_init(A_pin);
    gpio_init(B_pin);
    gpio_set_dir(A_pin, GPIO_IN);
    gpio_set_dir(B_pin, GPIO_IN);
    switch (pull_dir)
    {
    case pull_direction::up:
        gpio_pull_up(A_pin);
        gpio_pull_up(B_pin);
        break;
    case pull_direction::down:
        gpio_pull_down(A_pin);
        gpio_pull_down(B_pin);
        break;
    case pull_direction::none:
        gpio_disable_pulls(A_pin);
        gpio_disable_pulls(B_pin);
        break;
    }

    // configure the interrupts according to resolution parameter
    switch (res)
    {
    case resolution::quarter:
        irq_handler.register_interrupt(A_pin, gpio_irq_handler::irq_event::fall, Quadrature_encoder<A_pin, B_pin>::on_A_change);
        break;
    case resolution::half:
        irq_handler.register_interrupt(A_pin, gpio_irq_handler::irq_event::change, Quadrature_encoder<A_pin, B_pin>::on_A_change);
        break;
    case resolution::full:
        irq_handler.register_interrupt(A_pin, gpio_irq_handler::irq_event::change, Quadrature_encoder<A_pin, B_pin>::on_A_change);
        irq_handler.register_interrupt(B_pin, gpio_irq_handler::irq_event::change, Quadrature_encoder<A_pin, B_pin>::on_B_change);
        break;
    }
}

template<int A_pin, int B_pin>
void Quadrature_encoder<A_pin, B_pin>::end()
{
    irq_handler.unregister_interrupt(A_pin);
    irq_handler.unregister_interrupt(B_pin);

    gpio_disable_pulls(A_pin);
    gpio_disable_pulls(B_pin);
}

// ISRs
template<int A_pin, int B_pin>
void Quadrature_encoder<A_pin, B_pin>::on_A_change(uint gpio, uint32_t events)
{
    if (events == GPIO_IRQ_EDGE_RISE)
    {
        if (gpio_get(B_pin))
            --cnt;
        else
            ++cnt;
    }
    else
    {
        if (gpio_get(B_pin))
            ++cnt;
        else
            --cnt;
    }
}

template<int A_pin, int B_pin>
void Quadrature_encoder<A_pin, B_pin>::on_B_change(uint gpio, uint32_t events)
{
    if (events == GPIO_IRQ_EDGE_FALL)
    {
        if (gpio_get(A_pin))
            --cnt;
        else
            ++cnt;
    }
    else {
        if (gpio_get(A_pin))
            ++cnt;
        else
            --cnt;
    }
}

#endif