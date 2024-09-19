#include <vector>
#include <memory>
#include <string>
#include <future>
#include "excel_types.h"
#include "onedrive_integration.h"
#include "onedrive_client.h"
#include "authentication_manager.h"
#include "workbook_serializer.h"
#include "error_handler.h"

const std::string ONEDRIVE_API_ENDPOINT = "https://graph.microsoft.com/v1.0/me/drive";

OneDriveIntegration::OneDriveIntegration(
    std::shared_ptr<OneDriveClient> oneDriveClient,
    std::shared_ptr<AuthenticationManager> authManager,
    std::shared_ptr<WorkbookSerializer> workbookSerializer,
    std::shared_ptr<ErrorHandler> errorHandler)
    : m_oneDriveClient(oneDriveClient),
      m_authManager(authManager),
      m_workbookSerializer(workbookSerializer),
      m_errorHandler(errorHandler) {}

bool OneDriveIntegration::SaveWorkbookToOneDrive(const std::shared_ptr<Workbook>& workbook, const std::string& oneDrivePath) {
    try {
        // Ensure the user is authenticated
        if (!EnsureAuthenticated()) {
            m_errorHandler->HandleError("Failed to authenticate user for OneDrive access");
            return false;
        }

        // Serialize the workbook
        std::vector<uint8_t> serializedData = m_workbookSerializer->Serialize(workbook);

        // Upload the serialized data to OneDrive
        bool uploadSuccess = m_oneDriveClient->UploadFile(oneDrivePath, serializedData);

        if (!uploadSuccess) {
            m_errorHandler->HandleError("Failed to upload workbook to OneDrive");
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        m_errorHandler->HandleError("Exception occurred while saving workbook to OneDrive: " + std::string(e.what()));
        return false;
    }
}

std::shared_ptr<Workbook> OneDriveIntegration::LoadWorkbookFromOneDrive(const std::string& oneDrivePath) {
    try {
        // Ensure the user is authenticated
        if (!EnsureAuthenticated()) {
            m_errorHandler->HandleError("Failed to authenticate user for OneDrive access");
            return nullptr;
        }

        // Download the file from OneDrive
        std::vector<uint8_t> downloadedData = m_oneDriveClient->DownloadFile(oneDrivePath);

        if (downloadedData.empty()) {
            m_errorHandler->HandleError("Failed to download workbook from OneDrive");
            return nullptr;
        }

        // Deserialize the downloaded data into a Workbook object
        std::shared_ptr<Workbook> workbook = m_workbookSerializer->Deserialize(downloadedData);

        if (!workbook) {
            m_errorHandler->HandleError("Failed to deserialize downloaded workbook data");
            return nullptr;
        }

        return workbook;
    } catch (const std::exception& e) {
        m_errorHandler->HandleError("Exception occurred while loading workbook from OneDrive: " + std::string(e.what()));
        return nullptr;
    }
}

bool OneDriveIntegration::SyncWorkbookWithOneDrive(const std::shared_ptr<Workbook>& workbook) {
    try {
        // Ensure the user is authenticated
        if (!EnsureAuthenticated()) {
            m_errorHandler->HandleError("Failed to authenticate user for OneDrive sync");
            return false;
        }

        // Get the OneDrive file metadata
        auto fileMetadata = m_oneDriveClient->GetFileMetadata(workbook->GetFilePath());

        if (!fileMetadata) {
            m_errorHandler->HandleError("Failed to retrieve OneDrive file metadata");
            return false;
        }

        // Compare local and OneDrive versions
        if (fileMetadata->lastModifiedTime > workbook->GetLastModifiedTime()) {
            // OneDrive version is newer, download and merge
            auto oneDriveWorkbook = LoadWorkbookFromOneDrive(workbook->GetFilePath());
            if (!oneDriveWorkbook) {
                return false;
            }

            auto mergedWorkbook = CompareAndMergeWorkbooks(workbook, oneDriveWorkbook);
            if (!mergedWorkbook) {
                m_errorHandler->HandleError("Failed to merge workbooks during sync");
                return false;
            }

            // Update the local workbook with merged changes
            *workbook = *mergedWorkbook;
        }

        // Upload the workbook (either merged or local if no changes)
        return SaveWorkbookToOneDrive(workbook, workbook->GetFilePath());
    } catch (const std::exception& e) {
        m_errorHandler->HandleError("Exception occurred during workbook sync with OneDrive: " + std::string(e.what()));
        return false;
    }
}

std::vector<std::string> OneDriveIntegration::ListOneDriveWorkbooks(const std::string& folderPath) {
    try {
        // Ensure the user is authenticated
        if (!EnsureAuthenticated()) {
            m_errorHandler->HandleError("Failed to authenticate user for OneDrive access");
            return {};
        }

        // List all files in the specified OneDrive folder
        auto allFiles = m_oneDriveClient->ListFiles(folderPath);

        // Filter the list to include only Excel workbook files
        std::vector<std::string> workbookFiles;
        for (const auto& file : allFiles) {
            if (file.ends_with(".xlsx") || file.ends_with(".xls")) {
                workbookFiles.push_back(file);
            }
        }

        return workbookFiles;
    } catch (const std::exception& e) {
        m_errorHandler->HandleError("Exception occurred while listing OneDrive workbooks: " + std::string(e.what()));
        return {};
    }
}

bool EnsureAuthenticated() {
    try {
        // Check if the user is already authenticated
        if (m_authManager->IsAuthenticated()) {
            return true;
        }

        // If not authenticated, initiate the authentication process
        bool authSuccess = m_authManager->Authenticate();

        if (!authSuccess) {
            m_errorHandler->HandleError("Failed to authenticate user for OneDrive access");
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        m_errorHandler->HandleError("Exception occurred during authentication: " + std::string(e.what()));
        return false;
    }
}

std::shared_ptr<Workbook> CompareAndMergeWorkbooks(const std::shared_ptr<Workbook>& localWorkbook, const std::shared_ptr<Workbook>& oneDriveWorkbook) {
    // Create a new merged workbook
    auto mergedWorkbook = std::make_shared<Workbook>();

    // Compare and merge worksheets
    for (const auto& localSheet : localWorkbook->GetWorksheets()) {
        auto oneDriveSheet = oneDriveWorkbook->GetWorksheet(localSheet->GetName());
        if (oneDriveSheet) {
            // Worksheet exists in both versions, merge cells
            auto mergedSheet = mergedWorkbook->AddWorksheet(localSheet->GetName());
            for (int row = 1; row <= std::max(localSheet->GetRowCount(), oneDriveSheet->GetRowCount()); ++row) {
                for (int col = 1; col <= std::max(localSheet->GetColumnCount(), oneDriveSheet->GetColumnCount()); ++col) {
                    auto localCell = localSheet->GetCell(row, col);
                    auto oneDriveCell = oneDriveSheet->GetCell(row, col);
                    if (localCell && oneDriveCell) {
                        // Cell exists in both versions, use the most recently modified one
                        mergedSheet->SetCell(row, col, localCell->GetLastModifiedTime() > oneDriveCell->GetLastModifiedTime() ? *localCell : *oneDriveCell);
                    } else if (localCell) {
                        mergedSheet->SetCell(row, col, *localCell);
                    } else if (oneDriveCell) {
                        mergedSheet->SetCell(row, col, *oneDriveCell);
                    }
                }
            }
        } else {
            // Worksheet only exists in local version, add it to merged workbook
            mergedWorkbook->AddWorksheet(*localSheet);
        }
    }

    // Add worksheets that only exist in OneDrive version
    for (const auto& oneDriveSheet : oneDriveWorkbook->GetWorksheets()) {
        if (!localWorkbook->GetWorksheet(oneDriveSheet->GetName())) {
            mergedWorkbook->AddWorksheet(*oneDriveSheet);
        }
    }

    // Merge workbook-level properties and settings
    mergedWorkbook->SetProperties(localWorkbook->GetLastModifiedTime() > oneDriveWorkbook->GetLastModifiedTime() ?
        localWorkbook->GetProperties() : oneDriveWorkbook->GetProperties());

    return mergedWorkbook;
}