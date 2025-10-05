// src/frontend/screens/HomeScreen.tsx
import React, { useState } from 'react';
import { View, Text, StyleSheet, Button, Alert } from 'react-native';
import { bleService } from '../services/BLEService';

export default function HomeScreen() {
  console.log("🎮 HomeScreen loaded!");
  const [connected, setConnected] = useState(false);
  const [connecting, setConnecting] = useState(false);

  // Connect to the device using shared BLE service
  const handleConnect = async () => {
    try {
      setConnecting(true);
      await bleService.connect();
      setConnected(true);
      Alert.alert('BLE', 'Connected to JoyLabToy ✅');
    } catch (e: any) {
      Alert.alert('BLE Error', e?.message ?? String(e));
    } finally {
      setConnecting(false);
    }
  };

  return (
    <View style={styles.container}>
      <Text style={styles.title}>JoyLab Controller!</Text>

      <Button
        title={
          connecting
            ? 'Connecting...'
            : connected
            ? 'Connected ✅'
            : 'Connect to Device'
        }
        onPress={handleConnect}
        disabled={connecting || connected}
      />

      <View style={{ height: 20 }} />
    </View>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1, justifyContent: 'center', alignItems: 'center' },
  title: { fontSize: 22, marginBottom: 20, fontWeight: '600' },
});
