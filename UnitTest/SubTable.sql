create table animal
(
  id             integer    not null
  ,discriminator varchar(5) not null
  ,animalName    varchar(200)
  ,has_claws     boolean
  ,has_hair      boolean
  ,has_wings     boolean
  ,numberOfLegs  integer
);

alter table animal
  add constraint pk_animal
      primary key(id);
      
create sequence animal_seq start with 1;      
      
create table cat
(
  id              integer not null
 ,color           varchar(50)
 ,catdoor         boolean
 ,likesWhiskas    boolean
);

alter table cat  
  add constraint pk_cat
      primary key(id);
      
create table dog
(
  id           integer not null
  ,subrace     varchar(200)
  ,walksPerDay integer
  ,hunting     boolean
  ,waterdog    boolean
);     

alter table dog
  add constraint pk_dog
       primary key(id);
       
create table kitten
(
   id           integer not null
  ,color        varchar(50)
  ,immuun       boolean
  ,inLitter     boolean
);

alter table kitten
  add constraint pk_kitten
       primary key (id);
