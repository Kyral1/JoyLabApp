import React from 'react';
import { View, Text, StyleSheet, Button, Alert } from 'react-native';
import { Buffer } from 'buffer';
import { bleService } from '../services/BLEService'; 

export default function GameModesScreen() {
  console.log("🎮 GameModesScreen loaded!");

    const toggleLED = async () => {
    try {
      const frame = Buffer.from([0x01, 0x02, 0x01, 0x63]).toString('base64');
      await bleService.sendControlFrame([0x01, 0x02, 0x01, 0x63]);
      Alert.alert('LED', 'Toggle command sent ✅');
    } catch (e: any) {
      Alert.alert('Error', e?.message ?? String(e));
    }
  };
    
  return (
    <View style={styles.container}>
      <Text>Game Modes</Text>
      <Button title="Toggle LED Strip" onPress={toggleLED} />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
});
