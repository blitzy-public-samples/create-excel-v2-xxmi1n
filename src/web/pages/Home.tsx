import React, { useState, useEffect } from 'react';
import { Link } from 'react-router-dom';
import { Stack, Text, PrimaryButton, SearchBox, List } from 'office-ui-fabric-react';
import { getUserWorkbooks, createNewWorkbook } from 'excel-api';
import { Header, Footer, WorkbookCard } from '../components';
import { useHomeStyles } from './HomeStyles';

// Define the Workbook interface
interface Workbook {
  id: string;
  name: string;
  lastModified: Date;
  owner: string;
}

const Home: React.FC = () => {
  // Initialize state variables
  const [workbooks, setWorkbooks] = useState<Workbook[]>([]);
  const [searchQuery, setSearchQuery] = useState<string>('');
  const [isLoading, setIsLoading] = useState<boolean>(true);

  // Get styles from custom hook
  const styles = useHomeStyles();

  // Load user's workbooks on component mount
  useEffect(() => {
    const loadWorkbooks = async () => {
      try {
        const userWorkbooks = await getUserWorkbooks();
        setWorkbooks(userWorkbooks);
        setIsLoading(false);
      } catch (error) {
        console.error('Error loading workbooks:', error);
        setIsLoading(false);
      }
    };

    loadWorkbooks();
  }, []);

  // Handle search query changes
  const handleSearch = (_event: React.ChangeEvent<HTMLInputElement>, newValue?: string) => {
    setSearchQuery(newValue || '');
  };

  // Handle creation of a new workbook
  const handleCreateNewWorkbook = async () => {
    try {
      const newWorkbook = await createNewWorkbook();
      setWorkbooks([newWorkbook, ...workbooks]);
    } catch (error) {
      console.error('Error creating new workbook:', error);
    }
  };

  // Render the list of workbooks
  const renderWorkbookList = () => {
    const filteredWorkbooks = workbooks.filter(workbook =>
      workbook.name.toLowerCase().includes(searchQuery.toLowerCase())
    );

    return (
      <List
        items={filteredWorkbooks}
        onRenderCell={(item?: Workbook) => {
          return item ? <WorkbookCard workbook={item} /> : null;
        }}
      />
    );
  };

  return (
    <Stack verticalFill>
      {/* Render Header component */}
      <Header />

      <Stack horizontal horizontalAlign="center" styles={styles.content}>
        <Stack styles={styles.mainContent}>
          {/* Render welcome message */}
          <Text variant="xxLarge" styles={styles.welcomeText}>
            Welcome to Excel Web App
          </Text>

          {/* Render 'Create New Workbook' button */}
          <PrimaryButton
            text="Create New Workbook"
            onClick={handleCreateNewWorkbook}
            styles={styles.createButton}
          />

          {/* Render search box */}
          <SearchBox
            placeholder="Search workbooks"
            onChange={handleSearch}
            styles={styles.searchBox}
          />

          {/* Render list of workbooks or loading indicator */}
          {isLoading ? (
            <Text>Loading workbooks...</Text>
          ) : (
            renderWorkbookList()
          )}
        </Stack>
      </Stack>

      {/* Render Footer component */}
      <Footer />
    </Stack>
  );
};

export default Home;