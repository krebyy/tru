#include "leds.h"

// Construtor
Leds::Leds()
{
	LED_RED = 115;
	LED_GREEN = 48;
	LED_BLUE = 49;
	FAROL = 22;

	gpio_export(LED_RED);
	gpio_set_dir(LED_RED, OUTPUT_PIN);
	gpio_set_value(LED_RED, OFF);

	gpio_export(LED_GREEN);
	gpio_set_dir(LED_GREEN, OUTPUT_PIN);
	gpio_set_value(LED_GREEN, OFF);

	gpio_export(LED_BLUE);
	gpio_set_dir(LED_BLUE, OUTPUT_PIN);
	gpio_set_value(LED_BLUE, OFF);
	
	gpio_export(FAROL);
	gpio_set_dir(FAROL, OUTPUT_PIN);
	gpio_set_value(FAROL, OFF);
}


// Atualiza a cor do LED RGB
void Leds::setColor(led_color_t color)
{
	ledsOff();

	switch(color)
	{
		case RED:
			gpio_set_value(LED_RED, ON);
			break;

		case GREEN:
			gpio_set_value(LED_GREEN, ON);
			break;

		case BLUE:
			gpio_set_value(LED_BLUE, ON);
			break;

		case YELLOW:
			gpio_set_value(LED_RED, ON);
			gpio_set_value(LED_GREEN, ON);
			break;

		case CYAN:
			gpio_set_value(LED_GREEN, ON);
			gpio_set_value(LED_BLUE, ON);
			break;

		case MAGENTA:
			gpio_set_value(LED_RED, ON);
			gpio_set_value(LED_BLUE, ON);
			break;

		case WHITE:
			gpio_set_value(LED_RED, ON);
			gpio_set_value(LED_GREEN, ON);
			gpio_set_value(LED_BLUE, ON);
			break;

		case NONE:
			ledsOff();
			break;
	}
}


// Apaga todos os LEDs
void Leds::ledsOff(void)
{
	gpio_set_value(LED_RED, OFF);
	gpio_set_value(LED_GREEN, OFF);
	gpio_set_value(LED_BLUE, OFF);
}


// Atualiza o estado do farol
void Leds::setFarol(PIN_VALUE value)
{
	gpio_set_value(FAROL, value);
}
