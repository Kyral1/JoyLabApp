// src/frontend/screens/HomeScreen.tsx
import React, { useState } from 'react';
import { View, Text, StyleSheet, Button, Alert } from 'react-native';
import { bleService } from '../services/BLEService';

export default function HomeScreen() {
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

  // Send toggle command
  const toggleLED = async () => {
    try {
      if (!connected) return Alert.alert('BLE', 'Not connected yet');
      // Frame: [Cat=0x01][Cmd=0x02][Len=1][Payload=99]
      await bleService.sendControlFrame([0x01, 0x02, 0x01, 0x63]);
      Alert.alert('LED', 'Toggle command sent ✅');
    } catch (e: any) {
      Alert.alert('Error', e?.message ?? String(e));
    }
  };

  return (
    <View style={styles.container}>
      <Text style={styles.title}>JoyLab Controller</Text>

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

      <Button
        title="Toggle LEDs"
        onPress={toggleLED}
        disabled={!connected}
      />
    </View>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1, justifyContent: 'center', alignItems: 'center' },
  title: { fontSize: 22, marginBottom: 20, fontWeight: '600' },
});
