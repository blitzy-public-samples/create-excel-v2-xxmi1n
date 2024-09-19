#include <vector>
#include <unordered_map>
#include <string>
#include <memory>
#include "excel_types.h"
#include "calculation_engine.h"
#include "file_system.h"
#include "cloud_storage.h"

// Global constants
constexpr int MAX_WORKSHEETS = 1024;
constexpr int MAX_ROWS = 1048576;
constexpr int MAX_COLUMNS = 16384;

class DataManager {
public:
    DataManager(std::shared_ptr<CalculationEngine> calculationEngine,
                std::shared_ptr<FileSystem> fileSystem,
                std::shared_ptr<CloudStorage> cloudStorage)
        : m_calculationEngine(calculationEngine),
          m_fileSystem(fileSystem),
          m_cloudStorage(cloudStorage) {
        // Initialize m_workbooks as an empty map
    }

    std::shared_ptr<Workbook> CreateWorkbook(const std::string& name) {
        // Check if a workbook with the given name already exists
        if (m_workbooks.find(name) != m_workbooks.end()) {
            return nullptr; // Or throw an exception
        }

        // Create a new Workbook object
        auto workbook = std::make_shared<Workbook>(name);

        // Add the new workbook to m_workbooks
        m_workbooks[name] = workbook;

        // Return the pointer to the new workbook
        return workbook;
    }

    std::shared_ptr<Workbook> OpenWorkbook(const std::string& path, bool isCloudStorage = false) {
        // Check if the workbook is already open in m_workbooks
        auto it = m_workbooks.find(path);
        if (it != m_workbooks.end()) {
            return it->second;
        }

        std::vector<uint8_t> fileContent;
        if (isCloudStorage) {
            // Download the file using m_cloudStorage
            fileContent = m_cloudStorage->DownloadFile(path);
        } else {
            // Read the file using m_fileSystem
            fileContent = m_fileSystem->ReadFile(path);
        }

        // Deserialize the file content into a Workbook object
        auto workbook = DeserializeWorkbook(fileContent);

        // Add the workbook to m_workbooks
        m_workbooks[path] = workbook;

        // Return the pointer to the opened workbook
        return workbook;
    }

    bool SaveWorkbook(const std::shared_ptr<Workbook>& workbook, const std::string& path, bool isCloudStorage = false) {
        // Serialize the workbook into a file format
        std::vector<uint8_t> serializedData = SerializeWorkbook(workbook);

        bool success = false;
        if (isCloudStorage) {
            // Upload the file using m_cloudStorage
            success = m_cloudStorage->UploadFile(path, serializedData);
        } else {
            // Write the file using m_fileSystem
            success = m_fileSystem->WriteFile(path, serializedData);
        }

        // Return true if the save operation was successful, false otherwise
        return success;
    }

    CellValue GetCellValue(const std::shared_ptr<Workbook>& workbook, const std::string& worksheetName, const CellReference& cellRef) {
        // Get the specified worksheet from the workbook
        auto worksheet = workbook->GetWorksheet(worksheetName);
        if (!worksheet) {
            return CellValue(); // Return empty value if worksheet not found
        }

        // Get the cell from the worksheet using the cell reference
        auto cell = worksheet->GetCell(cellRef);
        if (!cell) {
            return CellValue(); // Return empty value if cell not found
        }

        // Return the value of the cell
        return cell->GetValue();
    }

    void SetCellValue(const std::shared_ptr<Workbook>& workbook, const std::string& worksheetName, const CellReference& cellRef, const CellValue& value) {
        // Get the specified worksheet from the workbook
        auto worksheet = workbook->GetWorksheet(worksheetName);
        if (!worksheet) {
            return; // Exit if worksheet not found
        }

        // Set the cell value in the worksheet
        worksheet->SetCell(cellRef, value);

        // Notify the calculation engine of the cell update
        m_calculationEngine->UpdateCell(workbook, worksheetName, cellRef, value);

        // Trigger recalculation of dependent cells
        m_calculationEngine->Recalculate(workbook);
    }

private:
    std::vector<uint8_t> SerializeWorkbook(const std::shared_ptr<Workbook>& workbook) {
        std::vector<uint8_t> buffer;

        // Write workbook metadata (name, author, creation date, etc.)
        // TODO: Implement workbook metadata serialization

        // For each worksheet in the workbook:
        for (const auto& worksheet : workbook->GetWorksheets()) {
            // Write worksheet metadata (name, index, etc.)
            // TODO: Implement worksheet metadata serialization

            // Write cell data (including values, formulas, and styles)
            // TODO: Implement cell data serialization
        }

        // Write additional workbook data (named ranges, shared formulas, etc.)
        // TODO: Implement additional workbook data serialization

        return buffer;
    }

    std::shared_ptr<Workbook> DeserializeWorkbook(const std::vector<uint8_t>& data) {
        // Create a new Workbook object
        auto workbook = std::make_shared<Workbook>();

        // Read and set workbook metadata
        // TODO: Implement workbook metadata deserialization

        // While there is data remaining:
        // TODO: Implement data parsing loop
        {
            // Read worksheet metadata
            // TODO: Implement worksheet metadata deserialization

            // Create a new Worksheet object
            auto worksheet = std::make_shared<Worksheet>();

            // Read and set cell data for the worksheet
            // TODO: Implement cell data deserialization

            // Add the worksheet to the workbook
            workbook->AddWorksheet(worksheet);
        }

        // Read and set additional workbook data
        // TODO: Implement additional workbook data deserialization

        return workbook;
    }

    std::unordered_map<std::string, std::shared_ptr<Workbook>> m_workbooks;
    std::shared_ptr<CalculationEngine> m_calculationEngine;
    std::shared_ptr<FileSystem> m_fileSystem;
    std::shared_ptr<CloudStorage> m_cloudStorage;
};

// Human tasks:
// TODO: Implement workbook metadata serialization in SerializeWorkbook
// TODO: Implement worksheet metadata serialization in SerializeWorkbook
// TODO: Implement cell data serialization in SerializeWorkbook
// TODO: Implement additional workbook data serialization in SerializeWorkbook
// TODO: Implement workbook metadata deserialization in DeserializeWorkbook
// TODO: Implement data parsing loop in DeserializeWorkbook
// TODO: Implement worksheet metadata deserialization in DeserializeWorkbook
// TODO: Implement cell data deserialization in DeserializeWorkbook
// TODO: Implement additional workbook data deserialization in DeserializeWorkbook