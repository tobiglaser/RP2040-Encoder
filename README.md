# RP2040 Quadrature Encoder

## Encoder
An interrupt based quadrature encoder for the RP2040.

This simple OOP approach to an encoder is enabled by static variables and templates:
The interrupt service routines (isrs) are called without a reference to their object.
The static keyword is used to still allow access to object variables.
Static variables are the same for all instances of a class.
So we trick the compiler into creating a new class for each encoder using a template.

Instanciation may look somthing like this,
note that `Quadrature_encoder<MY_DATA_PIN, MY_CLK_PIN>` is the name of the class:
```
Quadrature_encoder<DATA_PIN, CLK_PIN> my_encoder = Quadrature_encoder<DATA_PIN, CLK_PIN>();
```
After construction call `begin()` during setup and `count()` to get the steps since the beginning.  
`end()` usually must not be called.

## GPIO Interrupt Handler
Because the RP2040 can only have one gpio_isr the `gpio_irq_handler` is used to
allow one function call to be registered per pin.

This header creates an interface using a jump table,
allowing the user to attach pin and core specific callback functions.

You may

`#define DEBUG_IRQ_DURATION_PIN 11 //for example`  
To debug the duration of your ISR with a logic analyser.
This will switch the pin high during the ISR and low otherwise.

and

`#define HIGHEST_INTERRUPT_PIN 15 //for example`  
To safe some memory in the jump table,
but disabling interrupts on pins higher than specified.

## CMake, Include, Build
  1. Clone this repo e.g. to `myproject/include/RP2040-Encoder`
  2. Add the repo the `.gitmodules` file as a submodule, adding:
    
    [submodule "Encoder"]
    path = include/RP2040-Encoder
    url = https://github.com/tobiglaser/RP2040-Encoder.git
    branch = main

  3. Tell CMake where to find the new include files,
    adding this somewhere after `add_executable()` in `CMakeLists.txt`:

    # Add Encoder
    target_include_directories(my_project PUBLIC include/RP2040-Encoder)

  4. Copy an example to your main.cpp and try it out.  
    Refer to
    [Pico-FreeRTOS-Project-Template](https://github.com/tobiglaser/Rpi-Pico-FreeRTOS-Project-Template)
    for a FreeRTOS template.

## Attribution
The approach using templates and static variables was inspired by
[zacsketches/Encoder](https://github.com/zacsketches/Encoder).