#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include "excel_types.h"
#include "color.h"
#include "font.h"
#include "border.h"

// Maximum number of styles allowed
const int MAX_STYLE_COUNT = 64000;

class FormattingEngine {
private:
    std::unordered_map<std::string, CellStyle> m_styleLibrary;
    std::unordered_map<std::string, std::shared_ptr<Workbook>> m_workbooks;

public:
    FormattingEngine() {
        // Initialize m_styleLibrary with default styles
        m_styleLibrary["Normal"] = CellStyle();
        m_styleLibrary["Heading 1"] = CellStyle(); // TODO: Set appropriate properties for Heading 1
        m_styleLibrary["Currency"] = CellStyle(); // TODO: Set appropriate properties for Currency

        // Initialize m_workbooks as an empty map
        // (No action needed as the map is already empty when created)
    }

    bool ApplyStyle(const std::shared_ptr<Workbook>& workbook, const std::string& worksheetName, const CellRange& range, const std::string& styleName) {
        // Check if the style exists in m_styleLibrary
        if (m_styleLibrary.find(styleName) == m_styleLibrary.end()) {
            return false;
        }

        // Get the specified worksheet from the workbook
        auto worksheet = workbook->GetWorksheet(worksheetName);
        if (!worksheet) {
            return false;
        }

        // Iterate through the cells in the given range
        for (int row = range.startRow; row <= range.endRow; ++row) {
            for (int col = range.startCol; col <= range.endCol; ++col) {
                // For each cell, call Worksheet::SetCellStyle with the specified style
                if (!worksheet->SetCellStyle(row, col, m_styleLibrary[styleName])) {
                    return false;
                }
            }
        }

        // Return true if all cells were styled successfully
        return true;
    }

    bool CreateStyle(const std::string& styleName, const CellStyle& style) {
        // Check if a style with the given name already exists
        if (m_styleLibrary.find(styleName) != m_styleLibrary.end()) {
            return false;
        }

        // Check if MAX_STYLE_COUNT has been reached
        if (m_styleLibrary.size() >= MAX_STYLE_COUNT) {
            return false;
        }

        // Add the new style to m_styleLibrary
        m_styleLibrary[styleName] = style;
        return true;
    }

    bool ModifyStyle(const std::string& styleName, const CellStyle& newStyle) {
        // Check if the style exists in m_styleLibrary
        if (m_styleLibrary.find(styleName) == m_styleLibrary.end()) {
            return false;
        }

        // Update the existing style with the new style properties
        m_styleLibrary[styleName] = newStyle;
        return true;
    }

    CellStyle GetStyle(const std::string& styleName) {
        // Check if the style exists in m_styleLibrary
        auto it = m_styleLibrary.find(styleName);
        if (it != m_styleLibrary.end()) {
            return it->second;
        }

        // If it doesn't exist, return a default style
        return CellStyle();
    }

    bool SetCellFormat(const std::shared_ptr<Workbook>& workbook, const std::string& worksheetName, const CellRange& range, const CellStyle& formatProperties) {
        // Get the specified worksheet from the workbook
        auto worksheet = workbook->GetWorksheet(worksheetName);
        if (!worksheet) {
            return false;
        }

        // Iterate through the cells in the given range
        for (int row = range.startRow; row <= range.endRow; ++row) {
            for (int col = range.startCol; col <= range.endCol; ++col) {
                // For each cell, call Worksheet::SetCellFormat with the specified format properties
                if (!worksheet->SetCellFormat(row, col, formatProperties)) {
                    return false;
                }
            }
        }

        // Return true if all cells were formatted successfully
        return true;
    }

    CellStyle GetCellFormat(const std::shared_ptr<Workbook>& workbook, const std::string& worksheetName, const CellReference& cellRef) {
        // Get the specified worksheet from the workbook
        auto worksheet = workbook->GetWorksheet(worksheetName);
        if (!worksheet) {
            return CellStyle(); // Return default style if worksheet not found
        }

        // Call Worksheet::GetCellFormat with the specified cell reference
        return worksheet->GetCellFormat(cellRef);
    }
};