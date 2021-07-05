#include "Client.hpp"

Client::Client()
	: chunk_(0),
	  headersReq_(false),
	  fihishReadingRequest_(false)
{
	this->setTimeLastConn();
}

Client::Client(int sockClient, int sockFd, ConfigServer const &server)
	: sockClient_(sockClient),
	  sockServer_(sockFd),
	  server_(server),
	  chunk_(0),
	  headersReq_(false),
	  fihishReadingRequest_(false)
{
	(void)sockServer_;

	this->setTimeLastConn();

#if DEBUG_SERV == 1
	g_log.printAccessMessage("Create client " + positiveNumberToString(this->sockClient_) + " connected to server " + server.getServerIpPort());
#endif
}

Client::~Client()
{
}

void Client::setPath(std::string const &path)
{
	this->path_ = path;
}

void Client::setFileName(std::string const &fileName)
{
	this->fileName_ = fileName;
}

void Client::setBody(std::string const &body)
{
	if (this->getMethod() == "POST" && this->getHeaderContentType() == "application/x-www-form-urlencoded")
		this->queryString_ = body; // no limit max length?
	else
		this->body_ = body;
}

bool Client::parseBody(std::string const &body)
{
	if (this->getMethod() == "POST" && this->getHeaderContentType() == "application/x-www-form-urlencoded")
		this->queryString_ += body; // no limit max length?
	else
		this->body_ += body;

	return true;
}

void Client::setRequest(std::string const &request)
{
	this->request_ = request;
}

std::string const &Client::getMethod() const
{
	return this->method_;
}

std::string const &Client::getUrl() const
{
	return this->url_;
}

std::string const &Client::getHttpVersion() const
{
	return this->httpVersion_;
}

std::map<std::string, std::string> const &Client::getHeaders() const
{
	return this->headers_;
}

std::string const &Client::getBody() const
{
	return this->body_;
}

std::string const &Client::getQueryString() const
{
	return this->queryString_;
}

std::string const &Client::getServerName() const
{
	return this->serverName_;
}

std::string const &Client::getPath() const
{
	return this->path_;
}

std::string const &Client::getFileName() const
{
	return this->fileName_;
}

int Client::getSocketClient() const
{
	return this->sockClient_;
}

ConfigServer const &Client::getServer() const
{
	return this->server_;
}

std::string const &Client::getRequest() const
{
	return this->request_;
}

std::string Client::getHeaderAcceptCharsets() const
{
	headersMap::const_iterator it = this->headers_.find("Accept-Charsets");
	return (it != this->headers_.end()) ? it->second : "";
}

std::string Client::getHeaderAcceptLanguage() const
{
	headersMap::const_iterator it = this->headers_.find("Accept-Language");
	return (it != this->headers_.end()) ? it->second : "";
}

std::map<std::string, std::string> Client::getHeaderAuthorization() const
{
	std::map<std::string, std::string> ret;
	headersMap::const_iterator it = this->headers_.find("Authorization");

	if (it != this->headers_.end())
	{
		size_t sep = 0;
		while (it->second[sep] == ' ')
			++sep;

		std::string auth = WEBSERV_AUTH " ";
		if (strncmp(it->second.substr(sep).c_str(), auth.c_str(), auth.length()) == 0)
			ret[it->second.substr(sep, auth.length() - 1)] = it->second.substr(sep + auth.length());
	}

	return ret;
}

std::string Client::getHeaderHost() const
{
	headersMap::const_iterator it = this->headers_.find("Host");
	return (it != this->headers_.end()) ? it->second : "";
}

std::string Client::getHeaderReferer() const
{
	headersMap::const_iterator it = this->headers_.find("Referer");
	return (it != this->headers_.end()) ? it->second : "";
}

std::string Client::getHeaderUserAgent() const
{
	headersMap::const_iterator it = this->headers_.find("User-Agent");
	return (it != this->headers_.end()) ? it->second : "";
}

std::string Client::getHeaderContentLanguage() const
{
	headersMap::const_iterator it = this->headers_.find("Content-Language");
	return (it != this->headers_.end()) ? it->second : "";
}

std::string Client::getHeaderContentType() const
{
	headersMap::const_iterator it = this->headers_.find("Content-Type");
	return (it != this->headers_.end()) ? it->second : "";
}

size_t Client::getHeaderContentLength() const
{
	headersMap::const_iterator it = this->headers_.find("Content-Length");
	if (it != this->headers_.end())
	{
		return std::atoi(it->second.c_str());
	}
	return 0;
}

std::string Client::getHeaderDate() const
{
	headersMap::const_iterator it = this->headers_.find("Date");
	return (it != this->headers_.end()) ? it->second : "";
}

std::string Client::getHeaderTransferEncoding() const
{
	headersMap::const_iterator it = this->headers_.find("Transfer-Encoding");
	return (it != this->headers_.end()) ? it->second : "";
}

void Client::setChunk(size_t num10)
{
	this->chunk_ = num10;
}

void Client::setChunk(std::string const &num16)
{
#if DEBUG_SERV == 1
	std::cout << ">>>>>>>>>>>>> num16: " << num16 << " >>>>>>>>>>>>>>>>\n";
#endif
	size_t decNum = 0;
	size_t len = num16.length();
	for (size_t i = 0; i < num16.length(); i++)
	{
		if (num16[i] <= 57)
			decNum += ((int)num16[i] - 48) * ft_pow(16, len - i - 1); // for numbers
		else if (num16[i] >= 65 && num16[i] <= 70)
			decNum += ((int)num16[i] - 55) * ft_pow(16, len - i - 1); // for A B C D E F
		else
			decNum += ((int)num16[i] - 87) * ft_pow(16, len - i - 1); // for a b c d e f
	}
#if DEBUG_SERV == 1
	std::cout << ">>>>>>>>>>>>> decNum: " << decNum << " >>>>>>>>>>>>>>>>\n";
#endif
	this->chunk_ = decNum;
}

