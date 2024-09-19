#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <any>
#include "excel_types.h"
#include "data_manager.h"
#include "rendering_engine.h"
#include "charting_engine.h"

// Global constants
const int MAX_SERIES_PER_CHART = 255;
const int MAX_DATA_POINTS_PER_SERIES = 32000;

// ChartingEngine class implementation
ChartingEngine::ChartingEngine(std::shared_ptr<DataManager> dataManager, std::shared_ptr<RenderingEngine> renderingEngine)
    : m_dataManager(dataManager), m_renderingEngine(renderingEngine), m_charts() {}

std::shared_ptr<Chart> ChartingEngine::CreateChart(ChartType chartType, const CellRange& dataRange, const std::string& chartName) {
    // Validate the data range
    if (!ValidateDataRange(dataRange)) {
        throw std::invalid_argument("Invalid data range for chart creation");
    }

    // Create a new Chart object
    auto chart = std::make_shared<Chart>(chartType, chartName);

    // Create chart series
    auto series = CreateChartSeries(dataRange, chartType);
    for (const auto& s : series) {
        chart->AddSeries(s);
    }

    // Set up default chart properties
    chart->SetAxis(ChartAxis::X, std::make_shared<ChartAxis>("X Axis"));
    chart->SetAxis(ChartAxis::Y, std::make_shared<ChartAxis>("Y Axis"));
    chart->SetLegend(std::make_shared<ChartLegend>());
    chart->SetTitle(chartName);

    // Add the chart to m_charts
    m_charts[chartName] = chart;

    return chart;
}

bool ChartingEngine::UpdateChartData(const std::string& chartName, const CellRange& newDataRange) {
    auto it = m_charts.find(chartName);
    if (it == m_charts.end()) {
        return false;
    }

    if (!ValidateDataRange(newDataRange)) {
        return false;
    }

    auto chart = it->second;
    UpdateChartSeries(chart, newDataRange);

    // Update chart axes and other properties as needed
    chart->UpdateAxesRanges();

    return true;
}

void ChartingEngine::RenderChart(const std::string& chartName, RenderTarget target) {
    auto it = m_charts.find(chartName);
    if (it == m_charts.end()) {
        throw std::runtime_error("Chart not found: " + chartName);
    }

    m_renderingEngine->RenderChart(it->second, target);
    it->second->UpdateLastRenderTime();
}

bool ChartingEngine::SetChartProperty(const std::string& chartName, ChartProperty property, const std::any& value) {
    auto it = m_charts.find(chartName);
    if (it == m_charts.end()) {
        return false;
    }

    return it->second->SetProperty(property, value);
}

bool ChartingEngine::DeleteChart(const std::string& chartName) {
    auto it = m_charts.find(chartName);
    if (it == m_charts.end()) {
        return false;
    }

    // Remove the chart from m_charts
    m_charts.erase(it);

    // Clean up any resources associated with the chart
    // (assuming Chart destructor handles resource cleanup)

    return true;
}

// Helper functions
bool ValidateDataRange(const CellRange& dataRange) {
    // Check if the data range is empty
    if (dataRange.IsEmpty()) {
        return false;
    }

    // Verify that the data range contains at least one row or column of data
    if (dataRange.GetRowCount() < 2 && dataRange.GetColumnCount() < 2) {
        return false;
    }

    // Ensure the number of data points doesn't exceed MAX_DATA_POINTS_PER_SERIES
    int dataPoints = std::max(dataRange.GetRowCount(), dataRange.GetColumnCount()) - 1;
    if (dataPoints > MAX_DATA_POINTS_PER_SERIES) {
        return false;
    }

    // Check for any non-numeric data in the range (excluding headers)
    for (int row = 1; row < dataRange.GetRowCount(); ++row) {
        for (int col = 1; col < dataRange.GetColumnCount(); ++col) {
            const CellValue& value = dataRange.GetCell(row, col);
            if (!value.IsNumeric()) {
                return false;
            }
        }
    }

    return true;
}

std::vector<ChartSeries> CreateChartSeries(const CellRange& dataRange, ChartType chartType) {
    std::vector<ChartSeries> series;

    // Determine if the data is in rows or columns
    bool dataInRows = dataRange.GetRowCount() > dataRange.GetColumnCount();

    // Extract series names from the first row or column if present
    std::vector<std::string> seriesNames;
    if (dataInRows) {
        for (int col = 1; col < dataRange.GetColumnCount(); ++col) {
            seriesNames.push_back(dataRange.GetCell(0, col).ToString());
        }
    } else {
        for (int row = 1; row < dataRange.GetRowCount(); ++row) {
            seriesNames.push_back(dataRange.GetCell(row, 0).ToString());
        }
    }

    // Create ChartSeries objects for each data series
    for (size_t i = 0; i < seriesNames.size(); ++i) {
        ChartSeries chartSeries(seriesNames[i]);

        // Populate ChartSeries objects with data points
        if (dataInRows) {
            for (int row = 1; row < dataRange.GetRowCount(); ++row) {
                chartSeries.AddDataPoint(dataRange.GetCell(row, 0).ToDouble(), dataRange.GetCell(row, i + 1).ToDouble());
            }
        } else {
            for (int col = 1; col < dataRange.GetColumnCount(); ++col) {
                chartSeries.AddDataPoint(dataRange.GetCell(0, col).ToDouble(), dataRange.GetCell(i + 1, col).ToDouble());
            }
        }

        // Set default formatting for each series based on chart type
        chartSeries.SetDefaultFormatting(chartType);

        series.push_back(std::move(chartSeries));
    }

    return series;
}

void UpdateChartSeries(std::shared_ptr<Chart> chart, const CellRange& newDataRange) {
    // Create new chart series
    auto newSeries = CreateChartSeries(newDataRange, chart->GetChartType());

    // Compare new series with existing ones in the chart
    auto& existingSeries = chart->GetSeries();
    std::vector<ChartSeries> updatedSeries;

    for (const auto& newS : newSeries) {
        auto it = std::find_if(existingSeries.begin(), existingSeries.end(),
            [&](const ChartSeries& s) { return s.GetName() == newS.GetName(); });

        if (it != existingSeries.end()) {
            // Update existing series with new data
            it->UpdateData(newS.GetDataPoints());
            updatedSeries.push_back(*it);
        } else {
            // Add new series that don't exist in the current chart
            updatedSeries.push_back(newS);
        }
    }

    // Remove series from the chart that are not in the new data range
    chart->SetSeries(updatedSeries);

    // Update chart axes ranges based on the new data
    chart->UpdateAxesRanges();
}