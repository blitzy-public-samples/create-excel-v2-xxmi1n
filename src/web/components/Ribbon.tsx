import React, { useState, useCallback } from 'react';
import { CommandBar, ICommandBarItemProps, IContextualMenuItem } from 'office-ui-fabric-react';
import { executeCommand } from 'excel-api';
import { useRibbonStyles } from './RibbonStyles';

// Define interfaces
interface RibbonProps {
  onCommandExecute: (commandId: string) => void;
}

interface RibbonTab {
  key: string;
  name: string;
  items: ICommandBarItemProps[];
}

// Define constant for ribbon tabs
const RIBBON_TABS: RibbonTab[] = [
  {
    key: 'home',
    name: 'Home',
    items: [
      { key: 'paste', name: 'Paste', iconProps: { iconName: 'Paste' }, onClick: () => executeCommand('paste') },
      { key: 'cut', name: 'Cut', iconProps: { iconName: 'Cut' }, onClick: () => executeCommand('cut') },
      { key: 'copy', name: 'Copy', iconProps: { iconName: 'Copy' }, onClick: () => executeCommand('copy') },
    ],
  },
  {
    key: 'insert',
    name: 'Insert',
    items: [
      { key: 'insertTable', name: 'Table', iconProps: { iconName: 'Table' }, onClick: () => executeCommand('insertTable') },
      { key: 'insertChart', name: 'Chart', iconProps: { iconName: 'BarChart4' }, onClick: () => executeCommand('insertChart') },
    ],
  },
];

const Ribbon: React.FC<RibbonProps> = ({ onCommandExecute }) => {
  // Use custom styles
  const styles = useRibbonStyles();

  // State to keep track of the active tab
  const [activeTab, setActiveTab] = useState(RIBBON_TABS[0].key);

  // Callback to handle tab click
  const handleTabClick = useCallback((tab: RibbonTab) => {
    setActiveTab(tab.key);
  }, []);

  // Callback to handle command click
  const handleCommandClick = useCallback((ev?: React.MouseEvent<HTMLElement> | React.KeyboardEvent<HTMLElement>, item?: IContextualMenuItem) => {
    if (item) {
      onCommandExecute(item.key);
    }
  }, [onCommandExecute]);

  // Prepare items for the tab CommandBar
  const tabItems: ICommandBarItemProps[] = RIBBON_TABS.map((tab) => ({
    key: tab.key,
    name: tab.name,
    onClick: () => handleTabClick(tab),
    isActive: activeTab === tab.key,
  }));

  // Get the active tab's items
  const activeTabItems = RIBBON_TABS.find((tab) => tab.key === activeTab)?.items || [];

  return (
    <div className={styles.ribbon}>
      {/* Render CommandBar for tabs */}
      <CommandBar items={tabItems} />
      
      {/* Render CommandBar for active tab items */}
      <CommandBar items={activeTabItems} onItemClick={handleCommandClick} />
    </div>
  );
};

export default Ribbon;