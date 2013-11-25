/*
 *	DummyDriver.h
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

#include <map>

#include <boost/random.hpp>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

namespace cu_driver = chaos::cu::driver_manager::driver;

DEFINE_CU_DRIVER_DEFINITION_PROTOTYPE(DummyDriver)

typedef enum {
    //! init the simulation with an id and the return value of the user need to be an handler to the SinData struct
    OP_INIT_SIMULATION = cu_driver::OpcodeType::OP_USER,
    OP_SET_POINTS, //set the number of points of the simulation
    /*!
     freq|bias|phase|gain|gainNoise
     */
    OP_SET_SIMULATION_PARAMETER,    //require a pointer to a SimulParameter
    OP_STEP_SIMULATION,             //radvance the simulation
    OP_GET_SIN_DATA                 //require a pointer to an double array
} DummyDriverOpcode;

typedef double SimulParameter[5];

typedef struct SinData {
    int id;
    double *data;
    uint32_t points;
    SimulParameter parameter;
} SinData;

typedef enum Parameter {
    freq,
    bias,
    phase,
    gain,
    gainNoise
} Parameter;

/*
 driver definition
 */
class DummyDriver: ADD_CU_DRIVER_PLUGIN_SUPERCLASS {
    std::map<int, SinData*> simulations;
    typedef boost::mt19937 RNGType;
    RNGType rng;
    boost::uniform_int<> one_to_hundred;
    boost::variate_generator< RNGType, boost::uniform_int<> > randInt;
    
    long double PI;
    
	void driverInit(const char *initParameter) throw(chaos::CException);
	void driverDeinit() throw(chaos::CException);
    
    void initSimulation(int simulation_id, SinData **data);
    void setSimulationsPoints(SinData *sin_data);
    void computeSimulation(SinData *sin_data);
    
public:
    DummyDriver();
	~DummyDriver();
    //! Execute a command
	cu_driver::MsgManagmentResultType::MsgManagmentResult execOpcode(cu_driver::DrvMsgPtr cmd);
};

#endif /* defined(__ControlUnitTest__DummyDriver__) */
