#include "Config.hpp"

Config::Config(std::string const &configFile)
	: log_error_(false), log_access_(false)
{
	this->readFile(configFile);
	this->parseConfig();
	this->checkConfigErrors();
}

Config::Config(Config const &src)
{
	*this = src;
}

Config::~Config()
{
}

Config &Config::operator=(Config const &rhs)
{
	if (this != &rhs)
	{
		this->errors_ = rhs.errors_;
		this->config_ = rhs.config_;
		this->log_error_ = rhs.log_error_;
		this->log_access_ = rhs.log_access_;
		this->servers_ = rhs.servers_;
	}

	return *this;
}

bool Config::setLogError(std::string const &log)
{
	if (log == "on")
		this->log_error_ = true;
	else if (log == "off")
		this->log_error_ = false;
	else
		return false;

	return true;
}

bool Config::setLogAccess(std::string const &log)
{
	if (log == "on")
		this->log_access_ = true;
	else if (log == "off")
		this->log_access_ = false;
	else
		return false;

	return true;
}

bool Config::setServers(ConfigServer const &server)
{
	std::string serverIpPort = server.getServerIpPort();

	if (this->serversMap_.count(serverIpPort) > 0)
		return false;

	this->serversMap_[serverIpPort] = server;
	this->servers_.push_back(server);

	return true;
}

std::string const &Config::getErrors() const
{
	return this->errors_;
}

bool Config::getLogError() const
{
	return this->log_error_;
}

bool Config::getLogAccess() const
{
	return this->log_access_;
}

std::vector<ConfigServer> const &Config::getServers() const
{
	return this->servers_;
}

std::map<std::string, ConfigServer> const &Config::getServersMap() const
{
	return this->serversMap_;
}

bool Config::readFile(std::string const &configFile)
{
	int fd;

	if ((fd = open(configFile.c_str(), O_RDONLY)) == -1)
	{
		this->errors_.append(strerror(errno));
		this->errors_.append(": \"" + configFile + "\"");
		return false;
	}

	char buffer[BUFFER_SIZE];
	int ret;
	std::string configWithComments = "";

	while ((ret = read(fd, buffer, BUFFER_SIZE)) > 0)
		configWithComments.append(buffer, ret);

	if (ret == -1)
	{
		this->errors_.append(strerror(errno));
		this->errors_.append(": \"" + configFile + "\"");
		return false;
	}

	// --- Cat comments in config file ---

	char commentStart = '#';
	char commentEnd = '\n';
	std::string::size_type begin = 0;
	std::string::size_type end;

	while (begin != configWithComments.length())
	{
		end = configWithComments.find(commentStart, begin);

		if (end != std::string::npos)
		{
			if (begin != end)
				this->config_ += configWithComments.substr(begin, end - begin);

			end = configWithComments.find(commentEnd, end);
			if (end == std::string::npos)
				break;
		}
		else
		{
			end = configWithComments.length();
			this->config_ += configWithComments.substr(begin, end - begin);
			break;
		}

		begin = end;
	}

	close(fd);

	return true;
}

