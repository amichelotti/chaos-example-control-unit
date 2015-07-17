/*	
 *	RTWorkerCU.h
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
#ifndef ChaosFramework_RTWorkerCU_h
#define ChaosFramework_RTWorkerCU_h

#include <string>
#include <boost/random.hpp>
#include <boost/chrono.hpp>
#include <boost/thread.hpp>
#include <chaos/common/utility/Atomic.h>
#include <chaos/cu_toolkit/ControlManager/RTAbstractControlUnit.h>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

using namespace chaos;
using namespace chaos::common::data;

class RTWorkerCU : public chaos::cu::control_manager::RTAbstractControlUnit {
	PUBLISHABLE_CONTROL_UNIT_INTERFACE(RTWorkerCU)
    typedef boost::mt19937 RNGType; 
    RNGType rng;
    uniform_int<> one_to_hundred;    
    variate_generator< RNGType, uniform_int<> > randInt; 
    double numberOfResponse;
    high_resolution_clock::time_point initTime;
    high_resolution_clock::time_point lastExecutionTime;
    high_resolution_clock::time_point currentExecutionTime;
    long double PI;
    
    READWRITE_ATTRIBUTE_HANDLE(double) out_sin_value;
    READWRITE_ATTRIBUTE_HANDLE(uint64_t) out_run_counter;
	int32_t out_sin_value_points;
	
	READONLY_ATTRIBUTE_HANDLE(int32_t) in_points;
	READONLY_ATTRIBUTE_HANDLE(double) in_freq;
    READONLY_ATTRIBUTE_HANDLE(double) in_gain;
    READONLY_ATTRIBUTE_HANDLE(double) in_phase;
    READONLY_ATTRIBUTE_HANDLE(double) in_bias;
    READONLY_ATTRIBUTE_HANDLE(double) in_gain_noise;
    
    boost::mutex pointChangeMutex;
    int32_t messageID;
	boost::shared_ptr<chaos::cu::control_manager::SharedCacheLockDomain> r_o_attr_lock;
public:
    /*
     Construct a new CU with an identifier
     */
    RTWorkerCU(const string& _control_unit_id, const string& _control_unit_param, const ControlUnitDriverList& _control_unit_drivers);
	
    /*
     Destructor a new CU with an identifier
     */
    ~RTWorkerCU();
    
    inline void setWavePoint();
protected:
    /*
     Define the Control Unit Dataset and Actions
     */
    void unitDefineActionAndDataset()throw(CException);
	
	void unitDefineCustomAttribute();
	
    /*(Optional)
     Initialize the Control Unit and all driver, with received param from MetadataServer
     */
    void unitInit() throw(CException);
    /*
     Execute the work, this is called with a determinated delay, it must be as fast as possible
     */
    void unitStart() throw(CException);
    /*
     Execute the work, this is called with a determinated delay, it must be as fast as possible
     */
    void unitRun() throw(CException);
	
	//! pre imput attribute change
	void unitInputAttributePreChangeHandler() throw(CException);
	
	//! attribute changed handler
	void unitInputAttributeChangedHandler() throw(CException);
	
    /*
     The Control Unit will be stopped
     */
    void unitStop() throw(CException);
    
    /*(Optional)
     The Control Unit will be deinitialized and disposed
     */
    void unitDeinit() throw(CException);
	
	//! restore the control unit to snapshot
	void unitRestoreToSnapshot(const std::string& restore_snapshot_tag,
							   chaos::cu::control_manager::AbstractSharedDomainCache * const restore_cache) throw(CException);
    /*
        Test Action Handler
     */
    CDataWrapper* actionTestOne(CDataWrapper*, bool&);
    
    /*
     Test Action Handler
     */
    CDataWrapper* actionTestTwo(CDataWrapper*, bool&);
    
    /*
     Test Action Handler
     */
    CDataWrapper* resetStatistic(CDataWrapper*, bool&);

};

#endif
