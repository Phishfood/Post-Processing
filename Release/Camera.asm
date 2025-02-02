; Listing generated by Microsoft (R) Optimizing Compiler Version 18.00.30501.0 

	TITLE	C:\Users\Simon\Documents\GitHub\Post-Processing\Camera.cpp
	.686P
	.XMM
	include listing.inc
	.model	flat

INCLUDELIB LIBCMT
INCLUDELIB OLDNAMES

PUBLIC	?mViewportWidth@CCamera@@0HA			; CCamera::mViewportWidth
PUBLIC	?mViewportHeight@CCamera@@0HA			; CCamera::mViewportHeight
_BSS	SEGMENT
?mViewportWidth@CCamera@@0HA DD 01H DUP (?)		; CCamera::mViewportWidth
?mViewportHeight@CCamera@@0HA DD 01H DUP (?)		; CCamera::mViewportHeight
_BSS	ENDS
PUBLIC	??0D3DXVECTOR3@@QAE@XZ				; D3DXVECTOR3::D3DXVECTOR3
PUBLIC	??0D3DXMATRIX@@QAE@XZ				; D3DXMATRIX::D3DXMATRIX
PUBLIC	??DD3DXMATRIX@@QBE?AU0@ABU0@@Z			; D3DXMATRIX::operator*
PUBLIC	??0_D3DXMATRIXA16@@QAE@XZ			; _D3DXMATRIXA16::_D3DXMATRIXA16
PUBLIC	??4_D3DXMATRIXA16@@QAEAAU0@ABUD3DXMATRIX@@@Z	; _D3DXMATRIXA16::operator=
PUBLIC	??0CCamera@@QAE@UD3DXVECTOR3@@0MMM@Z		; CCamera::CCamera
PUBLIC	?SetFOV@CCamera@@QAEXM@Z			; CCamera::SetFOV
PUBLIC	?SetNearClip@CCamera@@QAEXM@Z			; CCamera::SetNearClip
PUBLIC	?SetFarClip@CCamera@@QAEXM@Z			; CCamera::SetFarClip
PUBLIC	?UpdateMatrices@CCamera@@QAEXXZ			; CCamera::UpdateMatrices
PUBLIC	?Control@CCamera@@QAEXMW4EKeyCode@@0000000@Z	; CCamera::Control
EXTRN	_D3DXMatrixMultiply@12:PROC
EXTRN	_D3DXMatrixInverse@12:PROC
EXTRN	_D3DXMatrixTranslation@16:PROC
EXTRN	_D3DXMatrixRotationX@8:PROC
EXTRN	_D3DXMatrixRotationY@8:PROC
EXTRN	_D3DXMatrixRotationZ@8:PROC
EXTRN	_D3DXMatrixPerspectiveFovLH@20:PROC
EXTRN	?KeyHeld@@YA_NW4EKeyCode@@@Z:PROC		; KeyHeld
EXTRN	@__security_check_cookie@4:PROC
EXTRN	___security_cookie:DWORD
EXTRN	__fltused:DWORD
; Function compile flags: /Ogtp
; File c:\users\simon\documents\github\post-processing\camera.cpp
;	COMDAT ?Control@CCamera@@QAEXMW4EKeyCode@@0000000@Z
_TEXT	SEGMENT
_frameTime$ = 8						; size = 4
_turnUp$ = 12						; size = 4
_turnDown$ = 16						; size = 4
_turnLeft$ = 20						; size = 4
_turnRight$ = 24					; size = 4
_moveForward$ = 28					; size = 4
_moveBackward$ = 32					; size = 4
_moveLeft$ = 36						; size = 4
_moveRight$ = 40					; size = 4
?Control@CCamera@@QAEXMW4EKeyCode@@0000000@Z PROC	; CCamera::Control, COMDAT
; _this$ = ecx

