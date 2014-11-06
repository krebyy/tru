#include "buttons.h"

Buttons::Buttons()
{
	gpio_export(LEFT);
	gpio_set_dir(LEFT, INPUT_PIN);

	gpio_export(RIGHT);
	gpio_set_dir(RIGHT, INPUT_PIN);
}


// Retorna o estado dos botões
buttons_on_t Buttons::getStatus(void)
{
	unsigned int left;
	unsigned int right;
	buttons_on_t status;

	// Verifica o estado dos botões
	gpio_get_value(LEFT, &left);
	gpio_get_value(RIGHT, &right);

	// Estabelece o valor da variável de saída
	if(!left && !right) status = BOTH;
	else if(!left) status = LEFT;
	else if(!right) status = RIGHT;
	else status = FALSE;

	// Tratamento do debouncing
	if(status != FALSE) usleep(250000);

	return status;
}
