// ConsoleApplication1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <sstream>

#include "GTR2ShMem.h"

TCHAR szName[] = TEXT("$gtr2$");

int main()
{
	HANDLE hMapFile;
	do {
		Sleep(1000);
		std::cout << "Trying to open SHMEM" << std::endl;
		hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, szName);
	} while (NULL == hMapFile);
	
	GTR2ShMem *mem = (GTR2ShMem*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(GTR2ShMem));

	while (TRUE) {
		std::stringstream ss;
		ss << mem->gear;
		std::cout << ss.str() << std::endl;
		Sleep(1000);
	}

}