; 62   : {

	push	ebp
	mov	ebp, esp
	push	esi

; 63   : 	if (KeyHeld( turnDown ))

	push	DWORD PTR _turnDown$[ebp]
	mov	esi, ecx
	call	?KeyHeld@@YA_NW4EKeyCode@@@Z		; KeyHeld
	add	esp, 4
	test	al, al
	je	SHORT $LN8@Control

; 64   : 	{
; 65   : 		m_Rotation.x += RotSpeed * frameTime;

	movss	xmm0, DWORD PTR [esi+4]
	mulss	xmm0, DWORD PTR _frameTime$[ebp]
	addss	xmm0, DWORD PTR [esi+20]
	movss	DWORD PTR [esi+20], xmm0
$LN8@Control:

; 66   : 	}
; 67   : 	if (KeyHeld( turnUp ))

	push	DWORD PTR _turnUp$[ebp]
	call	?KeyHeld@@YA_NW4EKeyCode@@@Z		; KeyHeld
	add	esp, 4
	test	al, al
	je	SHORT $LN7@Control

; 68   : 	{
; 69   : 		m_Rotation.x -= RotSpeed * frameTime;

	movss	xmm1, DWORD PTR [esi+4]
	mulss	xmm1, DWORD PTR _frameTime$[ebp]
	movss	xmm0, DWORD PTR [esi+20]
	subss	xmm0, xmm1
	movss	DWORD PTR [esi+20], xmm0
$LN7@Control:

; 70   : 	}
; 71   : 	if (KeyHeld( turnRight ))

	push	DWORD PTR _turnRight$[ebp]
	call	?KeyHeld@@YA_NW4EKeyCode@@@Z		; KeyHeld
	add	esp, 4
	test	al, al
	je	SHORT $LN6@Control

; 72   : 	{
; 73   : 		m_Rotation.y += RotSpeed * frameTime;

	movss	xmm0, DWORD PTR [esi+4]
	mulss	xmm0, DWORD PTR _frameTime$[ebp]
	addss	xmm0, DWORD PTR [esi+24]
	movss	DWORD PTR [esi+24], xmm0
$LN6@Control:

; 74   : 	}
; 75   : 	if (KeyHeld( turnLeft ))

	push	DWORD PTR _turnLeft$[ebp]
	call	?KeyHeld@@YA_NW4EKeyCode@@@Z		; KeyHeld
	add	esp, 4
	test	al, al
	je	SHORT $LN5@Control

; 76   : 	{
; 77   : 		m_Rotation.y -= RotSpeed * frameTime;

	movss	xmm1, DWORD PTR [esi+4]
	mulss	xmm1, DWORD PTR _frameTime$[ebp]
	movss	xmm0, DWORD PTR [esi+24]
	subss	xmm0, xmm1
	movss	DWORD PTR [esi+24], xmm0
$LN5@Control:

; 78   : 	}
; 79   : 
; 80   : 	// Local X movement - move in the direction of the X axis, get axis from camera's "world" matrix
; 81   : 	if (KeyHeld( moveRight ))

	push	DWORD PTR _moveRight$[ebp]
	call	?KeyHeld@@YA_NW4EKeyCode@@@Z		; KeyHeld
	add	esp, 4
	test	al, al
	je	SHORT $LN4@Control

; 82   : 	{
; 83   : 		m_Position.x += m_WorldMatrix._11 * MoveSpeed * frameTime;

	movss	xmm1, DWORD PTR [esi]
	movss	xmm0, DWORD PTR [esi+48]
	movss	xmm2, DWORD PTR _frameTime$[ebp]
	mulss	xmm0, xmm1
	mulss	xmm0, xmm2
	addss	xmm0, DWORD PTR [esi+8]
	movss	DWORD PTR [esi+8], xmm0

; 84   : 		m_Position.y += m_WorldMatrix._12 * MoveSpeed * frameTime;

	movss	xmm0, DWORD PTR [esi+52]
	mulss	xmm0, xmm1
	mulss	xmm0, xmm2
	addss	xmm0, DWORD PTR [esi+12]
	movss	DWORD PTR [esi+12], xmm0

; 85   : 		m_Position.z += m_WorldMatrix._13 * MoveSpeed * frameTime;

	movss	xmm0, DWORD PTR [esi+56]
	mulss	xmm0, xmm1
	mulss	xmm0, xmm2
	addss	xmm0, DWORD PTR [esi+16]
	movss	DWORD PTR [esi+16], xmm0
$LN4@Control:

; 86   : 	}
; 87   : 	if (KeyHeld( moveLeft ))

	push	DWORD PTR _moveLeft$[ebp]
	call	?KeyHeld@@YA_NW4EKeyCode@@@Z		; KeyHeld
	add	esp, 4
	test	al, al
	je	SHORT $LN3@Control

; 88   : 	{
; 89   : 		m_Position.x -= m_WorldMatrix._11 * MoveSpeed * frameTime;

	movss	xmm2, DWORD PTR [esi]
	movss	xmm1, DWORD PTR [esi+48]
	movss	xmm3, DWORD PTR _frameTime$[ebp]
	movss	xmm0, DWORD PTR [esi+8]
	mulss	xmm1, xmm2
	mulss	xmm1, xmm3
	subss	xmm0, xmm1

; 90   : 		m_Position.y -= m_WorldMatrix._12 * MoveSpeed * frameTime;

	movss	xmm1, DWORD PTR [esi+52]
	mulss	xmm1, xmm2
	mulss	xmm1, xmm3
	movss	DWORD PTR [esi+8], xmm0
	movss	xmm0, DWORD PTR [esi+12]
	subss	xmm0, xmm1

; 91   : 		m_Position.z -= m_WorldMatrix._13 * MoveSpeed * frameTime;

	movss	xmm1, DWORD PTR [esi+56]
	mulss	xmm1, xmm2
	mulss	xmm1, xmm3
	movss	DWORD PTR [esi+12], xmm0
	movss	xmm0, DWORD PTR [esi+16]
	subss	xmm0, xmm1
	movss	DWORD PTR [esi+16], xmm0
$LN3@Control:

; 92   : 	}
; 93   : 
; 94   : 	// Local Z movement - move in the direction of the Z axis, get axis from view matrix
; 95   : 	if (KeyHeld( moveForward ))

	push	DWORD PTR _moveForward$[ebp]
	call	?KeyHeld@@YA_NW4EKeyCode@@@Z		; KeyHeld
	add	esp, 4
	test	al, al
	je	SHORT $LN2@Control

; 96   : 	{
; 97   : 		m_Position.x += m_WorldMatrix._31 * MoveSpeed * frameTime;

	movss	xmm1, DWORD PTR [esi]
	movss	xmm0, DWORD PTR [esi+80]
	movss	xmm2, DWORD PTR _frameTime$[ebp]
	mulss	xmm0, xmm1
	mulss	xmm0, xmm2
	addss	xmm0, DWORD PTR [esi+8]
	movss	DWORD PTR [esi+8], xmm0

; 98   : 		m_Position.y += m_WorldMatrix._32 * MoveSpeed * frameTime;

	movss	xmm0, DWORD PTR [esi+84]
	mulss	xmm0, xmm1
	mulss	xmm0, xmm2
	addss	xmm0, DWORD PTR [esi+12]
	movss	DWORD PTR [esi+12], xmm0

; 99   : 		m_Position.z += m_WorldMatrix._33 * MoveSpeed * frameTime;

	movss	xmm0, DWORD PTR [esi+88]
	mulss	xmm0, xmm1
	mulss	xmm0, xmm2
	addss	xmm0, DWORD PTR [esi+16]
	movss	DWORD PTR [esi+16], xmm0
$LN2@Control:

; 100  : 	}
; 101  : 	if (KeyHeld( moveBackward ))

	push	DWORD PTR _moveBackward$[ebp]
	call	?KeyHeld@@YA_NW4EKeyCode@@@Z		; KeyHeld
	add	esp, 4
	test	al, al
	je	SHORT $LN1@Control

; 102  : 	{
; 103  : 		m_Position.x -= m_WorldMatrix._31 * MoveSpeed * frameTime;

	movss	xmm2, DWORD PTR [esi]
	movss	xmm1, DWORD PTR [esi+80]
	movss	xmm3, DWORD PTR _frameTime$[ebp]
	movss	xmm0, DWORD PTR [esi+8]
	mulss	xmm1, xmm2
	mulss	xmm1, xmm3
	subss	xmm0, xmm1

; 104  : 		m_Position.y -= m_WorldMatrix._32 * MoveSpeed * frameTime;

	movss	xmm1, DWORD PTR [esi+84]
	mulss	xmm1, xmm2
	mulss	xmm1, xmm3
	movss	DWORD PTR [esi+8], xmm0
	movss	xmm0, DWORD PTR [esi+12]
	subss	xmm0, xmm1

; 105  : 		m_Position.z -= m_WorldMatrix._33 * MoveSpeed * frameTime;

	movss	xmm1, DWORD PTR [esi+88]
	mulss	xmm1, xmm2
	mulss	xmm1, xmm3
	movss	DWORD PTR [esi+12], xmm0
	movss	xmm0, DWORD PTR [esi+16]
	subss	xmm0, xmm1
	movss	DWORD PTR [esi+16], xmm0
$LN1@Control:
	pop	esi

; 106  : 	}
; 107  : }

	pop	ebp
	ret	36					; 00000024H
