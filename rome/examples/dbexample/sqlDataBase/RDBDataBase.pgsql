-- CREATE DATABASE RDBDataBase;
-- USE RDBDataBase;

CREATE TABLE AAA (
  id smallint NOT NULL default '-1',
  BBB_id smallint default NULL,
  CCC_id smallint default NULL,
  PRIMARY KEY  (id)
) ;

INSERT INTO AAA VALUES (0,0,0);
INSERT INTO AAA VALUES (1,1,1);


CREATE TABLE BBB (
  id smallint NOT NULL default '-1',
  bbb smallint default NULL,
  DDD_id smallint default NULL,
  EEE_id smallint default NULL,
  HHH_id smallint default NULL,
  III_id smallint default NULL,
  JJJ_id smallint default NULL,
  XXX text,
  PRIMARY KEY  (id)
) ;

INSERT INTO BBB VALUES (0,3,0,0,0,0,0,'GGG="/EEE/FFF"\nJJJ="/HHH{,idx}/III{,idx}[id=(@@III_id)]"');
INSERT INTO BBB VALUES (1,3,1,1,1,1,1,'GGG="/EEE/FFF"\nJJJ="/HHH{,idx}/III{,idx}[id=(@@III_id)]"');


CREATE TABLE CCC (
  id smallint NOT NULL default '-1',
  idx smallint NOT NULL default '-1',
  inverseOrder smallint default NULL,
  ccc varchar(255) default NULL,
  PRIMARY KEY  (id,idx)
) ;

INSERT INTO CCC VALUES (0,0,5,'ccc-0-0');
INSERT INTO CCC VALUES (0,1,4,'ccc-0-1');
INSERT INTO CCC VALUES (0,2,3,'ccc-0-2');
INSERT INTO CCC VALUES (0,3,2,'ccc-0-3');
INSERT INTO CCC VALUES (0,4,1,'ccc-0-4');
INSERT INTO CCC VALUES (0,5,0,'ccc-0-5');
INSERT INTO CCC VALUES (1,0,5,'ccc-1-0');
INSERT INTO CCC VALUES (1,1,4,'ccc-1-1');
INSERT INTO CCC VALUES (1,2,3,'ccc-1-2');
INSERT INTO CCC VALUES (1,3,2,'ccc-1-3');
INSERT INTO CCC VALUES (1,4,1,'ccc-1-4');
INSERT INTO CCC VALUES (1,5,0,'ccc-1-5');


CREATE TABLE DDD (
  id smallint NOT NULL default '-1',
  idx smallint NOT NULL default '-1',
  ddd__0 varchar(255) default NULL,
  ddd__1 varchar(255) default NULL,
  ddd__2 varchar(255) default NULL,
  ddd__3 varchar(255) default NULL,
  PRIMARY KEY  (id,idx)
) ;

INSERT INTO DDD VALUES (0,0,'ddd-0-0-0','ddd-0-0-1','ddd-0-0-2','ddd-0-0-3');
INSERT INTO DDD VALUES (0,1,'ddd-0-1-0','ddd-0-1-1','ddd-0-1-2','ddd-0-1-3');
INSERT INTO DDD VALUES (0,2,'ddd-0-2-0','ddd-0-2-1','ddd-0-2-2','ddd-0-2-3');
INSERT INTO DDD VALUES (1,0,'ddd-1-0-0','ddd-1-0-1','ddd-1-0-2','ddd-1-0-3');
INSERT INTO DDD VALUES (1,1,'ddd-1-1-0','ddd-1-1-1','ddd-1-1-2','ddd-1-1-3');
INSERT INTO DDD VALUES (1,2,'ddd-1-2-0','ddd-1-2-1','ddd-1-2-2','ddd-1-2-3');


CREATE TABLE EEE (
  id smallint NOT NULL default '-1',
  FFF_id smallint default NULL,
  PRIMARY KEY  (id)
) ;

INSERT INTO EEE VALUES (0,0);
INSERT INTO EEE VALUES (1,1);


CREATE TABLE FFF (
  id smallint NOT NULL default '-1',
  GGG_id smallint default NULL,
  PRIMARY KEY  (id)
) ;

INSERT INTO FFF VALUES (0,0);
INSERT INTO FFF VALUES (1,1);


CREATE TABLE GGG (
  id smallint NOT NULL default '-1',
  ggg varchar(255) default NULL,
  PRIMARY KEY  (id)
) ;

INSERT INTO GGG VALUES (0,'ggg-0');
INSERT INTO GGG VALUES (1,'ggg-1');


CREATE TABLE HHH (
  id smallint NOT NULL default '-1',
  idx smallint NOT NULL default '-1',
  III_idx smallint default NULL,
  PRIMARY KEY  (id,idx)
) ;

INSERT INTO HHH VALUES (0,0,2);
INSERT INTO HHH VALUES (0,1,1);
INSERT INTO HHH VALUES (0,2,0);
INSERT INTO HHH VALUES (1,0,2);
INSERT INTO HHH VALUES (1,1,1);
INSERT INTO HHH VALUES (1,2,0);


CREATE TABLE III (
  id smallint NOT NULL default '-1',
  idx smallint NOT NULL default '-1',
  JJJ_idx smallint default NULL,
  PRIMARY KEY  (id,idx)
) ;

INSERT INTO III VALUES (0,0,2);
INSERT INTO III VALUES (0,1,0);
INSERT INTO III VALUES (0,2,1);
INSERT INTO III VALUES (1,0,2);
INSERT INTO III VALUES (1,1,0);
INSERT INTO III VALUES (1,2,1);
CREATE TABLE JJJ (
  id smallint NOT NULL default '-1',
  idx smallint NOT NULL default '-1',
  jjj varchar(255) default NULL,
  PRIMARY KEY  (id,idx)
) ;

INSERT INTO JJJ VALUES (0,0,'jjj-0-0');
INSERT INTO JJJ VALUES (0,1,'jjj-0-1');
INSERT INTO JJJ VALUES (0,2,'jjj-0-2');
INSERT INTO JJJ VALUES (1,0,'jjj-1-0');
INSERT INTO JJJ VALUES (1,1,'jjj-1-1');
INSERT INTO JJJ VALUES (1,2,'jjj-1-2');

CREATE TABLE TTT (
  id smallint NOT NULL default '-1',
  ttt int NOT NULL default '-1',
  PRIMARY KEY  (id)
) ;
