#DROP DATABASE facebook3;

#CREATE DATABASE facebook3;
USE facebook3;
CREATE TABLE train (id INT, title CHAR(40), tags VARCHAR(150));
CREATE TABLE sample (id INT, title TEXT, titleTags VARCHAR(120), language VARCHAR(50), tags VARCHAR(150));

CREATE TABLE tags (name VARCHAR(50), countAll INT, countYes INT, countNo INT);
CREATE TABLE tags_use (name VARCHAR(50), countAll INT, countYes INT, countNo INT, countIncorrect INT, relatedTags VARCHAR(150));

CREATE TABLE tags_body (name VARCHAR(50), countAll INT, countYes INT, countNo INT, countIncorrect INT);