?Control@CCamera@@QAEXMW4EKeyCode@@0000000@Z ENDP	; CCamera::Control
_TEXT	ENDS
; Function compile flags: /Ogtp
; File c:\users\simon\documents\github\post-processing\camera.cpp
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl
; File c:\users\simon\documents\github\post-processing\camera.cpp
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl
; File c:\users\simon\documents\github\post-processing\camera.cpp
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl
; File c:\users\simon\documents\github\post-processing\camera.cpp
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl
; File c:\users\simon\documents\github\post-processing\camera.cpp
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl
; File c:\users\simon\documents\github\post-processing\camera.cpp
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl
; File c:\users\simon\documents\github\post-processing\camera.cpp
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl
; File c:\users\simon\documents\github\post-processing\camera.cpp
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl
; File c:\users\simon\documents\github\post-processing\camera.cpp
;	COMDAT ?UpdateMatrices@CCamera@@QAEXXZ
_TEXT	SEGMENT
_this$GSCopy$1$ = -468					; size = 4
_matrixTranslation$ = -464				; size = 64
$T1 = -400						; size = 64
_matrixXRot$ = -336					; size = 64
$T2 = -272						; size = 64
_matrixYRot$ = -208					; size = 64
_matrixZRot$ = -144					; size = 64
$T3 = -80						; size = 64
$T4 = -80						; size = 64
__$ArrayPad$ = -4					; size = 4
?UpdateMatrices@CCamera@@QAEXXZ PROC			; CCamera::UpdateMatrices, COMDAT
; _this$ = ecx

