import React, { useState, useEffect, useCallback, useRef } from 'react';
import { FixedSizeGrid } from 'react-window';
import { getCellData, updateCellData } from 'excel-api';
import { Cell } from './Cell';
import { useWorksheetGridStyles } from './WorksheetGridStyles';

// Define interfaces
interface WorksheetGridProps {
  workbookId: string;
  worksheetId: string;
  onCellSelect: (row: number, col: number) => void;
}

interface CellData {
  value: string;
  formula: string;
  format: string;
}

// Define constants
const COLUMN_WIDTH = 120;
const ROW_HEIGHT = 30;
const GRID_SIZE = { rows: 1000, columns: 26 };

const WorksheetGrid: React.FC<WorksheetGridProps> = ({ workbookId, worksheetId, onCellSelect }) => {
  // State hooks
  const [cellData, setCellData] = useState<CellData[][]>([]);
  const [selectedCell, setSelectedCell] = useState<{ row: number; col: number } | null>(null);

  // Ref for the grid
  const gridRef = useRef<FixedSizeGrid>(null);

  // Styles
  const classes = useWorksheetGridStyles();

  // Effect to load initial cell data
  useEffect(() => {
    const loadCellData = async () => {
      try {
        const data = await getCellData(workbookId, worksheetId);
        setCellData(data);
      } catch (error) {
        console.error('Error loading cell data:', error);
      }
    };

    loadCellData();
  }, [workbookId, worksheetId]);

  // Callback for cell click
  const handleCellClick = useCallback((row: number, col: number) => {
    setSelectedCell({ row, col });
    onCellSelect(row, col);
  }, [onCellSelect]);

  // Callback for cell change
  const handleCellChange = useCallback(async (row: number, col: number, value: string) => {
    try {
      await updateCellData(workbookId, worksheetId, row, col, value);
      setCellData(prevData => {
        const newData = [...prevData];
        newData[row] = [...newData[row]];
        newData[row][col] = { ...newData[row][col], value };
        return newData;
      });
    } catch (error) {
      console.error('Error updating cell data:', error);
    }
  }, [workbookId, worksheetId]);

  // Render an individual cell
  const renderCell = ({ columnIndex, rowIndex, style }: { columnIndex: number; rowIndex: number; style: React.CSSProperties }) => {
    const cellValue = cellData[rowIndex]?.[columnIndex]?.value || '';
    const isSelected = selectedCell?.row === rowIndex && selectedCell?.col === columnIndex;

    return (
      <Cell
        key={`${rowIndex}-${columnIndex}`}
        value={cellValue}
        onChange={(value) => handleCellChange(rowIndex, columnIndex, value)}
        onClick={() => handleCellClick(rowIndex, columnIndex)}
        isSelected={isSelected}
        style={style}
      />
    );
  };

  // Render column headers (A, B, C, ...)
  const renderColumnHeaders = () => (
    <div className={classes.columnHeaders}>
      {Array.from({ length: GRID_SIZE.columns }, (_, i) => (
        <div key={i} className={classes.headerCell} style={{ width: COLUMN_WIDTH }}>
          {String.fromCharCode(65 + i)}
        </div>
      ))}
    </div>
  );

  // Render row headers (1, 2, 3, ...)
  const renderRowHeaders = () => (
    <div className={classes.rowHeaders}>
      {Array.from({ length: GRID_SIZE.rows }, (_, i) => (
        <div key={i} className={classes.headerCell} style={{ height: ROW_HEIGHT }}>
          {i + 1}
        </div>
      ))}
    </div>
  );

  return (
    <div className={classes.worksheetGrid}>
      {renderColumnHeaders()}
      {renderRowHeaders()}
      <FixedSizeGrid
        ref={gridRef}
        className={classes.grid}
        columnCount={GRID_SIZE.columns}
        columnWidth={COLUMN_WIDTH}
        height={600}
        rowCount={GRID_SIZE.rows}
        rowHeight={ROW_HEIGHT}
        width={800}
      >
        {renderCell}
      </FixedSizeGrid>
    </div>
  );
};

export default WorksheetGrid;