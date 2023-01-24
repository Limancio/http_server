drop database if exists web_database;
CREATE DATABASE web_database;

USE web_database;

CREATE TABLE user_table(
	id INT NOT NULL AUTO_INCREMENT,
    username VARCHAR(32),
    guest_id VARCHAR(32),
    hashed_password VARCHAR(32),
    CONSTRAINT pk_id_user_table PRIMARY KEY (id)
);

CREATE TABLE entry_table(
	id INT NOT NULL AUTO_INCREMENT,
    path VARCHAR(100),
    title VARCHAR(100),
    summary VARCHAR(500),
    CONSTRAINT pk_id_entry_table PRIMARY KEY (id)
);
