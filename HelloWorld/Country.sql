-- Table that holds various countries of the world
-- and their number of inhabitants

CREATE TABLE country
(
  id          INTEGER NOT NULL
 ,name        VARCHAR(100)
 ,inhabitants INTEGER
 ,continent   VARCHAR(20)

);

ALTER TABLE country
  ADD CONSTRAINT pk_country
      PRIMARY KEY(id);



INSERT INTO country VALUES (1,'China',         1386641728,'Asia');
INSERT INTO country VALUES (2,'India',         1349521845,'Asia');
INSERT INTO country VALUES (3,'United States',  325773000,'North America');
INSERT INTO country VALUES (4,'Indonesia',      262571000,'Asia');
INSERT INTO country VALUES (5,'Brazil',         210707000,'South America');

SELECT * FROM country;