#include "Base/SkaConstants.h"
#if defined(SKA_PLATFORM_WIN)

#include <Windows.h>
#include <vector>
#include <stdexcept>
#include "FileUtilsWin.h"

BOOL DirectoryExists(LPCTSTR szPath);

BOOL DirectoryExists(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);

	return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}


ska::FileUtilsWin::FileUtilsWin() {
}

std::string ska::FileUtilsWin::getExecutablePath() {
  std::vector<char> pathExecutable(1024, 0);
  auto pathExecutableSize = pathExecutable.size();

  auto continueLoop = true;

  do {
    auto result = GetModuleFileNameA(nullptr, &pathExecutable[0], pathExecutableSize);
    auto lastError = GetLastError();

    if (result == 0) {
        //Failure
        pathExecutable[0] = '\0';
        continueLoop = false;
    } else if (result <= pathExecutableSize && lastError != ERROR_INSUFFICIENT_BUFFER) {
        //Done
        continueLoop = false;
    } else if ( result == pathExecutableSize && (lastError == ERROR_INSUFFICIENT_BUFFER || lastError == ERROR_SUCCESS)) {
        //Buffer too small
        pathExecutableSize *= 2;
        pathExecutable.resize(pathExecutableSize);
    }
  } while (continueLoop);

  std::string ret = &pathExecutable[0];
  return ret;
}

std::string ska::FileUtilsWin::getCurrentDirectory() {
	CHAR NPath[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, NPath);
	return std::string(NPath);
}

void ska::FileUtilsWin::createDirectory(const std::string& directoryName) {
	if (!DirectoryExists(directoryName.c_str())) {
		if (CreateDirectoryA(directoryName.c_str(), 0) == 0) {
			const DWORD& errorCode = GetLastError();
			if (errorCode == ERROR_PATH_NOT_FOUND) {
				throw std::runtime_error(("Unable to create directory " + directoryName));
			}
			else if (errorCode == ERROR_ALREADY_EXISTS) {
				throw std::runtime_error(("Directory already exists : " + directoryName));
			}
			else {
				throw std::runtime_error(("Unknown error during creation of the directory " + directoryName));
			}
		}
	}

}

bool ska::FileUtilsWin::isAbsolutePath(const std::string& path) {
  return !path.empty() && path.find(":\\") != std::string::npos;
}

ska::FileUtilsWin::~FileUtilsWin() {
}


#endif
