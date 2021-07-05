#include "cgi.hpp"

void print_headers_tmp(Client const &client, ConfigLocation location)
{
	// std::map<std::string, std::string> const &client.getHeaders() const; // to check
	// std::map<std::string, std::string> client.getHeaderAuthorization() const; // to check

	std::cout << "********************************" << std::endl;

	std::map<std::string, std::string> headers_ = client.getHeaders();
	std::map<std::string, std::string> autorization_ = client.getHeaderAuthorization();
	std::map<std::string, std::string>::iterator it;

	std::cout << "getHeaders: " << std::endl;
	for (it = headers_.begin(); it != headers_.end(); it++)
		std::cout << it->first << " = " << it->second << std::endl;
	std::cout << std::endl;
	std::cout << "getHeaderAuthorization: " << std::endl;
	for (it = autorization_.begin(); it != autorization_.end(); it++)
		std::cout << it->first << " = " << it->second << std::endl;

	std::cout << "********************************" << std::endl;

	std::cout << std::endl;

	std::cout << "client.getMethod(): " << client.getMethod() << std::endl;
	std::cout << "client.getUrl(): " << client.getUrl() << std::endl;
	std::cout << "cient.getHttpVersion() " << client.getHttpVersion() << std::endl;
	std::cout << "client.getBody(): " << client.getBody() << std::endl;
	// std::cout << "client.getQueryString(): " << client.getQueryString() << std::endl;
	std::cout << "client.getServerName(): " << client.getServerName() << std::endl;
	std::cout << "client.getPath(): " << client.getPath() << std::endl;
	std::cout << "client.getHeaderAcceptCharsets(): " << client.getHeaderAcceptCharsets() << std::endl;
	std::cout << "client.getHeaderAcceptLanguage(): " << client.getHeaderAcceptLanguage() << std::endl;
	std::cout << "client.getHeaderHost(): " << client.getHeaderHost() << std::endl;
	std::cout << "client.getHeaderReferer(): " << client.getHeaderReferer() << std::endl;
	std::cout << "client.getHeaderUserAgent(): " << client.getHeaderUserAgent() << std::endl;
	std::cout << "client.getHeaderContentLanguage(): " << client.getHeaderContentLanguage() << std::endl;
	std::cout << "client.getHeaderContentLength(): " << client.getHeaderContentLength() << std::endl;
	std::cout << "client.getHeaderDate(): " << client.getHeaderDate() << std::endl;
	std::cout << "client.getHeaderTransferEncoding(): " << client.getHeaderTransferEncoding() << std::endl;
	std::cout << "client.getFileName(): " << client.getFileName() << std::endl;

	std::cout << std::endl;

	std::cout << "location->getPrefix(): " << location.getPrefix() << std::endl;
	std::cout << "location->getMethodsList(): " << location.getMethodsList() << std::endl;
	std::cout << "location->getRoot(): " << location.getRoot() << std::endl;
	std::cout << "location->getAlias(): " << location.getAlias() << std::endl;
	std::cout << "location->getIndex(): " << location.getIndex() << std::endl;
	std::cout << "location->getAuthBasic(): " << location.getAuthBasic() << std::endl;
	std::cout << "location->getExtension(): " << location.getExtension() << std::endl;
	std::cout << "location->getScrypt(): " << location.getScrypt() << std::endl;
	std::cout << "location->getClientBodyBufferSize(): " << location.getClientBodyBufferSize() << std::endl;

	std::cout << std::endl;

	// std::map<std::string, bool> const &getMethods() const; // to check
	// bool getAutoindex() const; // to check
	// std::vector<std::string> getAuthBasicUserFile() const; // to check
	// std::map<std::string, bool> const &getFields() const; // to check

	std::cout << "********************************" << std::endl;
}

