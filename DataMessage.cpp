#include <string.h>
#include "DataMessage.h"
#include "Logger.h"

#define DATA_MESSAGE_MAGIC 0x39fd47ca

DataMessage::DataMessage(enum MessageType type) : data(nullptr), data_size(0), message_type(type)
{
	this->message_type = type;
	this->header.magic = DATA_MESSAGE_MAGIC;
	this->header.message_type = type;
	this->header.number_of_items = 0;
}

DataMessage::DataMessage(const uint8_t *buffer, size_t size) : data(nullptr), data_size(0),
	message_type(MESSAGE_TYPE_INVALID_MESSAGE)
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
			MessageItem item;
			for (uint32_t i = 0; i < this->header.number_of_items; i++) {
				memcpy(&item, &buffer[sizeof(struct MessageHeader) + i * sizeof(struct MessageItem)], sizeof(struct MessageItem));
				this->items.push_back(item);
			}
		}
	} else {
		log_error("Incorrect size of data message (%u)\n", size);
	}
}

DataMessage::~DataMessage()
{
	if (this->data != nullptr)
		delete this->data;
}

void DataMessage::addMessageItem(struct MessageItem item)
{
	this->items.push_back(item);
}

uint8_t *DataMessage::getRawData(size_t & size)
{
	if (this->data == nullptr)
		delete this->data;

	this->data = new uint8_t(sizeof(struct MessageHeader) * this->header.number_of_items * sizeof(struct MessageItem));

	if (this->data != nullptr) {
		memcpy(this->data, &this->header, sizeof(struct MessageHeader));

		for (uint32_t i = 0; i < this->header.number_of_items; i++) {
			memcpy(&this->data[sizeof(struct MessageHeader) + i * sizeof(struct MessageItem)],
					&this->items[i], sizeof(struct MessageItem));
		}

		this->data_size = sizeof(struct MessageHeader) * this->header.number_of_items * sizeof(struct MessageItem);
		size = this->data_size;
	} else {
		log_error("Memory allocation error.\n");
		this->data_size = 0;
	}

	return this->data;
}

DataMessage::MessageType DataMessage::getMessageType(void)
{
	return static_cast<MessageType>(this->header.message_type);
}

std::vector<struct MessageItem> & DataMessage::getMessageItems(void)
{
	return this->items;
}
