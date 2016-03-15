#include "DirectoryUtils.h"
#include <algorithm>
#include <exception>
#include <Shlwapi.h>
#include <fstream>
using namespace std;

string Utility::CurrentDirectory()
{
	WCHAR buffer[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buffer);
	wstring currentDirectoryW(buffer);

	return string(currentDirectoryW.begin(), currentDirectoryW.end());
}

wstring Utility::ExecutableDirectory()
{
	WCHAR buffer[MAX_PATH];
	GetModuleFileName(nullptr, buffer, MAX_PATH);
	PathRemoveFileSpec(buffer);

	return wstring(buffer);
}

void Utility::GetFileName(const string& inputPath, string& filename)
{
	string fullPath(inputPath);
	replace(fullPath.begin(), fullPath.end(), '\\', '/');

	string::size_type lastSlashIndex = fullPath.find_last_of('/');

	if (lastSlashIndex == string::npos)
	{
		filename = fullPath;
	}
	else
	{
		filename = fullPath.substr(lastSlashIndex + 1, fullPath.size() - lastSlashIndex - 1);
	}
}

void Utility::GetDirectory(const string& inputPath, string& directory)
{
	string fullPath(inputPath);
	replace(fullPath.begin(), fullPath.end(), '\\', '/');

	string::size_type lastSlashIndex = fullPath.find_last_of('/');

	if (lastSlashIndex == string::npos)
	{
		directory = "";
	}
	else
	{
		directory = fullPath.substr(0, lastSlashIndex);
	}
}

void Utility::GetFileNameAndDirectory(const string& inputPath, string& directory, string& filename)
{
	string fullPath(inputPath);
	replace(fullPath.begin(), fullPath.end(), '\\', '/');

	string::size_type lastSlashIndex = fullPath.find_last_of('/');

	if (lastSlashIndex == string::npos)
	{
		directory = "";
		filename = fullPath;
	}
	else
	{
		directory = fullPath.substr(0, lastSlashIndex);
		filename = fullPath.substr(lastSlashIndex + 1, fullPath.size() - lastSlashIndex - 1);
	}
}

void Utility::LoadBinaryFile(const wstring& filename, vector<char>& data)
{
	ifstream file(filename.c_str(), ios::binary);
	if (file.bad())
	{
		throw exception("Could not open file.");
	}

	file.seekg(0, ios::end);
	UINT size = (UINT)file.tellg();

	if (size > 0)
	{
		data.resize(size);
		file.seekg(0, std::ios::beg);
		file.read(&data.front(), size);
	}

	file.close();
}

void Utility::ToWideString(const string& source, wstring& dest)
{
	dest.assign(source.begin(), source.end());
}

wstring Utility::ToWideString(const string& source)
{
	wstring dest;
	dest.assign(source.begin(), source.end());

	return dest;
}

void Utility::PathJoin(wstring& dest, const wstring& sourceDirectory, const wstring& sourceFile)
{
	WCHAR buffer[MAX_PATH];

	PathCombine(buffer, sourceDirectory.c_str(), sourceFile.c_str());
	dest = buffer;
}

void Utility::GetPathExtension(const wstring& source, wstring& dest)
{
	dest = PathFindExtension(source.c_str());
}
