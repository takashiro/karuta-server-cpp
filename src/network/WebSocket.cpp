#include "WebSocket.h"
#include "TcpSocket.h"
#include "util/util.h"

BA_NAMESPACE_BEGIN

#define strnicmp _strnicmp
#define stricmp _stricmp

struct WebSocket::Private
{
	TcpSocket *socket;

	Private()
		: socket(nullptr)
	{
	}

	~Private()
	{
		if (socket) {
			socket->close();
			delete socket;
		}
	}

	static const char *readValue(const char *str)
	{
		while (*str && isspace(*str)) {
			str++;
		}
		return str;
	}
};

WebSocket::WebSocket()
	: d(new Private)
{
}

WebSocket::WebSocket(TcpSocket *socket)
	: d(new Private)
{
	std::string sec_key;
	char line[255];
	uint64 length;

	length = socket->readLine(line, 255);
	if (length == 0 || line[0] == '\0' || strnicmp(line, "GET ", 4) != 0) {
		socket->close();
		return;
	}

	for (;;) {
		length = socket->readLine(line, 255);
		if (length == 0 || line[0] == '\0') {
			break;
		}

		if (strnicmp(line, "Sec-WebSocket-Key:", 18) == 0) {
			sec_key = Private::readValue(line + 18);
		} else if (strnicmp(line, "Upgrade:", 8) == 0) {
			const char *value = Private::readValue(line + 8);

			if (strnicmp(value, "websocket", 9) != 0) {
				socket->close();
				return;
			}
		} else if (strnicmp(line, "Connection:", 11) == 0) {
			const char *value = Private::readValue(line + 11);

			if (strnicmp(value, "Upgrade", 7) != 0) {
				socket->close();
				return;
			}
		}
	}

	if (!sec_key.empty()) {
		sec_key.append("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
		std::string key_accept = base64_encode(sha1(sec_key));

		socket->write("HTTP/1.1 101 Switching Protocols\n");
		socket->write("Upgrade: websocket\n");
		socket->write("Connection: Upgrade\n");
		socket->write("Sec-WebSocket-Accept: ");
		socket->write(key_accept);
		socket->write("\n\n");

		d->socket = socket;
	} else {
		socket->close();
	}
}

WebSocket::~WebSocket()
{
	delete d;
}

bool WebSocket::open(const HostAddress &ip, ushort port)
{
	if (d->socket != nullptr) {
		return false;
	}

	TcpSocket *socket = new TcpSocket;
	bool opened = socket->open(ip, port);
	if (opened) {
		int sec_key_value[2] = {rand(), rand()};
		const uchar *sec_key = reinterpret_cast<const uchar *>(sec_key_value);
		std::string base64_sec_key = base64_encode(sec_key, 8);

		socket->write("GET / HTTP/1.1\n");
		socket->write("Connection: Upgrade\n");
		socket->write("Upgrade: websocket\n");
		socket->write("Sec-WebSocket-Version: 13\n");
		socket->write("Sec-WebSocket-Key: ");
		socket->write(base64_sec_key);
		socket->write("\n\n");

		char line[255];
		uint64 length = socket->readLine(line, 255);
		if (length == 0 || stricmp(line, "HTTP/1.1 101 Switching Protocols") != 0) {
			delete socket;
			return false;
		}

		std::string key_accept;
		for (;;) {
			length = socket->readLine(line, 255);
			if (length == 0 || line[0] == '\0') {
				break;
			}

			if (strnicmp(line, "Upgrade:", 8) == 0) {
				const char *value = Private::readValue(line + 8);
				if (strnicmp(value, "websocket", 9) != 0) {
					delete socket;
					return false;
				}
			} else if (strnicmp(line, "Connection:", 11) == 0) {
				const char *value = Private::readValue(line + 11);
				if (strnicmp(value, "upgrade", 9) != 0) {
					delete socket;
					return false;
				}
			} else if (strnicmp(line, "Sec-WebSocket-Accept:", 21) == 0) {
				key_accept = Private::readValue(line + 21);
			}
		}

		if (key_accept.empty()) {
			delete socket;
			return false;
		}

		std::string correct_key_accept = base64_sec_key;
		correct_key_accept.append("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
		correct_key_accept = base64_encode(sha1(correct_key_accept));

		if (correct_key_accept == key_accept) {
			d->socket = socket;
			return true;
		} else {
			delete socket;
			return false;
		}

	} else {
		delete socket;
	}

	return false;
}

void WebSocket::close()
{
	if (d->socket) {
		d->socket->close();
	}
}

std::string WebSocket::read()
{
	if (d->socket == nullptr) {
		return std::string();
	}

	char header[2];
	d->socket->read(header, 2);
	/*  0-7: FIN RSV1 RSV2 RSV3 opcode(4) */
	/* 8-15: MASK payload len(7) */

	uint64 payload_length = header[1] & 0x7f;
	if (payload_length == 126) {
		char ext_length[2];
		d->socket->read(ext_length, 2);
		payload_length = (ext_length[0] << 8) | ext_length[1];
	} else if (payload_length == 127) {
		char ext_length[8];
		d->socket->read(ext_length, 8);
		payload_length = 0;
		for (int i = 0; i < 8; i++) {
			payload_length <<= 8;
			payload_length |= ext_length[i];
		}
	}

	char mask[4];
	if (header[1] & 0x80) {
		d->socket->read(mask, 4);
	}

	char *data = new char[payload_length];
	d->socket->read(data, payload_length);

	int j = 0;
	for (uint64 i = 0; i < payload_length; i++) {
		data[i] ^= mask[j];
		j++;
		if (j == 4) {
			j = 0;
		}
	}

	std::string message;
	message.append(data, payload_length);
	delete[] data;

	return message;
}

bool WebSocket::write(const std::string &message, bool masked)
{
	char header[10] = {'\x81', '\0'};
	int header_length = 2;
	size_t length = message.length();
	if (length < 126) {
		header[1] = static_cast<char>(length);

	} else if (length <= 0xFFFF) {
		header[1] = 126;
		const char *bytes = reinterpret_cast<const char *>(&length);
		header[2] = bytes[1];
		header[3] = bytes[0];
		header_length = 4;
	} else {
		header[1] = 127;
		const char *bytes = reinterpret_cast<const char *>(&length);
		for (int i = 0; i < 8; i++) {
			header[2 + i] = bytes[7 - i];
		}
		header_length = 10;
	}

	if (masked) {
		uint32 mask = rand();
		const char *bytes = reinterpret_cast<const char *>(&mask);
		header[1] |= 0x80;
		d->socket->write(header, header_length);
		d->socket->write(bytes, 4);

		std::string masked_message = message;
		int j = 0;
		for (char &ch : masked_message) {
			ch ^= bytes[j];
			j++;
			if (j == 4) {
				j = 0;
			}
		}
		d->socket->write(masked_message);
	} else {
		d->socket->write(header, header_length);
		d->socket->write(message);
	}

	return true;
}

bool WebSocket::isConnected() const
{
	return d->socket != nullptr;
}

BA_NAMESPACE_END