void Client::setHeadersReq(bool result)
{
	this->headersReq_ = result;
}

size_t Client::getChunk() const
{
	return this->chunk_;
}

bool Client::getHeadersReq() const
{
	return this->headersReq_;
}

void Client::addToRequest(std::string const &chunk)
{
	this->request_ += chunk;
}

void Client::setFihishReadingRequest(bool isFihised)
{
	this->fihishReadingRequest_ = isFihised;
}

bool Client::getFihishReadingRequest() const
{
	return this->fihishReadingRequest_;
}

void Client::setTimeLastConn()
{
	this->timeLastConn_ = std::time(NULL);
}

std::time_t Client::getTimeLastConnow() const
{
	return this->timeLastConn_;
}

void Client::clearClient()
{
	this->method_ = "";
	this->url_ = "";
	this->httpVersion_ = "";
	this->headers_.clear();
	this->body_ = "";
	this->queryString_ = "";
	this->serverName_ = "";
	this->path_ = "";
	this->fileName_ = "";

	this->request_ = "";
	this->chunk_ = 0;
	this->headersReq_ = false;
	this->fihishReadingRequest_ = false;
}

void Client::parseRequest(std::string const &request) throw(Client::ClientException)
{
	// https://tools.ietf.org/html/rfc2616#page-35
	//  Request       = Request-Line CRLF
	// ( general-header | request-header | entity-header ) CRLF
	// CRLF
	// [ message-body ]

	// parse request-line
	std::size_t sep = request.find("\r\n");
	if (sep == std::string::npos)
		throw(ClientException("Client parse: Not valid 'request-line'"));

	if (!this->parseRequestLine(request.substr(0, sep)))
		throw(ClientException("Client parse: Not valid 'request-line'"));

	//parse headers
	std::size_t sep2 = request.find("\r\n\r\n");
	if (sep2 == std::string::npos)
		throw(ClientException("Client parse: Not valid 'headers end'"));

	if (!this->parseHeaders(request.substr(sep + 2, sep2 - sep)))
		throw(ClientException("Client parse: Not valid 'headers'"));

	//parse body
	this->setBody(request.substr(sep2 + 4));
}

void Client::parseUrl(std::string url)
{
	// https://tools.ietf.org/html/rfc1808

	char const *http = "http://";
	std::size_t sep;

	if (strncmp(url.data(), http, strlen(http)) == 0)
	{
		url = url.substr(strlen(http));

		sep = url.find("/");
		if (sep == std::string::npos)
		{
			this->serverName_ = url;
			url += "/";
		}
		else
			this->serverName_ = url.substr(0, sep);

		url = url.substr(this->serverName_.length());
	}

	sep = url.find_last_of("/");
	if (sep == std::string::npos)
		this->path_ = url;
	else
		this->path_ = url.substr(0, sep + 1);
	url = url.substr(this->path_.length());

	sep = url.find("?");
	if (sep == std::string::npos)
		this->fileName_ = url;
	else
	{
		this->fileName_ = url.substr(0, sep);
		this->queryString_ = url.substr(sep + 1);
	}
}

bool Client::parseRequestLine(std::string const &request)
{
	// https://tools.ietf.org/html/rfc2616#page-35
	// Request-Line   = Method SP Request-URI SP HTTP-Version CRLF

	std::size_t sep = request.find(" ");
	std::size_t sep2 = request.find(" ", sep + 1);

	if (sep == std::string::npos || sep2 == std::string::npos)
		return false;

	this->method_ = request.substr(0, sep);
	this->url_ = request.substr(sep + 1, sep2 - sep - 1);
	this->httpVersion_ = request.substr(sep2 + 1);

	this->parseUrl(this->url_);

	return true;
}

bool Client::parseHeaders(std::string const &headers)
{
	// https://tools.ietf.org/html/rfc2616#page-31
	// message-header = field-name ":" [ field-value ]
	//        field-name     = token
	//        field-value    = *( field-content | LWS )
	//        field-content  = <the OCTETs making up the field-value
	//                         and consisting of either *TEXT or combinations
	//                         of token, separators, and quoted-string>

	std::size_t sepPrev = 0;
	std::size_t sep = 0;
	std::size_t sepName;
	std::size_t sepValue;

	while ((sep = headers.find("\r\n", sep)) != std::string::npos)
	{
		std::string header = headers.substr(sepPrev, sep - sepPrev);

		sepName = header.find(":");
		std::string name = header.substr(0, sepName++); // case-insensitive

		while (strchr(" \n\t\r\v\f", header[sepName]) != NULL)
			++sepName;
		sepValue = header.length() - 1;
		while (strchr(" \n\t\r\v\f", header[sepValue]) != NULL)
			--sepValue;
		std::string value = header.substr(sepName, sepValue + 1 - sepName);

		this->headers_.insert(std::pair<std::string, std::string>(name, value));

		sep += 2;
		sepPrev = sep;
	}

	return true;
}

std::ostream &operator<<(std::ostream &os, Client const &client)
{
	os << YELLOW_C;
	os << client.getMethod() << " "
	   << client.getUrl() << " "
	   << client.getHttpVersion() << "\n";

	std::map<std::string, std::string>::const_iterator it = client.getHeaders().begin();
	while (it != client.getHeaders().end())
	{
		os << it->first << ": " << it->second << "\n";
		++it;
	}

	os << GREEN_C
	   << "Body length: " << client.getBody().length() << "\n"
	   << "getQueryString: " << client.getQueryString() << "\n";
	os << RESET_C;

	return os;
}
