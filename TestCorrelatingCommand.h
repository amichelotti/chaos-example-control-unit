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
#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommand.h>

using namespace chaos;

namespace c_data = chaos::common::data;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;

class TestCorrelatingCommand : public ccc_slow_command::SlowCommand {
	    uint64_t start_time;
	static uint64_t instance_cout;
protected:
	// return the implemented handler
    uint8_t implementedHandler();
    
    // Start the command execution
    void setHandler(c_data::CDataWrapper *data);
   
    // Correlation and commit phase
    void ccHandler();
	bool timeoutHandler();
public:
	TestCorrelatingCommand();
	~TestCorrelatingCommand();
};

#endif /* defined(__ExampleControlUnit__TestCorrelatingCommand__) */
