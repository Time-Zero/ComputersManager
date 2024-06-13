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
	libssh2_exit();
	WSACleanup();
}

std::string ServerConnector::ExecCommand(const std::string& command)
{
	std::string res = "";
	std::unique_ptr<LIBSSH2_CHANNEL, Libssh2ChannelDeleter> 
		channel(libssh2_channel_open_session(session_.get()), Libssh2ChannelDeleter());
	if (channel.get() == nullptr) {
		throw std::runtime_error("channel open session failed");
	}

	int ret = libssh2_channel_exec(channel.get(), command.c_str());
	if (ret != 0) {
		throw std::runtime_error("chanel exec command failed");
	}

	char buffer[4096];
	memset(buffer, 0, sizeof(buffer));
	int len = libssh2_channel_read(channel.get(), buffer, sizeof(buffer) - 1);
	if (len > 0) {
		buffer[len] = '\0';
		res.assign(buffer);
	}

	return res;
}

void ServerConnector::DownloadFileFromServer2Local(const std::string& server_path, const std::string& local_path)
{

	//从server中获取文件的大小
	int file_size = 0;
	try
	{
		std::string command = "ls -l " + server_path + " | awk '{print $5}'";
		auto ret = this->ExecCommand(command);
		file_size = atoi(ret.c_str());
	}
	catch (const std::exception& e)
	{
		BDEBUG(e.what());
		return;
	}

	std::unique_ptr<LIBSSH2_SFTP, Libssh2SftpSessionDeleter>
		sftp_session(libssh2_sftp_init(session_.get()), Libssh2SftpSessionDeleter());
	if (sftp_session.get() == nullptr) {
		throw std::runtime_error("sftp init failed");
	}

	std::unique_ptr<LIBSSH2_SFTP_HANDLE, Libssh2SftpHandleDeleter>
		sftp_handle(libssh2_sftp_open(sftp_session.get(), server_path.c_str(), LIBSSH2_FXF_READ, 0));
	if (sftp_handle.get() == nullptr) {
		throw std::runtime_error("sftp handle create failed");
	}

	std::ofstream out_file(local_path, std::ios::out);
	if (!out_file.is_open()) {
		throw std::runtime_error("local file open failed");
	}

	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	while (libssh2_sftp_read(sftp_handle.get(), buffer, sizeof(buffer)) > 0) {
		int byte_should_write = std::min(file_size, (int)sizeof(buffer));		// 比较剩余字节数，用于处理最后一次的写入
		out_file.write(buffer, byte_should_write);
		file_size = file_size - sizeof(buffer);
		memset(buffer, 0, sizeof(buffer));
	}

	out_file.flush();
	out_file.close();
}

void ServerConnector::UploadFileFromLocal2Server(const std::string& local_path, const std::string& server_path)
{
	//获取本地文件大小
	long file_size = 0;
	struct stat file_stat;
	if (stat(local_path.c_str(), &file_stat) == 0) {
		file_size = file_stat.st_size;
	}
	else {
		throw std::runtime_error("get local file size failed");
	}

	// 创建sftp会话
	std::unique_ptr<LIBSSH2_SFTP, Libssh2SftpSessionDeleter>
		sftp_session(libssh2_sftp_init(session_.get()), Libssh2SftpSessionDeleter());
	if (sftp_session.get() == nullptr) {
		throw std::runtime_error("sftp init failed");
	}

	// 创建服务器文件
	std::unique_ptr<LIBSSH2_SFTP_HANDLE, Libssh2SftpHandleDeleter>
		sftp_handle(libssh2_sftp_open(sftp_session.get(), 
			server_path.c_str(), 
			LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC,
			LIBSSH2_SFTP_S_IRUSR | LIBSSH2_SFTP_S_IWUSR | LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IROTH));
	if (sftp_handle.get() == nullptr) {
		throw std::runtime_error("sftp file open failed");
	}

	char buffer[1024];
	FILE* file = nullptr;
	fopen_s(&file,local_path.c_str(), "rb");
	if (file == nullptr) {
		throw std::runtime_error("file open failed");
	}

	int bytes = 0;
	do {
		memset(buffer, 0, sizeof(buffer));
		bytes = fread(buffer, 1, sizeof(buffer), file);
		if (bytes > 0) {
			libssh2_sftp_write(sftp_handle.get(), buffer, bytes);
		}
	} while (bytes > 0);
	fclose(file);

}


