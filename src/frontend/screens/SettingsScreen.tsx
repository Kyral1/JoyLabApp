import React, {useState} from "react";
import { View, Text, Switch, StyleSheet } from "react-native";
import Slider from '@react-native-community/slider';
import { Buffer } from 'buffer';
import { bleService } from '../services/BLEService'; 

 export default function SettingsScreen() {
  console.log("⚙️ SettingsScreen loaded!");
  // --- Toggle states (placeholders)
  const [vibration, setVibration] = useState(true);
  const [setting1, setSetting1] = useState(false);
  const [setting2, setSetting2] = useState(false);
  const [setting3, setSetting3] = useState(true);
  const [setting4, setSetting4] = useState(false);

  // --- Slider states
  const [volume, setVolume] = useState(0.6);
  const [brightness, setBrightness] = useState(0.4);

  return (
    <View style={styles.container}>
      <Text style={styles.header}>Settings</Text>

      {/* Toggles */}
      <View style={styles.card}>
        <View style={styles.settingRow}>
          <Text style={styles.label}>Vibration</Text>
          <Switch
            value={vibration}
            onValueChange={setVibration}
            trackColor={{ false: '#D6DBDF', true: '#4A7FFB' }}
            thumbColor="#fff"
          />
        </View>

        <View style={styles.settingRow}>
          <Text style={styles.label}>Setting 1</Text>
          <Switch
            value={setting1}
            onValueChange={setSetting1}
            trackColor={{ false: '#D6DBDF', true: '#4A7FFB' }}
            thumbColor="#fff"
          />
        </View>

        <View style={styles.settingRow}>
          <Text style={styles.label}>Setting 2</Text>
          <Switch
            value={setting2}
            onValueChange={setSetting2}
            trackColor={{ false: '#D6DBDF', true: '#4A7FFB' }}
            thumbColor="#fff"
          />
        </View>

        <View style={styles.settingRow}>
          <Text style={styles.label}>Setting 3</Text>
          <Switch
            value={setting3}
            onValueChange={setSetting3}
            trackColor={{ false: '#D6DBDF', true: '#4A7FFB' }}
            thumbColor="#fff"
          />
        </View>

        <View style={styles.settingRow}>
          <Text style={styles.label}>Setting 4</Text>
          <Switch
            value={setting4}
            onValueChange={setSetting4}
            trackColor={{ false: '#D6DBDF', true: '#4A7FFB' }}
            thumbColor="#fff"
          />
        </View>
      </View>

      {/* Sliders */}
      <View style={styles.card}>
        <Text style={styles.label}>Volume</Text>
        <Slider
          style={styles.slider}
          value={volume}
          onValueChange={setVolume}
          minimumValue={0}
          maximumValue={1}
          minimumTrackTintColor="#4A7FFB"
          maximumTrackTintColor="#D6DBDF"
        />

        <Text style={[styles.label, { marginTop: 15 }]}>Brightness</Text>
        <Slider
          style={styles.slider}
          value={brightness}
          onValueChange={(val) => {
            setBrightness(val);
            const scaled = Math.round(val * 100); // 0–100 brightness
            const frame = [0x01, 0x03, 0x01, scaled]; // CAT=1, CMD=3
            bleService.sendControlFrame(frame);
          }}
          minimumValue={0}
          maximumValue={1}
          minimumTrackTintColor="#4A7FFB"
          maximumTrackTintColor="#D6DBDF"
        />
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#F9FAFF',
    paddingHorizontal: 25,
    paddingTop: 40,
  },
  header: { 
    fontSize: 28, 
    fontWeight: '700', 
    color: '#2C3E50', 
    marginBottom: 30 
  },
  card: {
    backgroundColor: '#FFFFFF',
    borderRadius: 20,
    padding: 20,
    marginBottom: 20,
    shadowColor: '#000',
    shadowOpacity: 0.1,
    shadowRadius: 6,
    elevation: 3,
  },
  settingRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginVertical: 10,
  },
  label: {
    fontSize: 18,
    color: '#2C3E50',
    fontWeight: '500',
  },
  slider: {
    width: '100%',
    height: 40,
  },
});