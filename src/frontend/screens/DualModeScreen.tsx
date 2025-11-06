import React, { useState, useEffect } from "react";
import { View, Text, StyleSheet, ScrollView } from 'react-native';
import { Picker } from '@react-native-picker/picker';
import { Buffer } from 'buffer';
import { bleService } from '../services/BLEService';

export default function DualModeScreen() {
  // BLE Frame Function 
  const sendFrame = async (frame: number[]) => {
    try {
      await bleService.sendControlFrame(frame);
      console.log("Frame sent:", frame);
    } catch (e: any) {
      console.error('Error sending frame:', e?.message ?? String(e));
    }
  };

  // Handle BLE notifications - if no notifications for sound can delete
  useEffect(() => {
    const sub = bleService.enableNotifications((data: string) => {
      const bytes = Buffer.from(data, 'base64');
      if (bytes[0] === 0xA0) {
        console.log("Sound notification:", bytes);
        // handle Speaker state updates here
      }
    });
    return () => bleService.disableNotifications();
  }, []);

  return (
    <ScrollView contentContainerStyle={styles.container}>
      <View style={styles.card}>
        <Text style={styles.sectionHeader}>LED</Text>
      </View>

      <View style={styles.card}>
        <Text style={styles.sectionHeader}>Sound</Text>
      </View>
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#F9FAFF',
    paddingTop: 50,
    paddingHorizontal: 30
  },
  header: {
    fontSize: 28,
    fontWeight: '700',
    color: '#2C3E50',
    marginBottom: 30
  },
  sectionHeader: {
    fontSize: 20,
    fontWeight: "600",
    marginBottom: 10,
    color: "#2C3E50",
  },
  card: {
    backgroundColor: "#FFFFFF",
    borderRadius: 20,
    padding: 20,
    marginBottom: 25,
    shadowColor: "#000",
    shadowOpacity: 0.1,
    shadowRadius: 6,
    elevation: 3,
  },
  label: {
    fontSize: 18,
    color: '#2C3E50',
    fontWeight: '500',
  },
  row: {
    flexDirection: "row",
    justifyContent: "space-between",
    alignItems: "center",
    marginVertical: 8,
  },
  picker: {
    height: 90,
    width: 140,
  },
});
