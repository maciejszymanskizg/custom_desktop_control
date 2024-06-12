#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>
#include <limits.h>
#include <signal.h>
#include <thread>
#include <chrono>
#include "Logger.h"
#include "TrainConfiguration.h"
#include "MaszynaUART.h"
#include "DummyInputController.h"
#include "DummyOutputController.h"
#include "I2C.h"
#include "UART.h"
#include "TCPIP.h"
#include "VirtEU07.h"
#include "PhysEU07.h"

enum InputControllerType {
	INPUT_CONTROLLER_TYPE_UNKNOWN,
	INPUT_CONTROLLER_TYPE_MASZYNA_UART,
	INPUT_CONTROLLER_TYPE_DUMMY
};

enum OutputControllerType {
	OUTPUT_CONTROLLER_TYPE_UNKOWN,
	OUTPUT_CONTROLLER_TYPE_VIRT_EU07_TCPIP,
	OUTPUT_CONTROLLER_TYPE_PHYS_EU07_I2C,
	OUTPUT_CONTROLLER_TYPE_DUMMY
};

struct MainParameters {
	enum InputControllerType inputControllerType;
	enum OutputControllerType outputControllerType;
	std::string uart_node;
	UART::Baudrate uart_baudrate;
	std::string i2c_node;
	std::string input_ip;
	unsigned short input_port;
	std::string output_ip;
	unsigned short output_port;
	bool log_changes;
};

struct MainOptions {
	struct MainParameters params;
	ICommunicationHandler *i2c_handler;
	ICommunicationHandler *uart_handler;
	ICommunicationHandler *tcpip_input_handler;
	ICommunicationHandler *tcpip_output_handler;
	IController *input_controller;
	IController *output_controller;
	Configuration *configuration;
	std::thread *main_thread;
	bool thread_running_flag;
};

IController *createMaszynaUARTInputController(ICommunicationHandler *communication_handler,
		Configuration *configuration)
{
	IController *ret = nullptr;

	if ((communication_handler == nullptr) ||
			(communication_handler->getHandlerType() !=
			 ICommunicationHandler::HandlerType::COMMUNICATION_HANDLER_UART)) {
		log_error("Invalid communication handler (expected UART)\n");
		goto out;
	}

	if (configuration == nullptr) {
		log_error("Invalid configuration\n");
		goto out;
	}

	ret = new MaszynaUART(communication_handler, configuration);

out:
	return ret;
}

void printVersion(void)
{
	log_info("Custom desktop control by Maciej Szymanski (maciej.szymanski.zg@gmail.com)\n");
	log_info("Application version : %s\n", VERSION);
	log_info("Compilation date & time : %s %s\n\n", __DATE__, __TIME__);
}

void printUsage(const char *name)
{
	log_info("Usage : %s [options]\n", name);
	log_info("\t-i | --input         (mandatory) input controller name\n");
	log_info("\t                        supported input controllers :\n");
	log_info("\t                           maszyna-uart\n");
	log_info("\t                           dummy\n");
	log_info("\t-o | --output        (mandatory) output controller name\n");
	log_info("\t                        supported output controllers\n");
	log_info("\t                           virt-eu07-tcpip\n");
	log_info("\t                           phys-eu07-i2c\n");
	log_info("\t                           dummy\n");
	log_info("\t-u | --uart          (optional) UART device node\n");
	log_info("\t-b | --uart-baudrate (optional) UART baudrate\n");
	log_info("\t-c | --i2c           (optional) I2C device node\n");
	log_info("\t-a | --input-ip      (optional) IP address for TCPIP connection in input engine\n");
	log_info("\t-p | --input-port    (optional) IP port for TCPIP connection in input engine\n");
	log_info("\t-d | --output-ip     (optional) IP address for TCPIP connection in input engine\n");
	log_info("\t-r | --output-port   (optional) IP port for TCPIP connection in input engine\n");
	log_info("\t-l | --log-changes   (optional) Print configuration value changes\n");
	log_info("\t-v | --version       (optional) print version string\n");
	log_info("\t-h | --help          (optional) print help information\n");
}

bool convertUARTBaudrate(unsigned int baud, UART::Baudrate & baudrate)
{
	bool result = true;

	switch (baud) {
		case 9600:
			{
				baudrate = UART::Baudrate::BAUDRATE_9600;
				break;
			}
		case 19200:
			{
				baudrate = UART::Baudrate::BAUDRATE_19200;
				break;
			}
		case 38400:
			{
				baudrate = UART::Baudrate::BAUDRATE_38400;
				break;
			}
		case 57600:
			{
				baudrate = UART::Baudrate::BAUDRATE_57600;
				break;
			}
		case 115200:
			{
				baudrate = UART::Baudrate::BAUDRATE_115200;
				break;
			}
		case 230400:
			{
				baudrate = UART::Baudrate::BAUDRATE_230400;
				break;
			}
		case 460800:
			{
				baudrate = UART::Baudrate::BAUDRATE_460800;
				break;
			}
		default:
			{
				log_error("Unsupported baudrate [%u].\n");
				result = false;
				break;
			}
	}

	return result;
}

