import SwiftUI
import ExcelCore

struct CellEditor: View {
    // Binding to the cell being edited
    @Binding var cell: Cell
    // Binding to control the editing state
    @Binding var isEditing: Bool
    // Access to the Excel core functionality
    @EnvironmentObject var excelCore: ExcelCore
    
    var body: some View {
        VStack {
            // Text field for entering cell value
            TextField("Enter cell value", text: $cell.value)
                .textFieldStyle(RoundedBorderTextFieldStyle())
                .keyboardType(.default)
                .autocapitalization(.none)
                .disableAutocorrection(true)
            
            HStack {
                // Cancel button to exit editing mode
                Button("Cancel") {
                    isEditing = false
                }
                
                // Done button to save changes and exit editing mode
                Button("Done") {
                    saveCell()
                    isEditing = false
                }
            }
        }
    }
    
    // Function to save the edited cell content
    private func saveCell() {
        excelCore.updateCell(cell: cell)
        // Additional logic for formula parsing or validation could be added here
    }
}

// MARK: - Human Tasks
// TODO: Implement formula parsing in the saveCell method
// TODO: Add data validation for cell input
// TODO: Implement undo/redo functionality for cell edits
// TODO: Add support for different cell data types (number, date, etc.)