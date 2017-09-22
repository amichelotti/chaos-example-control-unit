/*
 *	TestSetOnly.cpp
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

#include "TestSetOnly.h"

#define CMDCU_ LAPP_ << "[TestSetOnly] - " << getDeviceID() << " - [" << getUID() <<"] "

using namespace chaos;

using namespace chaos::common::data;

using namespace chaos::common::batch_command;

using namespace chaos::cu::control_manager::slow_command;

/*
 * T
 */
BATCH_COMMAND_OPEN_DESCRIPTION(,TestSetOnly,
                               "Testset only command managment",
                               "02C76F24-EBD2-4ABC-9C5A-3390E2889715")
BATCH_COMMAND_CLOSE_DESCRIPTION()

TestSetOnly::TestSetOnly(){}

TestSetOnly::~TestSetOnly() {}

// return the implemented handler
uint8_t TestSetOnly::implementedHandler() {
    return  HandlerType::HT_Set;
}

// Start the command execution
void TestSetOnly::setHandler(CDataWrapper *data) {
    CMDCU_ << "Empty set command for test only";
    BC_END_RUNNING_PROPERTY
}
