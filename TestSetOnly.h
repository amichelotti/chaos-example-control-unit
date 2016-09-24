/*
 *	TestSetOnly.h
 *
 *	!CHAOS [ExampleControlUnit]
 *	Created by bisegni.
 *
 *    	Copyright 25/08/16 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#ifndef __ExampleControlUnit__913CD18_4BA7_411B_BBB2_207079D0296F_TestSetOnly_h
#define __ExampleControlUnit__913CD18_4BA7_411B_BBB2_207079D0296F_TestSetOnly_h


#include <boost/random.hpp>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>
#include <chaos/cu_toolkit/control_manager/slow_command/SlowCommand.h>

using namespace chaos;

namespace c_data = chaos::common::data;
namespace ccc_slow_command = chaos::cu::control_manager::slow_command;

DEFINE_BATCH_COMMAND_CLASS(TestSetOnly,
                           ccc_slow_command::SlowCommand) {
protected:
    // return the implemented handler
    uint8_t implementedHandler();
    
    // Start the command execution
    void setHandler(c_data::CDataWrapper *data);
public:
    TestSetOnly();
    ~TestSetOnly();
};


#endif /* __ExampleControlUnit__913CD18_4BA7_411B_BBB2_207079D0296F_TestSetOnly_h */
