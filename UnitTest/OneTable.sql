-- Script for one-table strategy

create table subject
(
    -- Subject
    id              integer not null
   ,name            varchar(200)
   ,adresline1      varchar(250)
   ,adresline2      varchar(250)
   ,account_id      integer
   -- Natural person
   ,firstname       varchar(200)
   ,date_of_birth   timestamp
   ,length          decimal
   -- Supplier
   ,deliveryAddress varchar(200)
   ,balance         decimal
   ,open_orders     decimal
   ,open_billings   decimal
   -- Infant
   ,immunisation    varchar(4000)
   ,complication    varchar(4000)
   ,hospital        varchar(100)
);

alter table subject
  add constraint pk_subject
      primary key(id);
      
      
create sequence subject_seq;      