; 37   : {

	push	ebx
	mov	ebx, esp
	sub	esp, 8
	and	esp, -16				; fffffff0H
	add	esp, 4
	push	ebp
	mov	ebp, DWORD PTR [ebx+4]
	mov	DWORD PTR [esp+4], ebp
	mov	ebp, esp
	sub	esp, 472				; 000001d8H
	mov	eax, DWORD PTR ___security_cookie
	xor	eax, ebp
	mov	DWORD PTR __$ArrayPad$[ebp], eax
	push	esi
	mov	esi, ecx

; 38   : 	// Make matrices for position and rotations, then multiply together to get a "camera world matrix"
; 39   : 	D3DXMATRIXA16 matrixXRot, matrixYRot, matrixZRot, matrixTranslation;
; 40   : 	D3DXMatrixRotationX( &matrixXRot, m_Rotation.x );

	lea	eax, DWORD PTR _matrixXRot$[ebp]
	push	edi
	push	ecx
	mov	DWORD PTR _this$GSCopy$1$[ebp], esi
	movss	xmm0, DWORD PTR [esi+20]
	movss	DWORD PTR [esp], xmm0
	push	eax
	call	_D3DXMatrixRotationX@8

; 41   : 	D3DXMatrixRotationY( &matrixYRot, m_Rotation.y );

	movss	xmm0, DWORD PTR [esi+24]
	lea	eax, DWORD PTR _matrixYRot$[ebp]
	push	ecx
	movss	DWORD PTR [esp], xmm0
	push	eax
	call	_D3DXMatrixRotationY@8

; 42   : 	D3DXMatrixRotationZ( &matrixZRot, m_Rotation.z );

	movss	xmm0, DWORD PTR [esi+28]
	lea	eax, DWORD PTR _matrixZRot$[ebp]
	push	ecx
	movss	DWORD PTR [esp], xmm0
	push	eax
	call	_D3DXMatrixRotationZ@8

; 43   : 	D3DXMatrixTranslation( &matrixTranslation, m_Position.x, m_Position.y, m_Position.z);

	movss	xmm0, DWORD PTR [esi+16]
	lea	eax, DWORD PTR _matrixTranslation$[ebp]
	sub	esp, 12					; 0000000cH
	movss	DWORD PTR [esp+8], xmm0
	movss	xmm0, DWORD PTR [esi+12]
	movss	DWORD PTR [esp+4], xmm0
	movss	xmm0, DWORD PTR [esi+8]
	movss	DWORD PTR [esp], xmm0
	push	eax
	call	_D3DXMatrixTranslation@16
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl

; 898  :     D3DXMatrixMultiply(&matT, this, &mat);

	lea	eax, DWORD PTR _matrixXRot$[ebp]
	push	eax
	lea	eax, DWORD PTR _matrixZRot$[ebp]
	push	eax
	lea	eax, DWORD PTR $T2[ebp]
	push	eax
	call	_D3DXMatrixMultiply@12
	lea	eax, DWORD PTR _matrixYRot$[ebp]
	push	eax
	lea	eax, DWORD PTR $T2[ebp]
	push	eax
	lea	eax, DWORD PTR $T1[ebp]
	push	eax
	call	_D3DXMatrixMultiply@12
	lea	eax, DWORD PTR _matrixTranslation$[ebp]
	push	eax
	lea	eax, DWORD PTR $T1[ebp]
	push	eax
	lea	eax, DWORD PTR $T4[ebp]
	push	eax
	call	_D3DXMatrixMultiply@12

; 1057 :     memcpy(&_11, &rhs, sizeof(D3DXMATRIX));

	movdqu	xmm0, XMMWORD PTR $T4[ebp]
; File c:\users\simon\documents\github\post-processing\camera.cpp

; 44   : 	m_WorldMatrix = matrixZRot * matrixXRot * matrixYRot * matrixTranslation;

	lea	eax, DWORD PTR [esi+48]

; 47   : 	D3DXMatrixInverse( &m_ViewMatrix, NULL, &m_WorldMatrix );

	push	eax
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl

; 1057 :     memcpy(&_11, &rhs, sizeof(D3DXMATRIX));

	movdqu	XMMWORD PTR [eax], xmm0
; File c:\users\simon\documents\github\post-processing\camera.cpp

; 47   : 	D3DXMatrixInverse( &m_ViewMatrix, NULL, &m_WorldMatrix );

	push	0
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl

; 1057 :     memcpy(&_11, &rhs, sizeof(D3DXMATRIX));

	movdqu	xmm0, XMMWORD PTR $T4[ebp+16]
; File c:\users\simon\documents\github\post-processing\camera.cpp

; 47   : 	D3DXMatrixInverse( &m_ViewMatrix, NULL, &m_WorldMatrix );

	lea	edi, DWORD PTR [esi+112]
	push	edi
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl

; 1057 :     memcpy(&_11, &rhs, sizeof(D3DXMATRIX));

	movdqu	XMMWORD PTR [eax+16], xmm0
	movdqu	xmm0, XMMWORD PTR $T4[ebp+32]
	movdqu	XMMWORD PTR [eax+32], xmm0
	movdqu	xmm0, XMMWORD PTR $T4[ebp+48]
	movdqu	XMMWORD PTR [eax+48], xmm0
; File c:\users\simon\documents\github\post-processing\camera.cpp

; 47   : 	D3DXMatrixInverse( &m_ViewMatrix, NULL, &m_WorldMatrix );

	call	_D3DXMatrixInverse@12

; 48   : 
; 49   : 	// Initialize the projection matrix. This determines viewing properties of the camera such as field of view (FOV) and near clip distance
; 50   : 	// One other factor in the projection matrix is the aspect ratio of screen (width/height) - used to adjust FOV between horizontal and vertical
; 51   : 	float aspect = (float)mViewportWidth / mViewportHeight; 
; 52   : 	D3DXMatrixPerspectiveFovLH( &m_ProjMatrix, m_FOV, aspect, m_NearClip, m_FarClip );

	mov	eax, DWORD PTR _this$GSCopy$1$[ebp]
	add	esi, 176				; 000000b0H
	movd	xmm1, DWORD PTR ?mViewportWidth@CCamera@@0HA ; CCamera::mViewportWidth
	sub	esp, 16					; 00000010H
	cvtdq2ps xmm1, xmm1
	movss	xmm0, DWORD PTR [eax+40]
	movss	DWORD PTR [esp+12], xmm0
	movss	xmm0, DWORD PTR [eax+36]
	movss	DWORD PTR [esp+8], xmm0
	movd	xmm0, DWORD PTR ?mViewportHeight@CCamera@@0HA ; CCamera::mViewportHeight
	cvtdq2ps xmm0, xmm0
	divss	xmm1, xmm0
	movss	xmm0, DWORD PTR [eax+32]
	movss	DWORD PTR [esp+4], xmm1
	movss	DWORD PTR [esp], xmm0
	push	esi
	call	_D3DXMatrixPerspectiveFovLH@20
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl

; 898  :     D3DXMatrixMultiply(&matT, this, &mat);

	push	esi
	push	edi
	lea	eax, DWORD PTR $T3[ebp]
	push	eax
	call	_D3DXMatrixMultiply@12

; 1057 :     memcpy(&_11, &rhs, sizeof(D3DXMATRIX));

	mov	eax, DWORD PTR _this$GSCopy$1$[ebp]
	movdqu	xmm0, XMMWORD PTR $T3[ebp]
; File c:\users\simon\documents\github\post-processing\camera.cpp

; 56   : }

	mov	ecx, DWORD PTR __$ArrayPad$[ebp]
	pop	edi
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl

