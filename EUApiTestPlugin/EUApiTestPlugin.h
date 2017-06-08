/*
 *  EUApiTestPluginPriv.hpp
 *  EUApiTestPlugin
 *
 *  Created by bisegni on 08/06/2017.
 *  Copyright © 2017 INFN. All rights reserved.
 *
 */

/* The classes below are not exported */

#define CHAOS_ENABLE_PLUGIN

#include <chaos/cu_toolkit/control_manager/script/api/plugin/EUAbstractApiPlugin.h>

//test the eu api in plugin
using namespace chaos::cu::control_manager::script::api::plugin;
DECLARE_EUAPI_PLUGIN_SCLASS(EUTestApiPLugin) {
    int execute(const char *in_data,
                uint32_t in_data_size,
                char **out_data,
                uint32_t *out_data_size);
    const char *getApiName();
};