CGI::CGI(Client const &client, ConfigLocation location)
	: cgi_input_file_ptr_(new std::string),
	  client_(client),
	  location_(location)
{


	// print_headers_tmp(client, location);


	char cwd[PATH_MAX];
	getcwd(cwd, sizeof(cwd));

	std::string cgi_input_file;

	cgi_input_file = cwd;

	if (location_.getRoot().length() > 0)
	{
		cgi_input_file += location_.getRoot().substr(1);
		cgi_input_file += client_.getUrl().substr(1);
	}
	else
		cgi_input_file += location_.getAlias(); //to do

	*cgi_input_file_ptr_ = cgi_input_file;

	args_[0] = ft_strdup(location_.getScrypt().c_str());
	args_[1] = ft_strdup((*cgi_input_file_ptr_).c_str());
	args_[2] = NULL;

	std::string cgi_output_file;
	cgi_output_file = cwd;
	cgi_output_file += "/tests/body_cgi";

	cgi_output_file_ptr_ = new std::string;
	*cgi_output_file_ptr_ = cgi_output_file;

	env_ = NULL;
}

CGI::~CGI()
{
	// int i = 0;
	// std::map<std::string, std::string>::iterator it;

	// for (it = envMap_.begin(); it != envMap_.end(); it++)
	// {
	// 	free(env_[i]);
	// 	i++;
	// }
	// free(env_);

	// unlink((*cgi_output_file_ptr_).c_str()); // uncomment when ready

	free(args_[0]);
	free(args_[1]);

	delete cgi_input_file_ptr_;
	delete cgi_output_file_ptr_;
}

std::string readFile(std::string file)
{
	char buffer[BUFFER_SIZE + 1] = {0};
	int fd;
	int i;
	int res;
	std::string result;

	fd = open(file.c_str(), O_RDONLY);
	if (fd < -1)
	{
		std::cout << "Error" << std::endl;
		// throw ParsingException(0, "The file " + file + " does not exists.");
	}
	while ((res = read(fd, buffer, BUFFER_SIZE)) > 0)
	{
		result += buffer;
		i = 0;
		while (i < BUFFER_SIZE)
			buffer[i++] = 0;
	}
	// if (res < 0)
	// 	throw ParsingException(0, "Error while reading " + file + ".");
	close(fd);
	return (result);
}