; 1057 :     memcpy(&_11, &rhs, sizeof(D3DXMATRIX));

	movdqu	XMMWORD PTR [eax+240], xmm0
; File c:\users\simon\documents\github\post-processing\camera.cpp

; 56   : }

	xor	ecx, ebp
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl

; 1057 :     memcpy(&_11, &rhs, sizeof(D3DXMATRIX));

	movdqu	xmm0, XMMWORD PTR $T3[ebp+16]
; File c:\users\simon\documents\github\post-processing\camera.cpp

; 56   : }

	pop	esi
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl

; 1057 :     memcpy(&_11, &rhs, sizeof(D3DXMATRIX));

	movdqu	XMMWORD PTR [eax+256], xmm0
	movdqu	xmm0, XMMWORD PTR $T3[ebp+32]
	movdqu	XMMWORD PTR [eax+272], xmm0
	movdqu	xmm0, XMMWORD PTR $T3[ebp+48]
	movdqu	XMMWORD PTR [eax+288], xmm0
; File c:\users\simon\documents\github\post-processing\camera.cpp

; 56   : }

	call	@__security_check_cookie@4
	mov	esp, ebp
	pop	ebp
	mov	esp, ebx
	pop	ebx
	ret	0
?UpdateMatrices@CCamera@@QAEXXZ ENDP			; CCamera::UpdateMatrices
_TEXT	ENDS
; Function compile flags: /Ogtp
; File c:\users\simon\documents\github\post-processing\camera.h
;	COMDAT ?SetFarClip@CCamera@@QAEXM@Z
_TEXT	SEGMENT
_farClip$ = 8						; size = 4
?SetFarClip@CCamera@@QAEXM@Z PROC			; CCamera::SetFarClip, COMDAT
; _this$ = ecx

