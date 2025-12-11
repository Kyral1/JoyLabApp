import React, { useState, useEffect } from "react";
import { View, Text, StyleSheet, ScrollView, TouchableOpacity } from 'react-native';
import { Picker } from '@react-native-picker/picker';
import { Buffer } from 'buffer';
import { bleService } from '../services/BLEService';

export default function DualModeScreen() {
  const [gameRunning, setGameRunning] = useState(false);
  const [hits, setHits] = useState(0);
  const [attempts, setAttempts] = useState(0);
  // BLE Frame Function 
  const sendFrame = async (frame: number[]) => {
    try {
      await bleService.sendControlFrame(frame);
      console.log("Frame sent:", frame);
    } catch (e: any) {
      console.error('Error sending frame:', e?.message ?? String(e));
    }
  };

  const startDualGame = async () => {
    await sendFrame([0x04, 0x06, 0x00]); // example CMD: start whack
    //await sendFrame([0x04, 0x05, 0x00]); //stop reg
      setGameRunning(true);
    };

    const stopDualGame = async () => {
        await sendFrame([0x04, 0x07, 0x00]); // example CMD: stop
        setGameRunning(false);
    };

  // Handle BLE notifications - if no notifications for sound can delete
  useEffect(() => {
    const sub = bleService.enableNotifications((data: string) => {
      const bytes = Buffer.from(data, 'base64');
      const eventCode = bytes[0];
      if (eventCode === 0x85) {
        const newHits = bytes[1];
        const newAttempts = bytes[2];
        setHits(newHits);
        setAttempts(newAttempts);
        console.log("Game Update:", bytes);
        // handle Speaker state updates here
      }
    });
    return () => bleService.disableNotifications();
  }, []);

return (
    <ScrollView contentContainerStyle={styles.container}>
      <View style={styles.card}>
        <Text style={styles.sectionHeader}>Dual Mode Game</Text>

        {/* Description */}
        <Text style={styles.description}>
          Description: Press buttons to turn on sound and lights associated with each button.
        </Text>

        {/* Scoreboard */}
        <View style={styles.scoreboard}>
          <View style={styles.scoreBox}>
            <Text style={styles.scoreLabel}>Hits</Text>
            <Text style={styles.scoreValue}>{hits}</Text>
          </View>
          <View style={styles.scoreBox}>
            <Text style={styles.scoreLabel}>Attempts</Text>
            <Text style={styles.scoreValue}>{attempts}</Text>
          </View>
        </View>

        {/* Start / Stop buttons */}
        <View style={[styles.row, { marginTop: 20 }]}>
          <TouchableOpacity
            style={[styles.button, { opacity: gameRunning ? 0.5 : 1 }]}
            onPress={startDualGame}
            disabled={gameRunning}
          >
            <Text style={styles.buttonText}>Start</Text>
          </TouchableOpacity>

          <TouchableOpacity
            style={[styles.buttonOff, { opacity: gameRunning ? 1 : 0.5 }]}
            onPress={stopDualGame}
            disabled={!gameRunning}
          >
            <Text style={styles.buttonText}>Stop</Text>
          </TouchableOpacity>
        </View>
      </View>
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: "#F9FAFF",
    paddingTop: 50,
    paddingHorizontal: 30,
  },
  sectionHeader: {
    fontSize: 20,
    fontWeight: "600",
    marginBottom: 10,
    color: "#2C3E50",
  },
  description: {
    fontSize: 14,
    color: "#5D6D7E",
    marginBottom: 20,
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
  row: {
    flexDirection: "row",
    justifyContent: "center",
    alignItems: "center",
  },
  button: {
    backgroundColor: "#C9D6FF",
    borderRadius: 15,
    paddingVertical: 10,
    paddingHorizontal: 30,
    marginRight: 10,
  },
  buttonOff: {
    backgroundColor: "#E6B0AA",
    borderRadius: 15,
    paddingVertical: 10,
    paddingHorizontal: 30,
  },
  buttonText: {
    color: "#2C3E50",
    fontWeight: "600",
  },
  scoreboard: {
    flexDirection: "row",
    justifyContent: "space-evenly",
    marginBottom: 12,
  },
  scoreBox: {
    backgroundColor: "#F4F6F7",
    borderRadius: 12,
    paddingVertical: 8,
    paddingHorizontal: 20,
    alignItems: "center",
  },
  scoreLabel: {
    fontSize: 14,
    color: "#566573",
  },
  scoreValue: {
    fontSize: 18,
    fontWeight: "700",
    color: "#2C3E50",
  },
});