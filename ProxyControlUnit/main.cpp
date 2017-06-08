/*
 *	ControlUnitTest.cpp
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
#include <chaos/common/global.h>
#include <chaos/common/chaos_types.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/cu_toolkit/ChaosCUToolkit.h>

#include <boost/thread.hpp>

#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace chaos;
using namespace chaos::cu;

CHAOS_DEFINE_MAP_FOR_TYPE(std::string,
                          boost::shared_ptr<chaos::cu::control_manager::ControlUnitApiInterface>,
                          MapProxyInterface);

struct DaqInfo {
    bool daq_run;
    boost::shared_ptr<boost::thread> thread;
    DaqInfo():daq_run(false){}
};

CHAOS_DEFINE_MAP_FOR_TYPE(std::string,
                          boost::shared_ptr<DaqInfo>,
                          MapDaq);

MapProxyInterface map_proxy_interface;
MapDaq map_daq;



bool proxyHandler(const bool load,
                  const std::string& control_unit_id,//control unit id
                  const boost::shared_ptr<chaos::cu::control_manager::ControlUnitApiInterface>& api_interface);

bool attributeHandler(const std::string& control_unit_id,//control unit id
                      const std::string& control_attribute_name,//attribute name
                      const chaos::common::data::CDataVariant& value);

bool controlUnitEvent(const std::string& control_unit_id,//control unit id
                      const chaos::cu::control_manager::ControlUnitProxyEvent& value);

int main (int argc, char* argv[] ) {
    std::vector<string> tmp_device_id;
    try {
        
        //! [CUTOOLKIT Init]
        ChaosCUToolkit::getInstance()->init(argc, argv);
        //! [CUTOOLKIT Init]
        ChaosCUToolkit::getInstance()->setProxyCreationHandler(boost::bind(proxyHandler, _1, _2, _3));
        
        //! [Starting the Framework]
        ChaosCUToolkit::getInstance()->start();
    } catch (CException& e) {
        cerr<<"Exception::"<<endl;
        std::cerr<< "in:"<<e.errorDomain << std::endl;
        std::cerr<< "cause:"<<e.errorMessage << std::endl;
    } catch (program_options::error &e){
        cerr << "Unable to parse command line: " << e.what() << endl;
    } catch (...){
        cerr << "unexpected exception caught.. " << endl;
    }
    try{
        ChaosCUToolkit::getInstance()->stop();
    }catch(...){}
    try{
        ChaosCUToolkit::getInstance()->deinit();
    }catch(...){}
    return 0;
}

bool proxyHandler(const bool load,
                  const std::string& control_unit_id,//control unit id
                  const boost::shared_ptr<chaos::cu::control_manager::ControlUnitApiInterface>& api_interface) {
    if(load) {
        map_proxy_interface.insert(MapProxyInterfacePair(control_unit_id,
                                                         api_interface));
        api_interface->setAttributeHandlerFunctor(boost::bind(attributeHandler, _1, _2, _3));
        api_interface->setEventHandlerFunctor(boost::bind(controlUnitEvent, _1, _2));
    } else {
        map_proxy_interface.erase(control_unit_id);
    }
    return true;
}

bool attributeHandler(const std::string& control_unit_id,//control unit id
                      const std::string& control_attribute_name,//attribute name
                      const chaos::common::data::CDataVariant& value) {
    std::cout<<CHAOS_FORMAT("[%1%] attributeHandler %2%[%3%]",%control_unit_id%control_attribute_name%value.asString())<<endl;
    return true;
}

void daqThread(std::string control_unit_id,
               boost::shared_ptr<chaos::cu::control_manager::ControlUnitApiInterface> api_interface) {
    int64_t counter = 0;
    
    int64_t *output_1_value = api_interface->getAttributeCache()->getRWPtr<int64_t>(DOMAIN_OUTPUT,
                                                                                    "out_1");
    while(map_daq[control_unit_id]->daq_run) {
        *output_1_value = counter++;
        api_interface->getAttributeCache()->setOutputDomainAsChanged();
        api_interface->pushOutputDataset();
        boost::this_thread::sleep_for(boost::chrono::milliseconds(500));
    }
}

bool controlUnitEvent(const std::string& control_unit_id,//control unit id
                      const chaos::cu::control_manager::ControlUnitProxyEvent& value) {
    switch(value) {
        case chaos::cu::control_manager::ControlUnitProxyEventDefine: {
            std::cout<<CHAOS_FORMAT("[%1%]chaos::cu::control_manager::ControlUnitProxyEventDefine",%control_unit_id)<<endl;
            
            map_proxy_interface[control_unit_id]->addAttributeToDataSet("out_1",
                                                                        "description attribute out 1", DataType::TYPE_INT64,
                                                                        DataType::Output);
            map_proxy_interface[control_unit_id]->addAttributeToDataSet("in_1",
                                                                        "description attribute in 1", DataType::TYPE_INT64,
                                                                        DataType::Input);
            map_proxy_interface[control_unit_id]->enableHandlerOnInputAttributeName("in_1");
        }
            break;
        case chaos::cu::control_manager::ControlUnitProxyEventInit:
            std::cout<<CHAOS_FORMAT("[%1%]chaos::cu::control_manager::ControlUnitProxyEventInit",%control_unit_id)<<endl;
            break;
        case chaos::cu::control_manager::ControlUnitProxyEventStart: {
            std::cout<<CHAOS_FORMAT("[%1%]chaos::cu::control_manager::ControlUnitProxyEventStart",%control_unit_id)<<endl;
            //allocate daq thread
            map_daq[control_unit_id].reset(new DaqInfo());
            map_daq[control_unit_id]->daq_run = true;
            map_daq[control_unit_id]->thread.reset(new boost::thread(boost::bind(daqThread,
                                                                                 control_unit_id,
                                                                                 map_proxy_interface[control_unit_id])));
            break;
        }
        case chaos::cu::control_manager::ControlUnitProxyEventStop:{
            std::cout<<CHAOS_FORMAT("[%1%]chaos::cu::control_manager::ControlUnitProxyEventStop",%control_unit_id)<<endl;
            map_daq[control_unit_id]->daq_run = false;
            map_daq[control_unit_id]->thread->join();
            break;
        }
        case chaos::cu::control_manager::ControlUnitProxyEventDeinit:
            std::cout<<CHAOS_FORMAT("[%1%]chaos::cu::control_manager::ControlUnitProxyEventDeinit",%control_unit_id)<<endl;
            break;
    }
    return true;
}

