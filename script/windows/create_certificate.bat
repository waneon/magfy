@echo off

set CERT_PATH="Cert:/CurrentUser/My"
set /p OUTPUT_PATH=Input the output certificate file path: 

powershell "if ( !(Get-ChildItem '%CERT_PATH%' | where { $_.subject -eq 'CN=magfy' }) ) { New-SelfSignedCertificate -Type CodeSigningCert -Subject 'CN=magfy' -CertStoreLocation '%CERT_PATH%' -NotAfter (Get-Date).AddYears(10) }"
powershell "Export-Certificate -Cert ( Get-ChildItem '%CERT_PATH%' | where { $_.subject -eq 'CN=magfy' } ) -FilePath '%OUTPUT_PATH%'"