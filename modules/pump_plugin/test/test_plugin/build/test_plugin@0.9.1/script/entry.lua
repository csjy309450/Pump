--[<plugin_install_dir>/<name@version>/script/entry.lua]
--[[
Plugin代表一个插件对象，拥有插件的全部方法和数据
]]--
local PluginServer = {
  name="test_plugin",
  version="0.9.1",
  -- 请求队列
  requests_locker=SLB.CMutex(),
  requests={1,2,3}, 
}

function Sleep(n)
    if n > 0 then os.execute("ping -n " .. tonumber(n + 1) .. " localhost > NUL") end
end

-- 检测
function PluginServer:Check()
    print('PluginServer:OnCheck()');
    print('PluginServer.name ' .. PluginServer.name);
    print('PluginServer.version ' .. PluginServer.version);
    print('PluginServer.requests_locker ' .. type(PluginServer.requests_locker));
    print('PluginServer.requests ' .. type(PluginServer.requests));
end

-- 构造函数
function PluginServer:Init()
    print('PluginServer:OnInit()');
end

-- 插件安装回调
function PluginServer:Load()
    print('PluginServer:OnLoad()');
    PluginServer:__Main();
end

-- 插件卸载回调
function PluginServer:Unload()
    print('PluginServer:OnUnload()');
end

-- 投递请求
function PluginServer:Post(req)
    print('PluginServer:Post()'.. req);
    (PluginServer.requests_locker):Lock();
    table.insert((PluginServer.requests), 1, req);
    (PluginServer.requests_locker):Unlock();
end

function PluginServer:__MainCore()
    while(1) 
    do
        print("--batch begin---");
        (PluginServer.requests_locker):Lock();
        for k,v in ipairs(PluginServer.requests) do
            print("#" .. tostring(v));
            -- table.remove(PluginServer.requests);
        end
        (PluginServer.requests_locker):Unlock();
        print('--batch end---');
        Sleep(1);
    end
end

-- 插件执行入口
-- 内部会不断从请求队列中获取请求,调用__OnRequest()，处理后调用__OnResponse()通知上层应用处理接口
function PluginServer:__Main()
    print('PluginServer:__Main()\n')
    thxData = SLB.CLuaThreadData()
    print(thxData)
    thxData:SetLuaScript("SLB." .. PluginServer.name .. '.Main();')
    thx = SLB.CLuaThread()
    print(thx:This())
    thx:SetData(thxData:This())
    thx:Start()
end

-- 插件请求的处理回调
function PluginServer:__OnRequest()
    print('PluginServer:__OnRequest()\n')
end

-- 插件响应回调
function PluginServer:__OnResponse()
    print('PluginServer:__OnResponse()')
end

return {
    Check = PluginServer.Check,
    Init = PluginServer.Init,
    Load = PluginServer.Load,
    Unload = PluginServer.Unload,
    Post = PluginServer.Post,
    Main = PluginServer.__MainCore
}