bool parseParams(int argc, char *argv[], struct MainParameters & params)
{
	const char *getopt_string = "i:o:u:b:c:a:p:d:r:ilvh";
	int c, opt_index;
	bool result = true;

	struct option getopt_long_opts[] = {
		{ "input", required_argument, NULL, 'i' },
		{ "output", required_argument, NULL, 'o' },
		{ "uart", required_argument, NULL, 'u' },
		{ "uart-baudrate", required_argument, NULL, 'b' },
		{ "i2c", required_argument, NULL, 'c' },
		{ "input-ip", required_argument, NULL, 'a' },
		{ "input-port", required_argument, NULL, 'p' },
		{ "output-ip", required_argument, NULL, 'd' },
		{ "output-port", required_argument, NULL, 'r' },
		{ "log-changes", no_argument, NULL, 'l' },
		{ "version", no_argument, NULL, 'v' },
		{ "help", no_argument, NULL, 'h' },
		{ 0, 0, NULL, 0 },
	};

	while ((result) && ((c = getopt_long(argc, argv, getopt_string,
					getopt_long_opts, &opt_index)) != -1)) {
		switch (c) {
			case 'i':
				{
					if (strcmp(optarg, "maszyna-uart") == 0) {
						params.inputControllerType = INPUT_CONTROLLER_TYPE_MASZYNA_UART;
					} else if (strcmp(optarg, "dummy") == 0) {
						params.inputControllerType = INPUT_CONTROLLER_TYPE_DUMMY;
					} else {
						log_error("Unsupported input controller type [%s].\n");
						result = false;
					}
					break;
				}
			case 'o':
				{
					if (strcmp(optarg, "virt-eu07-tcpip") == 0) {
						params.outputControllerType = OUTPUT_CONTROLLER_TYPE_VIRT_EU07_TCPIP;
					} else if (strcmp(optarg, "phys-eu07-i2c") == 0) {
						params.outputControllerType = OUTPUT_CONTROLLER_TYPE_PHYS_EU07_I2C;
					} else if (strcmp(optarg, "dummy") == 0) {
						params.outputControllerType = OUTPUT_CONTROLLER_TYPE_DUMMY;
					} else {
						log_error("Unsupported output controller type [%s].\n");
						result = false;
					}
					break;
				}
			case 'u':
				{
					struct stat s;

					if (stat(optarg, &s) == 0) {
						params.uart_node = std::string(optarg);
					} else {
						log_error("UART device [%s] does not exists.\n");
						result = false;
					}
					break;
				}
			case 'b':
				{
					unsigned int baud;

					if (sscanf(optarg, "%u", &baud) == 1) {
						result = convertUARTBaudrate(baud, params.uart_baudrate);
					} else {
						log_error("Could not parse baudrate value [%s].\n", optarg);
						result = false;
					}
					break;
				}
			case 'c':
				{
					struct stat s;

					if (stat(optarg, &s) == 0) {
						params.i2c_node = std::string(optarg);
					} else {
						log_error("I2C device [%s] does not exists.\n");
						result = false;
					}
					break;
				}
			case 'a':
				{
					unsigned int a,b,c,d;

					if (sscanf(optarg, "%u.%u.%u.%u", &a, &b, &c, &d) == 4) {
						params.input_ip = std::string(optarg);
					} else {
						log_error("Invalid input controller IP address [%s].\n", optarg);
						result = false;
					}
					break;
				}
			case 'p':
				{
					unsigned int port;

					if ((sscanf(optarg, "%u", &port) == 1) && (port <= USHRT_MAX)) {
						params.input_port = (unsigned short) port;
					} else {
						log_error("Invalid input controller IP port [%s].\n", optarg);
						result = false;
					}
					break;
				}

			case 'd':
				{
					unsigned int a,b,c,d;

					if (sscanf(optarg, "%u.%u.%u.%u", &a, &b, &c, &d) == 4) {
						params.output_ip = std::string(optarg);
					} else {
						log_error("Invalid output controller IP address [%s].\n", optarg);
						result = false;
					}
					break;
				}
			case 'r':
				{
					unsigned int port;

					if ((sscanf(optarg, "%u", &port) == 1) && (port <= USHRT_MAX)) {
						params.output_port = (unsigned short) port;
					} else {
						log_error("Invalid output controller IP port [%s].\n", optarg);
						result = false;
					}

					break;
				}
			case 'l':
				{
					params.log_changes = true;
					break;
				}
			case 'v':
				{
					printVersion();
					exit(EXIT_SUCCESS);
					break;
				}
			case 'h':
				{
					printUsage(argv[0]);
					exit(EXIT_SUCCESS);
					break;
				}
			default:
				{
					break;
				}
		}
	}

	return result;
}

