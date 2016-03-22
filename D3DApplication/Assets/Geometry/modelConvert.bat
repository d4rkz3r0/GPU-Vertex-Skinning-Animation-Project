cd "Assets\Geometry\"
ECHO Starting File Conversion...
IF EXIST "FBX\Teddy_Attack1.fbx" ( FbxConverter.exe "FBX\Teddy_Attack1.fbx" "DAE\Teddy_Attack1.dae" /sffFBX /dffDAE && echo Teddy_Attack1.fbx Converted! ) ELSE ( echo Teddy_Attack1.fbx is missing. )
IF EXIST "FBX\Teddy_Attack2.fbx" ( FbxConverter.exe "FBX\Teddy_Attack2.fbx" "DAE\Teddy_Attack2.dae" /sffFBX /dffDAE && echo Teddy_Attack2.fbx Converted! ) ELSE ( echo Teddy_Attack2.fbx is missing. )
IF EXIST "FBX\Teddy_Idle.fbx" ( FbxConverter.exe "FBX\Teddy_Idle.fbx" "DAE\Teddy_Idle.dae" /sffFBX /dffDAE && echo Teddy_Idle.fbx Converted! ) ELSE ( echo Teddy_Idle.fbx File Missing is missing. )
IF EXIST "FBX\Teddy_Run.fbx" ( FbxConverter.exe "FBX\Teddy_Run.fbx" "DAE\Teddy_Run.dae" /sffFBX /dffDAE && echo Teddy_Run.fbx Converted! ) ELSE ( echo Teddy_Run.fbx File is missing. )