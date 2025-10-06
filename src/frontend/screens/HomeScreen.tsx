// src/frontend/screens/HomeScreen.tsx
import React, { useState } from 'react';
import { View, Text, StyleSheet, Button, Alert, Pressable, TouchableOpacity, Modal} from 'react-native';
import { bleService } from '../services/BLEService';


export default function HomeScreen() {
  console.log("🎮 HomeScreen loaded!");
  const [connected, setConnected] = useState(false);
  const [connecting, setConnecting] = useState(false);
  const [showModal, setShowModal] = useState(false);

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
      {/* Header */}
      <Text style={styles.welcomeText}>Welcome to</Text>
      <Text style={styles.logoText}>
        <Text style={{ color: '#4A7FFB' }}>Joy</Text>
        <Text style={{ color: '#E74C3C' }}>LAB</Text>
      </Text>

      {/* Buttons */}
      <TouchableOpacity
        style={[styles.button, connected && { backgroundColor: '#A5D6A7' }]}
        onPress={handleConnect}
        disabled={connecting || connected}
      >
        <Text style={styles.buttonText}>
          {connecting ? 'Connecting...' : connected ? 'Connected ✅' : 'Connect Devices'}
        </Text>
      </TouchableOpacity>

      <TouchableOpacity style={styles.button} onPress={() => setShowModal(true)}>
        <Text style={styles.buttonText}>How to Use</Text>
      </TouchableOpacity>

      {/* Modal popup */}
      <Modal visible={showModal} transparent animationType="fade">
        <View style={styles.modalBackdrop}>
          <View style={styles.modalContent}>
            <Text style={styles.modalTitle}>How to Use</Text>
            <Text style={styles.modalBody}>
              1. Turn on your JoyLab device.{"\n"}
              2. Tap “Connect Devices” to pair via Bluetooth.{"\n"}
              3. Once connected, use the Game and Settings tabs to interact!
            </Text>
            <Pressable style={styles.closeButton} onPress={() => setShowModal(false)}>
              <Text style={styles.closeButtonText}>✕</Text>
            </Pressable>
          </View>
        </View>
      </Modal>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#F9FAFF',
    alignItems: 'center',
    justifyContent: 'center',
    paddingHorizontal: 20,
  },
  welcomeText: {
    fontSize: 22,
    fontWeight: '500',
    color: '#333',
    marginBottom: 4,
  },
  logoText: {
    fontSize: 48,
    fontWeight: '800',
    marginBottom: 40,
  },
  button: {
    backgroundColor: '#C9D6FF',
    borderRadius: 20,
    paddingVertical: 12,
    paddingHorizontal: 40,
    marginVertical: 10,
    shadowColor: '#000',
    shadowOpacity: 0.15,
    shadowRadius: 6,
    elevation: 3,
  },
  buttonText: {
    fontSize: 16,
    fontWeight: '600',
    color: '#2C3E50',
  },
  modalBackdrop: {
    flex: 1,
    backgroundColor: 'rgba(0,0,0,0.35)',
    alignItems: 'center',
    justifyContent: 'center',
  },
  modalContent: {
    backgroundColor: '#fff',
    borderRadius: 20,
    width: '80%',
    padding: 25,
    shadowColor: '#000',
    shadowOpacity: 0.3,
    shadowRadius: 10,
    elevation: 8,
  },
  modalTitle: {
    fontSize: 20,
    fontWeight: '700',
    color: '#4A7FFB',
    marginBottom: 12,
    textAlign: 'center',
  },
  modalBody: {
    fontSize: 15,
    color: '#333',
    lineHeight: 22,
    marginBottom: 15,
  },
  closeButton: {
    alignSelf: 'center',
    backgroundColor: '#4A7FFB',
    borderRadius: 20,
    paddingVertical: 8,
    paddingHorizontal: 20,
  },
  closeButtonText: {
    color: '#fff',
    fontWeight: '600',
    fontSize: 16,
  },
});
