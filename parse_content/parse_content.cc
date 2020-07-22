#include "parse_content.h"

// read buffer one line, and return LINE_STATUS
LINE_STATUS parse_line(char* buffer, int& checked_index, int& read_index) {
    char tmp;
    for(; checked_index < read_index; ++checked_index) {
        tmp = buffer[checked_index];
        if(tmp == '\r') {
            // read the last elem
            if(checked_index+1 == read_index) {
                return LINE_OPEN;
            } // read one line
            else if (buffer[checked_index+1] == '\n') {
                buffer[checked_index++] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
        else if(tmp == '\n') {
            if(checked_index>1 && buffer[checked_index-1] == '\r') {
                buffer[checked_index-1] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
    }
    // haven't read over
    return LINE_OPEN;
}

// parse request line
HTTP_CODE parse_requestline(char* temp, CHECK_STATE& check_state) {
    // get the url's first position
    char* url = strpbrk(temp, " \t");
    if(!url){
        return BAD_REQUEST;
    }
    *url++ = '\0';

    // check get/post
    char* method = temp;
    if(strcasecmp(method, "GET") == 0) {
        printf("The request method is GET!\n");
    }else{
        return BAD_REQUEST;
    }

    url += strspn(url, " \t");
    char* version = strpbrk(url, " \t");
    if(!version){
        return BAD_REQUEST;
    }
    *version++ = '\0';
    version += strspn(version, " \t");

    // check http version and url
    if(strcasecmp(version, "HTTP/1.1") == 0) {
        printf("The vesion of http is 1.1\n");
    }else{
        return BAD_REQUEST;
    }

    // check url format: http://xxx/
    if(strncasecmp(url, "HTTP://", 7) == 0) {
        url+=7;
        url = strchr(url, '/');
    }
    if(!url || url[0]!='/')
        return BAD_REQUEST;

    printf("The url is %s\n", url);

    check_state = CHECK_STATE_HEADER;
    return NO_REQUEST;
}

// parse header, now only parse host
HTTP_CODE parse_header(char* temp) {
    if(temp[0] == '\0')
        return GET_REQUEST;
    else if(strncasecmp(temp, "HOST:", 5) == 0) {
        temp += 5;
        temp += strspn(temp, " \t");
        printf("The host is %s\n", temp);
    }
    else{
        printf("Other header code not handler\n");
    }

    return NO_REQUEST;
}

HTTP_CODE parse_content(char* buffer, int&checkd_index, int& read_index, CHECK_STATE& check_state, int& start_line) {
    LINE_STATUS line_status = LINE_OK;
    HTTP_CODE retcode = GET_REQUEST;

    // process buffer
    while( (line_status = parse_line(buffer, checkd_index, read_index)) == LINE_OK ) {
        char* temp = buffer + start_line;
        start_line = checkd_index;
        switch (check_state) {
        case CHECK_STATE_REQUESTLINE:
            retcode = parse_requestline(temp, check_state);
            if(retcode == BAD_REQUEST) {
                return BAD_REQUEST;
            }
            break;
        case CHECK_STATE_HEADER:
            retcode = parse_header(temp);
            if(retcode == BAD_REQUEST)
                return BAD_REQUEST;
            else if(retcode == GET_REQUEST)
                return GET_REQUEST;
        default:
            return INTERNAL_ERROR;
        }
    }

    if(line_status == LINE_OPEN)
        return NO_REQUEST;
    else
        return BAD_REQUEST;
}
