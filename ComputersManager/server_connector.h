#pragma once
#include "libssh2.h"
#include "libssh2_sftp.h"
#include "memory"
#include "stdexcept"
#include "string"
#include "WinSock2.h"
#include "WS2tcpip.h"
#include "common_tools.h"
#include "cstring"
#include "fstream"
#include "algorithm"
#include "sys/stat.h"
#include "cstdio"
#include "filesystem"
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

struct Libssh2ChannelDeleter {
	void operator()(LIBSSH2_CHANNEL* channel) {
		BDEBUG("channel delete");
		if (channel != nullptr) {
			libssh2_channel_close(channel);
			libssh2_channel_free(channel);
		}
	}
};

struct Libssh2SftpSessionDeleter {
	void operator()(LIBSSH2_SFTP* sftp_sessoin) {
		BDEBUG("sftp session delete");
		if (sftp_sessoin != nullptr) {
			libssh2_sftp_shutdown(sftp_sessoin);
		}
	}
};

struct Libssh2SftpHandleDeleter {
	void operator()(LIBSSH2_SFTP_HANDLE* sftp_handle) {
		BDEBUG("sftp handle delete");
		if (sftp_handle != nullptr) {
			libssh2_sftp_close(sftp_handle);
		}
	}
};

class ServerConnector
{
public:
	ServerConnector(const ServerInfo& server_info);
	~ServerConnector();
	std::string ExecCommand(const std::string& command);
	void DownloadFileFromServer2Local(const std::string& server_path, const std::string& local_path);
	void UploadFileFromLocal2Server(const std::string& local_path, const std::string& server_path);

private:
	SOCKET sock_;
	std::unique_ptr<LIBSSH2_SESSION, Libssh2SessionDeleter> session_;
};

