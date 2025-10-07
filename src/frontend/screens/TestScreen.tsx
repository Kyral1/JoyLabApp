import React, {useState} from "react";
import {View, Text, StyleSheet, TouchableOpacity, Alert, ScrollView } from "react-native";
import Slider from '@react-native-community/slider';
import {Picker} from '@react-native-picker/picker';
import { Buffer } from 'buffer';
import { bleService } from '../services/BLEService'; 

export default function TestScreen() {
    console.log("🧪 TestScreen loaded!");

    // LED States -- when the app is closed and repened, it will reset to these default states
    const [selectedLED, setSelectedLED] = useState('All');
    const [brightness, setBrightness] = useState(0.5); //Scaled from 0-1 based on slider position
    const [currentColor, setCurrentColor] = useState({ r: 255, g: 255, b: 255 }); //object {r,g,b}

    //Button Test States
    const [buttonTestActive, setButtonTestActive] = useState(false);

    //BLE Frame Function 
    const sendFrame = async (frame: number[]) => {
        try{
            await bleService.sendControlFrame(frame);
            console.log("Frame sent:", frame);
        } catch (e: any) {
            console.error('Error sending frame:', e?.message ?? String(e));
        }
    };

    //LED Control Handlers
    //Toggle LED On/Off
    const handleToggleLED = (on:boolean) => {
        const idx = selectedLED === "All" ? 255 : parseInt(selectedLED) - 1;
        const { r, g, b } = currentColor;
        const br = Math.round(brightness * 100); //slider placement

        const frame = [0x01, 0x01, 0x05, idx, on ? r : 0, on ? g : 0, on ? b : 0, br];
        sendFrame(frame);
    }

    //Change LED Color
    const handleChangeColor = (color: {r: number; g: number; b:number})=>{
        const idx = selectedLED === "All" ? 255 : parseInt(selectedLED) - 1;
        const br = Math.round(brightness * 100);
        const frame = [0x01,0x01, 0x05, idx, color.r, color.g, color.b, br];
        sendFrame(frame);
    }

    //Change LED Brightness
    const handleBrightnessChange = (val: number) => {
        setBrightness(val); //updates the state to remember the slider position
        const br = Math.round(val * 100);
        const frame = [0x01, 0x03, 0x01, br]; // CAT=1, CMD=3
        sendFrame(frame);
    }

    //button test handler
    /*
    const handleInitializeButtonTest = () => {
        setButtonTestActive(true);
        console.log("🟢 Button test initialized: LED0 ON");
    }*/

    return (
    <ScrollView contentContainerStyle={styles.container}>
      <Text style={styles.header}>Hardware BLE Testing (temp screen)</Text>

      {/* ========== LED TEST SECTION ========== */}
      <View style={styles.card}>
        <Text style={styles.sectionHeader}>LED Test (Indv & ALL)</Text>

        {/* LED Selector */}
        <View style={styles.row}>
          <Text style={styles.label}>Select LED:</Text>
          <Picker
            selectedValue={selectedLED}
            style={styles.picker}
            onValueChange={(val) => setSelectedLED(val)}
          >
            <Picker.Item label="All" value="All" />
            <Picker.Item label="LED 1" value="1" />
            <Picker.Item label="LED 2" value="2" />
            <Picker.Item label="LED 3" value="3" />
            <Picker.Item label="LED 4" value="4" />
          </Picker>
        </View>

        {/* Toggle Buttons */}
        <View style={styles.row}>
          <TouchableOpacity style={styles.button} onPress={() => handleToggleLED(true)}>
            <Text style={styles.buttonText}>Turn ON</Text>
          </TouchableOpacity>
          <TouchableOpacity style={styles.buttonOff} onPress={() => handleToggleLED(false)}>
            <Text style={styles.buttonText}>Turn OFF</Text>
          </TouchableOpacity>
        </View>

        {/* Color Options */}
        <Text style={[styles.label, { marginTop: 15 }]}>Change Color:</Text>
        <View style={styles.colorRow}>
          {[
            { name: "Red", color: "#E74C3C", rgb: { r: 255, g: 0, b: 0 } },
            { name: "Green", color: "#27AE60", rgb: { r: 0, g: 255, b: 0 } },
            { name: "Blue", color: "#2980B9", rgb: { r: 0, g: 0, b: 255 } },
            { name: "Yellow", color: "#F1C40F", rgb: { r: 255, g: 255, b: 0 } },
            { name: "White", color: "#FFFFFF", rgb: { r: 255, g: 255, b: 255 } },
          ].map((c) => (
            <TouchableOpacity
              key={c.name}
              style={[styles.colorBtn, { backgroundColor: c.color }]}
              onPress={() => {setCurrentColor(c.rgb);  handleChangeColor(c.rgb);}} //change color + remember color
            />
          ))}
        </View>

        {/* Brightness */}
        <Text style={[styles.label, { marginTop: 20 }]}>Brightness</Text>
        <Slider
          style={styles.slider}
          value={brightness}
          onValueChange={handleBrightnessChange}
          minimumValue={0}
          maximumValue={1}
          minimumTrackTintColor="#4A7FFB"
          maximumTrackTintColor="#D6DBDF"
        />
      </View>
    </ScrollView>
  );
}

// =================== STYLES ===================
const styles = StyleSheet.create({
  container: {
    flexGrow: 1,
    backgroundColor: "#F9FAFF",
    paddingHorizontal: 25,
    paddingTop: 40,
    paddingBottom: 40,
  },
  header: {
    fontSize: 28,
    fontWeight: "700",
    textAlign: "center",
    color: "#4A7FFB",
    marginBottom: 25,
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
  picker: {
    height: 90,
    width: 140,
  },
  label: {
    fontSize: 16,
    color: "#2C3E50",
    fontWeight: "500",
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
  slider: {
    width: "100%",
    height: 40,
  },
  simBtn: {
    backgroundColor: "#C9D6FF",
    borderRadius: 10,
    padding: 10,
    margin: 5,
    flex: 1,
    alignItems: "center",
  },
  simBtnText: {
    fontWeight: "500",
    color: "#2C3E50",
  },
});
