/**
 * FTP API：使用 Windows Socket 实现基础的 FTP 客户端功能。
 *
 */

#ifndef FTPAPI_H
#define FTPAPI_H

#include <iostream>
#include <io.h>
#include <string.h>
#include <stdlib.h>
#include <winsock2.h>

#define BUFSIZE 512
#define FTP_SUCCESS 200  //成功
#define FTP_SERVICE_READY 220 //服务器就绪
#define FTP_LOGIN_SUCCESS 230 //登录成功
#define FTP_FILE_ACTION_COMPLETE 250 //文件行为完成
#define FTP_FILE_CREATED 257 //文件创建成功
#define FTP_PASSWORD_REQUIREd 331 //要求输入密码
#define FTP_LOGIN_PASSWORD_INCORRECT 530 //用户密码错误
#define FTP_SERVICE_CLOSE 421 //服务关闭

class FTPAPI
{
public:
    /**
     * @brief 连接并登录FTP服务器 USER PASS
     * @param 服务器ip
     * @param 端口号
     * @param 用户名
     * @param 密码
     * @return 已连接到FTP服务器的socket，-1：连接远程主机失败，-2：用户名或密码错误
     */
    int login_server(char* ip, int port, char* user, char* pwd)
    {
        clientSocket = connect_server(ip, port);
        if (clientSocket == -1)
        {
            return -1;
        }
        if (login_server(clientSocket, user, pwd) == -1)
        {
            closesocket(clientSocket);
            return -2;
        }

        return clientSocket;
    }

    /**
     * @brief 断开FTP服务器
     * @return 断开服务器的状态码
     */
    int ftp_quit()
    {
        return ftp_quit(clientSocket);
    }

    /**
     * @brief 更改工作目录 CWD
     * @param 工作目录
     * @return 0：成功，-1：失败
     */
    int ftp_cwd(char* path)
    {
        return ftp_cwd(clientSocket, path);
    }

    /**
     * @brief 回到上级目录 CDUP
     * @return 0：正常操作返回，其它：服务器返回错误码
     */
    int ftp_cdup()
    {
        return ftp_cdup(clientSocket);
    }

    /**
     * @brief 创建目录 MKD
     * @param 文件目录路径(可相对路径，绝对路径)
     * @return 0：正常操作返回，其他：服务器返回错误码
     */
    int ftp_mkd(char* path)
    {
        return ftp_mkd(clientSocket, path);
    }

    /**
     * @brief 如果是文件名列出文件信息，如果是目录则列出文件列表 LIST
     * @param 相对路径或绝对路径
     * @param 列表信息
     * @param 列表信息大小
     * @return 0：成功，-1：创建pasv错误，其他：服务器返回其他错误码
     */
    int ftp_list(char* path, char** data, int* data_len)
    {
       return ftp_list(clientSocket, path, data, data_len);
    }

    /**
     * @brief 删除目录 RMD
     * @param 相对路径或绝对路径
     * @return 0：成功，其他：服务器返回错误码
     */
    int ftp_deletefolder(char* path)
    {
        return ftp_deletefolder(clientSocket, path);
    }

    /**
     * @brief 删除文件 DELE
     * @param 相对路径或绝对路径
     * @return 0：成功，其他：服务器返回错误码
     */
    int ftp_deletefile(char* filename)
    {
        return ftp_deletefile(clientSocket, filename);
    }

    /**
     * @brief 修改文件名&移动目录 RNFR RNTO
     * @param 源地址
     * @param 目的地址
     * @return 0：成功，其他：服务器返回错误码
     */
    int ftp_renamefile(char* s, char* d)
    {
        return ftp_renamefile(clientSocket, s, d);
    }

    /**
     * @brief 从服务器复制文件到本地 TYPE PASV RETR
     * @param 源地址
     * @param 目的地址
     * @param 文件大小
     * @return 0：成功，-1：文件创建失败，-2：pasv接口错误，其他：服务器返回其他错误码
     */
    int ftp_server2local(char* s, char* d, int* size)
    {
        return ftp_server2local(clientSocket, s, d, size);
    }

