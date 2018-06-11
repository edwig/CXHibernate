create table animal
(
  id             integer    not null
  ,discriminator varchar(5) not null
  ,animalName    varchar(200)
  ,has_claws     bool
  ,has_hair      bool
  ,has_wings     bool
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

       
insert into ANIMAL
(id,discriminator,animalname,has_claws,has_hair,has_wings,numberoflegs)
VALUES
(1,'kit','Fluffy Kitten',true,true,false,4);

insert into CAT
(id,color,catdoor,likeswhiskas)
VALUES
(1,'Black',false,true);

insert into KITTEN
(id,color,immuun,inlitter)
VALUES
(1,'Black-and-White',true,true);


select * from kitten;       