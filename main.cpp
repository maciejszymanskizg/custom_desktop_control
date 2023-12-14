#include "Logger.h"
#include "TrainConfiguration.h"
#include "I2C.h"
#include "UART.h"

int main(int argc, char *argv[])
{
	int result = -1;
	ICommunicationHandler *i2c;
	ICommunicationHandler *uart;
	Configuration *conf;

	log_debug("Custom desktop control\n");
	log_info("Compilation %s %s\n", __DATE__, __TIME__);

	i2c = new I2C("/dev/i2c0");
	if (i2c == nullptr) {
		log_error("Could not create I2C communication handler.\n");
		goto err;
	}

	uart = new UART("/dev/ttyUSB0", UART::Baudrate::BAUDRATE_115200);
	if (uart == nullptr) {
		log_error("Could not create UART communication handler.\n");
		goto delete_i2c;
	}

	conf = new TrainConfiguration();
	if (conf == nullptr) {
		log_error("Could not create EU07 configuration.\n");
		goto delete_uart;
	}

	result = 0;

	delete conf;
delete_uart:
	delete uart;
delete_i2c:
	delete i2c;
err:
	return result;
}
