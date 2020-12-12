#include "CustomHooks.h"

std::ofstream outfile;
std::mutex mtx;


std::vector<std::string> memoryFilenames;
std::vector<std::string> CurrentHashLst;

void LoadCurrentHash()
{
	std::ifstream csv;
	csv.open("archivehashes.csv", std::ios::in);
	if (csv.is_open())
	{
		std::string hashnumber;
		while (getline(csv, hashnumber))
		{
			hashnumber = hashnumber.substr(hashnumber.find(',') + 1, hashnumber.length() - 1);
			CurrentHashLst.push_back(hashnumber);
		}
	}
	else
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);

		printf("Missing archivehashes.csv file\n");
		printf("Please download it from https://github.com/rfuzzo/CP77Tools/blob/main/CP77Tools/Resources/archivehashes.csv \n");
		printf("Then copy it into 'Cyberpunk 2077\\bin\\x64'\n");
		printf("Thank you !!!\n");
		system("pause");
		FreeConsole();
	}
}

bool HashExist(std::string hash, std::vector<std::string> CurrentHashLst)
{
	return std::any_of(CurrentHashLst.begin(), CurrentHashLst.end(), compare(hash));
}

void WriteFilenames(std::vector<std::string>& memoryFilenames, std::vector<std::string> CurrentHashLst)
{
	std::string temp;
	for (int i = 0; i < memoryFilenames.size(); i++)
	{
		std::string hash = memoryFilenames[i].substr(memoryFilenames[i].find(',') + 1, memoryFilenames[i].length() - 1);
		if (!HashExist(hash, CurrentHashLst))
		{
			temp += memoryFilenames[i];
			temp += '\n';
		}
	}
	memoryFilenames.clear();

	outfile.open("Cyberpunk2077.log", std::ios::out | std::ios::app);
	outfile << temp;
	outfile.close();
}


HANDLE Current = GetCurrentProcess();

LPVOID GetArchiveFunctionAddress()
{
	unsigned short scanBytes[19] = { 0x41, 0x56, 0x48, 0x81, 0xEC, 0xF0, 0x00, 0x00, 0x00, 0x44, 0x8B, 0x42, 0x08, 0x4C, 0x8B, 0xF1, 0x45, 0x85, 0xC0 };
	return AOBScanner::GetSingleton()->Scan(scanBytes, 19);
}

typedef INT64* (*HOOKON)(INT64*, INT64);
HOOKON fpHookOn = NULL;


int counthash = 0;

INT64* tHookOn(INT64* unk1, INT64 a2)
{
	int rax = 0;
	GetRax(rax);
	int v2 = *(UINT*)(a2 + 8);
	char v10 = 0;
	GetAL(v10);
	int size = 0;
	std::string line = "";

	if ((DWORD)v2)
	{
		char* v5 = *(char**)a2;
		char* v6 = *(char**)a2;
		char* v8 = &v5[v2];

		while (v6 != v8)
		{
			char v9 = *v6;
			if (*v6 == '/' || v9 == '\\')
			{
				do
				{
					do {
						v10 = (v6++)[1];
						size += 1;
					} while (v10 == '/');
				} while (v10 == '\\');
			}
			else {
				if (v9 == '"' || v9 == '\'')
					break;
				++v6;
				size += 1;
			}
			

		}
		v6 -= size;
		std::string str(v5, size);
		for (int i = 0; i < size; i++)
		{
			if (str[i] == '/')
				line += '\\';
			else
				line += char(tolower(str[i]));
		}

	}
	// Orignal Function
	SetRax(rax);
	INT64* res = fpHookOn(unk1, a2);

	if (size > 0)
	{
		line += ',';
		UINT64 hash = *res;
		line += std::to_string(hash);
	}

	mtx.lock();
	if (size > 0)
		memoryFilenames.push_back(line);
	counthash += 1;
	if (counthash % 10000 == 0)
	{
		WriteFilenames(memoryFilenames, CurrentHashLst);
		counthash = 0;
	}
	mtx.unlock();

	return res;
}



void SetupHooks()
{
	LoadCurrentHash();
	LPVOID hookAddress = GetArchiveFunctionAddress();
	MH_Initialize();

	
	if (MH_CreateHook(hookAddress, &tHookOn, reinterpret_cast<LPVOID*>(&fpHookOn)) != MH_OK)
	{
		return;
	}
	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
	{
		return;
	}

	//HANDLE myhandleB = (HANDLE)_beginthreadex(0, 0, &ExitHook, 0, 0, 0);
}



