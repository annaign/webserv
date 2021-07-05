#include "Response.hpp"

std::map<std::string, std::string> Response::initStatusCodes()
{
	// https://tools.ietf.org/html/rfc2616

	std::map<std::string, std::string> statusCodes;

	statusCodes.insert(code("100", "Continue"));
	statusCodes.insert(code("101", "Switching Protocols"));
	statusCodes.insert(code("200", "OK"));
	statusCodes.insert(code("201", "Created"));
	statusCodes.insert(code("202", "Accepted"));
	statusCodes.insert(code("203", "Non-Authoritative Information"));
	statusCodes.insert(code("204", "No Content"));
	statusCodes.insert(code("205", "Reset Content"));
	statusCodes.insert(code("206", "Partial Content"));
	statusCodes.insert(code("300", "Multiple Choices"));
	statusCodes.insert(code("301", "Moved Permanently"));
	statusCodes.insert(code("302", "Found"));
	statusCodes.insert(code("303", "See Other"));
	statusCodes.insert(code("304", "Not Modified"));
	statusCodes.insert(code("305", "Use Proxy"));
	statusCodes.insert(code("307", "Temporary Redirect"));
	statusCodes.insert(code("400", "Bad Request"));
	statusCodes.insert(code("401", "Unauthorized"));
	statusCodes.insert(code("402", "Payment Required"));
	statusCodes.insert(code("403", "Forbidden"));
	statusCodes.insert(code("404", "Not Found"));
	statusCodes.insert(code("405", "Method Not Allowed"));
	statusCodes.insert(code("406", "Not Acceptable"));
	statusCodes.insert(code("407", "Proxy Authentication Required"));
	statusCodes.insert(code("408", "Request Timeout"));
	statusCodes.insert(code("409", "Conflict"));
	statusCodes.insert(code("410", "Gone"));
	statusCodes.insert(code("411", "Length Required"));
	statusCodes.insert(code("412", "Precondition Failed"));
	statusCodes.insert(code("413", "Request Entity Too Large"));
	statusCodes.insert(code("414", "Request-URI Too Long"));
	statusCodes.insert(code("415", "Unsupported Media Type"));
	statusCodes.insert(code("416", "Requested Range Not Satisfiable"));
	statusCodes.insert(code("417", "Expectation Failed"));
	statusCodes.insert(code("500", "Internal Server Error"));
	statusCodes.insert(code("501", "Not Implemented"));
	statusCodes.insert(code("502", "Bad Gateway"));
	statusCodes.insert(code("503", "Service Unavailable"));
	statusCodes.insert(code("504", "Gateway Timeout"));
	statusCodes.insert(code("505", "HTTP Version Not Supported"));

	return statusCodes;
}

std::map<std::string, std::string> Response::initMimeTypes()
{
	// https://developer.mozilla.org/ru/docs/Web/HTTP/Basics_of_HTTP/MIME_types
	// https://www.iana.org/assignments/media-types/media-types.xhtml
	// https://www.nginx.com/resources/wiki/start/topics/examples/full/#mime-types

	std::map<std::string, std::string> mimeTypes;

	mimeTypes["html"] = "text/html";
	mimeTypes["css"] = "text/css";
	mimeTypes["js"] = "application/javascript";
	mimeTypes["ico"] = "image/x-icon";
	mimeTypes["svg"] = "image/svg+xml";
	mimeTypes["gif"] = "image/gif";
	mimeTypes["jpeg"] = "image/jpeg";
	mimeTypes["jpg"] = "image/jpeg";
	mimeTypes["png"] = "image/png";

	return mimeTypes;
}

std::map<std::string, std::string> Response::mimeTypes_ = Response::initMimeTypes();
std::map<std::string, std::string> Response::statusCodes_ = Response::initStatusCodes();

Response::Response()
{
}

Response::~Response()
{
}

bool Response::getPage(std::string const &path_file, std::string &data)
{
	char buffer[1024] = {0};
	int fd, ret;
	if ((fd = open(path_file.c_str(), O_RDONLY)) == -1)
	{
		g_log.printErrorMessage("File did not open! Path - " + path_file + " : " + std::string(strerror(errno)));
		this->code_ = "404";
		return false;
	}

	data = "";
	while ((ret = read(fd, buffer, 1024 - 1)) > 0)
		data.append(buffer, ret);

	if (ret == -1)
	{
		data = "";
		g_log.printErrorMessage("Error in file reading: " + std::string(strerror(errno)));
		this->code_ = "500";
		return false;
	}
	close(fd);
	return true;
}

