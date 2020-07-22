#ifndef PARSE_CONTENT_H
#define PARSE_CONTENT_H

#include <string.h>
#include <stdio.h>

const int BUFFER_SIZE = 4096;

// 主状态机，用于具体分析，是请求行还是头部，main DFA status: check requst_line , check header
enum CHECK_STATE {CHECK_STATE_REQUESTLINE=0, CHECK_STATE_HEADER};

// 从状态机，用于解析一行，读完，出错，未读完，sub DFA status: line ok, line bad, line open
enum LINE_STATUS {LINE_OK = 0, LINE_BAD, LINE_OPEN};

// 服务器处理http请求的状态，未读完，读完，语法错误，没有权限，服务器内部错误，已关闭连接
enum HTTP_CODE {NO_REQUEST, GET_REQUEST, BAD_REQUEST, FORBIDDEN_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION};

static const char* szret[] = {"I got a correct result\n", "someting wrong\n"};

LINE_STATUS parse_line(char* buffer, int& checked_index, int& read_index);
HTTP_CODE parse_requestline(char* temp, CHECK_STATE& check_state);
HTTP_CODE parse_header(char* temp);
HTTP_CODE parse_content(char* buffer, int&checkd_index, int& read_index, CHECK_STATE& check_state, int& start_line);

#endif // PARSE_CONTENT_H
