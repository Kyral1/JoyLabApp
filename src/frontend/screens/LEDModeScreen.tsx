import React, {useState, useEffect} from "react";
import { View, Text, StyleSheet, TouchableOpacity, ScrollView, Modal } from 'react-native';
import Slider from '@react-native-community/slider';
import {Picker} from '@react-native-picker/picker';
import { Buffer } from 'buffer';
import { bleService } from '../services/BLEService';


export default function LEDModeScreen() {    
    // Whack-A-Mole States
    const [gameRunning, setGameRunning] = useState(false);
    const [gameSpeed, setGameSpeed] = 
        useState<'slow' | 'normal' | 'fast' | 'custom'>('normal');
    const [intervalMs, setIntervalMs] = useState(600);
    const [rounds, setRounds] = useState(20);

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
                console.log("LED notification:", bytes);
                // handle LED state updates here
            }
        });
        return () => bleService.disableNotifications();
    }, []);

    // Whack-A-Mole
    const lo = (n: number) => n & 0xff;
    const hi = (n: number) => (n >> 8) & 0xff;

    // Map speed - NEEDS EDITING MAYBE
    const speedToCode = (s: 'slow' | 'normal' | 'fast' | 'custom') => {
        switch (s) {
            case 'slow':   return 0x00;
            case 'normal': return 0x01;
            case 'fast':   return 0x02;
            case 'custom': return 0x03;
        }
    };

    // Game configurations - NEEDS EDITING
    const sendWhackConfig = async () => {
     const speedCode = speedToCode(gameSpeed);
    // If custom, use intervalMs; else let firmware pick based on speedCode
    const interval = gameSpeed === 'custom' ? intervalMs : 0;
    const frame = [
        0x02,      // CAT: Games (example)
        0x10,      // CMD: Configure whack-a-mole (example)
        speedCode, // speed
        lo(interval), hi(interval),   // custom interval ms
        lo(rounds),   hi(rounds),     // rounds (number of moles)
    ];
    await sendFrame(frame);
    };

    // Start the Game - NEEDS EDITING
    const startWhackGame = async () => {
        await sendWhackConfig(); // ensure device has latest settings
        const frame = [0x02, 0x11, 0x01]; // CMD: start (example)
        await sendFrame(frame);
        setGameRunning(true);
    };

    // Stop the Game - NEEDS EDITING
    const stopWhackGame = async () => {
        const frame = [0x02, 0x11, 0x00]; // CMD: stop (example)
        await sendFrame(frame);
        setGameRunning(false);
    };

    return (
    <ScrollView contentContainerStyle={styles.container}>
        <View style={styles.card}>
            <Text style={styles.sectionHeader}>LED Whack-A-Mole</Text>

            {/* Speed mode */}
            <View style={styles.row}>
                <Text style={styles.label}>Speed:</Text>
                <Picker
                    selectedValue={gameSpeed}
                    style={styles.picker}
                    onValueChange={(val) => setGameSpeed(val)}
                >
                    <Picker.Item label="Slow" value="slow" />
                    <Picker.Item label="Normal" value="normal" />
                    <Picker.Item label="Fast" value="fast" />
                    <Picker.Item label="Custom" value="custom" />
                </Picker>
            </View>

            {/* Custom interval (only when Custom) */}
            {gameSpeed === 'custom' && (
                <>
                    <Text style={[styles.label, { marginTop: 8 }]}>
                        LED On Time (ms): {intervalMs}
                    </Text>
                    <Slider
                        style={styles.slider}
                        value={intervalMs}
                        onValueChange={(v) => setIntervalMs(Math.round(v))}
                        minimumValue={100}
                        maximumValue={2000}
                        step={50}
                        minimumTrackTintColor="#4A7FFB"
                        maximumTrackTintColor="#D6DBDF"
                    />
                </>
            )}

            {/* Rounds */}
            <Text style={[styles.label, { marginTop: 8 }]}>Rounds: {rounds}</Text>
            <Slider
                style={styles.slider}
                value={rounds}
                onValueChange={(v) => setRounds(Math.round(v))}
                minimumValue={5}
                maximumValue={50}
                step={1}
                minimumTrackTintColor="#4A7FFB"
                maximumTrackTintColor="#D6DBDF"
            />

            {/* Actions */}
            <View style={[styles.row, { marginTop: 10 }]}>
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
    row: {
        flexDirection: "row",
        justifyContent: "space-between",
        alignItems: "center",
        marginVertical: 8,
    },
    label: {
        fontSize: 16,
        color: "#2C3E50",
        fontWeight: "500",
    },
    slider: {
        width: "100%",
        height: 40,
    },
    button: {
        backgroundColor: "#C9D6FF",
        borderRadius: 15,
        paddingVertical: 10,
        paddingHorizontal: 20,
        marginRight: 10,
    },
    buttonOff: {
        backgroundColor: "#E6B0AA",
        borderRadius: 15,
        paddingVertical: 10,
        paddingHorizontal: 20,
    },
    buttonText: {
        color: "#2C3E50",
        fontWeight: "600",
    },
    colorRow: {
        flexDirection: "row",
        justifyContent: "space-around",
        marginTop: 10,
    },
    colorBtn: {
        width: 35,
        height: 35,
        borderRadius: 20,
        borderWidth: 1,
        borderColor: "#ccc",
    },
    picker: {
        height: 90,
        width: 140,
    },
    modalBackdrop: {
        flex: 1,
        backgroundColor: 'rgba(0,0,0,0.35)',
        justifyContent: 'center',
        alignItems: 'center',
    },
    modalCard: {
        width: '88%',
        backgroundColor: '#fff',
        borderRadius: 16,
        padding: 16,
    },
    modalTitle: {
        fontSize: 18,
        fontWeight: '700',
        marginBottom: 8,
        color: '#2C3E50',
    },
    modalBtn: {
        marginTop: 12,
        alignSelf: 'center',
        paddingVertical: 8,
        paddingHorizontal: 16,
    },
    modalBtnText: {
        color: '#007AFF',
        fontSize: 16,
        fontWeight: '600',
    },
});
