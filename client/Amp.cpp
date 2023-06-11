#include "Amp.hpp"

#include "DXHook.hpp"
#include "ConsoleHandler.hpp"
#include "Roblox.hpp"
#include "Executor.hpp"
#include "ScriptScheduler.hpp"

#include <mutex>

#define ASSERT_FUNCTION(f) ? f : (void)(0)
#define ASSERT_FUNCTION_BODY(body) ? ([&](void) -> void {body})() : (void)(0)

HINSTANCE DLLInstanceHandle;
TextEditor editor;
Amp::LoadingState lS = Amp::LoadingState::LOADING;

void notimplemented() {
    ConsoleHandler::Send("This feature hasn't been implemented in amp yet.");
}

void PushInterfaces() {
    Amp::DXHook::PushInterface("console", [](void) -> void {
        const ImVec2 minSize = ImVec2(450, 350);
        const ImVec2 maxSize = ImVec2(Amp::DXHook::getScreenWidth(), Amp::DXHook::getScreenHeight());
        ImGui::SetNextWindowSizeConstraints(minSize, maxSize);
        ImGui::Begin("amp console", 0, ImGuiWindowFlags_NoCollapse);
        {
            const ImVec2 windowSize = ImGui::GetWindowSize();

            ImGui::InputTextMultiline("", const_cast<char*>(ConsoleHandler::GetBuffer()), ConsoleHandler::GetSize(), { windowSize.x-20, windowSize.y-35 }, ImGuiInputTextFlags_ReadOnly);
        }
        ImGui::End();
    });

    Amp::DXHook::PushInterface("script", [](void) -> void {
        const ImVec2 minSize = ImVec2(550, 300);
        const ImVec2 maxSize = ImVec2(Amp::DXHook::getScreenWidth(), Amp::DXHook::getScreenHeight());
        ImGui::SetNextWindowSizeConstraints(minSize, maxSize);
        ImGui::Begin("amp script execution environment", Amp::DXHook::InterfaceOpened(), ImGuiWindowFlags_NoCollapse);
        {
            const ImVec2 windowSize = ImGui::GetWindowSize();

            editor.Render("", { windowSize.x - 20,windowSize.y - 60 }, true);

            ImGui::Spacing();
            ImGui::Button("Execute", ImVec2(70, 20)) ASSERT_FUNCTION_BODY(
                /*Amp::ScriptScheduler::PushScript(editor.GetText().c_str());*/
                Amp::Executor::DoString(Roblox::GetLuaState(), editor.GetText());
            );
            ImGui::SameLine();
            ImGui::Button("Clear", ImVec2(70, 20)) ASSERT_FUNCTION_BODY(
                editor.SetText("");
            );
            ImGui::SameLine();
            ImGui::Button("Open", ImVec2(70, 20)) ASSERT_FUNCTION(notimplemented());
            ImGui::SameLine();
            ImGui::Button("Save", ImVec2(70, 20)) ASSERT_FUNCTION(notimplemented());
        }
        ImGui::End();
    });
}

void Amp::Main(HINSTANCE hinstDLL)
{
    const char* res;

    DLLInstanceHandle = hinstDLL;

    /* Prepare our text editor before doing anything with DXHook */
    editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());

    Amp::DXHook::HookInit();
    ConsoleHandler::Send("DXHooked\n");

    PushInterfaces(); /* Push our interfaces! */
    ConsoleHandler::Send("Interfaces pushed\n");

    res = Roblox::Scan();
    if (res != nullptr) {
        ConsoleHandler::Send("Failed to scan Roblox.\n\t%s\n", res);
        return;
    }
    ConsoleHandler::Send("Roblox scanned!\n\tLuaState: 0x%p\n", (uintptr_t)Roblox::GetLuaState());

    /* We're ready! */
    ConsoleHandler::Send("We're ready!\n\tPress Insert to hide interfaces.\n");
    lS = Amp::LoadingState::READY;
}

HINSTANCE Amp::getHinstDLL()
{
    return DLLInstanceHandle;
}

HWND RobloxWindow = nullptr;
HWND Amp::getRobloxWindowHwnd()
{
    if (RobloxWindow == nullptr) RobloxWindow = FindWindowW(NULL, L"Roblox");
    return RobloxWindow;
}

HWND cachedWindow = nullptr;
HWND Amp::getCachedWindowHwnd()
{
	if (cachedWindow == nullptr) cachedWindow = reinterpret_cast<HWND>(CreateMenu());
	return cachedWindow;
}

Amp::LoadingState Amp::getLoadingState()
{
    return lS;
}
