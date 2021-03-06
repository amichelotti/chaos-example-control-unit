/*
 *	SinWaveCommand.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#ifndef __ControlUnitTest__SinWaveCommand__
#define __ControlUnitTest__SinWaveCommand__

#include <boost/random.hpp>
#include <boost/thread.hpp>
#include <chaos/cu_toolkit/control_manager/slow_command/SlowCommand.h>

using namespace chaos;

using namespace chaos::common::data;
using namespace chaos::cu::control_manager::slow_command;
using namespace chaos::common::batch_command;

DEFINE_BATCH_COMMAND_CLASS(SinWaveCommand, SlowCommand) {
    typedef boost::mt19937 RNGType;
    RNGType rng;
    uniform_int<> one_to_hundred;
    variate_generator< RNGType, uniform_int<> > randInt;

	uint32_t out_sin_value_points;
    uint64_t lastStartTime;
    
    long double PI;
    int32_t messageID;
    boost::mutex pointChangeMutex;
    inline void _setWavePoint(int32_t new_points);
    inline void setWavePoint();
protected:
    // return the implemented handler
    uint8_t implementedHandler();
    
    // Start the command execution
    void setHandler(CDataWrapper *data);
    
    // Aquire the necessary data for the command
    /*!
     The acquire handler has the purpose to get all necessary data need the by CC handler.
     \return the mask for the runnign state
     */
    void acquireHandler();
    
    // Correlation and commit phase
    void ccHandler();
    
public:
    SinWaveCommand();
    ~SinWaveCommand();
};

#endif /* defined(__ControlUnitTest__SinWaveCommand__) */
