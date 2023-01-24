#ifndef DATABASE_H
#define DATABASE_H

#include "mysql.h"

#define database_response MYSQL_RES
#define row_data MYSQL_ROW

struct database_data {
    MYSQL *connection;
    
    char *host;
    char *username;
    char *password;
    char *database_name;
    char *sock;

    u32 port;
    u32 flags;
};

internal void
database_init_and_connect(database_data *database) {
    database->connection = mysql_init(NULL);
    if(!mysql_real_connect(database->connection, database->host, database->username, database->password, database->database_name, database->port, database->sock, database->flags)) {
        log_error("Error %s (%d)", mysql_error(database->connection), mysql_errno(database->connection));
    }
}

internal b32
database_query(database_data *database, string query_string) {
    b32 result = mysql_query(database->connection, query_string.data);
    return(result != 0);
}

internal MYSQL_RES*
database_query_request(database_data *database, string query_string) {
    MYSQL_RES *response = NULL;

    b32 result = mysql_query(database->connection, query_string.data);
    if(result == 0) {
        response = mysql_use_result(database->connection);
    }
    return(response);
}

internal void
database_close(database_data *database) {
    mysql_close(database->connection);
}

#endif // !DATABASE_H
