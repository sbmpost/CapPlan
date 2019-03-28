CREATE TABLE docs(
    PERSONAL_ID INT NOT NULL,
    DOC_NAME VARCHAR (60) NOT NULL,
    DOC_SIZE INT DEFAULT 0 NOT NULL,
    COMMENT VARCHAR (60) NOT NULL,
    LAST_UPDATE DATETIME NOT NULL,
    LAST_UPDATE_UTC DATETIME NOT NULL,
    SIGNED TINYINT DEFAULT 0 NOT NULL,
    URGENT TINYINT DEFAULT 0 NOT NULL,
    PENDING_REVIEW TINYINT DEFAULT 0 NOT NULL,
    PENDING_SCHEDULE TINYINT DEFAULT 0 NOT NULL,
    PENDING_REPURCHASE TINYINT DEFAULT 0 NOT NULL,
    PENDING_INVOICE TINYINT DEFAULT 0 NOT NULL,
    REVIEW_BEFORE DATETIME, PLAN_BEFORE DATETIME,
    ORDER_BEFORE DATETIME, CHARGE_BEFORE DATETIME,
    REJECTED_DATE DATETIME, PLANNED_DATE DATETIME,
    ORDERED_DATE DATETIME, CHARGED_DATE DATETIME,
    CREATED_DATE DATETIME NOT NULL,
    CREATED_BY VARCHAR (16) NOT NULL, 
    STATETEXT VARCHAR (1024) NOT NULL,
    DOC_DATA VARCHAR (260) DEFAULT NULL,
    PRIMARY KEY(PERSONAL_ID,DOC_NAME)
);