bool Config::parseLocation(ConfigServer &serv, std::vector<std::string> &words, std::vector<std::string>::const_iterator &word)
{
	ConfigLocation location;

	if (!location.setPrefix(*word))
	{
		this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
		return false;
	}
	++word;

	if (*word != "{")
	{
		this->errors_ += "Expect \"{\" after 'location' directive, but was \"" + *word + "\"";
		return false;
	}

	while (++word != words.end())
	{
		if (*word == "root")
		{
			++word;
			std::string::size_type pos = (*word).find(';');

			if (pos == std::string::npos || (*word).length() == 1 || pos != (*word).length() - 1)
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}

			if (!location.setRoot((*word).substr(0, pos)))
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}
		}
		else if (*word == "index")
		{
			++word;
			std::string::size_type pos = (*word).find(';');

			if (pos == std::string::npos || (*word).length() == 1 || pos != (*word).length() - 1)
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}

			if (!location.setIndex((*word).substr(0, pos)))
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}
		}
		else if (*word == "http_methods")
		{
			++word;
			std::string::size_type pos = (*word).find(';');
			std::vector<std::string> methods;

			while (word != words.end())
			{
				if (pos != std::string::npos)
				{
					if ((*word).length() == 1 || pos != (*word).length() - 1)
					{
						this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
						return false;
					}
					methods.push_back((*word).substr(0, pos));
					break;
				}

				methods.push_back(*word);
				++word;
				pos = (*word).find(';');
			}

			if (!location.setMethods(methods))
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}
		}
		else if (*word == "alias")
		{
			++word;
			std::string::size_type pos = (*word).find(';');

			if (pos == std::string::npos || (*word).length() == 1 || pos != (*word).length() - 1)
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}

			if (!location.setAlias((*word).substr(0, pos)))
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}
		}
		else if (*word == "autoindex")
		{
			++word;
			std::string::size_type pos = (*word).find(';');

			if (pos == std::string::npos || (*word).length() == 1 || pos != (*word).length() - 1)
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}

			if (!location.setAutoindex((*word).substr(0, pos)))
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}
		}
		else if (*word == "auth_basic")
		{
			++word;
			std::string::size_type pos = (*word).find(';');

			if (pos == std::string::npos || (*word).length() == 1 || pos != (*word).length() - 1)
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}

			if (!location.setAuthBasic((*word).substr(0, pos)))
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}
		}
		else if (*word == "auth_basic_user_file")
		{
			++word;
			std::string::size_type pos = (*word).find(';');

			if (pos == std::string::npos || (*word).length() == 1 || pos != (*word).length() - 1)
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}

			if (!location.setAuthBasicUserFile((*word).substr(0, pos)))
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}
		}
		else if (*word == "extension")
		{
			++word;
			std::string::size_type pos = (*word).find(';');

			if (pos == std::string::npos || (*word).length() == 1 || pos != (*word).length() - 1)
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}

			if (!location.setExtension((*word).substr(0, pos)))
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}
		}
		else if (*word == "scrypt")
		{
			++word;
			std::string::size_type pos = (*word).find(';');

			if (pos == std::string::npos || (*word).length() == 1 || pos != (*word).length() - 1)
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}

			if (!location.setScrypt((*word).substr(0, pos)))
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}
		}
		else if (*word == "client_body_buffer_size")
		{
			++word;
			std::string::size_type pos = (*word).find(';');

			if (pos == std::string::npos || (*word).length() == 1 || pos != (*word).length() - 1)
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}

			if (!location.setClientBodyBufferSize((*word).substr(0, pos)))
			{
				this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
				return false;
			}
		}
		else if (*word == "}")
		{
			break;
		}
		else
		{
			this->errors_ += "Unknown directive in 'location' block: \"" + *word + "\"";
			return false;
		}
	}

	serv.setLocations(location);

	return true;
}

bool Config::parseServer(std::vector<std::string> &words, std::vector<std::string>::const_iterator &word)
{
	if (*word != "{")
	{
		this->errors_ += "Expect \"{\" after 'server' directive, but was \"" + *word + "\"";
		return false;
	}

	ConfigServer serv;

	while (++word != words.end())
	{
		if (*word == "location")
		{
			if (!this->parseLocation(serv, words, ++word))
				return false;
		}
		else if (*word == "listen")
		{
			++word;
			std::string::size_type pos = (*word).find(';');

			if (pos == std::string::npos || (*word).length() == 1 || pos != (*word).length() - 1)
			{
				this->errors_ += "Unknown directive in 'server' block: \"" + *word + "\"";
				return false;
			}

			std::string listen = std::string((*word).begin(), --(*word).end());

			if (!serv.setServAddr(AF_INET, listen))
			{
				this->errors_ += "Wrong host address: \"" + listen + "\"";
				return false;
			}
		}
		else if (*word == "server_name")
		{
			++word;
			std::string::size_type pos = (*word).find(';');

			if (pos == std::string::npos || (*word).length() == 1 || pos != (*word).length() - 1)
			{
				this->errors_ += "Unknown directive in 'server' block: \"" + *word + "\"";
				return false;
			}

			if (!serv.setServerName((*word).substr(0, pos)))
			{
				this->errors_ += "Unknown directive in 'server' block: \"" + *word + "\"";
				return false;
			}
		}
		else if (*word == "error_page")
		{
			++word;
			std::string::size_type pos = (*word).find(';');
			std::vector<uint32_t> errorCodes;

			while (word != words.end())
			{
				if (pos != std::string::npos)
					break;

				if ((*word).length() > 3)
				{
					this->errors_ += "Unknown directive in 'server' block: \"" + *word + "\"";
					return false;
				}

				std::string::const_iterator it = (*word).begin();

				while (it != (*word).end())
				{
					if (!std::isdigit(*it))
					{
						this->errors_ += "Unknown directive in 'server' block: \"" + *word + "\"";
						return false;
					}
					++it;
				}

				uint32_t num = std::atoi((*word).c_str());
				if (num == 0)
				{
					this->errors_ += "Unknown directive in 'server' block: \"" + *word + "\"";
					return false;
				}

				errorCodes.push_back(num);

				++word;
				pos = (*word).find(';');
			}

			if (pos == std::string::npos || (*word).length() == 1 || pos != (*word).length() - 1)
			{
				this->errors_ += "Unknown directive in 'server' block: \"" + *word + "\"";
				return false;
			}

			if (!serv.setErrorPage((*word).substr(0, pos), errorCodes))
			{
				this->errors_ += "Unknown directive in 'server' block: \"" + *word + "\"";
				return false;
			}
		}
		else if (*word == "client_body_buffer_size")
		{
			++word;
			std::string::size_type pos = (*word).find(';');

			if (pos == std::string::npos || (*word).length() == 1 || pos != (*word).length() - 1)
			{
				this->errors_ += "Unknown directive in 'server' block: \"" + *word + "\"";
				return false;
			}

			if (!serv.setClientBodyBufferSize((*word).substr(0, pos)))
			{
				this->errors_ += "Unknown directive in 'server' block: \"" + *word + "\"";
				return false;
			}
		}
		else if (*word == "root")
		{
			++word;
			std::string::size_type pos = (*word).find(';');

			if (pos == std::string::npos || (*word).length() == 1 || pos != (*word).length() - 1)
			{
				this->errors_ += "Unknown directive in 'server' block: \"" + *word + "\"";
				return false;
			}

			if (!serv.setRoot((*word).substr(0, pos)))
			{
				this->errors_ += "Unknown directive in 'server' block: \"" + *word + "\"";
				return false;
			}
		}
		else if (*word == "}")
		{
			break;
		}
		else
		{
			this->errors_ += "Unknown directive in 'server' block: \"" + *word + "\"";
			return false;
		}
	}

	if (!this->setServers(serv))
	{
		this->errors_ += "Duplicate 'server': \"" + serv.getServerIpPort() + "\"";
		return false;
	}

	return true;
}

