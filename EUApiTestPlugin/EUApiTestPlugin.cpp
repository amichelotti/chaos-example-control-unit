/*
 *  EUApiTestPlugin.cpp
 *  EUApiTestPlugin
 *
 *  Created by bisegni on 08/06/2017.
 *  Copyright © 2017 INFN. All rights reserved.
 *
 */


//define the general plugin
#include "EUApiTestPlugin.h"
#include <cstring>

OPEN_EUAPI_LUGIN_CLASS_DEFINITION(EUPluginAlgotest, 1.0, EUTestApiPLugin)
CLOSE_EUAPI_CLASS_DEFINITION

//register the plugin
OPEN_REGISTER_PLUGIN
REGISTER_PLUGIN(EUPluginAlgotest)
CLOSE_REGISTER_PLUGIN

int EUTestApiPLugin::init(const char *init_data) {
    return 0;
}

int EUTestApiPLugin::execute(const char *in_data,
                             uint32_t in_data_size,
                             char **out_data,
                             uint32_t *out_data_size){
    std::string in_str(in_data, in_data_size);
    std::string ou_str = "[out]" + in_str;
    std::cout << in_str << std::endl;
    *out_data = (char*)malloc(ou_str.size());
    std::strcpy(*out_data,
                ou_str.c_str());
    *out_data_size = (uint32_t)ou_str.size();
    return 0;
}

void EUTestApiPLugin::deinit() {
    int i = 0;
}

const char *EUTestApiPLugin::getApiName() {
    return "plugin_test";
}
