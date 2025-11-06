import React, {useState, useEffect} from "react";
import { View, Text, StyleSheet, TouchableOpacity, ScrollView, Modal } from 'react-native';
import Slider from '@react-native-community/slider';
import {Picker} from '@react-native-picker/picker';
import { Buffer } from 'buffer';
import { bleService } from '../services/BLEService';


export default function LEDModeScreen() {    
    // Whack-A-Mole States
    const [gameRunning, setGameRunning] = useState(false);
    const [points, setPoints] = useState(0);
    const [attempts, setAttempts] = useState(0);

    // BLE Frame Function 
    const sendFrame = async (frame: number[]) => {
        try{
            await bleService.sendControlFrame(frame);
            console.log("Frame sent:", frame);
        } catch (e: any) {
            console.error('Error sending frame:', e?.message ?? String(e));
        }
    };

    // Handle BLE notifications - if no notifications for LED can delete
    useEffect(() => {
        const sub = bleService.enableNotifications((data: string) => {
            const bytes = Buffer.from(data, 'base64');
            if (bytes[0] === 0xA0) { 
                console.log("Game Update:", bytes);
                // handle LED state updates here
            }
        });
        return () => bleService.disableNotifications();
    }, []);

    // Whack-A-Mole
    const startWhackGame = async () => {
        await sendFrame([0x04, 0x03, 0x00]); // example CMD: start
        setPoints(0);
        setAttempts(0);
        setGameRunning(true);
    };

    const stopWhackGame = async () => {
        await sendFrame([0x04, 0x02, 0x00]); // example CMD: stop
        setGameRunning(false);
    };
 return (
    <ScrollView contentContainerStyle={styles.container}>
      <View style={styles.card}>
        <Text style={styles.sectionHeader}>LED Whack-A-Mole</Text>

        {/* Description */}
        <Text style={styles.description}>
          Description: Choose colors for the buttons in settings. One button will turn on at a time, press it to turn it off.
        </Text>

        {/* Scoreboard */}
        <View style={styles.scoreboard}>
          <View style={styles.scoreBox}>
            <Text style={styles.scoreLabel}>Points</Text>
            <Text style={styles.scoreValue}>{points}</Text>
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
            onPress={startWhackGame}
            disabled={gameRunning}
          >
            <Text style={styles.buttonText}>Start</Text>
          </TouchableOpacity>

          <TouchableOpacity
            style={[styles.buttonOff, { opacity: gameRunning ? 1 : 0.5 }]}
            onPress={stopWhackGame}
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