void clearOptions(struct MainOptions & options)
{
	options.params.inputControllerType = INPUT_CONTROLLER_TYPE_UNKNOWN;
	options.params.outputControllerType = OUTPUT_CONTROLLER_TYPE_UNKOWN;
	options.params.uart_baudrate = UART::Baudrate::BAUDRATE_9600;
	options.params.input_port = USHRT_MAX;
	options.params.output_port = USHRT_MAX;
	options.configuration = new TrainConfiguration();
	options.input_controller = nullptr;
	options.output_controller = nullptr;
	options.main_thread = nullptr;
	options.thread_running_flag = true;
}

bool setup(struct MainOptions & options)
{
	bool result = false;

	if (options.params.inputControllerType == INPUT_CONTROLLER_TYPE_MASZYNA_UART) {
		if (options.params.uart_node.size() > 0) {
			options.uart_handler = new UART(options.params.uart_node, options.params.uart_baudrate);
			options.input_controller = new MaszynaUART(options.uart_handler, options.configuration);
		} else {
			log_error("UART device not specified.\n");
			goto out;
		}
	} else if (options.params.inputControllerType == INPUT_CONTROLLER_TYPE_DUMMY) {
		options.input_controller = new DummyInputController(options.configuration);
	} else {
		log_error("Input controller not specified.\n");
		goto out;
	}

	if (options.params.outputControllerType == OUTPUT_CONTROLLER_TYPE_VIRT_EU07_TCPIP) {
		if (options.params.output_ip.size() > 0) {
			if (options.params.output_port != USHRT_MAX) {
				options.tcpip_output_handler = new TCPIP(TCPIP::Mode::TCPIP_MODE_SERVER,
						options.params.output_ip, options.params.output_port);
				options.output_controller = new VirtEU07(options.configuration, dynamic_cast<TCPIP *>(options.tcpip_output_handler));
			} else {
				log_error("TCPIP port of output controller not specified.\n");
				goto out;
			}
		} else {
			log_error("TCPIP ip address of output controller not specified.\n");
			goto out;
		}
	} else if (options.params.outputControllerType == OUTPUT_CONTROLLER_TYPE_PHYS_EU07_I2C) {
		if (options.params.i2c_node.size() > 0) {
			options.i2c_handler = new I2C(options.params.i2c_node);
			options.output_controller = new PhysEU07(options.configuration, options.i2c_handler);
		} else {
			log_error("I2C node of output controller not specified.\n");
			goto out;
		}
	} else if (options.params.outputControllerType == OUTPUT_CONTROLLER_TYPE_DUMMY) {
		options.output_controller = new DummyOutputController(options.configuration);
	} else {
		log_error("Output controller not specified.\n");
		goto out;
	}

	result = true;

out:
	return result;
}

void freeOptions(struct MainOptions & options)
{
	if (options.input_controller != nullptr)
		delete options.input_controller;

	if (options.output_controller != nullptr)
		delete options.output_controller;

	if (options.configuration != nullptr)
		delete options.configuration;

	if (options.main_thread != nullptr) {
		options.thread_running_flag = false;
		options.main_thread->join();
		delete options.main_thread;
	}
}

void mainThreadFunc(struct MainOptions & options)
{
	log_debug("Starting main thread.\n");

	while (options.thread_running_flag) {
		options.input_controller->sync(IController::SyncDirection::FROM_CONTROLLER);
		options.output_controller->sync(IController::SyncDirection::TO_CONTROLLER);
		if (options.params.log_changes)
			options.configuration->dumpConfigUpdates();
		options.configuration->cleanUpdates();

		options.output_controller->sync(IController::SyncDirection::FROM_CONTROLLER);
		options.input_controller->sync(IController::SyncDirection::TO_CONTROLLER);
		if (options.params.log_changes)
			options.configuration->dumpConfigUpdates();
		options.configuration->cleanUpdates();

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}

static volatile sig_atomic_t keep_running = 1;

static void signalHandler(int signal_no)
{
	(void)signal_no;
	keep_running = 0;
	log_debug("Interrupted.\n");
}

int main(int argc, char *argv[])
{
	int status = EXIT_FAILURE;
	struct MainOptions options;

	clearOptions(options);

	if (parseParams(argc, argv, options.params) == false) {
		goto out;
	}

	if (setup(options) == false) {
		goto out;
	}

	signal(SIGINT, signalHandler);

	options.main_thread = new std::thread(mainThreadFunc, std::ref(options));
	if (options.main_thread == nullptr) {
		log_error("Could not create main thread.\n");
		goto out;
	}

	while (keep_running) {
		std::this_thread::sleep_for(std::chrono::microseconds(25));
	}

	options.thread_running_flag = false;
	status = EXIT_SUCCESS;

out:
	freeOptions(options);
	return status;
}
