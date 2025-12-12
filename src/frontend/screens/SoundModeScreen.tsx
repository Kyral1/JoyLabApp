import React, { useState, useEffect } from "react";
import { View, Text, StyleSheet, TouchableOpacity, ScrollView } from 'react-native';
import { Picker } from '@react-native-picker/picker';
import { Buffer } from 'buffer';
import { bleService } from '../services/BLEService';
import { bleWristbandService } from '../services/BLEWristBandService';


export default function SoundModeScreen() {
  const [gameRunning, setGameRunning] = useState(false);
  const [hits, setHits] = useState(0);
  const [attempts, setAttempts] = useState(0);

  const [motionDetected, setMotionDetected] = useState(0);
  const [motionRunning, setMotionRunning] = useState(false);

    const sendWBFrame = async (frame: number[]) => {
        try{
            await bleWristbandService.sendControlFrame(frame);
            console.log("Frame sent:", frame);
        } catch (e: any) {
            console.error('Error sending frame to wristband:', e?.message ?? String(e));
        }
    };

    useEffect(() => {
        const sub = bleWristbandService.enableNotifications((data: string) => {
          const bytes = Buffer.from(data, 'base64');
          const eventCode = bytes[0];
          if (eventCode === 0x87) {
            const newMotionDetected = bytes[1];
            setMotionDetected(newMotionDetected);
            console.log("Motion Detected Update:", bytes);
          }
        });
        return () => bleWristbandService.disableIMUNotifications();
    }, []);

    const startMotionDetection = async () => {
        await sendWBFrame([0x1, 0x01, 0x00]); // example CMD: start motion detection
        setMotionRunning(true);
    }

    const stopMotionDetection = async () => {
        await sendWBFrame([0x1, 0x02, 0x00]); // example CMD: stop motion detection
        setMotionRunning(false);
    }

  // BLE Frame Function 
  const sendFrame = async (frame: number[]) => {
    try {
      await bleService.sendControlFrame(frame);
      console.log("Frame sent:", frame);
    } catch (e: any) {
      console.error('Error sending frame:', e?.message ?? String(e));
    }
  };

  const startSoundGame = async () => {
    await sendFrame([0x04, 0x06, 0x00]); // example CMD: start whack
    //await sendFrame([0x04, 0x05, 0x00]); //stop reg
      setGameRunning(true);
      startMotionDetection();
    };

    const stopSoundGame = async () => {
        await sendFrame([0x04, 0x07, 0x00]); // example CMD: stop
        setGameRunning(false);
        stopMotionDetection();
    };

  // Handle BLE notifications - if no notifications for sound can delete
  useEffect(() => {
    const sub = bleService.enableNotifications((data: string) => {
      const bytes = Buffer.from(data, 'base64');
      const eventCode = bytes[0];
      if (eventCode === 0x84) {
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
        <Text style={styles.sectionHeader}>Sound Mode Game</Text>

        {/* Description */}
        <Text style={styles.description}>
          Description: Choose sounds for the buttons in settings. Press Buttons to turn on and off sounds.
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
          <View style={styles.scoreBox}>
            <Text style={styles.scoreLabel}>Motion</Text>
            <Text style={styles.scoreValue}>{motionDetected}</Text>
          </View>
        </View>

        {/* Start / Stop buttons */}
        <View style={[styles.row, { marginTop: 20 }]}>
          <TouchableOpacity
            style={[styles.button, { opacity: gameRunning ? 0.5 : 1 }]}
            onPress={startSoundGame}
            disabled={gameRunning}
          >
            <Text style={styles.buttonText}>Start</Text>
          </TouchableOpacity>

          <TouchableOpacity
            style={[styles.buttonOff, { opacity: gameRunning ? 1 : 0.5 }]}
            onPress={stopSoundGame}
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