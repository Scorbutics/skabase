#pragma once
#include "Base/SkaConstants.h"
#if defined(SKA_PLATFORM_LINUX)
#include <string>

namespace ska {
	class FileUtilsUnix {
	private:
		FileUtilsUnix();

	public:
		~FileUtilsUnix();
		static std::string getCurrentDirectory();
		static std::string getExecutablePath();
		static void createDirectory(const std::string& directoryName);
	};
}
#endif
