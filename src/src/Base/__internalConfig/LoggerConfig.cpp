#include <iostream>
#include <fstream>
#include "LoggerConfig.h"

ska::SkaLogger ska::GlobalLogger = ska::BuildBaseLogger("Base.log.txt");

ska::SkaLogger ska::BuildBaseLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);

    return logger;
}
