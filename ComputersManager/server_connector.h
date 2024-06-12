#pragma once
#include "libssh2.h"
#include "memory"
#include "stdexcept"
#include "string"
#include "WinSock2.h"
#include "WS2tcpip.h"
#include "common_tools.h"
#pragma comment (lib, "ws2_32.lib")

typedef struct ServerInfo {
	std::string ip;
	int port;
	std::string user;
	std::string password;
} ServerInfo;

/// @brief Libssh2Session结构体删除器，让LIBSSH2_SESSION结构体指针能够被智能指针持有
struct Libssh2SessionDeleter {
	void operator()(LIBSSH2_SESSION* session) {
		BDEBUG("libssh2 delete")
		if (session != nullptr) {
			libssh2_session_disconnect(session, "Bye");
			libssh2_session_free(session);
		}
	}
};

class ServerConnector
{
public:
	ServerConnector(const ServerInfo& server_info);
	~ServerConnector();


private:
	SOCKET sock_;
	std::unique_ptr<LIBSSH2_SESSION, Libssh2SessionDeleter> session_;
};

