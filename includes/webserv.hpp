#ifndef WEBSERV_HPP
#define WEBSERV_HPP

/*
** malloc, free, write, open, read, close, mkdir,
** rmdir, unlink, fork, wait, waitpid, wait3, wait4,
** signal, kill, exit, htons, htonl, ntohs, ntohl,
** getcwd, chdir, stat, lstat, fstat, lseek, opendir,
** readdir, closedir, execve, dup, dup2, pipe,
** strerror, errno, gettimeofday, strptime, strftime,
** usleep, select, socket, accept, listen, send, recv,
** bind, connect, inet_addr, setsockopt, getsockname,
** fcntl
*/

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <map>
#include <algorithm>
#include <exception>
#include <ctime>
#include <cstring>
#include "webserv_lib.hpp"

#define DEFAULT_PATH "./config/"
#define DEFAULT_CONF "./config/default.conf"
#define WEBSERV_NAME "webserv/1.0.0"
#define WEBSERV_HTTP "HTTP/1.1"
#define WEBSERV_AUTH "Basic"

#define DEBUG_SERV 2

#define RECV_BUFFER 16 * 1024
#define SEND_BUFFER 10000000
#define TIMEOUT_SEC 10

#define UNDERLINE_W "\e[4;37m"
#define RED_C "\e[0;31m"
#define GREEN_C "\e[0;32m"
#define YELLOW_C "\e[0;33m"
#define BLUE_C "\e[0;36m"
#define PURPLE_C "\e[0;35m"
#define RESET_C "\e[00m"

#include "Log.hpp"
class Log;
extern Log g_log;



std::string positiveNumberToString(size_t number);
size_t ft_pow(size_t num, size_t pow);
std::vector<std::string> ws_split(const std::string &str, std::string delimiter);
void exitWebServ(int signal);
char *ft_strdup(const char *s);

#endif
