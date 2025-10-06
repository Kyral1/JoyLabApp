import React from 'react';
import { View, Text, StyleSheet, TouchableOpacity, Alert} from 'react-native';
import { Buffer } from 'buffer';
import { bleService } from '../services/BLEService'; 

export default function GameModesScreen() {
  console.log("🎮 GameModesScreen loaded!");

    const handleLEDMode = async () => {
    try {
      const frame = Buffer.from([0x01, 0x02, 0x01, 0x63]).toString('base64');
      await bleService.sendControlFrame([0x01, 0x02, 0x01, 0x63]);
      Alert.alert('LED', 'Toggle command sent ✅');
    } catch (e: any) {
      Alert.alert('Error', e?.message ?? String(e));
    }
  };

  const handleSoundMode = () => {
    Alert.alert('Sound Mode', 'Sound mode feature in progress');
  };

  const handleDualMode = () => {
    Alert.alert('Dual Mode', 'Dual mode feature in progress');
  };

  return (
    <View style={styles.container}>
      <Text style={styles.header}>Modes</Text>

      <TouchableOpacity style={styles.modeButton} onPress={handleLEDMode}>
        <Text style={styles.modeText}>LED Mode</Text>
      </TouchableOpacity>

      <TouchableOpacity style={styles.modeButton} onPress={handleSoundMode}>
        <Text style={styles.modeText}>Sound Mode</Text>
      </TouchableOpacity>

      <TouchableOpacity style={styles.modeButton} onPress={handleDualMode}>
        <Text style={styles.modeText}>Dual Mode</Text>
      </TouchableOpacity>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#F9FAFF',
    paddingTop: 50,
    paddingHorizontal: 30,
  },
  header: {
    fontSize: 28,
    fontWeight: '700',
    color: '#2C3E50',
    marginBottom: 30,
  },
  modeButton: {
    backgroundColor: '#C9D6FF',
    borderRadius: 20,
    paddingVertical: 20,
    paddingHorizontal: 25,
    marginVertical: 10,
    shadowColor: '#000',
    shadowOpacity: 0.1,
    shadowRadius: 5,
    elevation: 3,
  },
  modeText: {
    fontSize: 18,
    fontWeight: '500',
    color: '#2C3E50',
  },
});
   