void Response::responseAddCode(std::string const &code)
{
	this->response_ = WEBSERV_HTTP " " + code + " " + this->statusCodes_[code] + "\r\n";
}

void Response::responseAddDate()
{
	char buffer[100] = {0};
	time_t curr_time;
	tm *curr_tm;
	time(&curr_time);
	curr_tm = gmtime(&curr_time);
	strftime(buffer, 100, "%a, %d %b %Y %X %Z", curr_tm);

	this->response_ += "Date: " + std::string(buffer) + "\r\n";
}

void Response::responseAddLastModified(std::string const &file)
{
	const int bufSize = 100;
	char buffer[bufSize] = {0};
	struct stat buf;

	if (file.length() > 0 && stat((file).c_str(), &buf) != -1)
	{
		tm *modif_tm = gmtime(&buf.st_mtime);
		strftime(buffer, bufSize, "%a, %d %b %Y %X %Z", modif_tm);

		this->response_ += "Last-Modified: " + std::string(buffer) + "\r\n";
	}
}

void Response::responseAddServer(std::string const &server)
{
	this->response_ += "Server: " + server + "\r\n";
}

void Response::responseAddLocation(std::string const &location)
{
	this->response_ += "Location: " + location + "\r\n";
}

void Response::responseAddAllow(std::string const &methodsList)
{
	if (methodsList.length() > 0)
		this->response_ += "Allow: " + methodsList + "\r\n";
}

void Response::responseAddContentType(std::string const &fileName)
{
	std::string type;
	size_t pos = fileName.find_last_of(".");

	if (pos != std::string::npos && this->mimeTypes_.count(fileName.substr(pos + 1)))
		type = this->mimeTypes_[fileName.substr(pos + 1)];
	if (type.empty())
		type = "text/html";

	this->response_ += "Content-Type: " + type + "\r\n";
}

void Response::responseAddContentLength(size_t length)
{
	this->response_ += "Content-Length: " + positiveNumberToString(length) + "\r\n";
}

void Response::responseAddContentLanguage(std::string const &languages)
{
	this->response_ += "Content-Language: " + languages + "\r\n";
}

void Response::responseAddContentLocation(std::string const &url)
{
	this->response_ += "Content-Location: " + url + "\r\n";
}

void Response::responseAddRetryAfter(int seconds)
{
	this->response_ += "Retry-After: " + positiveNumberToString(seconds) + "\r\n";
}

void Response::responseAddTransferEncoding(std::string const &encoding)
{
	this->response_ += "Transfer-Encoding: " + encoding + "\r\n";
}

void Response::responseAddWWWAuthenticate(std::string const &auth)
{
	this->response_ += std::string("WWW-Authenticate: ") + WEBSERV_AUTH + " realm=\"" + auth + "\"\r\n";
}

