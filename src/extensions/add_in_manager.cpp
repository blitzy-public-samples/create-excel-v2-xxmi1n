#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <any>
#include "excel_types.h"
#include "add_in_manager.h"
#include "security_manager.h"
#include "add_in_loader.h"
#include "event_manager.h"

// Maximum number of add-ins that can be loaded simultaneously
const int MAX_LOADED_ADDINS = 100;

// Validates an add-in before loading
bool ValidateAddIn(const std::string& addInPath) {
    // Check if the add-in file exists
    // TODO: Implement file existence check

    // Verify the add-in file format and structure
    // TODO: Implement file format and structure verification

    // Check for any known vulnerabilities or malicious code
    // TODO: Implement vulnerability and malicious code check

    // Validate the add-in manifest
    // TODO: Implement manifest validation

    // Return true if all checks pass, false otherwise
    return true; // Placeholder return value
}

AddInManager::AddInManager(std::shared_ptr<SecurityManager> securityManager,
                           std::shared_ptr<AddInLoader> addInLoader,
                           std::shared_ptr<EventManager> eventManager)
    : m_securityManager(securityManager),
      m_addInLoader(addInLoader),
      m_eventManager(eventManager),
      m_loadedAddIns() {
    // Constructor implementation
}

bool AddInManager::LoadAddIn(const std::string& addInPath) {
    // Check if the number of loaded add-ins is less than MAX_LOADED_ADDINS
    if (m_loadedAddIns.size() >= MAX_LOADED_ADDINS) {
        return false;
    }

    // Validate the add-in using ValidateAddIn
    if (!ValidateAddIn(addInPath)) {
        return false;
    }

    // Check add-in permissions using m_securityManager->CheckAddInPermissions
    if (!m_securityManager->CheckAddInPermissions(addInPath)) {
        return false;
    }

    // Load the add-in using m_addInLoader->LoadAddIn
    std::shared_ptr<AddIn> addIn = m_addInLoader->LoadAddIn(addInPath);
    if (!addIn) {
        return false;
    }

    // If loading is successful, add the add-in to m_loadedAddIns
    m_loadedAddIns[addIn->GetId()] = addIn;

    // Register add-in event handlers with m_eventManager
    m_eventManager->RegisterAddInEventHandlers(addIn);

    // Return true if the add-in was loaded successfully
    return true;
}

bool AddInManager::UnloadAddIn(const std::string& addInId) {
    // Find the add-in with the given addInId in m_loadedAddIns
    auto it = m_loadedAddIns.find(addInId);
    if (it == m_loadedAddIns.end()) {
        return false;
    }

    // Unregister add-in event handlers from m_eventManager
    m_eventManager->UnregisterAddInEventHandlers(it->second);

    // Unload the add-in using m_addInLoader->UnloadAddIn
    if (!m_addInLoader->UnloadAddIn(it->second)) {
        return false;
    }

    // Remove the add-in from m_loadedAddIns
    m_loadedAddIns.erase(it);

    // Return true if the add-in was unloaded successfully
    return true;
}

std::any AddInManager::ExecuteAddInFunction(const std::string& addInId,
                                            const std::string& functionName,
                                            const std::vector<std::any>& parameters) {
    // Find the add-in with the given addInId in m_loadedAddIns
    auto it = m_loadedAddIns.find(addInId);
    if (it == m_loadedAddIns.end()) {
        throw std::runtime_error("Add-in not found");
    }

    // Check function permissions using m_securityManager->CheckFunctionPermissions
    if (!m_securityManager->CheckFunctionPermissions(addInId, functionName)) {
        throw std::runtime_error("Function execution not allowed");
    }

    // Execute the function on the add-in object
    return it->second->ExecuteFunction(functionName, parameters);
}

std::vector<std::string> AddInManager::GetLoadedAddIns() {
    // Initialize an empty vector for add-in IDs
    std::vector<std::string> addInIds;

    // Iterate through m_loadedAddIns
    for (const auto& pair : m_loadedAddIns) {
        // Add each add-in's ID to the vector
        addInIds.push_back(pair.first);
    }

    // Return the vector of add-in IDs
    return addInIds;
}