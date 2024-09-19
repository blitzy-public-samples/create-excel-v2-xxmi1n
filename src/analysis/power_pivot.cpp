#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "excel_types.h"
#include "data_manager.h"
#include "data_model_engine.h"
#include "power_pivot.h"

// Validate a data model name
bool ValidateModelName(const std::string& modelName) {
    // Check if the model name is not empty
    if (modelName.empty()) {
        return false;
    }

    // Verify that the model name contains only alphanumeric characters and underscores
    for (char c : modelName) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }

    // Note: Ensuring the model name is not already in use should be done in the PowerPivot class
    // as it has access to the m_dataModels map

    return true;
}

PowerPivot::PowerPivot(std::shared_ptr<DataManager> dataManager, std::shared_ptr<DataModelEngine> dataModelEngine)
    : m_dataManager(dataManager), m_dataModelEngine(dataModelEngine) {
    // Initialize m_dataModels as an empty unordered_map
    m_dataModels = std::unordered_map<std::string, std::shared_ptr<DataModel>>();
}

std::shared_ptr<DataModel> PowerPivot::CreateDataModel(const std::string& modelName) {
    // Validate the model name using ValidateModelName
    if (!ValidateModelName(modelName)) {
        return nullptr;
    }

    // Ensure the model name is not already in use
    if (m_dataModels.find(modelName) != m_dataModels.end()) {
        return nullptr;
    }

    // Create a new DataModel object with the given name
    auto newModel = std::make_shared<DataModel>(modelName);

    // Add the data model to m_dataModels
    m_dataModels[modelName] = newModel;

    // Return the created data model object
    return newModel;
}

bool PowerPivot::AddTableToModel(const std::string& modelName, const std::string& tableName, const CellRange& tableRange) {
    // Find the data model in m_dataModels using modelName
    auto modelIt = m_dataModels.find(modelName);
    if (modelIt == m_dataModels.end()) {
        return false;
    }

    // Call m_dataModelEngine->CreateTable with the table name and range
    auto newTable = m_dataModelEngine->CreateTable(tableName, tableRange);
    if (!newTable) {
        return false;
    }

    // Add the created table to the data model
    modelIt->second->AddTable(newTable);

    // Return true if the table was added successfully
    return true;
}

bool PowerPivot::CreateRelationship(const std::string& modelName, const std::string& fromTable, const std::string& fromColumn, const std::string& toTable, const std::string& toColumn) {
    // Find the data model in m_dataModels using modelName
    auto modelIt = m_dataModels.find(modelName);
    if (modelIt == m_dataModels.end()) {
        return false;
    }

    // Call m_dataModelEngine->CreateRelationship with the provided parameters
    auto newRelationship = m_dataModelEngine->CreateRelationship(fromTable, fromColumn, toTable, toColumn);
    if (!newRelationship) {
        return false;
    }

    // Add the created relationship to the data model
    modelIt->second->AddRelationship(newRelationship);

    // Return true if the relationship was created successfully
    return true;
}

bool PowerPivot::CreateMeasure(const std::string& modelName, const std::string& measureName, const std::string& expression) {
    // Find the data model in m_dataModels using modelName
    auto modelIt = m_dataModels.find(modelName);
    if (modelIt == m_dataModels.end()) {
        return false;
    }

    // Call m_dataModelEngine->CreateMeasure with the measure name and expression
    auto newMeasure = m_dataModelEngine->CreateMeasure(measureName, expression);
    if (!newMeasure) {
        return false;
    }

    // Add the created measure to the data model
    modelIt->second->AddMeasure(newMeasure);

    // Return true if the measure was created successfully
    return true;
}

bool PowerPivot::RefreshDataModel(const std::string& modelName) {
    // Find the data model in m_dataModels using modelName
    auto modelIt = m_dataModels.find(modelName);
    if (modelIt == m_dataModels.end()) {
        return false;
    }

    // Call m_dataModelEngine->RefreshModel with the found data model
    bool refreshSuccess = m_dataModelEngine->RefreshModel(modelIt->second);
    if (!refreshSuccess) {
        return false;
    }

    // Update the data model's last refresh time
    modelIt->second->UpdateLastRefreshTime();

    // Return true if the refresh was successful
    return true;
}

bool PowerPivot::DeleteDataModel(const std::string& modelName) {
    // Find the data model in m_dataModels using modelName
    auto modelIt = m_dataModels.find(modelName);
    if (modelIt == m_dataModels.end()) {
        return false;
    }

    // Remove the data model from m_dataModels
    m_dataModels.erase(modelIt);

    // Clean up any resources associated with the data model
    // Note: This step might involve additional cleanup depending on the implementation details

    // Return true
    return true;
}