; 115  : 	{

	push	ebp
	mov	ebp, esp

; 116  : 		m_FarClip = farClip;

	movss	xmm0, DWORD PTR _farClip$[ebp]
	movss	DWORD PTR [ecx+40], xmm0

; 117  : 	}

	pop	ebp
	ret	4
?SetFarClip@CCamera@@QAEXM@Z ENDP			; CCamera::SetFarClip
_TEXT	ENDS
; Function compile flags: /Ogtp
; File c:\users\simon\documents\github\post-processing\camera.h
;	COMDAT ?SetNearClip@CCamera@@QAEXM@Z
_TEXT	SEGMENT
_nearClip$ = 8						; size = 4
?SetNearClip@CCamera@@QAEXM@Z PROC			; CCamera::SetNearClip, COMDAT
; _this$ = ecx

; 111  : 	{

	push	ebp
	mov	ebp, esp

; 112  : 		m_NearClip = nearClip;

	movss	xmm0, DWORD PTR _nearClip$[ebp]
	movss	DWORD PTR [ecx+36], xmm0

; 113  : 	}

	pop	ebp
	ret	4
?SetNearClip@CCamera@@QAEXM@Z ENDP			; CCamera::SetNearClip
_TEXT	ENDS
; Function compile flags: /Ogtp
; File c:\users\simon\documents\github\post-processing\camera.h
;	COMDAT ?SetFOV@CCamera@@QAEXM@Z
_TEXT	SEGMENT
_fov$ = 8						; size = 4
?SetFOV@CCamera@@QAEXM@Z PROC				; CCamera::SetFOV, COMDAT
; _this$ = ecx

