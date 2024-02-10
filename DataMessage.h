#ifndef DATA_MESSAGE_H
#define DATA_MESSAGE_H

#include <stdint.h>
#include <vector>

class DataMessage {
	public:
		struct MessageHeader {
			uint32_t magic;
			uint16_t message_type;
			uint16_t number_of_items;
		};
		struct MessageItem {
			uint32_t id;
			uint32_t value;
		};

	private:

		struct MessageHeader header;
		std::vector<struct MessageItem> items;
		uint8_t *data;
		size_t data_size;

	public:
		enum MessageType {
			MESSAGE_TYPE_INVALID_MESSAGE = 0x0,
			MESSAGE_TYPE_REQUEST_DATA = 0x1,
			MESSAGE_TYPE_RESPONSE_DATA = 0x2,
			MESSAGE_TYPE_SEND_DATA = 0x4,
			MESSAGE_TYPE_CONFIRM_DATA = 0x8
		};

		DataMessage(MessageType type);
		DataMessage(const uint8_t *buffer, size_t size);
		~DataMessage();

		void addMessageItem(const uint32_t id, const uint32_t value);
		uint8_t *getRawData(size_t & size);

		enum MessageType getMessageType(void);
		std::vector<struct MessageItem> & getMessageItems(void);

	private:
		enum MessageType message_type;
};

#endif /* DATA_MESSAGE_H */
