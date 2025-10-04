// src/frontend/screens/HomeScreen.tsx
import React, { useEffect, useRef, useState } from 'react';
import { View, Text, StyleSheet, Button, Alert, PermissionsAndroid, Platform } from 'react-native';
import { BleManager, Device, Characteristic } from 'react-native-ble-plx';
import { Buffer } from 'buffer';

const TOY_NAME = 'JoyLabToy';
const SERVICE_UUID = '00001234-0000-1000-8000-00805f9b34fb';
const CHAR_UUID    = '0000abcd-0000-1000-8000-00805f9b34fb';

export default function HomeScreen() {
  const [ready, setReady] = useState(false);        // BLE runtime ready?
  const [connected, setConnected] = useState(false);
  const managerRef = useRef<BleManager | null>(null);
  const deviceRef = useRef<Device | null>(null);
  const charRef = useRef<Characteristic | null>(null);

  useEffect(() => {
    // create after mount; destroy on unmount
    managerRef.current = new BleManager();
    setReady(true);
    return () => { managerRef.current?.destroy(); managerRef.current = null; };
  }, []);

  const ensurePermissions = async () => {
    if (Platform.OS !== 'android') return;
    await PermissionsAndroid.requestMultiple([
      'android.permission.BLUETOOTH_SCAN',
      'android.permission.BLUETOOTH_CONNECT',
      PermissionsAndroid.PERMISSIONS.ACCESS_FINE_LOCATION,
    ]);
  };

  const connectToDevice = async () => {
    try {
      if (!ready || !managerRef.current) throw new Error('BLE runtime not ready yet');
      await ensurePermissions();
      Alert.alert('BLE', 'Scanning for device...');

      const mgr = managerRef.current; // narrow non-null

      const device = await new Promise<Device>((resolve, reject) => {
        const timeout = setTimeout(() => {
          mgr.stopDeviceScan();
          reject(new Error('Device not found'));
        }, 8000);

        mgr.startDeviceScan(null, null, async (error, d) => {
          if (error) {
            clearTimeout(timeout);
            mgr.stopDeviceScan();
            return reject(error);
          }
          if (d?.name === TOY_NAME) {
            clearTimeout(timeout);
            mgr.stopDeviceScan();
            try {
              await d.connect();
              await d.discoverAllServicesAndCharacteristics();
              resolve(d);
            } catch (e) { reject(e); }
          }
        });
      });

      deviceRef.current = device;

      const chars = await device.characteristicsForService(SERVICE_UUID);
      const target = CHAR_UUID.toLowerCase();
      const found = chars.find(c => c.uuid.toLowerCase() === target);
      if (!found) throw new Error('Characteristic not found');
      charRef.current = found;

      setConnected(true);
      Alert.alert('BLE', 'Connected to device ✅');
    } catch (e: any) {
      Alert.alert('BLE Error', e?.message ?? String(e));
    }
  };

  const toggleLED = async () => {
    if (!connected || !charRef.current) {
      return Alert.alert('BLE', 'Not connected yet');
    }
    try {
      const payload = Buffer.from([0x10]).toString('base64'); // 0x10 = toggle
      await charRef.current.writeWithoutResponse(payload);
      Alert.alert('LED', 'Toggle command sent');
    } catch (e: any) {
      Alert.alert('Error', e?.message ?? String(e));
    }
  };

  return (
    <View style={styles.container}>
      <Text style={styles.title}>Homepage</Text>
      <Button title={ready ? "Connect to Device" : "Starting BLE..."} onPress={connectToDevice} disabled={!ready} />
      <View style={{ height: 20 }} />
      <Button title="Toggle LED" onPress={toggleLED} disabled={!connected} />
    </View>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1, justifyContent: 'center', alignItems: 'center' },
  title: { fontSize: 20, marginBottom: 20, fontWeight: '600' },
});
