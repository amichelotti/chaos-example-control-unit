//
//  TestCorrelatingCommand.h
//  ExampleControlUnit
//
//  Created by Claudio Bisegni on 08/11/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef __ExampleControlUnit__TestCorrelatingCommand__
#define __ExampleControlUnit__TestCorrelatingCommand__

#include <boost/random.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommand.h>

using namespace chaos;

namespace c_data = chaos::common::data;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;

DEFINE_BATCH_COMMAND_CLASS(TestCorrelatingCommand, ccc_slow_command::SlowCommand) {
    uint64_t		start_time;
    static			uint64_t instance_cout;
    uint64_t        local_instance_count;
	
	std::string		exception_message;
	uint32_t		exception_location; //0-set, 1-acquire, 2-correlation
	std::string		correlation_message;
protected:
	// return the implemented handler
    uint8_t implementedHandler();
    
    // Start the command execution
    void setHandler(c_data::CDataWrapper *data);
	
	//implementation empty
	void acquireHandler();
	
    // Correlation and commit phase
    void ccHandler();

	bool timeoutHandler();
public:
	TestCorrelatingCommand();
	~TestCorrelatingCommand();
};

#endif /* defined(__ExampleControlUnit__TestCorrelatingCommand__) */
