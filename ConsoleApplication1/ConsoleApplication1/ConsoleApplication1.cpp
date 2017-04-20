// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <sstream>

#include "GTR2ShMem.h"

TCHAR szName[] = TEXT("$gtr2$");
TCHAR gszPort[] = TEXT("COM1");

int main()
{
	HANDLE hMapFile;
	HANDLE hComm;
	hComm = CreateFile(gszPort,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		0,
		0);
	if (hComm == INVALID_HANDLE_VALUE)
		return -1;

	DCB dcbConfig;

	if (GetCommState(hComm, &dcbConfig))
	{
		dcbConfig.BaudRate = 9600;
		dcbConfig.ByteSize = 8;
		dcbConfig.Parity = NOPARITY;
		dcbConfig.StopBits = ONESTOPBIT;
		dcbConfig.fBinary = TRUE;
		dcbConfig.fParity = TRUE;
	}
	else
		return -2;

	if (!SetCommState(hComm, &dcbConfig))
		return -3;


	do {
		Sleep(1000);
		std::cout << "Trying to open SHMEM" << std::endl;
		hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, szName);
	} while (NULL == hMapFile);
	
	GTR2ShMem *mem = (GTR2ShMem*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(GTR2ShMem));

	while (TRUE) {
		/*std::stringstream ss;
		ss << mem->gear;
		std::cout << ss.str() << std::endl;*/
		DWORD bytesWritten = 0;
		WriteFile(hComm, &mem->gear, 1, &bytesWritten, NULL);
		Sleep(100);
	}

}

