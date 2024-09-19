#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "excel_types.h"
#include "data_manager.h"
#include "query_engine.h"
#include "power_query.h"

// Validate a query name
bool ValidateQueryName(const std::string& queryName, const std::unordered_map<std::string, std::shared_ptr<Query>>& queries) {
    // Check if the query name is not empty
    if (queryName.empty()) {
        return false;
    }

    // Verify that the query name contains only alphanumeric characters and underscores
    for (char c : queryName) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }

    // Ensure the query name is not already in use
    if (queries.find(queryName) != queries.end()) {
        return false;
    }

    return true;
}

PowerQuery::PowerQuery(std::shared_ptr<DataManager> dataManager, std::shared_ptr<QueryEngine> queryEngine)
    : m_dataManager(dataManager), m_queryEngine(queryEngine), m_queries() {
    // Initialize m_dataManager with the provided dataManager
    // Initialize m_queryEngine with the provided queryEngine
    // Initialize m_queries as an empty unordered_map
}

std::shared_ptr<Query> PowerQuery::CreateQuery(const std::string& queryName, const DataSource& dataSource) {
    // Validate the query name using ValidateQueryName
    if (!ValidateQueryName(queryName, m_queries)) {
        return nullptr;
    }

    // Create a new Query object with the given name and data source
    auto query = std::make_shared<Query>(queryName, dataSource);

    // Add the query to m_queries
    m_queries[queryName] = query;

    // Return the created query object
    return query;
}

bool PowerQuery::AddTransformationStep(const std::string& queryName, const TransformationStep& step) {
    // Find the query in m_queries using queryName
    auto it = m_queries.find(queryName);
    if (it == m_queries.end()) {
        // If not found, return false
        return false;
    }

    // Add the transformation step to the query's list of steps
    it->second->AddTransformationStep(step);

    // Return true
    return true;
}

QueryResult PowerQuery::ExecuteQuery(const std::string& queryName) {
    // Find the query in m_queries using queryName
    auto it = m_queries.find(queryName);
    if (it == m_queries.end()) {
        // If not found, return an error result
        return QueryResult::CreateErrorResult("Query not found");
    }

    // Call m_queryEngine->ExecuteQuery with the found query
    return m_queryEngine->ExecuteQuery(*(it->second));
}

bool PowerQuery::LoadQueryResultToWorksheet(const std::string& queryName, std::shared_ptr<Worksheet> worksheet, const CellReference& startCell) {
    // Execute the query using ExecuteQuery
    QueryResult result = ExecuteQuery(queryName);

    // If the query execution failed, return false
    if (!result.IsSuccess()) {
        return false;
    }

    // Write the query result data to the worksheet starting from startCell
    CellReference currentCell = startCell;
    for (const auto& row : result.GetData()) {
        for (const auto& cellValue : row) {
            worksheet->SetCellValue(currentCell, cellValue);
            currentCell = currentCell.OffsetColumn(1);
        }
        currentCell = CellReference(currentCell.GetRow() + 1, startCell.GetColumn());
    }

    // Apply any formatting specified in the query result
    // (This step would require additional implementation details)

    // Return true if the data was loaded successfully
    return true;
}

bool PowerQuery::DeleteQuery(const std::string& queryName) {
    // Find the query in m_queries using queryName
    auto it = m_queries.find(queryName);
    if (it == m_queries.end()) {
        // If not found, return false
        return false;
    }

    // Remove the query from m_queries
    m_queries.erase(it);

    // Return true
    return true;
}