ALTER TABLE employees ADD CONSTRAINT
FK__employees__schedules foreign key(SCHEDULE_ID) references schedules(SCHEDULE_ID);

CREATE INDEX I_personal_id ON employees (PERSONAL_ID);
ALTER TABLE employees ADD CONSTRAINT
FK__employees__personal foreign key(PERSONAL_ID) references personal(PERSONAL_ID);

ALTER TABLE entries ADD CONSTRAINT
FK__entries__personal foreign key(EMPLOYEE_ID) references personal(PERSONAL_ID);

ALTER TABLE entries ADD CONSTRAINT
FK__entries__apps foreign key(APP_ID) references apps(APP_ID);

ALTER TABLE apps ADD CONSTRAINT
FK__apps__personal foreign key(PERSONAL_ID) references personal(PERSONAL_ID);

ALTER TABLE docs ADD CONSTRAINT
FK__docs__personal foreign key(PERSONAL_ID) references personal(PERSONAL_ID);
