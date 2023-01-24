#include "http_server/http_server.cpp"

#define HOST_IN_SERVER 0

int main(int argc, char* argv[]){
    string_bucket* string_list = NULL;
    server_data server_data = {};
    database_data* database = &server_data.database_data;
    
    database->host     = "localhost"; 
    database->username = "root"; 
    database->database_name = "web_database"; 
    database->sock     = NULL; 
    
    database->port  = 3306;
    database->flags = 0;

#if HOST_IN_SERVER
    database->password = "";
    start_http_server(&server_data, "server_ip", "server_domain");   
#else
    database->password = "";
    start_http_server(&server_data, "192.168.0.21", "192.168.0.21");
#endif  
    
    return(0);
}
