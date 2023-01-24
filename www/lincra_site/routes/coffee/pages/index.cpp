#include "http_server/http_server.h"
#include "../code/entry.h"


extern "C"
{    

    HTML_FUNCTION(index_page) {
        LOAD_HTML(const_string("header.html"));
        LOAD_HTML(const_string("navbar.html"));
        LOAD_HTML(const_string("index.html"));
        LOAD_HTML(const_string("footer.html"));
    }

    HTML_FUNCTION(show_entry_list) {
        const u32 entry_max_count = 10;
        entry entry_array[entry_max_count];
        u32 entry_count = 0;

        get_entries_from_database(&server_data->database_data, string_list, entry_array, entry_count, entry_max_count);

        for(u32 i = 0; i < entry_count; i++) {
            entry entry = entry_array[i];

            INNER_HTML(
                <article class="contenido__producto"><img src="{URL_PATH()}/{IMG_PATH()}/{entry.path}" alt="imagen blog">
                    <h4 class="producto__titulo no-margin">{entry.title}</h4>
                    <p class="producto__parrafo no-margin">{entry.summary}</p>
                    <a class="producto__link" href="{URL_PATH()}/entry">Read entry</a>
                </article>
            );

            REPLACE_HTML(entry.path);
            REPLACE_HTML(entry.title);
            REPLACE_HTML(entry.summary);

            // @temporary: use clear_entries_array(entry_array, entry_count);
            string_free(string_list, entry.title);
            string_free(string_list, entry.path);
            string_free(string_list, entry.summary);
        }
    }


    HTML_FUNCTION(registrar) {
        database_data* database = &server_data->database_data;

        string username = request_data->get_data_array[0].value;
        string password = request_data->get_data_array[1].value;

        string query_string = to_string(INSERT INTO user_table (username, guest_id, hashed_password) VALUES (":username", "guest_id", ":password"));
        token_replace(query_string, ":username", username);
        token_replace(query_string, ":password", password);

        database_response *response = database_query_request(database, query_string);
        string_free(string_list, query_string); 

        http_change_url("/");
    }

    HTML_FUNCTION(login) {
        database_data* database = &server_data->database_data;

        // @temporary: use get_post_value("username");
        string username = request_data->get_data_array[0].value;
        string password = request_data->get_data_array[1].value;
        
        string query_string = to_string(SELECT guest_id FROM user_table WHERE username = ":username" AND hashed_password = ":password");
        token_replace(query_string, ":username", username);
        token_replace(query_string, ":password", password);
        
        database_response *response = database_query_request(database, query_string);
        string_free(string_list, query_string); 
        
        if(response) {
            row_data row = mysql_fetch_row(response);
            ulong* length_array = mysql_fetch_lengths(response);
            
            // @temporary: replace for http_send_immediate_cookie();
            http_change_url("/");
            if(row) {
                string guest_id = cast_string(row[0], length_array[0], length_array[0]);

                // @temporary: replace for http_new_cookie("value");
                string_alloc(string_list, request_data->extra_header_data, const_string("Set-Cookie: guest_id=:guest_id; Path=/\r\n"));
                token_replace(request_data->extra_header_data, ":guest_id", guest_id);
            }
            
            mysql_free_result(response);
        }
    }

    HTML_FUNCTION(logout) {
        // @temporary: replace for http_send_immediate_cookie();
        http_change_url("/");

        // @temporary: replace for http_new_cookie("value");
        string_alloc(string_list, request_data->extra_header_data, const_string("Set-Cookie: guest_id=test_guest_id; Path=/; expires=Thu, 01 Jan 1970 00:00:00 GMT\r\n"));
    }
}