    /**
     * @brief 从本地复制文件到服务器 TYPE PASV STOR
     * @param 源地址
     * @param 目的地址
     * @param 文件大小
     * @return 0：成功，-1：文件创建失败，-2：pasv接口错误，其他：服务器返回其他错误码
     */
    int ftp_local2server(char* s, char* d, int* size)
    {
        return ftp_local2server(clientSocket, s, d, size);
    }

private:
    SOCKET clientSocket; // 命令链路SOCKET

    /**
     * @brief 创建一个socket并返回
     * @param 服务器ip或域名
     * @param 端口号
     * @return SOCKET
     */
    SOCKET socket_connect(char* host, int port)
    {
        int i = 0;
        //初始化 Socket dll
        WSADATA wsaData;
        WORD socketVersion = MAKEWORD(2, 0);
        if (WSAStartup(socketVersion, &wsaData))
        {
            printf("Init socket dll error!");
            exit(1);
        }

        struct hostent* server = gethostbyname(host);
        if (!server)
            return -1;
        unsigned char ch[4];
        char ip[20];
        //一个hostname 可以对应多个ip
        while (server->h_addr_list[i] != nullptr)
        {
            memcpy(&ch, server->h_addr_list[i], 4);
            sprintf(ip, "%d.%d.%d.%d", ch[0], ch[1], ch[2], ch[3]);
            i++;
        }

        //创建Socket
        SOCKET s = socket(AF_INET, SOCK_STREAM, 0); //TCP socket
        if (SOCKET_ERROR == s)
        {
            printf("Create Socket Error!");
            exit(1);
        }
        //设置超时连接
        int timeout = 3000; //复杂的网络环境要设置超时判断
        int ret = setsockopt(s, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout));
        ret = setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
        //指定服务器地址
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_addr.S_un.S_addr = inet_addr(ip);
        address.sin_port = htons((unsigned short)port);
        //连接
        if (SOCKET_ERROR == connect(s, (LPSOCKADDR)&address, sizeof(address)))
        {
            printf("Can Not Connect To Server IP!\n");
            exit(1);
        }
        return s;
    }

    /**
     * @brief 连接到FTP服务器
     * @param 服务器ip或域名
     * @param 端口号
     * @return SOCKET
     */
    SOCKET connect_server(char* host, int port)
    {
        SOCKET ctrl_sock;
        char buf[BUFSIZE] = { 0 };
        int result;
        SSIZE_T len;

        ctrl_sock = socket_connect(host, port);
        if (-1 == ctrl_sock)
        {
            return -1;
        }

        while ((len = recv(ctrl_sock, buf, BUFSIZE, 0)) > 0)
        {
            buf[len] = 0;
            printf("%s\n", buf); //220-FileZilla Server version 0.9.43 beta
        }

        sscanf(buf, "%d", &result);

        if (FTP_SERVICE_READY != result)
        {
            printf("FTP Not ready, Close the socet.");
            closesocket(ctrl_sock); //关闭Socket
            return -1;
        }
        return ctrl_sock;
    }

    /**
     * @brief send发送命令，并返回recv结果
     * @param SOCKET
     * @param 命令
     * @param 命令返回码-命令返回描述
     * @param 命令返回字节数
     * @return 0：发送成功，-1：发送失败
     */
    int ftp_sendcmd_re(SOCKET sock, char* cmd, char* re_buf, SSIZE_T* len)
    {
        char buf[BUFSIZE];
        SSIZE_T r_len;
        if (send(sock, cmd, strlen(cmd), 0) == -1)
        {
            return -1;
        }
        r_len = recv(sock, buf, BUFSIZE, 0);
        if (r_len < 1)
            return -1;
        buf[r_len] = 0;
        if (nullptr != len)
            *len = r_len;
        if (nullptr != re_buf)
            sprintf(re_buf, "%s", buf);
        return 0;
    }

    /**
     * @brief send发送命令
     * @param SOCKET
     * @param 命令
     * @return FTP响应码
     */
    int ftp_sendcmd(SOCKET sock, char* cmd)
    {
        char buf[BUFSIZE];
        int result;
        SSIZE_T len;
        printf("FTP Client: %s", cmd);
        result = ftp_sendcmd_re(sock, cmd, buf, &len);
        printf("FTP Server: %s", buf);
        if (0 == result)
        {
            sscanf(buf, "%d", &result);
        }
        return result;
    }

    /**
     * @brief 登录FTP服务器 USER PASS
     * @param SOCKET
     * @param 用户名
     * @param 密码
     * @return 0：登录成功，-1：登录失败
     */
    int login_server(SOCKET sock, char* user, char* pwd)
    {
        char buf[BUFSIZE];
        int result;
        sprintf(buf, "USER %s\r\n", user);
        //这里要对socket进行阻塞
        int timeout = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
        result = ftp_sendcmd(sock, buf);
        if (FTP_LOGIN_SUCCESS == result) //直接登录
            return 0;
        else if (FTP_PASSWORD_REQUIREd == result) //需要密码
        {
            sprintf(buf, "PASS %s\r\n", pwd);
            result = ftp_sendcmd(sock, buf);
            if (FTP_LOGIN_SUCCESS == result)
            {
                return 0;
            }
            else //530 密码错误
            {
                return -1;
            }
        }
        else
        {
            return -1;
        }
    }

    /**
     * @brief winsock使用后，要调用WSACleanup函数关闭网络设备，以便释放其占用的资源
     * @param SOCKET
     */
    void socket_close(int c_sock)
    {
        WSACleanup();
    }

    /**
     * @brief 断开FTP服务器 QUIT
     * @param SOCKET
     * @return 成功断开状态码
     */
    int ftp_quit(SOCKET sock)
    {
        int result = 0;
        char* c = const_cast<char*>("QUIT\r\n");
        result = ftp_sendcmd(sock, c);
        closesocket(sock);
        socket_close(sock);
        return result;
    }

    /**
     * @brief 设置FTP传输类型 TYPE
     * @param SOCKET
     * @param 类型，A:ascii I:Binary
     * @return 0：成功，-1：失败
     */
    int ftp_type(SOCKET sock, char mode)
    {
        char buf[BUFSIZ];
        sprintf(buf, "TYPE %c\r\n", mode);
        if (FTP_SUCCESS != ftp_sendcmd(sock, buf))
            return -1;
        else
            return 0;
    }

    /**
     * @brief 更改工作目录 CWD
     * @param SOCKET
     * @param 工作目录
     * @return 0：成功，-1：失败
     */
    int ftp_cwd(SOCKET sock, char* path)
    {
        char buf[BUFSIZE];
        int result;
        sprintf(buf, "CWD %s\r\n", path);
        result = ftp_sendcmd(sock, buf);
        if (FTP_FILE_ACTION_COMPLETE != result)  //250 文件行为完成
            return -1;
        else
            return 0;
    }

    /**
     * @brief 回到上级目录 CDUP
     * @param SOCKET
     * @return 0：正常操作返回，其它：服务器返回错误码
     */
    int ftp_cdup(SOCKET sock)
    {
        int result;
        char* c = const_cast<char*>("CDUP\r\n");
        result = ftp_sendcmd(sock, c);
        if (FTP_FILE_ACTION_COMPLETE == result || FTP_SUCCESS == result)
            return 0;
        else
            return result;
    }

    /**
     * @brief 创建目录 MKD
     * @param SOCKET
     * @param 文件目录路径(可相对路径，绝对路径)
     * @return 0：正常操作返回，其他：服务器返回错误码
     */
    int ftp_mkd(SOCKET sock, char* path)
    {
        char buf[BUFSIZE];
        int result;
        sprintf(buf, "MKD %s\r\n", path);
        result = ftp_sendcmd(sock, buf);
        if (FTP_FILE_CREATED != result) //257 路径名建立
            return result; //550 目录已存在
        else
            return 0;
    }

    /**
     * @brief 连接到PASV接口 PASV
     *        被动方式连接过程：客户端向服务器的FTP端口（默认是21）发送连接请求，服务器接受连接，建立一条命令链路。
     * @param 命令链路SOCKET cmd-socket
     * @return 数据链路SOCKET raw-socket，-1：创建失败
     */
    SOCKET ftp_pasv_connect(SOCKET c_sock)
    {
        SOCKET r_sock;
        int send_result;
        SSIZE_T len;
        int addr[6]; //IP*4+Port*2
        char buf[BUFSIZE];
        char result_buf[BUFSIZE];

        //设置PASV被动模式
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "PASV\r\n");
        send_result = ftp_sendcmd_re(c_sock, buf, result_buf, &len);
        if (send_result == 0)
        {
            sscanf(result_buf, "%*[^(](%d,%d,%d,%d,%d,%d)",
                   &addr[0], &addr[1], &addr[2], &addr[3],
                    &addr[4], &addr[5]);
        }

        //连接PASV端口
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
        r_sock = socket_connect(buf, addr[4] * 256 + addr[5]);
        if (-1 == r_sock)
            return -1;
        return r_sock;
    }

    /**
     * @brief 如果是文件名列出文件信息，如果是目录则列出文件列表 LIST
     * @param SOCKET
     * @param 相对路径或绝对路径
     * @param 列表信息
     * @param 列表信息大小
     * @return 0：成功，-1：创建pasv错误，其他：服务器返回其他错误码
     */
    int ftp_list(SOCKET c_sock, char* path, char** data, int* data_len)
    {
        SOCKET r_sock;
        char buf[BUFSIZE];
        int send_re;
        int result;
        SSIZE_T len, buf_len, total_len;

        //连接到PASV接口
        r_sock = ftp_pasv_connect(c_sock);
        if (-1 == r_sock)
        {
            return -1;
        }
        //发送LIST命令
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "LIST %s\r\n", path);
        send_re = ftp_sendcmd(c_sock, buf);
        if (send_re >= 300 || send_re == 0)
            return send_re;
        len = total_len = 0;
        buf_len = BUFSIZE;
        char* re_buf = (char*)malloc(buf_len);
        while ((len = recv(r_sock, buf, BUFSIZE, 0)) > 0)
        {
            if (total_len + len > buf_len)
            {
                buf_len *= 2;
                char* re_buf_n = (char*)malloc(buf_len);
                memcpy(re_buf_n, re_buf, total_len);
                free(re_buf);
                re_buf = re_buf_n;
            }
            memcpy(re_buf + total_len, buf, len);
            total_len += len;
        }
        closesocket(r_sock);

        //向服务器接收返回值
        memset(buf, 0, sizeof(buf));
        len = recv(c_sock, buf, BUFSIZE, 0);
        buf[len] = 0;
        sscanf(buf, "%d", &result);
        if (result != 226)
        {
            free(re_buf);
            return result;
        }
        *data = re_buf;
        *data_len = total_len;
        return 0;
    }

    /**
     * @brief 删除目录 RMD
     * @param 命令链路SOCKET
     * @param 相对路径或绝对路径
     * @return 0：成功，其他：服务器返回错误码
     */
    int ftp_deletefolder(SOCKET sock, char* path)
    {
        char buf[BUFSIZE];
        int result;
        sprintf(buf, "RMD %s\r\n", path);
        result = ftp_sendcmd(sock, buf);
        if (FTP_FILE_ACTION_COMPLETE != result)
        {
            //550 Directory not empty.
            //550 Directory not found.
            return result;
        }
        return 0;
    }

    /**
     * @brief 删除文件 DELE
     * @param 命令链路SOCKET
     * @param 相对路径或绝对路径
     * @return 0：成功，其他：服务器返回错误码
     */
    int ftp_deletefile(SOCKET sock, char* filename)
    {
        char buf[BUFSIZE];
        int result;
        sprintf(buf, "DELE %s\r\n", filename);
        result = ftp_sendcmd(sock, buf);
        if (FTP_FILE_ACTION_COMPLETE != 250) //250 File deleted successfully
        {
            //550 File not found.
            return result;
        }
        return 0;
    }

    /**
     * @brief 修改文件名&移动目录 RNFR RNTO
     * @param 命令链路SOCKET
     * @param 源地址
     * @param 目的地址
     * @return 0：成功，其他：服务器返回错误码
     */
    int ftp_renamefile(SOCKET sock, char* s, char* d)
    {
        char buf[BUFSIZE];
        int result;
        sprintf(buf, "RNFR %s\r\n", s);
        result = ftp_sendcmd(sock, buf);
        if (350 != result) //350 文件行为暂停，因为要进行移动操作
            return result;
        sprintf(buf, "RNTO %s\r\n", d);
        result = ftp_sendcmd(sock, buf);
        if (FTP_FILE_ACTION_COMPLETE != result)
        {
            return result;
        }
        return 0;
    }

    /**
     * @brief 从服务器复制文件到本地 TYPE PASV RETR
     * @param SOCKET
     * @param 源地址
     * @param 目的地址
     * @param 文件大小
     * @return 0：成功，-1：文件创建失败，-2：pasv接口错误，其他：服务器返回其他错误码
     */
    int ftp_server2local(SOCKET c_sock, char* s, char* d, int* size)
    {
        SOCKET d_sock;
        SSIZE_T len, write_len;
        char buf[BUFSIZ];
        int result;
        *size = 0;
        //打开本地文件
        FILE* fp = fopen(d, "wb");
        if (nullptr == fp)
        {
            printf("Can't Open the file.\n");
            return -1;
        }
        //设置传输模式
        ftp_type(c_sock, 'I');

        //连接到PASV接口 用于传输文件
        d_sock = ftp_pasv_connect(c_sock);
        if (-1 == d_sock)
        {
            fclose(fp); //关闭文件
            return -2;
        }

        //发送RETR命令
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "RETR %s\r\n", s);
        result = ftp_sendcmd(c_sock, buf);
        // 150 Opening data channel for file download from server of "xxxx"
        if (result >= 300 || result == 0) //失败可能是没有权限什么的，具体看响应码
        {
            fclose(fp);
            return result;
        }

        //开始向PASV读取数据(下载)
        memset(buf, 0, sizeof(buf));
        while ((len = recv(d_sock, buf, BUFSIZE, 0)) > 0)
        {
            write_len = fwrite(&buf, len, 1, fp);
            if (write_len != 1) //写入文件不完整
            {
                closesocket(d_sock); //关闭套接字
                fclose(fp); //关闭文件
                return -1;
            }
            if (nullptr != size)
            {
                *size += write_len;
            }
        }
        //下载完成
        closesocket(d_sock);
        fclose(fp);

        //向服务器接收返回值
        memset(buf, 0, sizeof(buf));
        len = recv(c_sock, buf, BUFSIZE, 0);
        buf[len] = 0;
        printf("%s\n", buf);
        sscanf(buf, "%d", &result);
        if (result >= 300)
        {
            return result;
        }
        //226 Successfully transferred "xxxx"
        return 0;
    }

    /**
     * @brief 从服务器复制文件到本地（支持断点续传） TYPE PASV REST RETR
     * @param SOCKET
     * @param 源地址
     * @param 目的地址
     * @param 文件大小
     * @return 0：成功，-1：文件创建失败，-2：pasv接口错误，其他：服务器返回其他错误码
     */
    int ftp_download(SOCKET c_sock, char* s, char* d, int* size)
    {
        SOCKET d_sock;
        SSIZE_T len, write_len;
        char buf[BUFSIZ];
        int result;
        long download_size;
        *size = 0;

        // 打开本地文件
        FILE* fp = fopen(d, "ab+");
        if (nullptr == fp)
        {
            printf("Can't Open the file.\n");
            return -1;
        }

        // 获取文件已下载的字节数
        download_size = filelength(fileno(fp));

        // 设置传输模式TYPE
        ftp_type(c_sock, 'I');

        // 连接到PASV接口用于传输文件
        d_sock = ftp_pasv_connect(c_sock);
        if (-1 == d_sock)
        {
            fclose(fp);
            return -2;
        }

        // 发送REST命令
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "REST %ld\r\n", download_size + 1);
        result = ftp_sendcmd(c_sock, buf);
        if (result >= 300 || result == 0)
        {
            fclose(fp);
            return result;
        }

        // 发送RETR命令
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "RETR %s\r\n", s);
        result = ftp_sendcmd(c_sock, buf);
        // 150 Opening data channel for file download from server of "xxxx"
        if (result >= 300 || result == 0) // 失败可能是没有权限什么的，具体看响应码
        {
            fclose(fp);
            return result;
        }

        // 开始向PASV读取数据(下载)
        memset(buf, 0, sizeof(buf));
        while ((len = recv(d_sock, buf, BUFSIZE, 0)) > 0)
        {
            write_len = fwrite(&buf, len, 1, fp);
            if (write_len != 1) // 写入文件不完整
            {
                closesocket(d_sock); // 关闭套接字
                fclose(fp); // 关闭文件
                return -1;
            }
            if (nullptr != size)
            {
                *size += write_len;
            }
        }
        // 下载完成
        closesocket(d_sock);
        fclose(fp);

        // 向服务器接收返回值
        memset(buf, 0, sizeof(buf));
        len = recv(c_sock, buf, BUFSIZE, 0);
        buf[len] = 0;
        printf("%s\n", buf);
        sscanf(buf, "%d", &result);
        if (result >= 300)
        {
            return result;
        }
        // 226 Successfully transferred "xxxx"
        return 0;
    }

    /**
     * @brief 从本地复制文件到服务器 TYPE PASV STOR
     * @param SOCKET
     * @param 源地址
     * @param 目的地址
     * @param 文件大小
     * @return 0：成功，-1：文件创建失败，-2：pasv接口错误，其他：服务器返回其他错误码
     */
    int ftp_local2server(SOCKET c_sock, char* s, char* d, int* size)
    {
        SOCKET d_sock;
        SSIZE_T len, send_len;
        char buf[BUFSIZE];
        FILE* fp;
        int send_re;
        int result;
        //打开本地文件
        fp = fopen(s, "rb");
        if (nullptr == fp)
        {
            printf("Can't Not Open the file.\n");
            return -1;
        }
        //设置传输模式
        ftp_type(c_sock, 'I');
        //连接到PASV接口
        d_sock = ftp_pasv_connect(c_sock);
        if (d_sock == -1)
        {
            fclose(fp);
            return -1;
        }

        //发送STOR命令
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "STOR %s\r\n", d);
        send_re = ftp_sendcmd(c_sock, buf);
        if (send_re >= 300 || send_re == 0)
        {
            fclose(fp);
            return send_re;
        }

        //开始向PASV通道写数据
        memset(buf, 0, sizeof(buf));
        while ((len = fread(buf, 1, BUFSIZE, fp)) > 0)
        {
            send_len = send(d_sock, buf, len, 0);
            if (send_len != len)
            {
                closesocket(d_sock);
                fclose(fp);
                return -1;
            }
            if (nullptr != size)
            {
                *size += send_len;
            }
        }
        //完成上传
        closesocket(d_sock);
        fclose(fp);

        //向服务器接收响应码
        memset(buf, 0, sizeof(buf));
        len = recv(c_sock, buf, BUFSIZE, 0);
        buf[len] = 0;
        sscanf(buf, "%d", &result);
        if (result >= 300)
        {
            return result;
        }
        return 0;
    }

    /**
     * @brief 获取一行响应码
     * @param SOCKET
     * @param 命令返回码-命令返回描述
     * @param 命令返回字节数
     * @return 0：发送成功，-1：发送失败
     */
    int ftp_recv(SOCKET sock, char* re_buf, SSIZE_T* len)
    {
        char buf[BUFSIZE];
        SSIZE_T r_len;
        int timeout = 3000;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
        r_len = recv(sock, buf, BUFSIZE, 0);
        timeout = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout));
        if (r_len < 1)
            return -1;
        buf[r_len] = 0;
        if (nullptr != len)
            *len = r_len;
        if (nullptr != re_buf)
            sprintf(re_buf, "%s", buf);
        return 0;
    }
};

#endif // FTPAPI_H
