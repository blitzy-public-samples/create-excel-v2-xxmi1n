#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "excel_types.h"
#include "data_manager.h"
#include "rendering_engine.h"
#include "sparklines.h"

// Maximum number of data points allowed in a sparkline
const int MAX_SPARKLINE_DATA_POINTS = 100;

// Validates the data range for a sparkline chart
bool ValidateSparklineData(const CellRange& dataRange) {
    // Check if the data range is empty
    if (dataRange.IsEmpty()) {
        return false;
    }

    // Verify that the data range is a single row or column
    if (!dataRange.IsSingleRowOrColumn()) {
        return false;
    }

    // Ensure the number of data points doesn't exceed MAX_SPARKLINE_DATA_POINTS
    if (dataRange.GetCellCount() > MAX_SPARKLINE_DATA_POINTS) {
        return false;
    }

    // Check for any non-numeric data in the range
    for (const auto& cell : dataRange.GetCells()) {
        if (!cell.GetValue().IsNumeric()) {
            return false;
        }
    }

    // All checks passed
    return true;
}

SparklineManager::SparklineManager(std::shared_ptr<DataManager> dataManager, std::shared_ptr<RenderingEngine> renderingEngine)
    : m_dataManager(dataManager), m_renderingEngine(renderingEngine) {
    // Initialize m_worksheetSparklines as an empty unordered_map
    m_worksheetSparklines = std::unordered_map<std::string, std::vector<std::shared_ptr<Sparkline>>>();
}

std::shared_ptr<Sparkline> SparklineManager::CreateSparkline(const std::string& worksheetName, SparklineType type, const CellRange& dataRange, const CellReference& location) {
    // Validate the data range
    if (!ValidateSparklineData(dataRange)) {
        return nullptr;
    }

    // Create a new Sparkline object with the specified type, data range, and location
    auto sparkline = std::make_shared<Sparkline>(type, dataRange, location);

    // Add the sparkline to m_worksheetSparklines for the specified worksheet
    m_worksheetSparklines[worksheetName].push_back(sparkline);

    // Return the created sparkline object
    return sparkline;
}

bool SparklineManager::UpdateSparkline(std::shared_ptr<Sparkline> sparkline, const CellRange& newDataRange) {
    // Validate the new data range
    if (!ValidateSparklineData(newDataRange)) {
        return false;
    }

    // Update the sparkline's data range
    sparkline->SetDataRange(newDataRange);

    // Mark the sparkline for re-rendering
    sparkline->SetNeedsRendering(true);

    // Update was successful
    return true;
}

bool SparklineManager::DeleteSparkline(const std::string& worksheetName, const CellReference& location) {
    // Find the worksheet sparklines
    auto it = m_worksheetSparklines.find(worksheetName);
    if (it == m_worksheetSparklines.end()) {
        return false;
    }

    // Find and remove the sparkline at the specified location
    auto& sparklines = it->second;
    auto sparklineIt = std::find_if(sparklines.begin(), sparklines.end(),
        [&location](const std::shared_ptr<Sparkline>& s) { return s->GetLocation() == location; });

    if (sparklineIt == sparklines.end()) {
        return false;
    }

    // Clear the cell content at the sparkline location
    m_dataManager->ClearCell(worksheetName, location);

    // Remove the sparkline from the vector
    sparklines.erase(sparklineIt);

    return true;
}

void SparklineManager::RenderSparkline(std::shared_ptr<Sparkline> sparkline) {
    // Retrieve the sparkline data from the data range
    std::vector<double> data;
    for (const auto& cell : sparkline->GetDataRange().GetCells()) {
        data.push_back(cell.GetValue().GetNumericValue());
    }

    // Call m_renderingEngine->RenderSparkline with the sparkline object and data
    std::string renderedSparkline = m_renderingEngine->RenderSparkline(sparkline, data);

    // Update the cell content at the sparkline location with the rendered result
    m_dataManager->SetCellValue(sparkline->GetWorksheetName(), sparkline->GetLocation(), CellValue(renderedSparkline));
}

void SparklineManager::RenderWorksheetSparklines(const std::string& worksheetName) {
    // Find the worksheet sparklines
    auto it = m_worksheetSparklines.find(worksheetName);
    if (it == m_worksheetSparklines.end()) {
        return;
    }

    // For each sparkline in the worksheet, call RenderSparkline to render it
    for (const auto& sparkline : it->second) {
        RenderSparkline(sparkline);
    }
}

void SparklineManager::SetSparklineStyle(std::shared_ptr<Sparkline> sparkline, const SparklineStyle& style) {
    // Update the sparkline's style properties
    sparkline->SetStyle(style);

    // Mark the sparkline for re-rendering
    sparkline->SetNeedsRendering(true);
}