bool Response::checkAllowes(Client const &client, ConfigLocation const &location)
{
	// check method
	std::map<std::string, bool> methods = location.getMethods();
	std::map<std::string, bool>::const_iterator method = methods.find(client.getMethod());
	if (method == methods.end() || method->second == false)
	{
		this->code_ = "405";
		return false;
	}

	// check http version
	if (client.getHttpVersion() != WEBSERV_HTTP)
	{
		this->code_ = "505";
		return false;
	}

	// check password
	// https://developer.mozilla.org/ru/docs/Web/HTTP/Authentication
	if (location.getAuthBasic().length() > 0 && client.getHeaderAuthorization().size() == 0)
	{
		this->code_ = "401";
		return false;
	}

	if (location.getAuthBasic().length() > 0)
	{
		if (client.getHeaderAuthorization().size() > 0)
		{
			std::string decoded = Base64::decode(client.getHeaderAuthorization().at(WEBSERV_AUTH));
			std::vector<std::string> users = location.getAuthBasicUserFile();
			std::vector<std::string>::iterator user = users.begin();
			while (user != users.end())
			{
				if (*user == decoded)
					return true;
				++user;
			}

			this->code_ = "403";
			return false;
		}
	}

	// if (client.getMethod() == "HEAD" || client.getMethod() == "GET")
	// 	return;

	// body size
	size_t bodyBuffer = 0;
	bool checkSize = false;
	std::map<std::string, bool>::const_iterator it = location.getFields().find("client_body_buffer_size");
	if (it != location.getFields().end() && it->second)
	{
		bodyBuffer = location.getClientBodyBufferSize();
		checkSize = true;
	}
	else
	{
		it = client.getServer().getFields().find("client_body_buffer_size");
		if (it != client.getServer().getFields().end() && it->second)
		{
			bodyBuffer = client.getServer().getClientBodyBufferSize();
			checkSize = true;
		}
	}

#if DEBUG_SERV > 1

	std::cout << UNDERLINE_W << "ClientBodyBufferSize limit:"
			  << YELLOW_C " " << bodyBuffer
			  << RESET_C << std::endl;
#endif

	if (checkSize && bodyBuffer > 0 && client.getBody().length() > bodyBuffer)
	{
		this->code_ = "413";
		return false;
	}

	return true;
}

void Response::returnErrorPage(Client const &client, std::string const &auth, std::string const &methodsList)
{
	ConfigServer server = client.getServer();

	if (this->code_.length() == 0)
		this->code_ = "404";

	this->responseAddCode(this->code_);
	this->responseAddDate();
	this->responseAddServer();
	if (this->code_ == "405")
		this->responseAddAllow(methodsList);
	if (this->code_ == "301")
	{
		this->responseAddLocation(this->locationNew_);
		// this->responseAddRetryAfter(10);
	}
	if (this->code_ == "401")
		this->responseAddWWWAuthenticate(auth);
	this->responseAddContentType("index.html");

	std::map<std::string, std::string> errorPage = server.getErrorPage();
	if (errorPage.count(this->code_) > 0)
	{
		std::string url = server.getRoot() + errorPage[this->code_];

		if (!this->getPage(url, this->data_))
		{
			DefaultPage page;
			this->data_ = page.getPage(this->code_, this->statusCodes_[this->code_]);
		}
	}
	else
	{
		DefaultPage page;
		this->data_ = page.getPage(this->code_, this->statusCodes_[this->code_]);
	}

	// if (client.getMethod() != "HEAD")
	this->responseAddContentLength(this->data_.size());
	this->response_ += "\r\n";
}

void Response::returnPage(ConfigServer const &server, Client const &client)
{
	(void)server;

	this->responseAddCode(this->code_);
	this->responseAddDate();
	this->responseAddLastModified(this->path_);
	this->responseAddServer();
	this->responseAddContentType(client.getFileName());
	if (this->code_ == "201" || this->code_ == "204")
		responseAddContentLocation(client.getUrl());
	if (this->data_.size() > 0)
		this->responseAddContentLength(this->data_.size());
	this->response_ += "\r\n";
}

bool Response::createUrl(ConfigServer const &server, ConfigLocation const &location, Client const &client)
{
	this->path_ = location.getAlias();
	if (this->path_.length() == 0)
	{
		this->path_ = location.getRoot();
		if (this->path_.length() == 0)
			this->path_ = server.getRoot();
	}

	if (this->path_[this->path_.length() - 1] != '/')
		this->path_ += "/";

	if (client.getPath().length() > 0)
	{
		size_t pos = 0;
		if (location.getAlias().length() > 0)
		{
			// alias = alias + (client.getPath - location) + client.getFileName
			pos = location.getPrefix().length();
			if (client.getPath().length() >= pos)
			{
				if (client.getPath()[pos] == '/')
					++pos;
				this->path_ += client.getPath().substr(pos);
			}
		}
		else
		{
			// root = root + client.getPath + client.getFileName
			if (client.getPath()[0] == '/')
				++pos;
			this->path_ += client.getPath().substr(pos);
		}
	}

	if (client.getFileName().length() > 0)
	{
		this->path_ += client.getFileName(); // if path without "/" => do code 301 to correct address
	}
	else if (location.getIndex().length() > 0)
	{
		ssize_t pos = (location.getIndex()[0] == '/') ? 1 : 0;
		this->path_ += location.getIndex().substr(pos);
	}
	else
		return false; // no fileName in url => try Autoindex

	return true;
}

