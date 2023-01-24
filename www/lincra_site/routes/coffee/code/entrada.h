#ifndef ENTRY_H
#define ENTRY_H

struct entry {
	string path;
	string title;
	string summary;
};

internal void
get_entries_from_database(database_data *database, string_bucket *string_list, entry *entry_array, u32 &entry_count, u32 entry_max_count) {
  	string query_string = to_string(SELECT path, title, summary FROM entry_table);
    database_response *response = database_query_request(database, query_string);
    string_free(string_list, query_string);

  	if(response) {
		row_data row = mysql_fetch_row(response);

		while(row != NULL && entry_count < entry_max_count) {
			ulong* length_array = mysql_fetch_lengths(response);

			string_alloc(string_list, entry_array[entry_count].path,    row[0], length_array[0]);
			string_alloc(string_list, entry_array[entry_count].title,   row[1], length_array[1]);
			string_alloc(string_list, entry_array[entry_count].summary, row[2], length_array[2]);

			entry_count += 1;
			row = mysql_fetch_row(response);
		}
    }

    mysql_free_result(response);
}

internal b32
set_new_entry_database(database_data *database, string_bucket *string_list, entry entry) {
	b32 result = false;
	
    string query_string = to_string(INSERT INTO entry_table (path, title, summary) VALUES (":path", ":title", ":summary"));
    token_replace(query_string, ":path",    entry.path);
    token_replace(query_string, ":title",   entry.title);
    token_replace(query_string, ":summary", entry.summary);
    
    result = database_query(database, query_string);
    string_free(string_list, query_string);
    return(result);
}

#endif