#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>
#include "excel_types.h"
#include "office_js_api.h"
#include "web_view_bridge.h"
#include "security_manager.h"
#include "data_manager.h"
#include "event_manager.h"

// Constructor implementation
OfficeJSAPI::OfficeJSAPI(std::shared_ptr<WebViewBridge> webViewBridge,
                         std::shared_ptr<SecurityManager> securityManager,
                         std::shared_ptr<DataManager> dataManager,
                         std::shared_ptr<EventManager> eventManager)
    : m_webViewBridge(webViewBridge),
      m_securityManager(securityManager),
      m_dataManager(dataManager),
      m_eventManager(eventManager),
      m_apiMethods() {}

void OfficeJSAPI::InitializeAPI() {
    // Register API methods
    RegisterAPIMethods();

    // Set up event listeners for Excel events
    // TODO: Implement event listener setup

    // Initialize the WebViewBridge for communication with web add-ins
    m_webViewBridge->Initialize();

    // Notify the web view that the API is ready
    nlohmann::json readyMessage = {{"type", "APIReady"}};
    m_webViewBridge->SendMessage(readyMessage.dump());
}

std::string OfficeJSAPI::HandleAPIRequest(const std::string& requestJson) {
    nlohmann::json request;
    nlohmann::json response;

    try {
        // Parse the requestJson into a JSON object
        request = nlohmann::json::parse(requestJson);

        // Validate the request
        if (!ValidateRequest(request)) {
            throw std::runtime_error("Invalid API request");
        }

        // Check API permissions
        if (!m_securityManager->CheckAPIPermissions(request["method"], request["parameters"])) {
            throw std::runtime_error("Permission denied for API method");
        }

        // Execute the requested API method
        response = ExecuteAPIMethod(request["method"], request["parameters"]);
    }
    catch (const std::exception& e) {
        response = {
            {"error", true},
            {"message", e.what()}
        };
    }

    // Construct and return the JSON response
    return response.dump();
}

void OfficeJSAPI::RegisterEventHandler(const std::string& eventName, std::function<void(const nlohmann::json&)> handler) {
    // Create a wrapper function that converts event data to JSON
    auto wrapperHandler = [this, handler](const std::string& eventData) {
        nlohmann::json jsonEventData = nlohmann::json::parse(eventData);
        handler(jsonEventData);
    };

    // Register the wrapper function with m_eventManager
    m_eventManager->RegisterHandler(eventName, wrapperHandler);

    // TODO: Store the original handler for future reference if needed
}

void RegisterAPIMethods() {
    // Register methods for Workbook operations
    m_apiMethods["Workbook.getWorksheets"] = [this](const nlohmann::json& params) {
        // TODO: Implement Workbook.getWorksheets
        return nlohmann::json();
    };

    // Register methods for Worksheet operations
    m_apiMethods["Worksheet.getRange"] = [this](const nlohmann::json& params) {
        // TODO: Implement Worksheet.getRange
        return nlohmann::json();
    };

    // Register methods for Range operations
    m_apiMethods["Range.setValue"] = [this](const nlohmann::json& params) {
        // TODO: Implement Range.setValue
        return nlohmann::json();
    };

    // Register methods for Chart operations
    m_apiMethods["Chart.setTitle"] = [this](const nlohmann::json& params) {
        // TODO: Implement Chart.setTitle
        return nlohmann::json();
    };

    // Register methods for Table operations
    m_apiMethods["Table.addRow"] = [this](const nlohmann::json& params) {
        // TODO: Implement Table.addRow
        return nlohmann::json();
    };

    // Register utility and helper methods
    m_apiMethods["Utilities.getVersion"] = [this](const nlohmann::json& params) {
        // TODO: Implement Utilities.getVersion
        return nlohmann::json({{"version", "1.0.0"}});
    };
}

bool ValidateRequest(const nlohmann::json& request) {
    // Check if the request has all required fields
    if (!request.contains("method") || !request.contains("parameters")) {
        return false;
    }

    // Validate the method name exists in m_apiMethods
    if (m_apiMethods.find(request["method"]) == m_apiMethods.end()) {
        return false;
    }

    // TODO: Validate the parameters match the expected types and counts
    // This would require a more detailed API specification

    return true;
}

nlohmann::json ExecuteAPIMethod(const std::string& methodName, const nlohmann::json& parameters) {
    try {
        // Retrieve the method handler from m_apiMethods
        auto methodHandler = m_apiMethods.at(methodName);

        // Execute the method handler with the provided parameters
        return methodHandler(parameters);
    }
    catch (const std::exception& e) {
        // Catch and handle any exceptions that occur during execution
        return nlohmann::json({
            {"error", true},
            {"message", std::string("Error executing method ") + methodName + ": " + e.what()}
        });
    }
}

// Human tasks:
// TODO: Implement event listener setup in InitializeAPI
// TODO: Implement Workbook.getWorksheets method
// TODO: Implement Worksheet.getRange method
// TODO: Implement Range.setValue method
// TODO: Implement Chart.setTitle method
// TODO: Implement Table.addRow method
// TODO: Implement parameter validation in ValidateRequest