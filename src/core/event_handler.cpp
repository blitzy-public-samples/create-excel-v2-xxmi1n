#include <vector>
#include <unordered_map>
#include <string>
#include <functional>
#include <memory>
#include "excel_types.h"
#include "event.h"

class EventHandler {
private:
    std::unordered_map<EventType, std::vector<std::function<void(const Event&)>>> m_eventListeners;
    std::weak_ptr<Workbook> m_activeWorkbook;

public:
    // Constructor
    EventHandler() {
        // Initialize m_eventListeners as an empty map
        // Initialize m_activeWorkbook as a null weak_ptr
    }

    // Register a listener for a specific event type
    void RegisterListener(EventType eventType, std::function<void(const Event&)> listener) {
        // Add the listener to the vector of listeners for the specified event type in m_eventListeners
        m_eventListeners[eventType].push_back(listener);
    }

    // Unregister a listener for a specific event type
    bool UnregisterListener(EventType eventType, std::function<void(const Event&)> listener) {
        // Find the listener in the vector of listeners for the specified event type
        auto& listeners = m_eventListeners[eventType];
        auto it = std::find(listeners.begin(), listeners.end(), listener);
        
        // If found, remove the listener from the vector
        if (it != listeners.end()) {
            listeners.erase(it);
            return true;
        }
        
        // Return false if the listener was not found
        return false;
    }

    // Dispatch an event to all registered listeners
    void DispatchEvent(const Event& event) {
        // Get the event type from the event object
        EventType eventType = event.GetType();
        
        // Find the vector of listeners for the event type in m_eventListeners
        auto it = m_eventListeners.find(eventType);
        
        // If listeners are found, iterate through the vector and call each listener with the event
        if (it != m_eventListeners.end()) {
            for (const auto& listener : it->second) {
                listener(event);
            }
        }
    }

    // Set the active workbook for event context
    void SetActiveWorkbook(std::shared_ptr<Workbook> workbook) {
        // Set m_activeWorkbook to a weak_ptr of the provided workbook
        m_activeWorkbook = workbook;
    }

    // Handle a cell change event
    void HandleCellChange(const CellReference& cellRef, const std::string& newValue) {
        // Create a CellChangeEvent object with the cell reference and new value
        CellChangeEvent event(cellRef, newValue);
        
        // Call DispatchEvent with the CellChangeEvent
        DispatchEvent(event);
    }

    // Handle a selection change event
    void HandleSelectionChange(const CellReference& startCell, const CellReference& endCell) {
        // Create a SelectionChangeEvent object with the start and end cell references
        SelectionChangeEvent event(startCell, endCell);
        
        // Call DispatchEvent with the SelectionChangeEvent
        DispatchEvent(event);
    }

    // Handle a workbook open event
    void HandleWorkbookOpen(std::shared_ptr<Workbook> workbook) {
        // Create a WorkbookOpenEvent object with the workbook
        WorkbookOpenEvent event(workbook);
        
        // Call DispatchEvent with the WorkbookOpenEvent
        DispatchEvent(event);
        
        // Call SetActiveWorkbook with the opened workbook
        SetActiveWorkbook(workbook);
    }

    // Handle a workbook close event
    void HandleWorkbookClose(std::shared_ptr<Workbook> workbook) {
        // Create a WorkbookCloseEvent object with the workbook
        WorkbookCloseEvent event(workbook);
        
        // Call DispatchEvent with the WorkbookCloseEvent
        DispatchEvent(event);
        
        // If the closed workbook is the active workbook, clear m_activeWorkbook
        if (m_activeWorkbook.lock() == workbook) {
            m_activeWorkbook.reset();
        }
    }
};