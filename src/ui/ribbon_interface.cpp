#include <vector>
#include <memory>
#include <string>
#include "excel_types.h"
#include "event_handler.h"
#include "ribbon_interface.h"
#include "ui_framework.h"

// Global constants
const int MAX_TABS = 10;
const int MAX_GROUPS_PER_TAB = 8;
const int MAX_COMMANDS_PER_GROUP = 20;

RibbonInterface::RibbonInterface() {
    // Initialize m_tabs as an empty vector
    m_tabs = std::vector<Tab>();

    // Call CreateDefaultTabs to populate m_tabs with default Excel tabs
    CreateDefaultTabs();
}

void RibbonInterface::Initialize(std::shared_ptr<EventHandler> eventHandler) {
    // Set m_eventHandler to the provided eventHandler
    m_eventHandler = eventHandler;

    // Register necessary event listeners with m_eventHandler
    m_eventHandler->RegisterListener("RibbonCommandTriggered", [this](const std::string& commandId) {
        this->HandleCommand(commandId);
    });
}

bool RibbonInterface::AddTab(const Tab& tab) {
    // Check if MAX_TABS has been reached
    if (m_tabs.size() >= MAX_TABS) {
        return false;
    }

    // Add the new tab to m_tabs
    m_tabs.push_back(tab);
    return true;
}

bool RibbonInterface::RemoveTab(const std::string& tabName) {
    // Find the tab with the given name in m_tabs
    auto it = std::find_if(m_tabs.begin(), m_tabs.end(), [&tabName](const Tab& tab) {
        return tab.GetName() == tabName;
    });

    // If found, remove the tab from m_tabs
    if (it != m_tabs.end()) {
        m_tabs.erase(it);
        return true;
    }

    return false;
}

void RibbonInterface::HandleCommand(const std::string& commandId) {
    // Find the command with the given commandId in m_tabs
    for (const auto& tab : m_tabs) {
        for (const auto& group : tab.GetGroups()) {
            for (const auto& command : group.GetCommands()) {
                if (command.GetId() == commandId) {
                    // If found, call ExecuteCommand with the found command
                    ExecuteCommand(command);
                    return;
                }
            }
        }
    }
}

void RibbonInterface::UpdateRibbonState() {
    // Iterate through all commands in m_tabs
    for (auto& tab : m_tabs) {
        for (auto& group : tab.GetGroups()) {
            for (auto& command : group.GetCommands()) {
                // Update each command's enabled/disabled state based on the current context
                bool isEnabled = true;

                // Example: Disable "Paste" if clipboard is empty
                if (command.GetId() == "paste") {
                    isEnabled = !m_clipboard.empty();
                }

                // Example: Disable "Bold" if no cells are selected
                if (command.GetId() == "bold") {
                    isEnabled = m_activeWorkbook && m_activeWorkbook->GetActiveSheet().HasSelection();
                }

                command.SetEnabled(isEnabled);
            }
        }
    }
}

void RibbonInterface::CreateDefaultTabs() {
    // Call CreateHomeTab and add the result to m_tabs
    m_tabs.push_back(CreateHomeTab());

    // Call CreateInsertTab and add the result to m_tabs
    m_tabs.push_back(CreateInsertTab());

    // Call CreatePageLayoutTab and add the result to m_tabs
    m_tabs.push_back(CreatePageLayoutTab());

    // Call CreateFormulasTab and add the result to m_tabs
    m_tabs.push_back(CreateFormulasTab());

    // Call CreateDataTab and add the result to m_tabs
    m_tabs.push_back(CreateDataTab());

    // Call CreateReviewTab and add the result to m_tabs
    m_tabs.push_back(CreateReviewTab());

    // Call CreateViewTab and add the result to m_tabs
    m_tabs.push_back(CreateViewTab());
}

void RibbonInterface::ExecuteCommand(const Command& command) {
    // Check if the command is enabled
    if (command.IsEnabled()) {
        // If enabled, execute the command's action
        command.Execute();

        // Update the ribbon state after command execution
        UpdateRibbonState();
    }
}

Tab CreateHomeTab() {
    // Create a new Tab object for the Home tab
    Tab homeTab("Home");

    // Add Clipboard group with Cut, Copy, Paste commands
    Group clipboardGroup("Clipboard");
    clipboardGroup.AddCommand(Command("cut", "Cut", []() { /* Cut action */ }));
    clipboardGroup.AddCommand(Command("copy", "Copy", []() { /* Copy action */ }));
    clipboardGroup.AddCommand(Command("paste", "Paste", []() { /* Paste action */ }));
    homeTab.AddGroup(clipboardGroup);

    // Add Font group with Font, Font Size, Bold, Italic, Underline commands
    Group fontGroup("Font");
    fontGroup.AddCommand(Command("font", "Font", []() { /* Font action */ }));
    fontGroup.AddCommand(Command("font_size", "Font Size", []() { /* Font Size action */ }));
    fontGroup.AddCommand(Command("bold", "Bold", []() { /* Bold action */ }));
    fontGroup.AddCommand(Command("italic", "Italic", []() { /* Italic action */ }));
    fontGroup.AddCommand(Command("underline", "Underline", []() { /* Underline action */ }));
    homeTab.AddGroup(fontGroup);

    // Add Alignment group with Align Left, Center, Align Right commands
    Group alignmentGroup("Alignment");
    alignmentGroup.AddCommand(Command("align_left", "Align Left", []() { /* Align Left action */ }));
    alignmentGroup.AddCommand(Command("align_center", "Center", []() { /* Center action */ }));
    alignmentGroup.AddCommand(Command("align_right", "Align Right", []() { /* Align Right action */ }));
    homeTab.AddGroup(alignmentGroup);

    // Add Number group with Number Format, Currency, Percent commands
    Group numberGroup("Number");
    numberGroup.AddCommand(Command("number_format", "Number Format", []() { /* Number Format action */ }));
    numberGroup.AddCommand(Command("currency", "Currency", []() { /* Currency action */ }));
    numberGroup.AddCommand(Command("percent", "Percent", []() { /* Percent action */ }));
    homeTab.AddGroup(numberGroup);

    // Add Styles group with Cell Styles, Format as Table commands
    Group stylesGroup("Styles");
    stylesGroup.AddCommand(Command("cell_styles", "Cell Styles", []() { /* Cell Styles action */ }));
    stylesGroup.AddCommand(Command("format_as_table", "Format as Table", []() { /* Format as Table action */ }));
    homeTab.AddGroup(stylesGroup);

    // Add Cells group with Insert, Delete, Format commands
    Group cellsGroup("Cells");
    cellsGroup.AddCommand(Command("insert", "Insert", []() { /* Insert action */ }));
    cellsGroup.AddCommand(Command("delete", "Delete", []() { /* Delete action */ }));
    cellsGroup.AddCommand(Command("format", "Format", []() { /* Format action */ }));
    homeTab.AddGroup(cellsGroup);

    // Add Editing group with Sum, Fill, Clear commands
    Group editingGroup("Editing");
    editingGroup.AddCommand(Command("sum", "Sum", []() { /* Sum action */ }));
    editingGroup.AddCommand(Command("fill", "Fill", []() { /* Fill action */ }));
    editingGroup.AddCommand(Command("clear", "Clear", []() { /* Clear action */ }));
    homeTab.AddGroup(editingGroup);

    // Return the created Tab object
    return homeTab;
}

// Note: Implementations for CreateInsertTab, CreatePageLayoutTab, CreateFormulasTab,
// CreateDataTab, CreateReviewTab, and CreateViewTab are not provided in this example
// but should be implemented similarly to CreateHomeTab.