#include "CustomHooks.h"


std::ofstream outfile;
std::mutex mtx;


std::map<std::string,int> CurrentHashLst;
std::vector<std::string> memoryFilenames;

std::wstring ExePath() {
	TCHAR buffer[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, buffer, MAX_PATH);
	std::wstring::size_type pos = std::wstring(buffer).find_last_of(L"\\/");
	return std::wstring(buffer).substr(0, pos);
}
std::wstring s2ws(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

void LoadCurrentHash()
{
	std::ifstream csv;
	std::wstring path = ExePath() + s2ws("\\missinghashes.txt");
	csv.open(path, std::ios::in);
	int i = 0;
	if (csv.is_open())
	{
		std::string hashnumber;
		while (getline(csv, hashnumber))
		{
			CurrentHashLst.insert(std::pair<std::string, int>(hashnumber, i));
		}

	}
	else
	{
		AllocConsole();
		freopen("CONOUT$", "w", stdout);
		
		printf("Missing file ");
		wprintf(path.c_str());
		printf("\n");
		printf("Please download it from CP77 Modding Tools discord in 'hash' channel\nLink invite: https://discord.gg/NTA2t5GngV \n");
		printf("Then copy it into ");
		wprintf(ExePath().c_str());

		printf("\nThank you !!!\n");
		system("pause");
		FreeConsole();
	}
}


void WriteFilenames(std::vector<std::string> memoryFilenames, std::map<std::string, int> CurrentHashLst)
{

	std::string templine;
	templine = "";

	for (long long i = 0; i < memoryFilenames.size(); i++)
	{

		std::string hash = memoryFilenames[i].substr(memoryFilenames[i].find(',') + 1, memoryFilenames[i].length() - 1);
		
		if(CurrentHashLst.find(hash) != CurrentHashLst.end())
		{
			templine += memoryFilenames[i];
			templine += '\n';
			//CurrentHashLst.erase(hash);
		}
	}

	outfile.open("Cyberpunk2077.log", std::ios::out | std::ios::app);
	outfile << templine;
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
int temphash = 0;
bool check = 0;
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

	if (memoryFilenames.size() % 10000 == 0)
	{
		if (!memoryFilenames.empty())
		{
			WriteFilenames(memoryFilenames, CurrentHashLst);
			memoryFilenames.clear();
		}
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
