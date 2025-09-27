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
#include "GlobalConfiguration.h"
#include "GlobalConfigurationIDs.h"
#include "MaszynaUART.h"
#include "MaszynaTCPIP.h"
#include "DummyInputController.h"
#include "DummyOutputController.h"
#include "I2C.h"
#include "UART.h"
#include "TCPIP.h"
#include "VirtEU07.h"
#include "PhysEU07.h"
#include "VirtualControlTCPIP.h"
#include "JsonTCPIP.h"

enum InputControllerType {
	INPUT_CONTROLLER_TYPE_UNKNOWN,
	INPUT_CONTROLLER_TYPE_MASZYNA_UART,
	INPUT_CONTROLLER_TYPE_MASZYNA_TCPIP,
	INPUT_CONTROLLER_TYPE_VIRTUAL_CONTROL_TCPIP,
	INPUT_CONTROLLER_TYPE_DUMMY,
	INPUT_CONTROLLER_TYPE_JSON_TCPIP
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
	char i2c_node[32];
	char input_ip[32];
	unsigned short input_port;
	bool input_server;
	char output_ip[32];
	unsigned short output_port;
	bool output_server;
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
	Configuration *train_configuration;
	Configuration *input_configuration;
	Configuration *output_configuration;
	Configuration *global_configuration;
};

