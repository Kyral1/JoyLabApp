import React, {useState, useEffect} from "react";
import { View, Text, StyleSheet, Switch,TouchableOpacity, ScrollView, Modal } from 'react-native';
import Slider from '@react-native-community/slider';
import {Picker} from '@react-native-picker/picker';
import { Buffer } from 'buffer';
import { bleService } from '../services/BLEService';
import ColorPicker from 'react-native-wheel-color-picker';
import LinearGradient from 'react-native-linear-gradient'

 export default function SettingsScreen() {
  //BLE send frame function: 
  const sendFrame = async (frame: number[]) => {
    try{
      await bleService.sendControlFrame(frame);
      console.log("Frame sent:", frame);
    } catch (e: any) {
      console.error('Error sending frame:', e?.message ?? String(e));
    }
  };

  //LED 
  const [selectedButton, setSelectedButton] = useState('All');
  const [brightness, setBrightness] = useState(0.5); //Scaled from 0-1 based on slider position
  const [currentColor, setCurrentColor] = useState({ r: 255, g: 255, b: 255 }); //object {r,g,b}
  const [pickerVisible, setPickerVisible] = useState(false);
  const [selectedColor, setSelectedColor] = useState("#FFFFFF");
  const [tempColor, setTempColor] = useState("#FFFFFF")
  console.log("⚙️ SettingsScreen loaded!");

  const openPicker = () => {
    setTempColor(selectedColor)
    setPickerVisible(true);
  };

  //LED Control Handlers
  //Toggle LED On/Off
  const handleToggleButton = (on:boolean) => {
      const idx = selectedButton === "All" ? 111 : parseInt(selectedButton)-1;
      const { r, g, b } = currentColor;
      const br = Math.round(brightness * 100); //slider placement
      const frame = [0x01, 0x01, 0x05, idx, on ? r : 0, on ? g : 0, on ? b : 0, br];
      sendFrame(frame);
  }

  //Change LED Color
  const handleChangeColor = (color: {r: number; g: number; b:number})=>{
      const idx = selectedButton === "All" ? 111 : parseInt(selectedButton)-1;
      const br = Math.round(brightness * 100);
      const frame = [0x01,0x01, 0x05, idx, color.r, color.g, color.b, br];
      sendFrame(frame);
  }

  //Change LED Brightness
  const handleBrightnessChange = (val: number) => {
    setBrightness(val); //updates the state to remember the slider position
    const {r,g,b} = currentColor;
    const idx = selectedButton  === "All" ? 111 : parseInt(selectedButton)-1;
    const br = Math.round(val * 100);
    const frame = [0x01, 0x01, 0x05, idx, r, g, b, br]; // CAT=1, CMD=3
    sendFrame(frame);
  }

  // --- Toggle states (placeholders)
  const [vibration, setVibration] = useState(true);
  const [setting1, setSetting1] = useState(false);
  const [setting2, setSetting2] = useState(false);
  const [setting3, setSetting3] = useState(true);
  const [setting4, setSetting4] = useState(false);

  // --- Slider states
  const [volume, setVolume] = useState(0.6);

  return (
    <ScrollView contentContainerStyle={styles.container}>
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

      <View style={styles.card}>
            <Text style={styles.sectionHeader}>Button Color Customizatioin</Text>

          {/* LED Selector */}
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

            {/* Toggle Buttons */}
            <View style={styles.row}>
                <TouchableOpacity style={styles.button} onPress={() => handleToggleButton(true)}>
                    <Text style={styles.buttonText}>Turn ON</Text>
                </TouchableOpacity>
                <TouchableOpacity style={styles.buttonOff} onPress={() => handleToggleButton(false)}>
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
                        onPress={() => {
                            setCurrentColor(c.rgb);  
                            handleChangeColor(c.rgb); //change color + remember color
                        }} 
                    />
                ))}

                {/* Rainbow Button */}
                <TouchableOpacity onPress={openPicker}>
                    <LinearGradient
                        colors={[
                            "#FF0000",
                            "#FF7F00",
                            "#FFFF00",
                            "#03cc03ff",
                            "#0000FF",
                            "#4B0082",
                            "#8B00FF",
                        ]}
                        start={{ x: 0, y: 0 }}
                        end={{ x: 1, y: 0 }}
                        style={styles.colorBtn}
                    />
                </TouchableOpacity>
            </View>

            <Modal visible={pickerVisible} animationType="slide" transparent>
                <View style={styles.modalBackdrop}>
                    <View style={styles.modalCard}>
                        <Text style={styles.modalTitle}>Pick a Color</Text>

                        <ColorPicker
                            color={tempColor}
                            onColorChangeComplete={(hex) => setTempColor(hex)}
                            thumbSize={28}
                            sliderSize={24}
                            noSnap
                            row={false}
                        />

                        <View style={{ flexDirection: 'row', justifyContent: 'flex-end', gap: 16, marginTop: 14 }}>
                            <TouchableOpacity onPress={() => setPickerVisible(false)}>
                                <Text style={styles.modalBtnText}>Cancel</Text>
                            </TouchableOpacity>

                            <TouchableOpacity
                                onPress={() => {
                                    setSelectedColor(tempColor);
                                    const r = parseInt(tempColor.slice(1,3), 16);
                                    const g = parseInt(tempColor.slice(3,5), 16);
                                    const b = parseInt(tempColor.slice(5,7), 16);
                                    setPickerVisible(false);
                                    setCurrentColor({ r, g, b });
                                    handleChangeColor({ r, g, b });
                                }}
                            >
                                <Text style={[styles.modalBtnText, { fontWeight: '700' }]}>Apply</Text>
                            </TouchableOpacity>
                        </View>
                    </View>
                </View>
            </Modal>


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
    </ScrollView>
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
  row: {
    flexDirection: "row",
    justifyContent: "space-between",
    alignItems: "center",
    marginVertical: 8,
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
  sectionHeader: {
    fontSize: 20,
    fontWeight: '600',
    color: '#34495E',
    marginBottom: 10,
  },
});