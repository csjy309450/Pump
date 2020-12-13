#ifndef __PUMP_PLUGIN_INNER_INTERFACE_H
#define __PUMP_PLUGIN_INNER_INTERFACE_H

namespace Pump
{
namespace Plugin
{

class CPluginClient;

/**根据id获取CPluginClient对象(内部接口)，上锁，防止使用时被卸载*/
PUMP_PLUGIN_CXXAPI CPluginClient  * PUMP_PSLOT_INNER_GetPluginClientByIDLocked(pump_plugin_id pid);

/**解锁id Plugin*/
PUMP_PLUGIN_CXXAPI void PUMP_PSLOT_INNER_PluginClientByIDUnLock(pump_plugin_id pid);

}
}

#endif // __PUMP_PLUGIN_INNER_INTERFACE_H