@echo off

REM MySQL 서버 정보 설정
set MYSQL_HOST=localhost
set MYSQL_PORT=3306
set MYSQL_USER=root
set MYSQL_PASSWORD=lge1234

set DATABASE_NAME=lge

echo Deleting MySQL user...
mysql -h %MYSQL_HOST% -P %MYSQL_PORT% -u %MYSQL_USER% -p%MYSQL_PASSWORD% -e "DROP USER 'server'@'localhost';"

echo Creating MySQL user...
mysql -h %MYSQL_HOST% -P %MYSQL_PORT% -u %MYSQL_USER% -p%MYSQL_PASSWORD% -e "CREATE USER 'server'@'localhost' IDENTIFIED BY 'lge12345!@';"

echo Creating database...
mysql -h %MYSQL_HOST% -P %MYSQL_PORT% -u %MYSQL_USER% -p%MYSQL_PASSWORD% -e "DROP DATABASE IF EXISTS %DATABASE_NAME%;"
mysql -h %MYSQL_HOST% -P %MYSQL_PORT% -u %MYSQL_USER% -p%MYSQL_PASSWORD% -e "CREATE DATABASE %DATABASE_NAME%;"

mysql -h %MYSQL_HOST% -P %MYSQL_PORT% -u %MYSQL_USER% -p%MYSQL_PASSWORD% -e "USE %DATABASE_NAME%;"

IF %ERRORLEVEL% EQU 0 (
    echo DB Create Sucess
) ELSE (
    echo DB Create Fail
)

echo Granting privileges...
mysql -h %MYSQL_HOST% -P %MYSQL_PORT% -u %MYSQL_USER% -p%MYSQL_PASSWORD% -e "GRANT CREATE, SELECT, INSERT, UPDATE, DELETE ON %DATABASE_NAME%.* TO 'server'@'localhost';"

echo User, database, and privileges created successfully.