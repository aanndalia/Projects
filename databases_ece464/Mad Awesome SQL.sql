CREATE TABLE Locations ( lname VARCHAR(15),
                         PRIMARY KEY (lname) );

CREATE TABLE Groups ( gname VARCHAR(15),
                      PRIMARY KEY (gname) );

CREATE TABLE Employees ( username VARCHAR(20),
                         password VARCHAR(20) NOT NULL,
                         ename VARCHAR(30) NOT NULL,
                         email VARCHAR(30) NOT NULL,
                         lname VARCHAR(15) NOT NULL,
                         gname VARCHAR(15) NOT NULL,
                         PRIMARY KEY (username),
                         FOREIGN KEY (lname) REFERENCES Locations ON DELETE CASCADE,
                         FOREIGN KEY (gname) REFERENCES Groups ON DELETE CASCADE );

CREATE TABLE Shifts ( sname VARCHAR(4),
                      sdate DATE,
					  PRIMARY KEY (sname, sdate) );

CREATE TABLE Works ( username VARCHAR(20),
                     sname VARCHAR(4),
					 sdate DATE,
					 status VARCHAR(4) NOT NULL,
					 PRIMARY KEY (username, sname, sdate),
					 FOREIGN KEY (username) REFERENCES Employees ON DELETE CASCADE,
					 FOREIGN KEY (sname) REFERENCES Shifts ON DELETE CASCADE,
					 FOREIGN KEY (sdate) REFERENCES Shifts ON DELETE CASCADE );
