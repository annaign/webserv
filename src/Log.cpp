#include "Log.hpp"

Log::Log()
	: saveLogError_(true),
	  errorToFile_(false),
	  errorFileName_("log_error_"),
	  saveLogAccess_(true),
	  accessToFile_(false),
	  accessFileName_("log_access_")
{
}

Log::~Log()
{
}

void Log::setLogError(bool toFile, bool saveLog)
{
	this->saveLogError_ = saveLog;
	this->errorToFile_ = toFile;
}

void Log::setLogAccess(bool toFile, bool saveLog)
{
	this->saveLogAccess_ = saveLog;
	this->accessToFile_ = toFile;
}

bool Log::getSaveLogError() const
{
	return this->saveLogError_;
}

bool Log::getErrorToFile() const
{
	return this->errorToFile_;
}

bool Log::getSaveLogAccess() const
{
	return this->saveLogAccess_;
}

bool Log::getAccessToFile() const
{
	return this->accessToFile_;
}

std::string Log::getTimeStamp() const
{
	time_t timeNow;
	time(&timeNow);
	tm *timeInfo = localtime(&timeNow);
	char timeStamp[24] = {0};
	strftime(timeStamp, 24, "[%F %T] ", timeInfo);

	return std::string(timeStamp);
}

void Log::writeToFile(std::exception const &msg, std::string const &fileName) const
{
	time_t timeNow;
	time(&timeNow);
	tm *timeInfo = localtime(&timeNow);
	char date[12] = {0};
	strftime(date, 12, "%Y%m%d", timeInfo);

	std::string file = fileName + date;

	int fd = open(file.c_str(), O_CREAT | O_APPEND | O_WRONLY, 0664);

	if (fd == -1)
	{
		std::cerr << RED_C << "Can't open file: " << file << RESET_C << std::endl;
		return;
	}

	strftime(date, 12, "[%T] ", timeInfo);
	date[11] = '\0';
	std::string message = date;
	message += msg.what();
	message += "\n";

	if (write(fd, message.c_str(), message.length()) == -1)
		std::cerr << RED_C << strerror(errno) << "File name: " << file << RESET_C << std::endl;

	close(fd);
}

void Log::writeToFile(std::string const &msg, std::string const &fileName) const
{
	time_t timeNow;
	time(&timeNow);
	tm *timeInfo = localtime(&timeNow);
	char date[12] = {0};
	strftime(date, 12, "%Y%m%d", timeInfo);

	std::string file = fileName + date;

	int fd = open(file.c_str(), O_CREAT | O_APPEND | O_RDWR, 0664);

	if (fd == -1)
	{
		std::cerr << RED_C << "Can't open file: " << file << RESET_C << std::endl;
		return;
	}

	strftime(date, 12, "[%T] ", timeInfo);
	date[11] = '\0';
	std::string message = date;
	message += msg;
	message += "\n";

	if (write(fd, message.c_str(), message.length()) == -1)
		std::cerr << RED_C << strerror(errno) << "File name: " << file << RESET_C << std::endl;

	close(fd);
}

void Log::writeToConsole(std::exception const &msg, bool error) const
{
	if (error)
		std::cerr << RED_C << this->getTimeStamp() << msg.what() << RESET_C << std::endl;
	else
		std::cout
			<< msg.what() << this->getTimeStamp() << std::endl;
}

void Log::writeToConsole(std::string const &msg, bool error) const
{
	if (error)
		std::cerr << RED_C << this->getTimeStamp() << msg << RESET_C << std::endl;
	else
		std::cout << this->getTimeStamp() << msg << std::endl;
}

void Log::printErrorMessage(std::exception const &msg) const
{
	if (!this->saveLogError_)
		return;

	if (this->errorToFile_)
		this->writeToFile(msg, this->errorFileName_);
	else
		this->writeToConsole(msg, true);
}

void Log::printErrorMessage(std::string const &msg) const
{
	if (!this->saveLogError_)
		return;

	if (this->errorToFile_)
		this->writeToFile(msg, this->errorFileName_);
	else
		this->writeToConsole(msg, true);
}

void Log::printAccessMessage(std::exception const &msg) const
{
	if (!this->saveLogAccess_)
		return;

	if (this->accessToFile_)
		this->writeToFile(msg, this->accessFileName_);
	else
		this->writeToConsole(msg);
}

void Log::printAccessMessage(std::string const &msg) const
{
	if (!this->saveLogAccess_)
		return;

	if (this->accessToFile_)
		this->writeToFile(msg, this->accessFileName_);
	else
		this->writeToConsole(msg);
}
