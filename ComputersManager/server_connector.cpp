#include "server_connector.h"

/// @brief 构造函数
/// @param server_info 结构体，包含服务器信息，例如ip、端口、用户名和密码
ServerConnector::ServerConnector(const ServerInfo& server_info) :
	session_(nullptr, Libssh2SessionDeleter())			// 初始化会话，并且配置默认删除器
{
	int ret = 0;

	WSADATA wsaData;				// 初始化Windows网络服务
	ret = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (ret != NO_ERROR) {
		throw std::runtime_error("wsa start up failed");
	}
	
	ret = libssh2_init(0);		// 初始化libssh2库
	if (ret != 0) {
		throw std::runtime_error("libssh2 init failed");
	}

	sock_ = socket(AF_INET, SOCK_STREAM, 0);		// 使用Windows api创建一个socket
	if (sock_ < 0) {
		throw std::runtime_error("socket creat failed");
	}

	struct sockaddr_in sin;							// 执行socket信息，包括要连接的对端ip和端口，在这里也就是我们的数据库服务器的ip和端口
	sin.sin_family = AF_INET;
	sin.sin_port = htons(server_info.port);
	ret = inet_pton(AF_INET,server_info.ip.c_str(), &sin.sin_addr);
	

	ret = connect(sock_, (struct sockaddr*)&sin, sizeof(sin));		// 连接
	if (ret != 0) {
		throw std::runtime_error("connect to server failed");
	}
	
	session_.reset(libssh2_session_init());				// 智能指针reset，把置设置为libssh2创建的会话
	if (session_ == nullptr) {
		throw std::runtime_error("session init failed");
	}

	libssh2_session_set_blocking(session_.get(), 1);		// 设置会话为阻塞模式
	ret = libssh2_session_handshake(session_.get(), sock_);			// 会话握手
	if (ret != 0) {
		throw std::runtime_error("session handshake failed");
	}

	ret = libssh2_userauth_password(session_.get(), server_info.user.c_str(), server_info.password.c_str());		// 与服务器鉴权
	if (ret != 0) {
		throw std::runtime_error("userauth failed");
	}
}

/// @brief 析构函数
ServerConnector::~ServerConnector()		// 析构函数
{
	closesocket(sock_);		//	关闭socket
	libssh2_exit();			// libssh2库退出
	WSACleanup();			// 清理Windows网络服务
}

/// @brief 让服务器执行shell命令
/// @param command	要执行的命令
/// @return	命令执行后服务器的返回内容，比如执行ifconfig，返回网卡有关信息
std::string ServerConnector::ExecCommand(const std::string& command)		// 执行命令
{
	std::string res = "";					// 存放命令执行结果
	std::unique_ptr<LIBSSH2_CHANNEL, Libssh2ChannelDeleter>		// 使用智能指针托管通道指针，并且设置默认的删除器以防止throw后内存得不到释放
		channel(libssh2_channel_open_session(session_.get()), Libssh2ChannelDeleter());
	if (channel.get() == nullptr) {		
		throw std::runtime_error("channel open session failed");
	}

	int ret = libssh2_channel_exec(channel.get(), command.c_str());			// 执行命令
	if (ret != 0) {
		throw std::runtime_error("chanel exec command failed");
	}

	// 创建缓冲区从管道中读取服务器执行命令后的返回结果
	char buffer[4096];
	memset(buffer, 0, sizeof(buffer));
	int len = libssh2_channel_read(channel.get(), buffer, sizeof(buffer) - 1);
	if (len > 0) {
		buffer[len] = '\0';
		res.assign(buffer);
	}

	return res;
}

/// @brief 从服务器下载文件到本地
/// @param server_path 服务器文件位置
/// @param local_path 本地文件位置
void ServerConnector::DownloadFileFromServer2Local(const std::string& server_path, const std::string& local_path)	
{

	//从server中获取文件的大小
	int file_size = 0;
	try
	{
		std::string command = "ls -l " + server_path + " | awk '{print $5}'";		// shell指令，用户获取文件大小
		auto ret = this->ExecCommand(command);		// 执行shell指令，返回文件大小
		file_size = atoi(ret.c_str());				// 将string类型文件大小转换为int	（其实long long更好，我这里没有处理大文件的问题，大文件需要全局考虑和设计了）
	}
	catch (const std::exception& e)
	{
		BDEBUG(e.what());
		return;
	}

	std::unique_ptr<LIBSSH2_SFTP, Libssh2SftpSessionDeleter>		// 建立sftp服务，并且把指针交给智能指针托管
		sftp_session(libssh2_sftp_init(session_.get()), Libssh2SftpSessionDeleter());
	if (sftp_session.get() == nullptr) {
		throw std::runtime_error("sftp init failed");
	}

	std::unique_ptr<LIBSSH2_SFTP_HANDLE, Libssh2SftpHandleDeleter>		// 建立sftp文件描述指针
		sftp_handle(libssh2_sftp_open(sftp_session.get(), server_path.c_str(), LIBSSH2_FXF_READ, 0));
	if (sftp_handle.get() == nullptr) {
		throw std::runtime_error("sftp handle create failed");
	}

	std::ofstream out_file(local_path, std::ios::out);			// 本地建立文件
	if (!out_file.is_open()) {
		throw std::runtime_error("local file open failed");
	}

	// 从服务器上读取文件到本地
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	while (libssh2_sftp_read(sftp_handle.get(), buffer, sizeof(buffer)) > 0) {
		int byte_should_write = std::min(file_size, (int)sizeof(buffer));		// 比较剩余字节数，用于处理最后一次的写入
		out_file.write(buffer, byte_should_write);
		file_size = file_size - sizeof(buffer);
		memset(buffer, 0, sizeof(buffer));
	}

	out_file.flush();			// 冲刷缓冲区
	out_file.close();				// 关闭本地文件描述符
}

/// @brief 从本地上传文件到服务器
/// @param local_path	本地文件位置
/// @param server_path	服务器文件存放位置 
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

	// 创建服务器文件，并且指定默认权限为拥有者可读可写，其他可读
	std::unique_ptr<LIBSSH2_SFTP_HANDLE, Libssh2SftpHandleDeleter>
		sftp_handle(libssh2_sftp_open(sftp_session.get(), 
			server_path.c_str(), 
			LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC,
			LIBSSH2_SFTP_S_IRUSR | LIBSSH2_SFTP_S_IWUSR | LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IROTH));
	if (sftp_handle.get() == nullptr) {
		throw std::runtime_error("sftp file open failed");
	}

	// 从本地读取文件发送到服务器
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


