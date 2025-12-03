import React, {useState, useEffect} from "react";
import { View, Text, StyleSheet, TouchableOpacity, ScrollView, Modal } from 'react-native';
import Slider from '@react-native-community/slider';
import {Picker} from '@react-native-picker/picker';
import { Buffer } from 'buffer';
import { bleService } from '../services/BLEService';
import { supabase } from "../../backend/app/services/supabase";
//import { start } from "repl";


export default function LEDModeScreen() {    
    // Whack-A-Mole States
    const [gameRunning, setGameRunning] = useState(false);
    const [hits, setHits] = useState(0);
    const [attempts, setAttempts] = useState(0);

    //LED Reg Mode states
    const [ledRegRunning, setLedRegRunning] = useState(false);

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
          const eventCode = bytes[0];
          if (eventCode === 0x82) { 
            const newHits = bytes[1];
            const newAttempts = bytes[2];
            setHits(newHits);
            setAttempts(newAttempts);
            console.log("Game Update:", bytes);
            }
        });
        return () => bleService.disableNotifications();
    }, []);

    // Whack-A-Mole
    const startWhackGame = async () => {
        await sendFrame([0x04, 0x03, 0x00]); // example CMD: start whack
        //await sendFrame([0x04, 0x05, 0x00]); //stop reg
        setGameRunning(true);
        setLedRegRunning(false);
    };

    //STATS: Can collect data from here 
    const stopWhackGame = async () => {
        await sendFrame([0x04, 0x02, 0x00]); // example CMD: stop
        setGameRunning(false);
        //when the game stops, the variable hit is the number of points, and attempt is the number of tries (based on IR sensor)
        //everytime the game stops, aka when this function is called you can add those numbers with a date and time stamp as an entry to a DB
        try {
          // get logged in user
          const { data: { user }, error: userError } = await supabase.auth.getUser();

          if (userError || !user) {
            console.error("User not logged in; cannot save stats.");
            return;
          }
          const timestamp = new Date().toISOString();

          const { error } = await supabase.from("stats").insert([
            {
              user_id: user.id,
              hits: hits,
              attempts: attempts,
              created_at: timestamp,
              mode: "whack-a-mole"
            },
          ]);
          
          if (error) {
            console.error("Error inserting stats:", error.message);
          } else {
            console.log("Stats inserted successfully");
          } 
        } catch (error) {
          console.error("Error inserting stats:", error);
        };       
    };

    // LED Regular Mode
    const startLedRegGame = async () => {
        await sendFrame([0x04, 0x04, 0x00]); // example CMD: start LED regular mode
        //await sendFrame([0x04, 0x02, 0x00]); //stop whack
        setGameRunning(false);
        setLedRegRunning(true);
    }

    const stopLedRegGame = async () => {
        await sendFrame([0x04, 0x05, 0x00]); // example CMD: stop LED regular mode
        setLedRegRunning(false);
    }


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

      <View style={styles.card}>
        <Text style={styles.sectionHeader}>LED Regular</Text>

        {/* Description */}
        <Text style={styles.description}>
          Description: Push buttons to turn them on and off.
        </Text>

        {/* Start / Stop buttons */}
        <View style={[styles.row, { marginTop: 20 }]}>
          <TouchableOpacity
            style={[styles.button, { opacity: ledRegRunning ? 0.5 : 1 }]}
            onPress={startLedRegGame}
            disabled={ledRegRunning}
          >
            <Text style={styles.buttonText}>Start</Text>
          </TouchableOpacity>

          <TouchableOpacity
            style={[styles.buttonOff, { opacity: ledRegRunning ? 1 : 0.5 }]}
            onPress={stopLedRegGame}
            disabled={!ledRegRunning}
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