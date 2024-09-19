import React, { useState, useEffect, useCallback } from 'react';
import { useParams, useNavigate } from 'react-router-dom';
import { Stack, Pivot, PivotItem, MessageBar, MessageBarType } from 'office-ui-fabric-react';
import { getWorkbook, saveWorkbook, getWorksheets } from 'excel-api';
import { Ribbon, WorksheetGrid, FormulaBar, ChartControls } from '../components';
import { useWorkbookStyles } from './WorkbookStyles';

// Define the WorkbookData interface
interface WorkbookData {
  id: string;
  name: string;
  worksheets: Array<{ id: string; name: string }>;
}

const Workbook: React.FC = () => {
  // Initialize state variables
  const [workbook, setWorkbook] = useState<WorkbookData | null>(null);
  const [activeWorksheetId, setActiveWorksheetId] = useState<string | null>(null);
  const [selectedCell, setSelectedCell] = useState<{ row: number; col: number } | null>(null);
  const [isLoading, setIsLoading] = useState<boolean>(true);
  const [error, setError] = useState<string | null>(null);

  // Get workbookId from URL params
  const { workbookId } = useParams<{ workbookId: string }>();
  const navigate = useNavigate();

  // Get styles
  const styles = useWorkbookStyles();

  // Load workbook data
  useEffect(() => {
    const loadWorkbook = async () => {
      try {
        setIsLoading(true);
        const workbookData = await getWorkbook(workbookId);
        setWorkbook(workbookData);
        if (workbookData.worksheets.length > 0) {
          setActiveWorksheetId(workbookData.worksheets[0].id);
        }
      } catch (err) {
        setError('Failed to load workbook');
      } finally {
        setIsLoading(false);
      }
    };

    loadWorkbook();
  }, [workbookId]);

  // Handle worksheet change
  const handleWorksheetChange = useCallback((item?: PivotItem) => {
    if (item) {
      setActiveWorksheetId(item.props.itemKey as string);
    }
  }, []);

  // Handle cell selection
  const handleCellSelect = useCallback((row: number, col: number) => {
    setSelectedCell({ row, col });
  }, []);

  // Handle save
  const handleSave = useCallback(async () => {
    if (workbook && activeWorksheetId) {
      try {
        await saveWorkbook(workbook.id, activeWorksheetId);
        // Show success message or update UI accordingly
      } catch (err) {
        setError('Failed to save workbook');
      }
    }
  }, [workbook, activeWorksheetId]);

  // Render worksheet tabs
  const renderWorksheetTabs = (): JSX.Element => {
    return (
      <Pivot onLinkClick={handleWorksheetChange}>
        {workbook?.worksheets.map((sheet) => (
          <PivotItem key={sheet.id} itemKey={sheet.id} headerText={sheet.name} />
        ))}
      </Pivot>
    );
  };

  // Render active worksheet
  const renderActiveWorksheet = (): JSX.Element => {
    if (!activeWorksheetId) {
      return <div>No active worksheet</div>;
    }

    return (
      <WorksheetGrid
        workbookId={workbook!.id}
        worksheetId={activeWorksheetId}
        onCellSelect={handleCellSelect}
      />
    );
  };

  // Render the component
  return (
    <Stack className={styles.workbook}>
      {error && (
        <MessageBar messageBarType={MessageBarType.error} isMultiline={false} onDismiss={() => setError(null)}>
          {error}
        </MessageBar>
      )}
      {isLoading ? (
        <div>Loading...</div>
      ) : (
        <>
          <Ribbon onSave={handleSave} />
          <FormulaBar selectedCell={selectedCell} />
          {renderWorksheetTabs()}
          {renderActiveWorksheet()}
          <ChartControls workbookId={workbook!.id} worksheetId={activeWorksheetId!} />
        </>
      )}
    </Stack>
  );
};

export default Workbook;