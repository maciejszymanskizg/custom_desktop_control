#include <string.h>
#include "DataMessage.h"
#include "Logger.h"

#define DATA_MESSAGE_MAGIC 0x39fd47ca

DataMessage::DataMessage(enum MessageType type) : message_type(type)
{
	this->message_type = type;
	this->header.magic = DATA_MESSAGE_MAGIC;
	this->header.message_type = type;
	this->header.number_of_items = 0;
}

DataMessage::DataMessage(const uint8_t *buffer, size_t size) : message_type(MESSAGE_TYPE_INVALID_MESSAGE)
{
	if (size >= sizeof(struct MessageHeader)) {
		memcpy(&this->header, buffer, sizeof(struct MessageHeader));
		if (this->header.magic != DATA_MESSAGE_MAGIC) {
			log_error("Incorrect data message magic value\n");
			this->message_type = MESSAGE_TYPE_INVALID_MESSAGE;
		} else if (size != sizeof(struct MessageHeader) + this->header.number_of_items * sizeof(struct MessageItem)) {
			log_error("Incorrect data message size\n");
			this->message_type = MESSAGE_TYPE_INVALID_MESSAGE;
		} else {
			for (uint32_t i = 0; i < this->header.number_of_items; i++) {
				struct MessageItem *item = new struct MessageItem;
				if (item) {
					memcpy(item, &buffer[sizeof(struct MessageHeader) + i * sizeof(struct MessageItem)], sizeof(struct MessageItem));
					this->items.push_back(item);
				} else {
					log_error("Memory allocation error.\n");
				}
			}
		}
	} else {
		log_error("Incorrect size of data message (%u)\n", size);
	}
}

DataMessage::~DataMessage()
{
	for (uint32_t i = 0; i < this->items.size(); i++) {
		delete this->items[i];
	}
}

void DataMessage::addMessageItem(const uint32_t id, const uint32_t value)
{
	struct MessageItem *item = new struct MessageItem;

	if (item) {
		item->id = id;
		item->value = value,

		this->items.push_back(item);
		this->header.number_of_items++;
	} else {
		log_error("Memory allocation error.\n");
	}
}

uint8_t *DataMessage::getRawData(size_t & size)
{
	size = sizeof(struct MessageHeader) + this->header.number_of_items * sizeof(struct MessageItem);
	uint8_t *data = new uint8_t[size];

	if (data != nullptr) {
		memcpy(&data[0], &this->header, sizeof(struct MessageHeader));

		for (uint32_t i = 0; i < this->header.number_of_items; i++) {
			struct MessageItem *item = this->items[i];
			memcpy(&data[sizeof(struct MessageHeader) + i * sizeof(struct MessageItem)],
					item, sizeof(struct MessageItem));
		}

	} else {
		log_error("Memory allocation error.\n");
	}

	return data;
}

DataMessage::MessageType DataMessage::getMessageType(void)
{
	return static_cast<MessageType>(this->header.message_type);
}

std::vector<struct DataMessage::MessageItem *> & DataMessage::getMessageItems(void)
{
	return this->items;
}
