@echo off

set /p BINARY_PATH=Input the binary file path: 

mt -manifest "%~dp0/../../manifest/magfy.manifest" -outputresource:"%BINARY_PATH%";#1
SignTool sign /v /n "magfy" /t http://timestamp.digicert.com "%BINARY_PATH%"