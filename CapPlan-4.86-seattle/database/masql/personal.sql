CREATE TABLE personal (
    PERSONAL_ID INT IDENTITY(1,1),
    LASTNAME VARCHAR (30) with compression NOT NULL,
    FIRSTNAME VARCHAR (30) with compression NOT NULL,
    PREFIX VARCHAR (10) with compression NOT NULL,
    INITIALS VARCHAR (6) with compression NOT NULL,
    ADDRESS VARCHAR (60) with compression NOT NULL,
    POSTCODE VARCHAR (12) with compression NOT NULL,
    CITY VARCHAR (30) with compression NOT NULL,
    TELEPHONE VARCHAR (16) with compression NOT NULL,
    TELEPHONE2 VARCHAR (16) with compression NOT NULL,
    BIRTHDATE DATETIME NOT NULL,
    HIDDEN TINYINT DEFAULT 0 NOT NULL,
    CREATED_DATE DATETIME NOT NULL,
    CREATED_BY VARCHAR (16) with compression NOT NULL,
    MEMOTEXT VARCHAR (255) with compression NOT NULL,
    PRIMARY KEY(PERSONAL_ID)
);

CREATE INDEX I_lastname ON personal (LASTNAME);
CREATE INDEX I_postcode ON personal (POSTCODE);
CREATE INDEX I_address ON personal (ADDRESS);
CREATE INDEX I_city ON personal (CITY);
