"C:\Program Files\MRE_SDK\tools\DllPackage.exe" "D:\MyGitHub\calculatingPi\calculatingPi.vcproj"
if %errorlevel% == 0 (
 echo postbuild OK.
  copy calculatingPi.vpp ..\..\..\MoDIS_VC9\WIN32FS\DRIVE_E\calculatingPi.vpp /y
exit 0
)else (
echo postbuild error
  echo error code: %errorlevel%
  exit 1
)