bool Response::checkLocation(Client &client, ConfigLocation const &location)
{
	bool cmpResult;

	size_t length = location.getPrefix().length();

	cmpResult = (strncmp(client.getPath().data(), location.getPrefix().data(), length) == 0);

	if (!cmpResult && length > 2 && location.getPrefix()[length - 1] == '/' && (client.getPath().length() == length - 1))
		cmpResult = (strncmp(client.getPath().data(), location.getPrefix().data(), length - 1) == 0);

	if (!cmpResult)
	{
		std::string pathFile = client.getPath() + client.getFileName();
		cmpResult = (strncmp(pathFile.data(), location.getPrefix().data(), length) == 0);

		if (!cmpResult && length > 2 && location.getPrefix()[length - 1] == '/' && (pathFile.length() == length - 1))
			cmpResult = (strncmp(pathFile.data(), location.getPrefix().data(), length - 1) == 0);

		if (cmpResult)
		{
			client.setPath(pathFile);
			client.setFileName("");
		}
	}

	return cmpResult;
}

// void Response::setGET(ConfigServer const &server, Client const &client)
void Response::setGET(Client &client)
{
	ConfigServer server = client.getServer();
	std::vector<ConfigLocation> locations = server.getLocations();
	std::vector<ConfigLocation>::const_iterator location = locations.begin();
	this->code_ = "";
	this->path_ = "";

	while (location != locations.end())
	{
		if (checkLocation(client, *location))
		{
			if (!this->checkAllowes(client, (*location)))
				break;

			if (createUrl(server, *location, client))
			{
				DIR *dir = opendir(this->path_.c_str());
				if (dir != NULL)
				{
					closedir(dir);
					this->locationNew_ = client.getUrl() + "/";
					this->code_ = "301";
					break;
				}
				if (!this->getPage(this->path_, this->data_))
					break;
			}
			else
			{
				if ((*location).getAutoindex())
				{
					Autoindex autoIndex;
					this->data_ = autoIndex.getPage(this->path_, client.getPath());
				}
				else
				{
					this->code_ = "403";
					break;
				}
			}
			this->code_ = "200";
			this->returnPage(server, client);
			return;
		}
		++location;
	}

	if (location != locations.end())
		this->returnErrorPage(client, (*location).getAuthBasic(), (*location).getMethodsList());
	else
		this->returnErrorPage(client);
}

void Response::setHEAD(Client &client)
{
	this->setGET(client);
	this->data_ = "";
}

void Response::setPOST(Client &client)
// void Response::setPOST(Client const &client)
{
	// https://efim360.ru/rfc-7231-protokol-peredachi-giperteksta-http-1-1-semantika-i-kontent/#6-3-1-200-OK

	ConfigServer server = client.getServer();
	std::vector<ConfigLocation> locations = server.getLocations();
	std::vector<ConfigLocation>::const_iterator location = locations.begin();
	this->code_ = "";
	this->path_ = "";
	this->data_ = "";

	while (location != locations.end())
	{
		if (checkLocation(client, *location))
		{
			if (!this->checkAllowes(client, (*location)))
				break;

			if (!createUrl(server, *location, client))
			{
				if (client.getPath() == "/post_body")
				{
					this->code_ = "200";
					this->data_ = "<p>post_body bla</>";

					this->responseAddCode(this->code_);
					this->responseAddContentLength(this->data_.size());
					this->responseAddDate();
					this->responseAddServer();
					this->responseAddContentType(".html");
					this->response_ += "\r\n";

					return;
				}

				this->code_ = "403";
				break;
			}

			// if POST need cgi we change body and response code here
			if (location->getScrypt().length() > 0 && client.getFileName().length() > 4)
			{
				std::string file = client.getFileName();
				std::size_t found = client.getFileName().find(location->getExtension(), file.length() - 4);
				if (found != std::string::npos)
				{
					CGI2 cgi(client, *location, this->path_);

					this->code_ = cgi.run(this->data_);

					this->responseAddCode(this->code_);
					this->responseAddContentLength(this->data_.size());
					this->responseAddDate();
					this->responseAddServer();
					this->responseAddContentType(".html");
					this->response_ += "\r\n";

					// try
					// {
					// 	CGI cgi(client, *location);

					// 	cgi.run();

					// 	this->data_ = cgi.getBody();
					// 	this->code_ = "200";

					// 	// this->responseAddCode(this->code_);

					// 	this->responseAddCode(this->code_);
					// 	this->response_ += cgi.getHeaders();
					// 	this->responseAddContentLength(this->data_.size());
					// 	this->responseAddDate();
					// 	this->responseAddServer();

					// 	// this->responseAddLastModified(this->path_);
					// 	// this->responseAddContentType(client.getFileName());

					// 	this->response_ += "\r\n";
					// }
					// catch (const std::exception &e)
					// {
					// 	// TODO: что делать в этом случае?

					// 	std::cerr << e.what() << std::endl;

					// 	// 	response_ = "";
					// 	// 	// response = (_generateResponse(500, headers, method != "HEAD" ?_getErrorHTMLPage(500) : ""));
					// 	// 	return;
					// }
					return;
				}
			}
		}
		++location;
	}

	if (location != locations.end())
		this->returnErrorPage(client, (*location).getAuthBasic(), (*location).getMethodsList());
	else
		this->returnErrorPage(client);
}

