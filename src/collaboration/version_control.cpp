#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <chrono>
#include "excel_types.h"
#include "data_manager.h"
#include "storage_manager.h"
#include "user_manager.h"
#include "version_control.h"

const int MAX_VERSIONS = 100;

class VersionControl {
private:
    std::shared_ptr<DataManager> m_dataManager;
    std::shared_ptr<StorageManager> m_storageManager;
    std::shared_ptr<UserManager> m_userManager;
    std::unordered_map<std::string, std::vector<std::shared_ptr<Version>>> m_versionHistory;

public:
    VersionControl(std::shared_ptr<DataManager> dataManager,
                   std::shared_ptr<StorageManager> storageManager,
                   std::shared_ptr<UserManager> userManager)
        : m_dataManager(dataManager),
          m_storageManager(storageManager),
          m_userManager(userManager) {
        // Initialize m_versionHistory as an empty unordered_map
    }

    std::string CreateVersion(std::shared_ptr<Workbook> workbook, const std::string& versionName) {
        // Generate a unique version ID
        std::string versionId = GenerateVersionId();

        // Create a ChangeSet representing the current state of the workbook
        ChangeSet changeSet = CreateChangeSet(workbook);

        // Create a new Version object
        auto newVersion = std::make_shared<Version>(versionId, versionName, changeSet);

        // Add the Version to the workbook's version history
        m_versionHistory[workbook->GetId()].push_back(newVersion);

        // Save the Version using StorageManager
        m_storageManager->SaveVersion(workbook->GetId(), *newVersion);

        return versionId;
    }

    bool RestoreVersion(std::shared_ptr<Workbook> workbook, const std::string& versionId) {
        // Find the specified version in the workbook's version history
        auto& versions = m_versionHistory[workbook->GetId()];
        auto it = std::find_if(versions.begin(), versions.end(),
                               [&](const auto& v) { return v->GetId() == versionId; });

        if (it == versions.end()) {
            return false;
        }

        // Load the Version data
        Version loadedVersion;
        if (!m_storageManager->LoadVersion(workbook->GetId(), versionId, loadedVersion)) {
            return false;
        }

        // Apply the Version's ChangeSet to the workbook
        ApplyChangeSet(workbook, loadedVersion.GetChangeSet());

        // Update the workbook's current state
        workbook->SetCurrentVersionId(versionId);

        // Notify listeners of the version restoration
        // (This would be implemented in a real system)

        return true;
    }

    std::vector<VersionInfo> GetVersionHistory(std::shared_ptr<Workbook> workbook) {
        std::vector<VersionInfo> history;

        // Find the workbook's version history
        auto it = m_versionHistory.find(workbook->GetId());
        if (it == m_versionHistory.end()) {
            return history;
        }

        // Create a vector of VersionInfo objects
        for (const auto& version : it->second) {
            history.emplace_back(version->GetId(), version->GetName(), version->GetTimestamp());
        }

        // Sort the VersionInfo vector by creation time (newest first)
        std::sort(history.begin(), history.end(),
                  [](const VersionInfo& a, const VersionInfo& b) {
                      return a.timestamp > b.timestamp;
                  });

        return history;
    }

    std::vector<CellChange> CompareVersions(std::shared_ptr<Workbook> workbook,
                                            const std::string& versionId1,
                                            const std::string& versionId2) {
        std::vector<CellChange> changes;

        // Load both versions
        Version v1, v2;
        if (!m_storageManager->LoadVersion(workbook->GetId(), versionId1, v1) ||
            !m_storageManager->LoadVersion(workbook->GetId(), versionId2, v2)) {
            return changes;
        }

        // Compare the ChangeSets of both versions
        const auto& changeSet1 = v1.GetChangeSet();
        const auto& changeSet2 = v2.GetChangeSet();

        // Generate a list of CellChange objects representing the differences
        for (const auto& [cellRef, cellData1] : changeSet1.GetChanges()) {
            auto it = changeSet2.GetChanges().find(cellRef);
            if (it == changeSet2.GetChanges().end() || it->second != cellData1) {
                changes.emplace_back(cellRef, cellData1, it->second);
            }
        }

        for (const auto& [cellRef, cellData2] : changeSet2.GetChanges()) {
            if (changeSet1.GetChanges().find(cellRef) == changeSet1.GetChanges().end()) {
                changes.emplace_back(cellRef, CellValue(), cellData2);
            }
        }

        return changes;
    }
};

std::string GenerateVersionId() {
    // Generate a UUID or use a combination of timestamp and random characters
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 35);

    const char* chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string id;

    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto value = now_ms.time_since_epoch();
    long long timestamp = value.count();

    id = std::to_string(timestamp);

    for (int i = 0; i < 8; ++i) {
        id += chars[dis(gen)];
    }

    // Ensure the generated ID is unique among existing versions
    // (This would require checking against existing IDs in a real system)

    return id;
}

ChangeSet CreateChangeSet(const std::shared_ptr<Workbook>& workbook) {
    ChangeSet changeSet;

    // Iterate through all worksheets in the workbook
    for (const auto& worksheet : workbook->GetWorksheets()) {
        // For each worksheet, iterate through all non-empty cells
        for (const auto& [cellRef, cellValue] : worksheet->GetCells()) {
            // Add each cell's data (value, formula, formatting) to the ChangeSet
            changeSet.AddChange(cellRef, cellValue);
        }
    }

    // Include workbook-level properties and settings in the ChangeSet
    changeSet.SetWorkbookProperties(workbook->GetProperties());

    return changeSet;
}

void ApplyChangeSet(std::shared_ptr<Workbook> workbook, const ChangeSet& changeSet) {
    // Clear the current content of the workbook
    workbook->Clear();

    // Iterate through the changes in the ChangeSet
    for (const auto& [cellRef, cellValue] : changeSet.GetChanges()) {
        // For each change, apply it to the corresponding cell
        m_dataManager->UpdateCell(workbook, cellRef, cellValue);
    }

    // Update workbook-level properties and settings
    workbook->SetProperties(changeSet.GetWorkbookProperties());

    // Recalculate any formulas affected by the changes
    workbook->RecalculateFormulas();
}