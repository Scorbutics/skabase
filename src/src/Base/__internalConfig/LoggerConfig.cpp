#include <iostream>
#include <fstream>
#include "LoggerConfig.h"
#include <Signal/SignalHandler.h>

ska::SkaLogger ska::GlobalLogger = ska::BuildBaseLogger("Base.log.txt");

ska::SkaLogger ska::BuildBaseLogger(const char * filename) {
	static auto TypeBuilderLogFileOutput = std::ofstream { filename };
	auto logger = SkaLogger{};
	logger.get<0>().addOutputTarget(TypeBuilderLogFileOutput);
	logger.get<1>().addOutputTarget(std::cout);
 
    ska::process::SignalHandlerAddAction([](int signalCode) {
        ska::GlobalLogger.terminate();
        TypeBuilderLogFileOutput.close();
    });
    
    return logger;
}
