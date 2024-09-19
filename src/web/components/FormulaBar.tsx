import React, { useState, useEffect, useCallback } from 'react';
import { TextField, IconButton, Stack } from 'office-ui-fabric-react';
import { getCellFormula, updateCellFormula } from 'excel-api';
import { useFormulaBarStyles } from './FormulaBarStyles';

interface FormulaBarProps {
  workbookId: string;
  worksheetId: string;
  selectedCell: { row: number, col: number } | null;
  onFormulaChange: (formula: string) => void;
}

const FormulaBar: React.FC<FormulaBarProps> = ({
  workbookId,
  worksheetId,
  selectedCell,
  onFormulaChange,
}) => {
  const [formula, setFormula] = useState<string>('');
  const [isEditing, setIsEditing] = useState<boolean>(false);
  const styles = useFormulaBarStyles();

  // Load formula for selected cell
  useEffect(() => {
    const loadFormula = async () => {
      if (selectedCell) {
        const cellFormula = await getCellFormula(workbookId, worksheetId, selectedCell.row, selectedCell.col);
        setFormula(cellFormula);
        setIsEditing(false);
      }
    };
    loadFormula();
  }, [workbookId, worksheetId, selectedCell]);

  const handleFormulaChange = useCallback((event: React.FormEvent<HTMLInputElement | HTMLTextAreaElement>, newValue?: string) => {
    setFormula(newValue || '');
    setIsEditing(true);
  }, [setFormula]);

  const handleFormulaSubmit = useCallback(async () => {
    if (selectedCell) {
      await updateCellFormula(workbookId, worksheetId, selectedCell.row, selectedCell.col, formula);
      onFormulaChange(formula);
      setIsEditing(false);
    }
  }, [workbookId, worksheetId, selectedCell, formula, onFormulaChange]);

  const handleFormulaCancel = useCallback(() => {
    if (selectedCell) {
      const loadFormula = async () => {
        const cellFormula = await getCellFormula(workbookId, worksheetId, selectedCell.row, selectedCell.col);
        setFormula(cellFormula);
        setIsEditing(false);
      };
      loadFormula();
    }
  }, [selectedCell, workbookId, worksheetId]);

  const renderCellReference = (): JSX.Element => {
    if (selectedCell) {
      const colLetter = String.fromCharCode(65 + selectedCell.col);
      return <div className={styles.cellReference}>{`${colLetter}${selectedCell.row + 1}`}</div>;
    }
    return <div className={styles.cellReference}>No cell selected</div>;
  };

  return (
    <Stack horizontal className={styles.formulaBar}>
      {renderCellReference()}
      <TextField
        value={formula}
        onChange={handleFormulaChange}
        className={styles.formulaInput}
        placeholder="Enter formula"
      />
      {isEditing && (
        <>
          <IconButton
            iconProps={{ iconName: 'CheckMark' }}
            onClick={handleFormulaSubmit}
            className={styles.actionButton}
          />
          <IconButton
            iconProps={{ iconName: 'Cancel' }}
            onClick={handleFormulaCancel}
            className={styles.actionButton}
          />
        </>
      )}
    </Stack>
  );
};

export default FormulaBar;