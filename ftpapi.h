/**
 * FTP API：使用 Windows Socket 实现基础的 FTP 客户端功能。
 *
 */

#ifndef FTPAPI_H
#define FTPAPI_H

#include <winsock2.h>
#include <QDebug>

class FTPAPI
{
    public:
        FTPAPI();

        int connect_server( char *host, int port ) {
            return port;
        }

        int login_server( SOCKET sock, char *user, char *pwd ) {

            return ( user[0] == '1' ) ? 0 : 530;
        }

        int login_server( char *user, char *pwd ) {
            return login_server( clientSocket, user, pwd );
        }

    private:
        SOCKET clientSocket;

};

#endif // FTPAPI_H