static volatile sig_atomic_t keep_running = 1;

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
	log_info("\t                           maszyna-tcpip\n");
	log_info("\t                           virtual-control-tcpip\n");
	log_info("\t                           dummy\n");
	log_info("\t                           json-tcpip\n");
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
	log_info("\t-s | --input-server  (optional) configure input TCPIP as server\n");
	log_info("\t-d | --output-ip     (optional) IP address for TCPIP connection in input engine\n");
	log_info("\t-r | --output-port   (optional) IP port for TCPIP connection in input engine\n");
	log_info("\t-l | --log-changes   (optional) Print configuration value changes\n");
	log_info("\t-e | --output-server (optional) configure output TCPIP as server\n");
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
		{ "input-server", no_argument, NULL, 's' },
		{ "output-ip", required_argument, NULL, 'd' },
		{ "output-port", required_argument, NULL, 'r' },
		{ "output-server", no_argument, NULL, 'e' },
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
					} else if (strcmp(optarg, "maszyna-tcpip") == 0) {
						params.inputControllerType = INPUT_CONTROLLER_TYPE_MASZYNA_TCPIP;
					} else if (strcmp(optarg, "virtual-control-tcpip") == 0) {
						params.inputControllerType = INPUT_CONTROLLER_TYPE_VIRTUAL_CONTROL_TCPIP;
					} else if (strcmp(optarg, "dummy") == 0) {
						params.inputControllerType = INPUT_CONTROLLER_TYPE_DUMMY;
					} else if (strcmp(optarg, "json-tcpip") == 0) {
						params.inputControllerType = INPUT_CONTROLLER_TYPE_JSON_TCPIP;
					} else {
						log_error("Unsupported input controller type [%s].\n", optarg);
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
						snprintf(params.i2c_node, sizeof(params.i2c_node), "%s", optarg);
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
						snprintf(params.input_ip, sizeof(params.input_ip), "%u.%u.%u.%u",
									a, b, c, d);
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
						snprintf(params.output_ip, sizeof(params.output_ip), "%u.%u.%u.%u",
									a, b, c, d);
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
			case 's':
				{
					params.input_server = true;
					break;
				}
			case 'e':
				{
					params.output_server = true;
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
	options.params.log_changes = false;
	options.params.input_server = false;
	options.params.output_server = true;
	options.i2c_handler = nullptr;
	options.uart_handler = nullptr;
	options.tcpip_input_handler = nullptr;
	options.tcpip_output_handler = nullptr;
	options.train_configuration = new TrainConfiguration();
	options.input_configuration = options.train_configuration->clone("input configuration");
	options.output_configuration = options.train_configuration->clone("output_configuration");
	options.input_configuration->addExternalConfiguration(options.output_configuration);
	options.output_configuration->addExternalConfiguration(options.input_configuration);
	options.global_configuration = new GlobalConfiguration();
	options.input_controller = nullptr;
	options.output_controller = nullptr;
}

bool setup(struct MainOptions & options)
{
	bool result = false;

	if (options.params.inputControllerType == INPUT_CONTROLLER_TYPE_MASZYNA_UART) {
		if (options.params.uart_node.size() > 0) {
			options.uart_handler = new UART(options.params.uart_node.c_str(), options.params.uart_baudrate, &keep_running);
			bool send_packet_when_received = options.global_configuration->getValue(CONFIGURATION_ID_MASZYNA_UART_CHECK_PACKET_RECEIVED);
			MaszynaUART *maszynaUART = new MaszynaUART(options.uart_handler, options.input_configuration, send_packet_when_received, options.params.log_changes);
			options.input_controller = dynamic_cast<IController *>(maszynaUART);
		} else {
			log_error("UART device not specified.\n");
			goto out;
		}
	} else if (options.params.inputControllerType == INPUT_CONTROLLER_TYPE_MASZYNA_TCPIP) {
		if (strlen(options.params.input_ip) > 0) {
			if (options.params.input_port != USHRT_MAX) {
				TCPIP::Mode mode = options.params.input_server ? TCPIP::Mode::TCPIP_MODE_SERVER : TCPIP::Mode::TCPIP_MODE_CLIENT;
				TCPIP *tcpip = new TCPIP(mode, options.params.input_ip, options.params.input_port, &keep_running);
				MaszynaTCPIP *maszynaTCPIP = new MaszynaTCPIP(tcpip, options.input_configuration, options.params.log_changes);
				options.tcpip_input_handler = dynamic_cast<ICommunicationHandler *>(tcpip);
				options.input_controller = dynamic_cast<IController *>(maszynaTCPIP);
			} else {
				log_error("TCPIP port of input controller not specified.\n");
				goto out;
			}
		} else {
			log_error("TCPIP ip address of input controller not specified.\n");
			goto out;
		}
	} else if (options.params.inputControllerType == INPUT_CONTROLLER_TYPE_VIRTUAL_CONTROL_TCPIP) {
		if (strlen(options.params.input_ip) > 0) {
			if (options.params.input_port != USHRT_MAX) {
				TCPIP::Mode mode = options.params.input_server ? TCPIP::Mode::TCPIP_MODE_SERVER : TCPIP::Mode::TCPIP_MODE_CLIENT;
				TCPIP *tcpip = new TCPIP(mode, options.params.input_ip, options.params.input_port, &keep_running);
				VirtualControlTCPIP *virtualControlTCPIP = new VirtualControlTCPIP(tcpip, options.input_configuration, options.params.log_changes);
				options.tcpip_input_handler = dynamic_cast<ICommunicationHandler *>(tcpip);
				options.input_controller = dynamic_cast<IController *>(virtualControlTCPIP);
			} else {
				log_error("TCPIP port of input controller not specified.\n");
				goto out;
			}
		} else {
			log_error("TCPIP ip address of input controller not specified.\n");
			goto out;
		}
	} else if (options.params.inputControllerType == INPUT_CONTROLLER_TYPE_DUMMY) {
		DummyInputController *dummy = new DummyInputController(options.input_configuration, options.params.log_changes);
		options.input_controller = dynamic_cast<IController *>(dummy);
	} else if (options.params.inputControllerType == INPUT_CONTROLLER_TYPE_JSON_TCPIP) {
		if (strlen(options.params.input_ip) > 0) {
			if (options.params.input_port != USHRT_MAX) {
				TCPIP *tcpip = new TCPIP(TCPIP::Mode::TCPIP_MODE_SERVER, options.params.input_ip, options.params.input_port, &keep_running);
				JsonTCPIP *jsonTCPIP = new JsonTCPIP(tcpip, options.input_configuration, options.params.log_changes);
				options.tcpip_input_handler = dynamic_cast<ICommunicationHandler *>(tcpip);
				options.input_controller = dynamic_cast<IController *>(jsonTCPIP);
			} else {
				log_error("TCPIP port of input controller not specified.\n");
				goto out;
			}
		} else {
			log_error("TCPIP ip address of input controller not specified.\n");
			goto out;
		}
	} else {
		log_error("Input controller not specified.\n");
		goto out;
	}

	if (options.params.outputControllerType == OUTPUT_CONTROLLER_TYPE_VIRT_EU07_TCPIP) {
		if (strlen(options.params.output_ip) > 0) {
			if (options.params.output_port != USHRT_MAX) {
				TCPIP::Mode mode = options.params.output_server ? TCPIP::Mode::TCPIP_MODE_SERVER : TCPIP::Mode::TCPIP_MODE_CLIENT;
				TCPIP *tcpip = new TCPIP(mode, options.params.output_ip, options.params.output_port, &keep_running);
				VirtEU07 *virteu07 = new VirtEU07(options.output_configuration, tcpip, options.params.log_changes);
				options.tcpip_output_handler = dynamic_cast<ICommunicationHandler *>(tcpip);
				options.output_controller = dynamic_cast<IController *>(virteu07);
			} else {
				log_error("TCPIP port of output controller not specified.\n");
				goto out;
			}
		} else {
			log_error("TCPIP ip address of output controller not specified.\n");
			goto out;
		}
	} else if (options.params.outputControllerType == OUTPUT_CONTROLLER_TYPE_PHYS_EU07_I2C) {
		if (strlen(options.params.i2c_node) > 0) {
			I2C *i2c = new I2C(options.params.i2c_node);
			PhysEU07 *physEU07 = new PhysEU07(options.output_configuration, options.global_configuration, i2c, options.params.log_changes);
			options.i2c_handler = dynamic_cast<ICommunicationHandler *>(i2c);
			options.output_controller = dynamic_cast<IController *>(physEU07);
		} else {
			log_error("I2C node of output controller not specified.\n");
			goto out;
		}
	} else if (options.params.outputControllerType == OUTPUT_CONTROLLER_TYPE_DUMMY) {
		DummyOutputController *dummy = new DummyOutputController(options.output_configuration, options.params.log_changes);
		options.output_controller = dynamic_cast<IController *>(dummy);
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
	if (options.i2c_handler != nullptr)
		delete options.i2c_handler;

	if (options.uart_handler != nullptr)
		delete options.uart_handler;

	if (options.tcpip_input_handler != nullptr)
		delete options.tcpip_input_handler;

	if (options.tcpip_output_handler != nullptr)
		delete options.tcpip_output_handler;

	if (options.input_controller != nullptr)
		delete options.input_controller;

	if (options.output_controller != nullptr)
		delete options.output_controller;

	if (options.train_configuration != nullptr)
		delete options.train_configuration;

	if (options.input_configuration != nullptr)
		delete options.input_configuration;

	if (options.output_configuration != nullptr)
		delete options.output_configuration;

	if (options.global_configuration != nullptr)
		delete options.global_configuration;
}

void input_controller_thread(struct MainOptions *options)
{
	unsigned int wait_us = options->global_configuration->getValue(CONFIGURATION_ID_INPUT_THREAD_SLEEP_US);
	log_debug("Starting input_controller thread\n");

	while (keep_running) {
		options->input_controller->sync(IController::SyncDirection::FROM_CONTROLLER);
		options->input_controller->sync(IController::SyncDirection::TO_CONTROLLER);

		usleep(wait_us);
	}

	log_debug("Exit input_controller thread\n");
}

void output_controller_thread(struct MainOptions *options)
{
	unsigned int wait_us = options->global_configuration->getValue(CONFIGURATION_ID_OUTPUT_THREAD_SLEEP_US);
	log_debug("Starting output_controller thread\n");

	while (keep_running) {
		options->output_controller->sync(IController::SyncDirection::TO_CONTROLLER);
		options->output_controller->sync(IController::SyncDirection::FROM_CONTROLLER);

		usleep(wait_us);
	}

	log_debug("Exit output_controller thread\n");
}

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
	std::thread *input_thread;
	std::thread *output_thread;
	unsigned int wait_us = 0;

	srand(time(NULL));
	clearOptions(options);

	if (parseParams(argc, argv, options.params) == false) {
		goto out;
	}

	if (setup(options) == false) {
		goto out;
	}

	signal(SIGINT, signalHandler);

	input_thread = new std::thread(input_controller_thread, &options);
	output_thread = new std::thread(output_controller_thread, &options);

	wait_us = options.global_configuration->getValue(CONFIGURATION_ID_MAIN_THREAD_SLEEP_US);

	while (keep_running) {
		usleep(wait_us);
	}

	input_thread->join();
	output_thread->join();

	delete input_thread;
	delete output_thread;

	status = EXIT_SUCCESS;

out:
	freeOptions(options);
	return status;
}
