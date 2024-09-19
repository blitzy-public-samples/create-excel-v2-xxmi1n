import React, { useState, useEffect, useCallback } from 'react';
import { Dropdown, IDropdownOption, PrimaryButton, TextField, Stack } from 'office-ui-fabric-react';
import { createChart, updateChart, getChartTypes } from 'excel-api';
import { useChartControlsStyles } from './ChartControlsStyles';

interface ChartControlsProps {
  workbookId: string;
  worksheetId: string;
  selectedRange: string | null;
  onChartCreated: (chartId: string) => void;
}

interface ChartType {
  id: string;
  name: string;
}

const ChartControls: React.FC<ChartControlsProps> = ({
  workbookId,
  worksheetId,
  selectedRange,
  onChartCreated,
}) => {
  const [chartTypes, setChartTypes] = useState<ChartType[]>([]);
  const [selectedChartType, setSelectedChartType] = useState<string | null>(null);
  const [chartTitle, setChartTitle] = useState<string>('');

  const styles = useChartControlsStyles();

  // Load available chart types
  useEffect(() => {
    const loadChartTypes = async () => {
      try {
        const types = await getChartTypes();
        setChartTypes(types);
      } catch (error) {
        console.error('Failed to load chart types:', error);
      }
    };
    loadChartTypes();
  }, []);

  const handleChartTypeChange = useCallback((event: React.FormEvent<HTMLDivElement>, option?: IDropdownOption) => {
    if (option) {
      setSelectedChartType(option.key as string);
    }
  }, []);

  const handleChartTitleChange = useCallback((event: React.FormEvent<HTMLInputElement | HTMLTextAreaElement>, newValue?: string) => {
    if (newValue !== undefined) {
      setChartTitle(newValue);
    }
  }, []);

  const handleCreateChart = useCallback(async () => {
    if (!selectedRange || !selectedChartType) {
      return;
    }

    try {
      const chartId = await createChart(workbookId, worksheetId, selectedRange, selectedChartType, chartTitle);
      onChartCreated(chartId);
    } catch (error) {
      console.error('Failed to create chart:', error);
    }
  }, [workbookId, worksheetId, selectedRange, selectedChartType, chartTitle, onChartCreated]);

  return (
    <Stack tokens={{ childrenGap: 16 }}>
      {/* Render chart type dropdown */}
      <Dropdown
        label="Chart Type"
        options={chartTypes.map((type) => ({ key: type.id, text: type.name }))}
        selectedKey={selectedChartType}
        onChange={handleChartTypeChange}
        styles={styles.dropdown}
      />

      {/* Render chart title input */}
      <TextField
        label="Chart Title"
        value={chartTitle}
        onChange={handleChartTitleChange}
        styles={styles.textField}
      />

      {/* Render create chart button */}
      <PrimaryButton
        text="Create Chart"
        onClick={handleCreateChart}
        disabled={!selectedRange || !selectedChartType}
        styles={styles.button}
      />

      {/* Conditionally render error message if no range is selected */}
      {!selectedRange && (
        <div className={styles.errorMessage}>
          Please select a range in the worksheet before creating a chart.
        </div>
      )}
    </Stack>
  );
};

export default ChartControls;