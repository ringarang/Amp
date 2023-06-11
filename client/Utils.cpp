#include "Utils.hpp"

#include "Addressing.hpp"
#include "Roblox.hpp"
#include "Addresses.hpp"

#include "WinReg/WinReg.hpp"

#include <windows.h>
#include <comdef.h>

unsigned int Amp::Utils::Rebase(unsigned int addr, int nbase)
{
	return addr - nbase + (unsigned int)GetModuleHandle(0);
}

void Amp::Utils::SetByte(unsigned int addr, std::uint8_t v)
{
    DWORD op;
    VirtualProtect((LPVOID)addr, sizeof(std::uint8_t), PAGE_EXECUTE_READWRITE, &op);
	*reinterpret_cast<std::uint8_t*>(addr) = v;
    VirtualProtect((LPVOID)addr, sizeof(std::uint8_t), op, &op);
}

std::uint8_t Amp::Utils::GetByte(unsigned int addr)
{
    DWORD op;
    VirtualProtect((LPVOID)addr, sizeof(std::uint8_t), PAGE_EXECUTE_READWRITE, &op);
	return *reinterpret_cast<std::uint8_t*>(addr);
    VirtualProtect((LPVOID)addr, sizeof(std::uint8_t), op, &op);
}

std::string Amp::Utils::ResolveHex(std::string hex) {
    int len = hex.length();
    std::string bytecode;
    for (int i = 0; i < len; i += 2)
    {
        std::string byte = hex.substr(i, 2);
        char chr = (char)(int)strtol(byte.c_str(), 0, 16);
        bytecode.push_back(chr);
    }
    return bytecode;
}

std::string Amp::Utils::GetCurrentDirectoryPath()
{
    winreg::RegKey reg;
    auto reg_mv2 = reg.TryOpen(HKEY_CURRENT_USER, L"Software\\amp");
    if (reg_mv2.Failed()) return std::string("failed");
    std::wstring ret;
    auto res = reg.TryGetStringValue(L"path", ret);
    if (res.Failed()) return std::string("failed");
    _bstr_t b(ret.c_str());
    const char* c = b;
    std::string d(c);
    d += "\\";
    return d;
}

void Amp::Utils::alua_getservice(unsigned long ls, const char* service) {
    /*lua_getglobal(ls, "game");
    lua_getfield(ls, -1, "GetService");
    lua_pushvalue(ls, -2);
    lua_pushstring(ls, service);
    lua_call(ls, 2, 1); // 2 arguments, we expect 1 return*/
}

const char* const rbxServices[] = { "ABTestService","AdService","AnalyticsService","AntiAddictionService","AppStorageService","AssetCounterService","AssetManagerService","AssetService","AvatarEditorService","ABTestService","AdService","AnalyticsService","AntiAddictionService","AppStorageService","AssetCounterService","AssetManagerService","AssetService","AvatarEditorService","CacheableContentProvider","ChangeHistoryService","Chat","ClusterPacketCache","CollectionService",
"ContentProvider","ContextActionService","ControllerService","CookiesService","CoreGui","CorePackages","CoreScriptSyncService","CSGDictionaryService","DataStoreService","Debris","DraftsService","EventIngestService","FileSyncService","FlagStandService","FlyweightService","FriendService","GamepadService","GamePassService","Geometry","GoogleAnalyticsConfiguration","GroupService","GuidRegistryService","GuiService","HapticService","Hopper","HttpRbxApiService","HttpService","InsertService","InstancePacketCache","InternalContainer",
"InternalSyncService","JointsService","KeyboardService","KeyframeSequenceProvider","LanguageService","Lighting","LocalizationService","LocalStorageService","LoginService","LogService","LuaWebService","MarketplaceService","MemStorageService","MeshContentProvider","MessagingService","MouseService","NavigationService","NetworkClient","NetworkServer","NetworkSettings","NonReplicatedCSGDictionaryService","NotificationService","PackageService","PathfindingService",
"PermissionsService","PersonalServerService","PhysicsPacketCache","PhysicsService","PlayerEmulatorService","Players","PluginDebugService","PluginGuiService","PointsService","PolicyService","PresenceService","RbxAnalyticsService","RenderSettings","ReplicatedFirst","ReplicatedScriptService","ReplicatedStorage","RobloxPluginGuiService","RobloxReplicatedStorage","RunService","RuntimeScriptService","ScriptContext","ScriptService","Selection","ServerScriptService","ServerStorage","SessionService","SocialService","SolidModelContentProvider","SoundService",
"SpawnerService","StarterGui","StarterPack","StarterPlayer","Stats","StopWatchReporter","Studio","StudioData","StudioService","TaskScheduler","Teams","TeleportService","TestService","TextService","ThirdPartyUserService","TimerService","TouchInputService","TweenService","UGCValidationService","UnvalidatedAssetService","UserInputService","UserService","UserStorageService","VersionControlService","VirtualInputManager","VirtualUser","Visit","VRService","Workspace" };

bool Amp::Utils::alua_isaservice(const char* service)
{
    for (int it = 0; it < sizeof(rbxServices) / sizeof(rbxServices[0]); it++) if (0 == std::strcmp(service, rbxServices[it])) return true;
    return false;
}