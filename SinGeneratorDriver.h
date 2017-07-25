/*
 *	SinGeneratorDriver.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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
#ifndef __ControlUnitTest__TestDeviceDriver__
#define __ControlUnitTest__TestDeviceDriver__

#include "SinGeneratorTypes.h"

#include <chaos/common/chaos_types.h>
#include <boost/random.hpp>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

namespace cu_driver = chaos::cu::driver_manager::driver;

DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(SinGeneratorDriver)

/*
 driver definition
 */
class SinGeneratorDriver: ADD_CU_DRIVER_PLUGIN_SUPERCLASS {
    std::map<int, ChaosSharedPtr<SinGeneratorData> > simulations;
    typedef boost::mt19937 RNGType;
    RNGType rng;
    boost::uniform_int<> one_to_hundred;
    boost::variate_generator< RNGType, boost::uniform_int<> > randInt;
    
    long double PI;
    
	void driverInit(const char *initParameter) throw(chaos::CException);
	void driverDeinit() throw(chaos::CException);
    
    void initSimulation(int simulation_id, SinGeneratorData **data);
    void setSimulationsPoints(SinGeneratorData *sin_data);
    void computeSimulation(SinGeneratorData *sin_data);
    
public:
    SinGeneratorDriver();
	~SinGeneratorDriver();
    //! Execute a command
	cu_driver::MsgManagmentResultType::MsgManagmentResult execOpcode(cu_driver::DrvMsgPtr cmd);
};

#endif /* defined(__ControlUnitTest__SinGeneratorDriver__) */