void Config::parseConfig()
{
	std::vector<std::string> words = splitWords(this->config_);

	if (words.empty())
	{
		this->errors_ += "Empty config file";
		return;
	}

	std::vector<std::string>::const_iterator word = words.begin();
	while (word != words.end())
	{
		if (*word == "log_error")
		{
			++word;
			std::string::size_type pos = (*word).find(';');

			if (pos == std::string::npos || (*word).length() == 1 || pos != (*word).length() - 1)
			{
				this->errors_ += "Unknown directive in 'log_error' block: \"" + *word + "\"";
				break;
			}

			if (!this->setLogError((*word).substr(0, pos)))
			{
				this->errors_ += "Unknown directive in 'log_error' block: \"" + *word + "\"";
				break;
			}
		}
		else if (*word == "log_access")
		{
			++word;
			std::string::size_type pos = (*word).find(';');

			if (pos == std::string::npos || (*word).length() == 1 || pos != (*word).length() - 1)
			{
				this->errors_ += "Unknown directive in 'log_access' block: \"" + *word + "\"";
				break;
			}

			if (!this->setLogAccess((*word).substr(0, pos)))
			{
				this->errors_ += "Unknown directive in 'log_access' block: \"" + *word + "\"";
				break;
			}
		}
		else if (*word == "server")
		{
			if (!this->parseServer(words, ++word))
				break;
		}
		else
		{
			this->errors_ += "Unknown directive: \"" + *word + "\"";
			break;
		}
		++word;
	}
}

void Config::checkConfigErrors()
{
	std::vector<ConfigServer> servers = this->getServers();
	std::vector<ConfigServer>::iterator server = servers.begin();
	std::vector<ConfigLocation> locations;
	std::vector<ConfigLocation>::iterator location;

	while (server != servers.end())
	{
		locations = (*server).getLocations();
		location = locations.begin();

		while (location != locations.end())
		{
			if ((*location).getRoot().length() > 0 && (*location).getAlias().length() > 0)
			{
				this->errors_ += (*server).getServerIp() + ":" + positiveNumberToString((*server).getPort());
				this->errors_ += ", location: \"" + (*location).getPrefix() + "\".";
				this->errors_ += " Should be only 'alias' or 'root'.\n";
			}

			++location;
		}

		++server;
	}
}

bool Config::delimiter(char ch)
{
	return std::isspace(ch) || ch == '\n';
}

bool Config::notdelimiter(char ch)
{
	return !(delimiter(ch));
}

std::vector<std::string> Config::splitWords(const std::string &str)
{
	std::vector<std::string> words;
	std::string::const_iterator begin = str.begin();

	while (begin != str.end())
	{
		begin = std::find_if(begin, str.end(), notdelimiter);
		std::string::const_iterator end = std::find_if(begin, str.end(), delimiter);
		if (begin != str.end())
		{
			std::string substr = std::string(begin, end);
			if (substr.length() > 0)
				words.push_back(substr);
			begin = end;
		}
	}

	return words;
}