; 107  : 	{

	push	ebp
	mov	ebp, esp

; 108  : 		m_FOV = fov;

	movss	xmm0, DWORD PTR _fov$[ebp]
	movss	DWORD PTR [ecx+32], xmm0

; 109  : 	}

	pop	ebp
	ret	4
?SetFOV@CCamera@@QAEXM@Z ENDP				; CCamera::SetFOV
_TEXT	ENDS
; Function compile flags: /Ogtp
; File c:\users\simon\documents\github\post-processing\camera.cpp
; File c:\users\simon\documents\github\post-processing\camera.h
; File c:\users\simon\documents\github\post-processing\camera.cpp
; File c:\users\simon\documents\github\post-processing\camera.h
; File c:\users\simon\documents\github\post-processing\camera.cpp
;	COMDAT ??0CCamera@@QAE@UD3DXVECTOR3@@0MMM@Z
_TEXT	SEGMENT
_position$ = 8						; size = 12
_rotation$ = 20						; size = 12
_fov$ = 32						; size = 4
_nearClip$ = 36						; size = 4
_farClip$ = 40						; size = 4
??0CCamera@@QAE@UD3DXVECTOR3@@0MMM@Z PROC		; CCamera::CCamera, COMDAT
; _this$ = ecx

; 17   : {

	push	ebp
	mov	ebp, esp

; 18   : 	m_Position = position;

	movq	xmm0, QWORD PTR _position$[ebp]
	mov	eax, DWORD PTR _position$[ebp+8]
	push	esi
	mov	esi, ecx
	movq	QWORD PTR [esi+8], xmm0

; 19   : 	m_Rotation = rotation;

	movq	xmm0, QWORD PTR _rotation$[ebp]
	mov	DWORD PTR [esi+16], eax
	mov	eax, DWORD PTR _rotation$[ebp+8]
	movq	QWORD PTR [esi+20], xmm0
	mov	DWORD PTR [esi+28], eax

; 20   : 	UpdateMatrices();

	call	?UpdateMatrices@CCamera@@QAEXXZ		; CCamera::UpdateMatrices
; File c:\users\simon\documents\github\post-processing\camera.h

; 108  : 		m_FOV = fov;

	movss	xmm0, DWORD PTR _fov$[ebp]
; File c:\users\simon\documents\github\post-processing\camera.cpp

; 28   : }

	mov	eax, esi
; File c:\users\simon\documents\github\post-processing\camera.h

; 108  : 		m_FOV = fov;

	movss	DWORD PTR [esi+32], xmm0

; 109  : 	}
; 110  : 	void SetNearClip( float nearClip )
; 111  : 	{
; 112  : 		m_NearClip = nearClip;

	movss	xmm0, DWORD PTR _nearClip$[ebp]
	movss	DWORD PTR [esi+36], xmm0

; 113  : 	}
; 114  : 	void SetFarClip( float farClip )
; 115  : 	{
; 116  : 		m_FarClip = farClip;

	movss	xmm0, DWORD PTR _farClip$[ebp]
	movss	DWORD PTR [esi+40], xmm0
; File c:\users\simon\documents\github\post-processing\camera.cpp

; 26   : 	MoveSpeed = 50.0f;

	mov	DWORD PTR [esi], 1112014848		; 42480000H

; 27   : 	RotSpeed = 2.0f;

	mov	DWORD PTR [esi+4], 1073741824		; 40000000H
	pop	esi

; 28   : }

	pop	ebp
	ret	36					; 00000024H
??0CCamera@@QAE@UD3DXVECTOR3@@0MMM@Z ENDP		; CCamera::CCamera
_TEXT	ENDS
; Function compile flags: /Ogtp
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl
;	COMDAT ??4_D3DXMATRIXA16@@QAEAAU0@ABUD3DXMATRIX@@@Z
_TEXT	SEGMENT
_rhs$ = 8						; size = 4
??4_D3DXMATRIXA16@@QAEAAU0@ABUD3DXMATRIX@@@Z PROC	; _D3DXMATRIXA16::operator=, COMDAT
; _this$ = ecx