void Response::setPUT(Client &client)
{
	ConfigServer server = client.getServer();
	std::vector<ConfigLocation> locations = server.getLocations();
	std::vector<ConfigLocation>::const_iterator location = locations.begin();
	this->code_ = "";
	this->path_ = "";
	this->data_ = "";

	while (location != locations.end())
	{
		if (checkLocation(client, *location))
		{
			if (!this->checkAllowes(client, (*location)))
				break;

			if (createUrl(server, *location, client))
			{
				struct stat buf;
				if (stat(this->path_.c_str(), &buf) == -1)
				{
					// create
					int fd = open(this->path_.c_str(), O_CREAT | O_WRONLY, 0644);
					if (fd == -1)
					{
						g_log.printErrorMessage(std::string(strerror(errno)) + ": " + this->path_);
						this->code_ = "500";
						break;
					}

					if (client.getBody().empty() || write(fd, client.getBody().c_str(), client.getBody().length()) == -1)
					{
						g_log.printErrorMessage(std::string(strerror(errno)) + ": " + this->path_);
						this->code_ = "500";
						close(fd);
						break;
					}

					close(fd);

					this->code_ = "201";
					this->data_ = "ONLY FOR TESTER !!!";
					this->returnPage(server, client);
					return;
				}
				else
				{
					// rewrite
					int fd = open(this->path_.c_str(), O_TRUNC | O_WRONLY);
					if (fd == -1)
					{
						g_log.printErrorMessage(std::string(strerror(errno)) + ": " + this->path_);
						this->code_ = "500";
						break;
					}

					if (client.getBody().empty() || write(fd, client.getBody().c_str(), client.getBody().length()) == -1)
					{
						g_log.printErrorMessage(std::string(strerror(errno)) + ": " + this->path_);
						this->code_ = "500";
						close(fd);
						break;
					}

					close(fd);

					this->code_ = "204";
					this->returnPage(server, client);
					return;
				}
			}
			else
			{
				this->code_ = "403";
				break;
			}
		}
		++location;
	}

	if (location != locations.end())
		this->returnErrorPage(client, (*location).getAuthBasic(), (*location).getMethodsList());
	else
		this->returnErrorPage(client);
}

void Response::setErrorCode(Client const &client, std::string const &code)
{
	if (this->statusCodes_.count(code))
		this->code_ = code;
	else
		this->code_ = "500";
	this->path_ = "";
	this->data_ = "";

	this->returnErrorPage(client);
}

void Response::clearResponse()
{
	this->response_ = "";
	this->data_ = "";
	this->code_ = "";
	this->path_ = "";
	this->locationNew_ = "";
}

std::string Response::getCode() const
{
	return this->code_;
}

std::string const &Response::getResponse() const
{
	return this->response_;
}

std::string const &Response::getData() const
{
	return this->data_;
}
