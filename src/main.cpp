#include "webserv.hpp"
#include "Core.hpp"

Log g_log;

// Change: positiveNumberToString => std::to_string(number)
std::string positiveNumberToString(size_t number)
{
	std::string to_string;

	if (number == 0)
		to_string = "0";

	while (number)
	{
		char num_char = (number % 10) + 48;
		to_string.insert(0, 1, num_char);
		number /= 10;
	}
	return to_string;
}

char *ft_strdup(const char *s)
{
	char *result;
	int i = 0;

	result = (char *)malloc(strlen(s) + 1);
	if (result)
	{
		for (i = 0; s[i]; ++i)
			result[i] = s[i];
		result[i] = 0;
	}
	return (result);
}

size_t ft_pow(size_t num, size_t pow)
{
	size_t answer = pow == 0 ? 1 : num;
	for (size_t i = 1; i < pow; i++)
		answer *= num;
	return answer;
}

std::vector<std::string> ws_split(const std::string &str, std::string delimiter)
{
	std::vector<std::string> lines;
	std::string::size_type begin = 0;
	std::string::size_type end = str.find(delimiter);

	while (end != std::string::npos)
	{
		std::string substr = str.substr(begin, end - begin);
		if (substr.length() > 0)
			lines.push_back(substr);
		begin = end + delimiter.length();
		end = str.find(delimiter, begin);
	}
	lines.push_back(str.substr(begin, end - begin));

	return lines;
}

void exitWebServ(int signal)
{
	(void)signal;

	g_log.printErrorMessage("Signal: " + positiveNumberToString(signal) + ". Ending webserv...");
	exit(1);
}

int main(int argc, char *argv[])
{
	if (argc > 2)
	{
		g_log.printErrorMessage("Too many arguments.");
		exit(1);
	}

	// --- read && parse config file ---
	std::string configFile = (argc == 1) ? DEFAULT_CONF : argv[1];
	Config config(configFile);
	if (config.getErrors().length() > 0)
	{
		g_log.printErrorMessage("Config file errors:\n" + config.getErrors());
		exit(1);
	}

	// --- run webServer ---
	signal(SIGINT, exitWebServ);
	signal(SIGTSTP, exitWebServ);

	g_log.setLogError(config.getLogError());
	g_log.setLogAccess(config.getLogAccess());

	Core serv(config);
	serv.startWebServer();

	return 0;
}