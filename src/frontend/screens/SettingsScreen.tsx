import React, {useState} from "react";
import { View, Text, Switch, StyleSheet } from "react-native";
import Slider from '@react-native-community/slider';
import { Buffer } from 'buffer';
import { bleService } from '../services/BLEService'; 

export default function SettingsScreen() {
  console.log("⚙️ SettingsScreen loaded!");
  // --- States for toggles (placeholders for now)
  const [vibration, setVibration] = useState(true);
  const [setting1, setSetting1] = useState(false);
  const [setting2, setSetting2] = useState(false);
  const [setting3, setSetting3] = useState(true);
  const [setting4, setSetting4] = useState(false);

  // --- States for sliders
  const [volume, setVolume] = useState(0.6);
  const [brightness, setBrightness] = useState(0.4);

  return (
    <View style={styles.container}>
      <Text style={styles.header}>Settings</Text>

      {/* Toggles */}
      <View style={styles.settingRow}>
        <Text style={styles.label}>Vibration</Text>
        <Switch
          value={vibration}
          onValueChange={setVibration}
          trackColor={{ false: '#ccc', true: '#4CAF50' }}
          thumbColor="#fff"
        />
      </View>

      <View style={styles.settingRow}>
        <Text style={styles.label}>Setting 1</Text>
        <Switch
          value={setting1}
          onValueChange={setSetting1}
          trackColor={{ false: '#ccc', true: '#4CAF50' }}
          thumbColor="#fff"
        />
      </View>

      <View style={styles.settingRow}>
        <Text style={styles.label}>Setting 2</Text>
        <Switch
          value={setting2}
          onValueChange={setSetting2}
          trackColor={{ false: '#ccc', true: '#4CAF50' }}
          thumbColor="#fff"
        />
      </View>

      <View style={styles.settingRow}>
        <Text style={styles.label}>Setting 3</Text>
        <Switch
          value={setting3}
          onValueChange={setSetting3}
          trackColor={{ false: '#ccc', true: '#4CAF50' }}
          thumbColor="#fff"
        />
      </View>

      <View style={styles.settingRow}>
        <Text style={styles.label}>Setting 4</Text>
        <Switch
          value={setting4}
          onValueChange={setSetting4}
          trackColor={{ false: '#ccc', true: '#4CAF50' }}
          thumbColor="#fff"
        />
      </View>

      {/* Sliders */}
      <View style={styles.sliderContainer}>
        <Text style={styles.label}>Volume</Text>
        <Slider
          style={styles.slider}
          value={volume}
          onValueChange={setVolume}
          minimumValue={0}
          maximumValue={1}
          minimumTrackTintColor="#4CAF50"
          maximumTrackTintColor="#ddd"
        />
      </View>

      <View style={styles.sliderContainer}>
        <Text style={styles.label}>Brightness</Text>
        <Slider
          style={styles.slider}
          value={brightness}
          onValueChange={setBrightness}
          minimumValue={0}
          maximumValue={1}
          minimumTrackTintColor="#4CAF50"
          maximumTrackTintColor="#ddd"
        />
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#fefef5',
    paddingHorizontal: 25,
    paddingTop: 40,
  },
  header: {
    fontSize: 26,
    fontWeight: '600',
    marginBottom: 25,
  },
  settingRow: {
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
    marginVertical: 8,
  },
  label: {
    fontSize: 18,
    color: '#333',
  },
  sliderContainer: {
    marginTop: 25,
  },
  slider: {
    width: '100%',
    height: 40,
  },
});