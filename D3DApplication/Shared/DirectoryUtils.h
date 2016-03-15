#pragma once
#include <string>
#include <vector>
using std::string;
using std::wstring;
using std::vector;

class Utility
{
public:
	static string CurrentDirectory();
	static wstring ExecutableDirectory();
	static void GetFileName(const string& inputPath, string& filename);
	static void GetDirectory(const string& inputPath, string& directory);
	static void GetFileNameAndDirectory(const string& inputPath, string& directory, string& filename);
	static void LoadBinaryFile(const wstring& filename, vector<char>& data);
	static void ToWideString(const string& source, wstring& dest);
	static wstring ToWideString(const string& source);
	static void PathJoin(wstring& dest, const wstring& sourceDirectory, const wstring& sourceFile);
	static void GetPathExtension(const wstring& source, wstring& dest);

private:
	Utility() = delete;
	Utility(const Utility& rhs) = delete;
	Utility& operator=(const Utility& rhs) = delete;
};