void CGI::run()
{
	if (location_.getExtension() == ".php")
	{
		envMap_["REDIRECT_STATUS"] = "200";
		envMap_["QUERY_STRING"] = client_.getBody();
		envMap_["REQUEST_URI"] = "result.php"; // to do
	}
	else
	{
		envMap_["CONTENT_LENGTH"] = positiveNumberToString(client_.getBody().length());
		envMap_["CONTENT_TYPE"] = "test/file";					 // to do
		envMap_["HOST"] = client_.getServer().getServerIpPort();

		envMap_["PATH_INFO"] = "/";		 // to do
		envMap_["PATH_TRANSLATED"] = ""; // to do
		envMap_["QUERY_STRING"] = "";	 // to do

		envMap_["REQUEST_URI"] = ""; // to do
	}

	// https://tools.ietf.org/html/rfc3875#section-4.1.5

	// envMap_["PATH_INFO"] = client_.getPath();
	// envMap_["PATH_INFO"] = "./"; // to do
	// envMap_["CONTENT_TYPE"] = "text/html; charset=utf-8"; // to do

	envMap_["SERVER_NAME"] = client_.getServer().getServerName();
	envMap_["GATEWAY_INTERFACE"] = "CGI/1.1";
	envMap_["REMOTE_ADDR"] = client_.getServer().getServerIp();
	envMap_["REQUEST_METHOD"] = client_.getMethod();
	envMap_["SERVER_PORT"] = positiveNumberToString(size_t(client_.getServer().getPort()));

	// envMap_["PATH_TRANSLATED"] = (*cgi_input_file_ptr_).c_str();
	// envMap_["AUTH_TYPE"] = WEBSERV_AUTH;
	// envMap_["REMOTE_ADDR"] = "127.0.0.1";
	// envMap_["AUTH_TYPE"] = WEBSERV_AUTH;
	// envMap_["REMOTE_IDENT"] = "someuser"; // to do
	// envMap_["REMOTE_USER"] = "someuser"; // to do

	envMap_["SERVER_PROTOCOL"] = WEBSERV_HTTP;
	envMap_["SERVER_SOFTWARE"] = WEBSERV_NAME;

	int i = 0;
	std::map<std::string, std::string>::iterator it;

	if (!(env_ = (char **)malloc(sizeof(char *) * (envMap_.size() + 1))))
		throw(std::runtime_error(strerror(errno)));
	for (it = envMap_.begin(); it != envMap_.end(); it++)
	{
		envir_ = (it->first + "=" + it->second);
		if (!(env_[i] = ft_strdup(envir_.c_str())))
			throw(std::runtime_error(strerror(errno)));
		i++;
	}
	env_[i] = NULL;

	std::cout << std::endl;

	pid_t pid;
	int fd[2];

	if (pipe(fd) != 0)
	{
		std::cerr << "Pipe error: " << strerror(errno) << std::endl;
		exit(1);
	}
	// if (pipe(fd) == -1)
	//     throw CGIException("Cannot create pip to execute CGI.");

	int tmp_fd = open((*cgi_output_file_ptr_).c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRWXU); //0644
	if (tmp_fd < 0)
	{
		g_log.printErrorMessage("CGI fail to open file: " + *cgi_output_file_ptr_);
		close(tmp_fd);
		return ;
	}

	pid = fork();
	if (pid < 0)
	{
		g_log.printErrorMessage("CGI fork error: " + std::string(strerror(errno)));
		close(tmp_fd);
		exit(1);
	}

	if (pid == 0)
	{
		close(fd[1]);
		dup2(tmp_fd, 1);
		dup2(fd[0], 0);
		close(tmp_fd);
		close(fd[0]);

		int exec_res = execve(args_[0], args_, env_);
		if (exec_res < 0)
		{
			std::cerr << "fail to execve cgi" << std::endl;
			exit(exec_res);
		}
		exit(exec_res);
	}
	else
	{
		close(fd[0]);
		write(fd[1], client_.getBody().c_str(), client_.getBody().length()); 
		// write(fd[1], headers_.c_str(), headers_.length()); // tmp solution!
		close(fd[1]);
		waitpid(pid, NULL, 0);
		// _freeArgs(args);
		// _freeArgs(exec_args);
	}

	this->body_ = readFile(*cgi_output_file_ptr_);

	size_t tmp_s;
	size_t tmp_e;

	if (location_.getExtension() == ".php")
	{
		tmp_s = body_.find("X-Powered-By:");
		tmp_e = body_.find("\n");
		body_.erase(tmp_s, tmp_e + 1);

		tmp_s = body_.find("Content-type:");
		tmp_e = body_.find("\n");
		body_.erase(tmp_s, tmp_e + 1);

		this->headers_ += "X-Powered-By: PHP/8.0.3\r\n";
	}
	else
	{
		tmp_s = body_.find("Status:");
		tmp_e = body_.find("\r\n\r\n");
		body_.erase(tmp_s, tmp_e + 4);
	}

	// this->headers_ += "HTTP/1.1 200 OK\r\n";
	// this->headers_ += "Content-Length: 100000000\r\n";
	this->headers_ += "Content-Type: text/html; charset=utf-8\r\n"; // inportant for tester only!!!

	return;
}

std::string const &CGI::getHeaders() const
{
	return this->headers_;
}

std::string const &CGI::getBody() const
{
	return this->body_;
}

// CGI::CGI(Client const &client, ConfigLocation const &location, std::string path)
// 	: env_(NULL),
// 	  cgi_input_file_ptr_(new std::string),
// 	  cgi_output_file_ptr_(new std::string),
// 	  client_(client),
// 	  location_(location)
// {
// 	char cwd[PATH_MAX];
// 	getcwd(cwd, sizeof(cwd));

// 	*cgi_input_file_ptr_ = std::string(cwd) + path;
// 	*cgi_output_file_ptr_ = std::string(cwd) + "/tests/body_cgi";

// 	args_[0] = ft_strdup(location_.getScrypt().c_str());
// 	args_[1] = const_cast<char *>((*cgi_input_file_ptr_).c_str()); //TODO: очень плохо...
// 	args_[2] = NULL;
// }

// CGI::~CGI()
// {
// 	// int i = 0;
// 	// std::map<std::string, std::string>::iterator it;

// 	// for (it = envMap_.begin(); it != envMap_.end(); it++)
// 	// {
// 	// 	free(env_[i]);
// 	// 	i++;
// 	// }
// 	// free(env_);

// 	// unlink((*cgi_output_file_ptr_).c_str()); // uncomment when ready

// 	free(args_[0]);
// 	// free(args_[1]); // TODO

// 	delete cgi_input_file_ptr_;
// 	delete cgi_output_file_ptr_;
// }
