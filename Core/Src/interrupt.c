// Copyright Jim Merkle, 11/20/2023
// File: interrupt.c
//
// Example interrupt code - Enable External interrupt for blue push-button

// When using STM32CubeIDE, with NUCLEO-F103RB board - default setup/configuration for the board,
//  everything is already configured and working for EXTI button presses.
// The problem is a STUB function is being called each time th button is pressed, vs doing something.
// See Drivers/STM32F1xx_HAL_Driver/Src/stm32f1xx_hal_gpio.c, HAL_GPIO_EXTI_Callback().
//  The default version of this function has the "weak" modifier, indicating this function can be
//  over-riden with one supplied by the user.

#include "main.h" // HAL, LL, and push button defines

extern uint32_t interrupt_counter; // main.c


// Over-ride the weak EXTI_Callback function.
// Have it toggle the LED2 LED Pin
// The 4.7K External Pull-Up, along with the .1uF capacitor create a fair hardware debounce.
// Based on configuration in the .ioc file, "trigger on rising edge", the interrupt is generated on the release of the button.
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	interrupt_counter++;
}

