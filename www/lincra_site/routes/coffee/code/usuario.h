#ifndef USER_H
#define USER_H

struct user {
	string id;
	string username;
};

internal void
get_user_by_guest_id(user *user, string guest_id, server_data *server_data, string_bucket *string_list) {
    database_data* database = &server_data->database_data;
    log_string(guest_id);
    string query_string = to_string(SELECT id, username FROM user_table WHERE guest_id = ":guest_id");
    token_replace(query_string, ":guest_id", guest_id);  

    database_response *response = database_query_request(database, query_string);
    string_free(string_list, query_string); 
    
    if(response) {
	    row_data row = mysql_fetch_row(response);
	    ulong* length_array = mysql_fetch_lengths(response);

	    if(row) {
	        string_alloc(string_list, user->id,       row[0], length_array[0]);
	        string_alloc(string_list, user->username, row[1], length_array[1]);
	    } else {
	        // TODO: error
	    }
	    mysql_free_result(response);
    }
}

internal b32
get_guest_cookie(string *out_value, server_data *server_data, string_bucket *string_list, request_data *request_data) {
    database_data* database = &server_data->database_data;
    b32 result = false;
    
    if(request_data->cookies_array_count) {
        string guest_cookie = request_data->cookies_array[0].value; // @temporary: call get_cookie("guest_id");

        string query_string = to_string(SELECT guest_id FROM user_table WHERE guest_id = ":guest_id");
        token_replace(query_string, ":guest_id", guest_cookie);  

        database_response *response = database_query_request(database, query_string);
        string_free(string_list, query_string); 
        
        if(response) {
            row_data row = mysql_fetch_row(response);
            ulong* length_array = mysql_fetch_lengths(response);

            if(row) {
                if(out_value != NULL) {
                    string guest_value = cast_string(row[0], length_array[0], length_array[0]);
                    string_alloc(string_list, *out_value, guest_value);
                }

                result = true;
            } else {
                // remove token.
            }
            mysql_free_result(response);
        }
    }

    return(result);
}

#endif // !USER_H