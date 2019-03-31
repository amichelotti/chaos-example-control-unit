//
//  main.cpp
//  SinGeneratorExternalDriver
//
//  Created by bisegni on 08/08/2017.
//  Copyright © 2017 INFN. All rights reserved.
//

#include <iostream>

#include <chaos_micro_unit_toolkit/micro_unit_toolkit.h>

#include <boost/random.hpp>
#include <boost/atomic.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace chaos::micro_unit_toolkit;
using namespace chaos::micro_unit_toolkit::data;
using namespace chaos::micro_unit_toolkit::connection;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy;
using namespace chaos::micro_unit_toolkit::connection::unit_proxy::raw_driver;
using namespace chaos::micro_unit_toolkit::connection::connection_adapter;

int event_handler(void *user_data, unsigned int event, void *event_data);

#define points "points"
#define freq "freq"
#define bias "bias"
#define gain "gain"
#define gainNoise "gainNoise"
#define phase "phase"

uint64_t last_auto_push_ts = 0;
const boost::posix_time::ptime EPOCH(boost::gregorian::date(1970,1,1));

long double PI = acos((long double) -1);

boost::mt19937 rng((const uint_fast32_t) time(0));
boost::uniform_int<> one_to_hundred( -100, 100 );
boost::variate_generator< boost::mt19937, boost::uniform_int<> > randInt(rng, one_to_hundred);

int main(int argc, const char * argv[]) {
    int err = 0;
    srand((unsigned)time(0));
    ChaosMicroUnitToolkit mut;
    const char *option="Content-Type: application/bson-json\r\n";
    ChaosUniquePtr<ExternalDriverHandlerWrapper> hw = mut.createNewExternalDriverHandlerWrapper(ConnectionTypeHTTP,
                                                                                                "ws://localhost:8080/sim/sine",
                                                                                                option,
                                                                                                event_handler,
                                                                                                NULL,
                                                                                                "");
    
    while (err == 0) {
        err = hw->poll(100);
    }
    std::cout << "Exti with code" << err;
    return err?EXIT_FAILURE:EXIT_SUCCESS;
}

int event_handler(void *user_data,
                  unsigned int event,
                  void *event_data) {
    int err = 0;
    switch(event){
        case UP_EV_USR_ACTION:{
            //            uint64_t curr_msec = (boost::posix_time::microsec_clock::universal_time()-EPOCH).total_milliseconds();
            //            if((curr_msec-last_auto_push_ts)>1000) {
            //                //push new data automatically
            //                RawDriverHandlerWrapper *pi = static_cast<RawDriverHandlerWrapper*>(event_data);
            //                DataPackUniquePtr message(new DataPack());
            //                message->addString("key", "async messages");
            //                pi->sendMessage(message);
            //                last_auto_push_ts = (boost::posix_time::microsec_clock::universal_time()-EPOCH).total_milliseconds();
            //            }
            break;
        }
        case UP_EV_CONN_CONNECTED:{
            std::cout << "UP_EV_CONN_CONNECTED" << std::endl;
            break;
        }
        case UP_EV_CONN_ERROR:{
            std::cout << "UP_EV_CONN_ERROR" << std::endl;
            break;
        }
        case UP_EV_CONN_RETRY:{
            std::cout << "UP_EV_CONN_RETRY" << std::endl;
            break;
        }
        case UP_EV_CONN_DISCONNECTED:{
            std::cout << "UP_EV_CONN_DISCONNECTED" << std::endl;
            break;
        }
        case UP_EV_CONN_ACCEPTED:{
            std::cout << "UP_EV_CONN_ACCEPT" << std::endl;
            //set te start base ts for push
            last_auto_push_ts = (boost::posix_time::microsec_clock::universal_time()-EPOCH).total_milliseconds();
            break;
        }
        case UP_EV_CONN_REJECTED:{
            std::cout << "UP_EV_CONN_REJECTED" << std::endl;
            break;
        }
        case UP_EV_AUTH_ACCEPTED:{
            std::cout << "UP_EV_AUTH_ACCEPTED" << std::endl;
            break;
        }
        case UP_EV_AUTH_REJECTED:{
            std::cout << "UP_EV_AUTH_REJECTED" << std::endl;
            break;
        }
        case UP_EV_MSG_RECEIVED:{
            std::cout << "UP_EV_MSG_RECEIVED" << std::endl;
            break;
        }
        
        case UP_EV_INIT_RECEIVED:{
            std::cout << "UP_EV_INIT_RECEIVED" << std::endl;
            EDInitRequest *req = static_cast<EDInitRequest*>(event_data);
            req->response.configuration_state = 1;
            req->response.new_uri_id ="new instance";
            break;
        }
           
        case UP_EV_DEINIT_RECEIVED:{
            std::cout << "UP_EV_DEINIT_RECEIVED" << std::endl;
            EDDeinitRequest *req = static_cast<EDDeinitRequest*>(event_data);
            //fetch instance
            break;
        }
            
        case UP_EV_REQ_RECEIVED:{
            //fetch instace
            std::cout << "UP_EV_REQ_RECEIVED" << std::endl;
            EDNormalRequest *req = static_cast<EDNormalRequest*>(event_data);
            if(req->message.opcode.compare("gen_sin") == 0) {
                const uint32_t _points = (uint32_t)req->message.opcode_par->getInt32Value(points);
                const double _freq = req->message.opcode_par->getDoubleValue(freq);
                const double _gain = req->message.opcode_par->getDoubleValue(gain);
                const double _gainNoise = req->message.opcode_par->getDoubleValue(gainNoise);
                const double _bias = req->message.opcode_par->getDoubleValue(bias);
                const double _phase = req->message.opcode_par->getDoubleValue(phase);
                double sin_points[_points];
                double interval = (2*PI)/(_points);
                for(int i=0; i<_points; i++){
                    double sin_point = sin((interval*i*_freq) + _phase);
                    double sin_point_rumor = (((double)randInt()/(double)100) * _gainNoise);
                    sin_points[i] = (_gain * sin_point) + sin_point_rumor + _bias;
                }
                req->response.message->addBinaryValue("sin_wave",
                                                      reinterpret_cast<const char *>(sin_points),
                                                      _points*sizeof(double));
            }
            break;
        }
        case UP_EV_ERR_RECEIVED:{
            std::cout << "UP_EV_ERR_RECEIVED" << std::endl;
            UPError *req = static_cast<UPError*>(event_data);
            std::cerr << "[Remote Error]\nCode:"<<req->error<<
            "\nError Message:"<<req->message<<
            "\nError Domain:"<<req->domain;
            break;
        }
    }
    return err;
}
