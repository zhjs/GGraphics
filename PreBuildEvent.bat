
xcopy $(ProjectDir)..\GLogs\$(Platform)\$(Configuration)\*.dll $(ProjectDir)Dependence\ /y
xcopy $(ProjectDir)..\GLogs\$(Platform)\$(Configuration)\*.pdb $(ProjectDir)Dependence\ /y
xcopy $(ProjectDir)..\GLogs\src\*.h $(ProjectDir)Dependence\include\ /y

xcopy $(ProjectDir)..\GSDL\$(Platform)\$(Configuration)\*.dll $(ProjectDir)Dependence\ /y
xcopy $(ProjectDir)..\GSDL\$(Platform)\$(Configuration)\*.pdb $(ProjectDir)Dependence\ /y
xcopy $(ProjectDir)..\GSDL\src\*.h $(ProjectDir)Dependence\include\ /y