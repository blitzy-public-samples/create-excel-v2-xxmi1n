import React, { useState, useEffect, useCallback } from 'react';
import {
  Stack,
  Toggle,
  Dropdown,
  IDropdownOption,
  PrimaryButton,
  TextField,
  MessageBar,
  MessageBarType
} from 'office-ui-fabric-react';
import { getUserSettings, updateUserSettings, changePassword } from 'excel-api';
import { Header, Footer } from '../components';
import { useSettingsStyles } from './SettingsStyles';

// Define interfaces for UserSettings and PasswordChangeData
interface UserSettings {
  id: string;
  username: string;
  email: string;
  language: string;
  theme: string;
  autoSave: boolean;
  notifications: boolean;
}

interface PasswordChangeData {
  currentPassword: string;
  newPassword: string;
  confirmNewPassword: string;
}

const Settings: React.FC = () => {
  // Initialize state variables
  const [settings, setSettings] = useState<UserSettings | null>(null);
  const [passwordData, setPasswordData] = useState<PasswordChangeData>({
    currentPassword: '',
    newPassword: '',
    confirmNewPassword: '',
  });
  const [isLoading, setIsLoading] = useState<boolean>(true);
  const [error, setError] = useState<string | null>(null);
  const [successMessage, setSuccessMessage] = useState<string | null>(null);

  // Get styles from custom hook
  const styles = useSettingsStyles();

  // Load user settings on component mount
  useEffect(() => {
    const fetchSettings = async () => {
      try {
        const userSettings = await getUserSettings();
        setSettings(userSettings);
        setIsLoading(false);
      } catch (err) {
        setError('Failed to load user settings');
        setIsLoading(false);
      }
    };
    fetchSettings();
  }, []);

  // Handle setting changes
  const handleSettingChange = useCallback(
    (setting: keyof UserSettings) => (value: any) => {
      if (settings) {
        setSettings({ ...settings, [setting]: value });
      }
    },
    [settings]
  );

  // Handle saving settings
  const handleSaveSettings = useCallback(async () => {
    if (settings) {
      setIsLoading(true);
      try {
        await updateUserSettings(settings);
        setSuccessMessage('Settings saved successfully');
        setIsLoading(false);
      } catch (err) {
        setError('Failed to save settings');
        setIsLoading(false);
      }
    }
  }, [settings]);

  // Handle password change form input
  const handlePasswordChange = useCallback(
    (field: keyof PasswordChangeData) => (
      event: React.FormEvent<HTMLInputElement | HTMLTextAreaElement>,
      newValue?: string
    ) => {
      if (newValue !== undefined) {
        setPasswordData({ ...passwordData, [field]: newValue });
      }
    },
    [passwordData]
  );

  // Handle password change submission
  const handleChangePassword = useCallback(async () => {
    setIsLoading(true);
    try {
      await changePassword(passwordData);
      setSuccessMessage('Password changed successfully');
      setPasswordData({ currentPassword: '', newPassword: '', confirmNewPassword: '' });
      setIsLoading(false);
    } catch (err) {
      setError('Failed to change password');
      setIsLoading(false);
    }
  }, [passwordData]);

  // Render general settings section
  const renderGeneralSettings = (): JSX.Element => (
    <Stack tokens={{ childrenGap: 16 }}>
      <h2>General Settings</h2>
      <Dropdown
        label="Language"
        selectedKey={settings?.language}
        onChange={(_, option) => handleSettingChange('language')(option?.key)}
        options={[
          { key: 'en', text: 'English' },
          { key: 'es', text: 'Spanish' },
          { key: 'fr', text: 'French' },
        ]}
      />
      <Dropdown
        label="Theme"
        selectedKey={settings?.theme}
        onChange={(_, option) => handleSettingChange('theme')(option?.key)}
        options={[
          { key: 'light', text: 'Light' },
          { key: 'dark', text: 'Dark' },
        ]}
      />
      <Toggle
        label="Auto Save"
        checked={settings?.autoSave}
        onChange={(_, checked) => handleSettingChange('autoSave')(checked)}
      />
      <Toggle
        label="Notifications"
        checked={settings?.notifications}
        onChange={(_, checked) => handleSettingChange('notifications')(checked)}
      />
    </Stack>
  );

  // Render account settings section
  const renderAccountSettings = (): JSX.Element => (
    <Stack tokens={{ childrenGap: 16 }}>
      <h2>Account Settings</h2>
      <TextField label="Username" value={settings?.username} readOnly />
      <TextField label="Email" value={settings?.email} readOnly />
    </Stack>
  );

  // Render password change form
  const renderPasswordChange = (): JSX.Element => (
    <Stack tokens={{ childrenGap: 16 }}>
      <h2>Change Password</h2>
      <TextField
        label="Current Password"
        type="password"
        value={passwordData.currentPassword}
        onChange={handlePasswordChange('currentPassword')}
      />
      <TextField
        label="New Password"
        type="password"
        value={passwordData.newPassword}
        onChange={handlePasswordChange('newPassword')}
      />
      <TextField
        label="Confirm New Password"
        type="password"
        value={passwordData.confirmNewPassword}
        onChange={handlePasswordChange('confirmNewPassword')}
      />
      <PrimaryButton text="Change Password" onClick={handleChangePassword} />
    </Stack>
  );

  return (
    <div className={styles.container}>
      <Header />
      {error && (
        <MessageBar messageBarType={MessageBarType.error}>{error}</MessageBar>
      )}
      {successMessage && (
        <MessageBar messageBarType={MessageBarType.success}>{successMessage}</MessageBar>
      )}
      {isLoading ? (
        <p>Loading...</p>
      ) : (
        <Stack tokens={{ childrenGap: 32 }}>
          {renderGeneralSettings()}
          {renderAccountSettings()}
          {renderPasswordChange()}
          <PrimaryButton text="Save Settings" onClick={handleSaveSettings} />
        </Stack>
      )}
      <Footer />
    </div>
  );
};

export default Settings;