#ifndef __PUMP_PLUGIN_INNER_INTERFACE_H
#define __PUMP_PLUGIN_INNER_INTERFACE_H

namespace Pump
{
namespace Plugin
{

class CPluginClient;

/**����id��ȡCPluginClient����(�ڲ��ӿ�)����������ֹʹ��ʱ��ж��*/
PUMP_PLUGIN_CXXAPI CPluginClient  * PUMP_PSLOT_INNER_GetPluginClientByIDLocked(pump_plugin_id pid);

/**����id Plugin*/
PUMP_PLUGIN_CXXAPI void PUMP_PSLOT_INNER_PluginClientByIDUnLock(pump_plugin_id pid);

}
}

#endif // __PUMP_PLUGIN_INNER_INTERFACE_H