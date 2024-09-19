#include <vector>
#include <memory>
#include <string>
#include <future>
#include "excel_types.h"
#include "sharepoint_integration.h"
#include "sharepoint_client.h"
#include "authentication_manager.h"
#include "workbook_serializer.h"
#include "error_handler.h"
#include "collaboration_manager.h"

const std::string SHAREPOINT_API_ENDPOINT = "https://<tenant>.sharepoint.com/_api/";

SharePointIntegration::SharePointIntegration(
    std::shared_ptr<SharePointClient> sharePointClient,
    std::shared_ptr<AuthenticationManager> authManager,
    std::shared_ptr<WorkbookSerializer> workbookSerializer,
    std::shared_ptr<ErrorHandler> errorHandler,
    std::shared_ptr<CollaborationManager> collaborationManager)
    : m_sharePointClient(sharePointClient),
      m_authManager(authManager),
      m_workbookSerializer(workbookSerializer),
      m_errorHandler(errorHandler),
      m_collaborationManager(collaborationManager) {}

bool SharePointIntegration::SaveWorkbookToSharePoint(const std::shared_ptr<Workbook>& workbook, const std::string& sharePointUrl) {
    // Ensure the user is authenticated
    if (!EnsureAuthenticated()) {
        m_errorHandler->HandleError("Authentication failed while saving workbook to SharePoint");
        return false;
    }

    try {
        // Serialize the workbook
        std::vector<uint8_t> serializedData = m_workbookSerializer->Serialize(workbook);

        // Upload the serialized data to SharePoint
        bool uploadSuccess = m_sharePointClient->UploadFile(sharePointUrl, serializedData);

        if (!uploadSuccess) {
            m_errorHandler->HandleError("Failed to upload workbook to SharePoint");
            return false;
        }

        return true;
    } catch (const std::exception& e) {
        m_errorHandler->HandleError("Error while saving workbook to SharePoint: " + std::string(e.what()));
        return false;
    }
}

std::shared_ptr<Workbook> SharePointIntegration::LoadWorkbookFromSharePoint(const std::string& sharePointUrl) {
    // Ensure the user is authenticated
    if (!EnsureAuthenticated()) {
        m_errorHandler->HandleError("Authentication failed while loading workbook from SharePoint");
        return nullptr;
    }

    try {
        // Download the file from SharePoint
        std::vector<uint8_t> fileData = m_sharePointClient->DownloadFile(sharePointUrl);

        // Deserialize the downloaded data into a Workbook object
        std::shared_ptr<Workbook> workbook = m_workbookSerializer->Deserialize(fileData);

        if (!workbook) {
            m_errorHandler->HandleError("Failed to deserialize workbook from SharePoint");
            return nullptr;
        }

        return workbook;
    } catch (const std::exception& e) {
        m_errorHandler->HandleError("Error while loading workbook from SharePoint: " + std::string(e.what()));
        return nullptr;
    }
}

bool SharePointIntegration::CollaborateOnWorkbook(const std::shared_ptr<Workbook>& workbook, const std::vector<std::string>& collaboratorEmails) {
    // Ensure the user is authenticated
    if (!EnsureAuthenticated()) {
        m_errorHandler->HandleError("Authentication failed while setting up collaboration");
        return false;
    }

    try {
        // Set up collaboration permissions on SharePoint
        bool setupSuccess = m_sharePointClient->SetupCollaboration(workbook->GetSharePointUrl(), collaboratorEmails);
        if (!setupSuccess) {
            m_errorHandler->HandleError("Failed to set up collaboration permissions on SharePoint");
            return false;
        }

        // Initialize a collaboration session
        bool sessionInitialized = m_collaborationManager->InitializeSession(workbook, collaboratorEmails);
        if (!sessionInitialized) {
            m_errorHandler->HandleError("Failed to initialize collaboration session");
            return false;
        }

        // Send invitations to collaborators (This could be implemented in CollaborationManager)
        // Set up real-time synchronization mechanisms (This could be implemented in CollaborationManager)

        return true;
    } catch (const std::exception& e) {
        m_errorHandler->HandleError("Error while setting up collaboration: " + std::string(e.what()));
        return false;
    }
}

std::vector<std::string> SharePointIntegration::ListSharePointWorkbooks(const std::string& folderUrl) {
    // Ensure the user is authenticated
    if (!EnsureAuthenticated()) {
        m_errorHandler->HandleError("Authentication failed while listing SharePoint workbooks");
        return {};
    }

    try {
        // List all files in the specified SharePoint folder
        std::vector<std::string> allFiles = m_sharePointClient->ListFiles(folderUrl);

        // Filter the list to include only Excel workbook files
        std::vector<std::string> workbookFiles;
        for (const auto& file : allFiles) {
            if (file.ends_with(".xlsx") || file.ends_with(".xls")) {
                workbookFiles.push_back(file);
            }
        }

        return workbookFiles;
    } catch (const std::exception& e) {
        m_errorHandler->HandleError("Error while listing SharePoint workbooks: " + std::string(e.what()));
        return {};
    }
}

bool SharePointIntegration::EnsureAuthenticated() {
    // Check if the user is already authenticated
    if (m_authManager->IsAuthenticated()) {
        return true;
    }

    // If not authenticated, initiate the authentication process
    try {
        bool authSuccess = m_authManager->Authenticate();
        if (!authSuccess) {
            m_errorHandler->HandleError("Authentication failed");
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        m_errorHandler->HandleError("Authentication error: " + std::string(e.what()));
        return false;
    }
}