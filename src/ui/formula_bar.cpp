#include <string>
#include <memory>
#include "excel_types.h"
#include "event_handler.h"
#include "formula_bar.h"
#include "ui_framework.h"
#include "calculation_engine.h"

FormulaBar::FormulaBar() {
    // Initialize UI components
    m_formulaBarContainer = UIElement();
    m_formulaInput = TextBox();
    m_cancelButton = Button();
    m_acceptButton = Button();

    // Set up layout for formula bar components
    // TODO: Implement layout setup
}

void FormulaBar::Initialize(std::shared_ptr<Workbook> workbook,
                            std::shared_ptr<EventHandler> eventHandler,
                            std::shared_ptr<CalculationEngine> calculationEngine) {
    // Set member variables
    m_workbook = workbook;
    m_eventHandler = eventHandler;
    m_calculationEngine = calculationEngine;

    // Register necessary event listeners with m_eventHandler
    // TODO: Implement event listener registration

    // Set up event handlers for m_formulaInput, m_cancelButton, and m_acceptButton
    m_formulaInput.SetOnChangeHandler([this](const std::string& input) {
        HandleFormulaInput(input);
    });

    m_cancelButton.SetOnClickHandler([this]() {
        HandleCancelEdit();
    });

    m_acceptButton.SetOnClickHandler([this]() {
        HandleAcceptEdit();
    });
}

void FormulaBar::UpdateFormulaBar(const CellReference& cellRef) {
    m_activeCell = cellRef;
    std::string cellContent = GetCellContent(cellRef);
    m_formulaInput.SetText(cellContent);

    // Update the name box with the current cell reference
    // TODO: Implement name box update
}

void FormulaBar::HandleFormulaInput(const std::string& input) {
    if (ValidateFormula(input)) {
        // Update the cell content in m_workbook
        Worksheet& activeSheet = m_workbook->GetActiveWorksheet();
        activeSheet.SetCellContent(m_activeCell, input);

        // Calculate the result
        CellValue result = m_calculationEngine->EvaluateFormula(input);

        // Update the cell display with the calculated result
        activeSheet.SetCellValue(m_activeCell, result);

        // Notify m_eventHandler of the cell content change
        m_eventHandler->NotifyCellContentChanged(m_activeCell, input, result);
    } else {
        // TODO: Handle invalid formula input (e.g., display error message)
    }
}

void FormulaBar::HandleCancelEdit() {
    // Revert m_formulaInput to the original cell content
    std::string originalContent = GetCellContent(m_activeCell);
    m_formulaInput.SetText(originalContent);

    // Refresh the display
    UpdateFormulaBar(m_activeCell);

    // Notify m_eventHandler of the edit cancellation
    m_eventHandler->NotifyFormulaEditCancelled(m_activeCell);
}

void FormulaBar::HandleAcceptEdit() {
    std::string currentContent = m_formulaInput.GetText();
    HandleFormulaInput(currentContent);

    // Clear focus from the formula bar
    m_formulaInput.ClearFocus();

    // Notify m_eventHandler of the edit completion
    m_eventHandler->NotifyFormulaEditCompleted(m_activeCell);
}

std::string GetCellContent(const CellReference& cellRef) {
    Worksheet& activeSheet = m_workbook->GetActiveWorksheet();
    CellValue cellValue = activeSheet.GetCellValue(cellRef);

    if (cellValue.IsFormula()) {
        return cellValue.GetFormulaString();
    } else {
        return cellValue.ToString();
    }
}

bool ValidateFormula(const std::string& formula) {
    if (formula.empty() || formula[0] != '=') {
        // Not a formula, consider it valid
        return true;
    }

    // Validate the formula using the calculation engine
    return m_calculationEngine->ValidateFormula(formula);
}

// Human tasks:
// 1. Implement layout setup in the FormulaBar constructor
// 2. Implement event listener registration in the Initialize method
// 3. Implement name box update in the UpdateFormulaBar method
// 4. Implement error handling for invalid formula input in HandleFormulaInput