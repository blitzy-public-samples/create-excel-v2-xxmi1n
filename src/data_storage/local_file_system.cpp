#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <filesystem>
#include "excel_types.h"
#include "local_file_system.h"
#include "file_io_manager.h"
#include "workbook_serializer.h"
#include "error_handler.h"

const std::string FILE_EXTENSION = ".xlsx";

// Helper function to validate file paths
bool ValidateFilePath(const std::string& filePath) {
    // Check if the file path is not empty
    if (filePath.empty()) {
        return false;
    }

    // Verify that the file has the correct extension (.xlsx)
    if (std::filesystem::path(filePath).extension() != FILE_EXTENSION) {
        return false;
    }

    // Check if the directory exists
    if (!std::filesystem::exists(std::filesystem::path(filePath).parent_path())) {
        return false;
    }

    // Ensure the file name is valid (no illegal characters)
    const std::string illegalChars = "\\/:*?\"<>|";
    if (filePath.find_first_of(illegalChars) != std::string::npos) {
        return false;
    }

    return true;
}

LocalFileSystem::LocalFileSystem(std::shared_ptr<FileIOManager> fileIOManager,
                                 std::shared_ptr<WorkbookSerializer> workbookSerializer,
                                 std::shared_ptr<ErrorHandler> errorHandler)
    : m_fileIOManager(fileIOManager),
      m_workbookSerializer(workbookSerializer),
      m_errorHandler(errorHandler) {}

bool LocalFileSystem::SaveWorkbook(const std::shared_ptr<Workbook>& workbook, const std::string& filePath) {
    // Validate the file path
    if (!ValidateFilePath(filePath)) {
        m_errorHandler->HandleError("Invalid file path for saving workbook");
        return false;
    }

    try {
        // Serialize the workbook
        std::vector<uint8_t> serializedData = m_workbookSerializer->Serialize(workbook);

        // Write the serialized data to the file
        if (!m_fileIOManager->WriteFile(filePath, serializedData)) {
            m_errorHandler->HandleError("Failed to write workbook to file");
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        m_errorHandler->HandleError("Error saving workbook: " + std::string(e.what()));
        return false;
    }
}

std::shared_ptr<Workbook> LocalFileSystem::LoadWorkbook(const std::string& filePath) {
    // Validate the file path
    if (!ValidateFilePath(filePath)) {
        m_errorHandler->HandleError("Invalid file path for loading workbook");
        return nullptr;
    }

    try {
        // Read the file contents
        std::vector<uint8_t> fileContents = m_fileIOManager->ReadFile(filePath);

        // Deserialize the file contents into a Workbook object
        return m_workbookSerializer->Deserialize(fileContents);
    } catch (const std::exception& e) {
        m_errorHandler->HandleError("Error loading workbook: " + std::string(e.what()));
        return nullptr;
    }
}

bool LocalFileSystem::DeleteWorkbook(const std::string& filePath) {
    // Validate the file path
    if (!ValidateFilePath(filePath)) {
        m_errorHandler->HandleError("Invalid file path for deleting workbook");
        return false;
    }

    try {
        // Delete the file
        if (!m_fileIOManager->DeleteFile(filePath)) {
            m_errorHandler->HandleError("Failed to delete workbook file");
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        m_errorHandler->HandleError("Error deleting workbook: " + std::string(e.what()));
        return false;
    }
}

std::vector<std::string> LocalFileSystem::ListWorkbooks(const std::string& directoryPath) {
    // Validate the directory path
    if (!std::filesystem::exists(directoryPath) || !std::filesystem::is_directory(directoryPath)) {
        m_errorHandler->HandleError("Invalid directory path for listing workbooks");
        return {};
    }

    try {
        // List all files in the directory
        std::vector<std::string> allFiles = m_fileIOManager->ListFiles(directoryPath);

        // Filter the list to include only files with the .xlsx extension
        std::vector<std::string> workbooks;
        for (const auto& file : allFiles) {
            if (std::filesystem::path(file).extension() == FILE_EXTENSION) {
                workbooks.push_back(file);
            }
        }

        return workbooks;
    } catch (const std::exception& e) {
        m_errorHandler->HandleError("Error listing workbooks: " + std::string(e.what()));
        return {};
    }
}