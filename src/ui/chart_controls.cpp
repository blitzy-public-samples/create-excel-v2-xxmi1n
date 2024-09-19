#include <vector>
#include <memory>
#include <string>
#include "excel_types.h"
#include "event_handler.h"
#include "chart_controls.h"
#include "ui_framework.h"
#include "visualization_engine.h"

// Define supported chart types
const std::vector<ChartType> SUPPORTED_CHART_TYPES = {ChartType::Column, ChartType::Bar, ChartType::Line, ChartType::Pie, ChartType::Scatter};

ChartControls::ChartControls() : 
    m_controlsContainer(UIElement()),
    m_createChartButton(Button()),
    m_chartTypeDropdown(Dropdown()),
    m_editDataRangeButton(Button()),
    m_chartColorPicker(ColorPicker())
{
    // Set up layout for chart control components
    // TODO: Implement layout setup

    // Populate m_chartTypeDropdown with SUPPORTED_CHART_TYPES
    for (const auto& chartType : SUPPORTED_CHART_TYPES) {
        m_chartTypeDropdown.AddItem(ChartTypeToString(chartType));
    }
}

void ChartControls::Initialize(std::shared_ptr<Workbook> workbook, std::shared_ptr<EventHandler> eventHandler, std::shared_ptr<VisualizationEngine> visualizationEngine)
{
    m_workbook = workbook;
    m_eventHandler = eventHandler;
    m_visualizationEngine = visualizationEngine;

    // Register necessary event listeners with m_eventHandler
    m_eventHandler->RegisterListener("ChartCreated", [this](const Event& e) { /* Handle chart creation */ });
    m_eventHandler->RegisterListener("ChartUpdated", [this](const Event& e) { /* Handle chart update */ });

    // Set up event handlers for UI components
    m_createChartButton.SetOnClickHandler([this]() { CreateChart(m_chartTypeDropdown.GetSelectedValue(), GetSelectedRange()); });
    m_chartTypeDropdown.SetOnChangeHandler([this](ChartType newType) { HandleChartTypeChange(newType); });
    m_editDataRangeButton.SetOnClickHandler([this]() { HandleDataRangeEdit(); });
    m_chartColorPicker.SetOnColorChangeHandler([this](const Color& newColor) { HandleColorChange(newColor); });
}

std::shared_ptr<Chart> ChartControls::CreateChart(ChartType chartType, const CellRange& dataRange)
{
    // Validate the selected data range
    if (!ValidateDataRange(dataRange)) {
        // TODO: Show error message to user
        return nullptr;
    }

    // Call m_visualizationEngine->CreateChart with chartType and dataRange
    auto newChart = m_visualizationEngine->CreateChart(chartType, dataRange);

    // Add the new chart to the active worksheet
    m_workbook->GetActiveWorksheet()->AddChart(newChart);

    // Set m_activeChart to the newly created chart
    m_activeChart = newChart;

    // Notify m_eventHandler of the new chart creation
    m_eventHandler->TriggerEvent("ChartCreated", {{"chartId", newChart->GetId()}});

    return newChart;
}

void ChartControls::UpdateChartProperties(const ChartProperties& properties)
{
    if (!m_activeChart) {
        // TODO: Show error message to user
        return;
    }

    // Call m_visualizationEngine->UpdateChartProperties with m_activeChart and properties
    m_visualizationEngine->UpdateChartProperties(m_activeChart, properties);

    // Refresh the chart display
    m_activeChart->Refresh();

    // Notify m_eventHandler of the chart property update
    m_eventHandler->TriggerEvent("ChartUpdated", {{"chartId", m_activeChart->GetId()}});
}

void ChartControls::HandleChartTypeChange(ChartType newType)
{
    ChartProperties properties;
    properties.type = newType;
    UpdateChartProperties(properties);
}

void ChartControls::HandleDataRangeEdit()
{
    // Open a dialog for the user to select a new data range
    CellRange newRange = OpenDataRangeSelectionDialog();

    // Validate the selected range
    if (!ValidateDataRange(newRange)) {
        // TODO: Show error message to user
        return;
    }

    // Update the chart's data range
    m_activeChart->SetDataRange(newRange);

    // Refresh the chart display
    m_activeChart->Refresh();

    // Notify m_eventHandler of the data range change
    m_eventHandler->TriggerEvent("ChartDataRangeChanged", {{"chartId", m_activeChart->GetId()}, {"newRange", newRange.ToString()}});
}

void ChartControls::HandleColorChange(const Color& newColor)
{
    ChartProperties properties;
    properties.color = newColor;
    UpdateChartProperties(properties);
}

void ChartControls::SetActiveChart(std::shared_ptr<Chart> chart)
{
    m_activeChart = chart;

    // Update UI controls to reflect the active chart's properties
    if (chart) {
        m_chartTypeDropdown.SetSelectedValue(chart->GetType());
        m_chartColorPicker.SetColor(chart->GetColor());
        // TODO: Update other UI controls as needed
    }

    // Enable or disable controls based on whether a chart is active
    bool hasActiveChart = (chart != nullptr);
    m_chartTypeDropdown.SetEnabled(hasActiveChart);
    m_editDataRangeButton.SetEnabled(hasActiveChart);
    m_chartColorPicker.SetEnabled(hasActiveChart);
}

bool ValidateDataRange(const CellRange& range)
{
    // Check if the range is empty
    if (range.IsEmpty()) {
        return false;
    }

    // Check if the range is contiguous
    if (!range.IsContiguous()) {
        return false;
    }

    // Check if the range contains at least some numeric data
    bool hasNumericData = false;
    for (const auto& cell : range.GetCells()) {
        if (cell.IsNumeric()) {
            hasNumericData = true;
            break;
        }
    }

    return hasNumericData;
}

// TODO: Implement helper functions like ChartTypeToString, GetSelectedRange, OpenDataRangeSelectionDialog