import React, { useState, useEffect } from "react";
import { View, Text, StyleSheet, ScrollView } from 'react-native';
import { Picker } from '@react-native-picker/picker';
import { Buffer } from 'buffer';
import { bleService } from '../services/BLEService';

export default function SoundModeScreen() {

  // Constants
  const [selectedButton, setSelectedButton] = useState('All');
  type CategoryKey = keyof typeof soundBank;
  const [selectedCategory, setSelectedCategory] = useState<CategoryKey>("animals");
  const [selectedSound, setSelectedSound] = useState("Dog");

  // Soundbank
  const soundBank = {
    animals: {
      id: 0x01,
      label: "Animals",
      sounds: [
        { id: 0x00, label: "Dog" },
        { id: 0x01, label: "Cat" },
        { id: 0x02, label: "Cow" },
      ],
    },
    instruments: {
      id: 0x02,
      label: "Instruments",
      sounds: [
        { id: 0x00, label: "Piano" },
        { id: 0x01, label: "Drum" },
        { id: 0x02, label: "Guitar" },
      ],
    },
    beats: {
      id: 0x03,
      label: "Beats",
      sounds: [
        { id: 0x00, label: "Beat 1" },
        { id: 0x01, label: "Beat 2" },
        { id: 0x02, label: "Beat 3" },
      ],
    },
  } as const;

  const currentCategory =
    soundBank[selectedCategory] ?? soundBank.animals;

  // also guard here
  const soundsForPicker = currentCategory?.sounds ?? [];

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

  const handlePlaySound = () => {
    const idx = selectedButton === "All" ? 111 : parseInt(selectedButton) - 1;
    const sound = currentCategory.sounds.find((s) => s.label.toLowerCase() === selectedSound.toLowerCase());
    const soundId = sound ? sound.id : 0x00;
    const categoryId = currentCategory.id;

    // EXAMPLE FRAME: [SOUND_CAT, SUBCMD, TARGET, CATEGORY, SOUND]
    const frame = [0x02, 0x01, idx, categoryId, soundId];
    sendFrame(frame);
  };

  // If category changes, make sure we don’t hold onto an old sound name
  useEffect(() => {
    // Default to first sound in that category
    setSelectedSound(currentCategory.sounds[0].label.toLowerCase());
  }, [selectedCategory]);


  return (
    <ScrollView contentContainerStyle={styles.container}>
      <View style={styles.card}>
        <Text style={styles.sectionHeader}>Sounds</Text>
        {/* Button Selector */}
        <View style={styles.row}>
          <Text style={styles.label}>Select Button:</Text>
          <Picker
            selectedValue={selectedButton}
            style={styles.picker}
            onValueChange={(val) => setSelectedButton(val)}
          >
            <Picker.Item label="All" value="All" />
            <Picker.Item label="Button 1" value="1" />
            <Picker.Item label="Button 2" value="2" />
            <Picker.Item label="Button 3" value="3" />
            <Picker.Item label="Button 4" value="4" />
          </Picker>
        </View>

        {/* Category selector */}
        <View style={styles.row}>
          <Text style={styles.label}>Category:</Text>
          <Picker
            selectedValue={selectedCategory}
            style={styles.picker}
            onValueChange={(val) => setSelectedCategory(val)}
          >
            {Object.entries(soundBank).map(([key, cat]) => (
              <Picker.Item key={key} label={cat.label} value={key} />
            ))}
          </Picker>
        </View>

        {/* Sound selector */}
        <View style={styles.row}>
          <Text style={styles.label}>Sound:</Text>
          <Picker
            selectedValue={selectedSound}
            style={styles.picker}
            onValueChange={(val) => setSelectedSound(val)}
          >
            {soundsForPicker.map((s) => (
              <Picker.Item key={s.id} label={s.label} value={s.label.toLowerCase()} />
            ))}
          </Picker>
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
