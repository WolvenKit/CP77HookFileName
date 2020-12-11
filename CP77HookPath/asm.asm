
.code

GetAL proc, Arg3:BYTE
		MOV Arg3, AL
		ret
GetAL endp

GetRax proc, Arg1:QWORD
		mov Arg1, RAX
		ret
GetRax endp

SetRax proc, Arg2:QWORD
		mov RAX, Arg2
		ret
SetRax endp


end