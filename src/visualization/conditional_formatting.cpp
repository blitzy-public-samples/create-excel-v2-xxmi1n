#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "excel_types.h"
#include "data_manager.h"
#include "calculation_engine.h"
#include "conditional_formatting.h"

const int MAX_CONDITIONAL_FORMATS_PER_RANGE = 3;

ConditionalFormatting::ConditionalFormatting(std::shared_ptr<DataManager> dataManager, std::shared_ptr<CalculationEngine> calculationEngine)
    : m_dataManager(dataManager), m_calculationEngine(calculationEngine) {
    // Initialize m_worksheetRules as an empty unordered_map
    m_worksheetRules = std::unordered_map<std::string, std::vector<std::shared_ptr<ConditionalFormatRule>>>();
}

std::shared_ptr<ConditionalFormatRule> ConditionalFormatting::CreateRule(const std::string& worksheetName, const CellRange& range, const FormatCondition& condition, const CellStyle& style) {
    // Create a new ConditionalFormatRule object
    auto rule = std::make_shared<ConditionalFormatRule>();
    
    // Set the rule's range, condition, and style
    rule->range = range;
    rule->condition = condition;
    rule->style = style;
    
    // Validate the rule
    if (!ValidateRule(*rule)) {
        return nullptr;
    }
    
    // Add the rule to m_worksheetRules for the specified worksheet
    m_worksheetRules[worksheetName].push_back(rule);
    
    return rule;
}

bool ConditionalFormatting::DeleteRule(const std::string& worksheetName, std::shared_ptr<ConditionalFormatRule> rule) {
    // Find the worksheet rules in m_worksheetRules
    auto it = m_worksheetRules.find(worksheetName);
    if (it == m_worksheetRules.end()) {
        return false;
    }
    
    // Find and remove the specified rule from the worksheet rules
    auto& rules = it->second;
    auto ruleIt = std::find(rules.begin(), rules.end(), rule);
    if (ruleIt != rules.end()) {
        rules.erase(ruleIt);
        return true;
    }
    
    return false;
}

bool ConditionalFormatting::ModifyRule(std::shared_ptr<ConditionalFormatRule> rule, const FormatCondition& newCondition, const CellStyle& newStyle) {
    // Store the original condition and style
    auto originalCondition = rule->condition;
    auto originalStyle = rule->style;
    
    // Update the rule's condition and style
    rule->condition = newCondition;
    rule->style = newStyle;
    
    // Validate the modified rule
    if (!ValidateRule(*rule)) {
        // If invalid, revert changes
        rule->condition = originalCondition;
        rule->style = originalStyle;
        return false;
    }
    
    return true;
}

void ConditionalFormatting::EvaluateRules(const std::string& worksheetName) {
    // Find the worksheet rules in m_worksheetRules
    auto it = m_worksheetRules.find(worksheetName);
    if (it == m_worksheetRules.end()) {
        return;
    }
    
    // For each rule in the worksheet rules, evaluate and apply it
    for (const auto& rule : it->second) {
        EvaluateRule(*rule);
    }
    
    // TODO: Notify the UI to update the affected cells
}

std::vector<std::shared_ptr<ConditionalFormatRule>> ConditionalFormatting::GetRulesForCell(const std::string& worksheetName, const CellReference& cellRef) {
    std::vector<std::shared_ptr<ConditionalFormatRule>> applicableRules;
    
    // Find the worksheet rules in m_worksheetRules
    auto it = m_worksheetRules.find(worksheetName);
    if (it == m_worksheetRules.end()) {
        return applicableRules;
    }
    
    // For each rule in the worksheet rules, check if it applies to the cell
    for (const auto& rule : it->second) {
        if (rule->range.Contains(cellRef)) {
            applicableRules.push_back(rule);
        }
    }
    
    return applicableRules;
}

bool ValidateRule(const ConditionalFormatRule& rule) {
    // Check if the rule's range is valid
    if (!rule.range.IsValid()) {
        return false;
    }
    
    // Validate the rule's condition based on its type
    switch (rule.condition.GetType()) {
        case FormatCondition::Type::CellValue:
            // TODO: Implement validation for CellValueCondition
            break;
        case FormatCondition::Type::Formula:
            // TODO: Implement validation for FormulaCondition
            break;
        case FormatCondition::Type::ColorScale:
            // TODO: Implement validation for ColorScaleCondition
            break;
        default:
            return false;
    }
    
    // Ensure the rule's style is valid and contains at least one formatting property
    if (rule.style.IsEmpty()) {
        return false;
    }
    
    // Check if adding this rule would exceed MAX_CONDITIONAL_FORMATS_PER_RANGE for any cell in the range
    // TODO: Implement this check
    
    return true;
}

void EvaluateRule(const ConditionalFormatRule& rule) {
    // Get the cells in the rule's range from m_dataManager
    auto cells = m_dataManager->GetCellsInRange(rule.range);
    
    for (const auto& cell : cells) {
        bool conditionMet = false;
        
        // Evaluate the rule's condition for the cell
        switch (rule.condition.GetType()) {
            case FormatCondition::Type::CellValue:
                // TODO: Implement evaluation for CellValueCondition
                break;
            case FormatCondition::Type::Formula:
                {
                    auto formulaCondition = static_cast<const FormulaCondition&>(rule.condition);
                    auto result = m_calculationEngine->EvaluateFormula(formulaCondition.GetFormula(), cell.GetReference());
                    conditionMet = result.GetBoolean();
                }
                break;
            case FormatCondition::Type::ColorScale:
                // TODO: Implement evaluation for ColorScaleCondition
                break;
        }
        
        if (conditionMet) {
            // Apply the rule's style to the cell
            m_dataManager->SetCellStyle(cell.GetReference(), rule.style);
        } else {
            // Remove the rule's style from the cell if previously applied
            // TODO: Implement removal of specific rule style
        }
    }
}

// Human tasks:
// TODO: Implement UI notification mechanism for cell updates
// TODO: Implement validation for CellValueCondition
// TODO: Implement validation for FormulaCondition
// TODO: Implement validation for ColorScaleCondition
// TODO: Implement check for MAX_CONDITIONAL_FORMATS_PER_RANGE
// TODO: Implement evaluation for CellValueCondition
// TODO: Implement evaluation for ColorScaleCondition
// TODO: Implement removal of specific rule style from a cell