; 1056 : {

	push	ebp
	mov	ebp, esp

; 1057 :     memcpy(&_11, &rhs, sizeof(D3DXMATRIX));

	mov	eax, DWORD PTR _rhs$[ebp]
	movdqu	xmm0, XMMWORD PTR [eax]
	movdqu	XMMWORD PTR [ecx], xmm0
	movdqu	xmm0, XMMWORD PTR [eax+16]
	movdqu	XMMWORD PTR [ecx+16], xmm0
	movdqu	xmm0, XMMWORD PTR [eax+32]
	movdqu	XMMWORD PTR [ecx+32], xmm0
	movdqu	xmm0, XMMWORD PTR [eax+48]

; 1058 :     return *this;

	mov	eax, ecx
	movdqu	XMMWORD PTR [ecx+48], xmm0

; 1059 : }

	pop	ebp
	ret	4
??4_D3DXMATRIXA16@@QAEAAU0@ABUD3DXMATRIX@@@Z ENDP	; _D3DXMATRIXA16::operator=
_TEXT	ENDS
; Function compile flags: /Ogtp
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.h
;	COMDAT ??0_D3DXMATRIXA16@@QAE@XZ
_TEXT	SEGMENT
??0_D3DXMATRIXA16@@QAE@XZ PROC				; _D3DXMATRIXA16::_D3DXMATRIXA16, COMDAT
; _this$ = ecx

; 415  :     _D3DXMATRIXA16() {};

	mov	eax, ecx
	ret	0
??0_D3DXMATRIXA16@@QAE@XZ ENDP				; _D3DXMATRIXA16::_D3DXMATRIXA16
_TEXT	ENDS
; Function compile flags: /Ogtp
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.inl
;	COMDAT ??DD3DXMATRIX@@QBE?AU0@ABU0@@Z
_TEXT	SEGMENT
___$ReturnUdt$ = 8					; size = 4
_mat$ = 12						; size = 4
??DD3DXMATRIX@@QBE?AU0@ABU0@@Z PROC			; D3DXMATRIX::operator*, COMDAT
; _this$ = ecx

; 896  : {

	push	ebp
	mov	ebp, esp

; 897  :     D3DXMATRIX matT;
; 898  :     D3DXMatrixMultiply(&matT, this, &mat);

	push	DWORD PTR _mat$[ebp]
	push	ecx
	push	DWORD PTR ___$ReturnUdt$[ebp]
	call	_D3DXMatrixMultiply@12

; 899  :     return matT;

	mov	eax, DWORD PTR ___$ReturnUdt$[ebp]

; 900  : }

	pop	ebp
	ret	8
??DD3DXMATRIX@@QBE?AU0@ABU0@@Z ENDP			; D3DXMATRIX::operator*
_TEXT	ENDS
; Function compile flags: /Ogtp
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.h
;	COMDAT ??0D3DXMATRIX@@QAE@XZ
_TEXT	SEGMENT
??0D3DXMATRIX@@QAE@XZ PROC				; D3DXMATRIX::D3DXMATRIX, COMDAT
; _this$ = ecx

; 348  :     D3DXMATRIX() {};

	mov	eax, ecx
	ret	0
??0D3DXMATRIX@@QAE@XZ ENDP				; D3DXMATRIX::D3DXMATRIX
_TEXT	ENDS
; Function compile flags: /Ogtp
; File c:\program files (x86)\microsoft directx sdk (june 2010)\include\d3dx10math.h
;	COMDAT ??0D3DXVECTOR3@@QAE@XZ
_TEXT	SEGMENT
??0D3DXVECTOR3@@QAE@XZ PROC				; D3DXVECTOR3::D3DXVECTOR3, COMDAT
; _this$ = ecx

; 198  :     D3DXVECTOR3() {};

	mov	eax, ecx
	ret	0
??0D3DXVECTOR3@@QAE@XZ ENDP				; D3DXVECTOR3::D3DXVECTOR3
_TEXT	ENDS
END
