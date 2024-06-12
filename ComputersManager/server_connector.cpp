#include "server_connector.h"

ServerConnector::ServerConnector(const ServerInfo& server_info) :
	session_(nullptr, Libssh2SessionDeleter())
{
	int ret = 0;

	WSADATA wsaData;
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != NO_ERROR) {
		throw std::runtime_error("wsa start up failed");
	}
	
	ret = libssh2_init(0);
	if (ret != 0) {
		throw std::runtime_error("libssh2 init failed");
	}

	sock_ = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_ < 0) {
		throw std::runtime_error("socket creat failed");
	}

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(server_info.port);
	ret = inet_pton(AF_INET,server_info.ip.c_str(), &sin.sin_addr);
	

	ret = connect(sock_, (struct sockaddr*)&sin, sizeof(sin));
	if (ret != 0) {
		throw std::runtime_error("connect to server failed");
	}
	
	session_.reset(libssh2_session_init());
	if (session_ == nullptr) {
		throw std::runtime_error("session init failed");
	}

	libssh2_session_set_blocking(session_.get(), 1);
	ret = libssh2_session_handshake(session_.get(), sock_);
	if (ret != 0) {
		throw std::runtime_error("session handshake failed");
	}

	ret = libssh2_userauth_password(session_.get(), server_info.user.c_str(), server_info.password.c_str());
	if (ret != 0) {
		throw std::runtime_error("userauth failed");
	}


}

ServerConnector::~ServerConnector()
{
	closesocket(sock_);
}
