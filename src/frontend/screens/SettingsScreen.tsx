import React, {useState, useEffect} from "react";
import { View, Text, StyleSheet, Switch,TouchableOpacity, ScrollView, Modal, Alert, Touchable} from 'react-native';
import Slider from '@react-native-community/slider';
import {Picker} from '@react-native-picker/picker';
import { Buffer } from 'buffer';
import { bleService } from '../services/BLEService';
import ColorPicker from 'react-native-wheel-color-picker';
import LinearGradient from 'react-native-linear-gradient';
import MultiSlider from "@ptomasroos/react-native-multi-slider";
import { bleWristbandService } from '../services/BLEWristBandService';

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

  //connecting wristband
  const [connectingBand, setConnectingBand] = useState(false);
  const [bandConnected, setBandConnected] = useState(false);
  
  const handleConnectBand = async () => {
    try{
      setConnectingBand(true);
        await bleWristbandService.connect();
        setBandConnected(true);
        Alert.alert('BLE', 'Connected to JoyLab WristBand ✅');
    } catch (e: any){
        Alert.alert('BLE Error', e?.message ?? String(e));
    } finally {
        setConnectingBand(false);
    }
  }

  const handleDisconnectBand = async () => {
    try {
      await bleWristbandService.disconnect();
      setBandConnected(false);
      Alert.alert("Wristband", "Disconnected from Wristband ❌");
    } catch (e: any) {
      Alert.alert("Wristband Error", e?.message ?? String(e));
    }
  };

  //LED 
  const [selectedButton, setSelectedButton] = useState('All');
  const [brightness, setBrightness] = useState(0.5); //Scaled from 0-1 based on slider position
  const [currentColor, setCurrentColor] = useState({ r: 255, g: 255, b: 255 }); //object {r,g,b}

  const [currentColor1, setCurrentColor1] = useState({ r: 255, g: 255, b: 255 });
  const [currentColor2, setCurrentColor2] = useState({ r: 255, g: 255, b: 255 });
  const [currentColor3, setCurrentColor3] = useState({ r: 255, g: 255, b: 255 });
  const [currentColor4, setCurrentColor4] = useState({ r: 255, g: 255, b: 255 });

  const getButtonColor = () => {
    switch (selectedButton) {
      case "1": return currentColor1;
      case "2": return currentColor2;
      case "3": return currentColor3;
      case "4": return currentColor4;
    default: return currentColor; // for "All"
    }
  };

const setButtonColor = (color: { r: number; g: number; b: number }) => {
  switch (selectedButton) {
    case "1": setCurrentColor1(color); break;
    case "2": setCurrentColor2(color); break;
    case "3": setCurrentColor3(color); break;
    case "4": setCurrentColor4(color); break;
    default: setCurrentColor(color); break; // for "All"
  }
};

//turning all buttons to clear when the screen first loads: 
useEffect(() => {
  // Reset local states
  setCurrentColor({ r: 0, g: 0, b: 0 });
  setCurrentColor1({ r: 0, g: 0, b: 0 });
  setCurrentColor2({ r: 0, g: 0, b: 0 });
  setCurrentColor3({ r: 0, g: 0, b: 0 });
  setCurrentColor4({ r: 0, g: 0, b: 0 });
  setBrightness(0);

  // Send BLE frame to actually turn off all LEDs
  const frame = [0x01, 0x01, 0x05, 111, 0, 0, 0, 0];
  sendFrame(frame);
}, []); 

  const [pickerVisible, setPickerVisible] = useState(false);
  const [selectedColor, setSelectedColor] = useState("#FFFFFF");
  const [tempColor, setTempColor] = useState("#FFFFFF")
  const [audio, setAudio] = useState(false);
  console.log("⚙️ SettingsScreen loaded!");

  const openPicker = () => {
    setTempColor(selectedColor)
    setPickerVisible(true);
  };
  
  //LED Control Handlers
  //Toggle LED On/Off
  const handleToggleButton = (on:boolean) => {
      const idx = selectedButton === "All" ? 111 : parseInt(selectedButton)-1;
      const { r, g, b } = getButtonColor();
      const br = Math.round(brightness * 100); //slider placement
      const frame = [0x01, 0x01, 0x05, idx, on ? r : 0, on ? g : 0, on ? b : 0, br];
      sendFrame(frame);
  }

  //Change LED Color
  const handleChangeColor = (color: { r: number; g: number; b: number })=>{
    setButtonColor(color);
    const idx = selectedButton === "All" ? 111 : parseInt(selectedButton)-1;
    const br = Math.round(brightness * 100);
    const frame = [0x01,0x01, 0x05, idx, color.r, color.g, color.b, br];
    sendFrame(frame);
  }

  //Change LED Brightness
  const handleBrightnessChange = (val: number) => {
    setBrightness(val); //updates the state to remember the slider position
    const { r, g, b } = getButtonColor();
    const idx = selectedButton === "All" ? 111 : parseInt(selectedButton) - 1;
    const br = Math.round(val * 100);
    const frame = [0x01, 0x01, 0x05, idx, r, g, b, br];
    sendFrame(frame);
  }

  // --- Slider states
  const [volume, setVolume] = useState(0.6);

  //audio handlers
  const [selectedSoundButton, setSelectedSoundButton] = useState('test.mp3');
  const [currentSound, setCurrentSound]= useState('test.mp3');
  const [currentSound1, setCurrentSound1] = useState('test.mp3');
  const [currentSound2, setCurrentSound2] = useState('test.mp3');
  const [currentSound3, setCurrentSound3] = useState('test.mp3');
  const [currentSound4, setCurrentSound4] = useState('test.mp3');

  const getButtonSound = () => {
    switch (selectedSoundButton) {
      case "1": return currentSound1;
      case "2": return currentSound2;
      case "3": return currentSound3;
      case "4": return currentSound4;
    default: return currentSound; // for "All"
    }
  };

  const setButtonSound = (sound: string) => {
    switch (selectedSoundButton) {
      case "1": setCurrentSound1(sound); break;
      case "2": setCurrentSound2(sound); break;
      case "3": setCurrentSound3(sound); break;
      case "4": setCurrentSound4(sound); break;
    default: setCurrentSound(sound); break; // for "All"
    }
  };

  const handleSoundChange = (sound: string) => {
    setButtonSound(sound);
    const idx = selectedSoundButton === "All" ? 111 : parseInt(selectedSoundButton)-1;
    const frame = [0x02, 0x03, 0x02, sound, idx];
  };

  //audio handlers
      const audioToggle = (on: boolean) => {
      setAudio(on);
      const frame = [0x02, 0x01, 0x01, on? 1:0];
      sendFrame(frame);
    }

    //Audio Volume Handler
    const handleVolumeChange = (val: number) => {
      setVolume(val);
      const scaled = Math.round(val * 100);
      const frame = [0x02, 0x02, 0x01, scaled];
      sendFrame(frame);
    };

    type CategoryKey = keyof typeof soundBank;
    const [selectedCategory, setSelectedCategory] = useState<CategoryKey>("animals");
    const [selectedSound, setSelectedSound] = useState("Sound 1");

  const soundBank = {
    animals: {
      id: 0x01,
      label: "Animals",
      sounds: [
        { id: 0x00, label: "HappyNoise.wav" }
      ],
    },
    instruments: {
      id: 0x02,
      label: "Instruments",
      sounds: [
        { id: 0x00, label: "HappyNoise.wav"}
      ],
    },
    beats: {
      id: 0x03,
      label: "Beats",
      sounds: [
        { id: 0x00, label: "HappyNoise.wav" }
      ],
    },
  } as const;

  const handlePlayPreview = () => {
    const idx = selectedSoundButton === "All" ? 111 : parseInt(selectedSoundButton)-1;
    const frame = [0x02, 0x04, 0x01, idx];
    sendFrame(frame);
  }

    //sensory pad settings 
    const [sensoryMode, setSensoryMode] = useState(0); //0=off,1=no vib,2=constant vib, 3=increasing vib
    const [constantIntensity, setConstantIntensity] = useState(50);
    const [minIntensity, setMinIntensity] = useState(10);
    const [maxIntensity, setMaxIntensity] = useState(80);

    const sendModeFrame = (m: number) => {
      setSensoryMode(m);
      switch(m){
        case 1:
          sendFrame([0x08, 0x01, 0x00]);
          break;
        case 2:
          sendFrame([0x08, 0x02, 0x01, constantIntensity]);
          break;
        case 3:
          sendFrame([0x08, 0x03, 0x02, minIntensity, maxIntensity]);
          break;
      }
    }

    const sendConstantIntensity = (value: number) => {
      setConstantIntensity(value);
      sendFrame([0x08, 0x02, 0x01, value]);
    }

    const sendIncreasingValues = (min: number, max: number) => {
      setMinIntensity(min);
      setMaxIntensity(max);
      sendFrame([0x08, 0x03, 0x02, min, max]);
    }


  return (
    <ScrollView contentContainerStyle={styles.container}>
      <Text style={styles.header}>Settings</Text>

      <View style={styles.card}>
        <Text style={styles.sectionHeader}>Wristband Connection</Text>

        <View style={styles.bandRow}>
        <View>
          <Text style={styles.subLabel}>
            {bandConnected ? "Connected" : "Not Connected"}
          </Text>
        </View>

        {/* Connect Button */}
        {!bandConnected && (
          <TouchableOpacity
            style={[
              styles.bandBtn,
              connectingBand && { backgroundColor: "#AAB7B8" }
            ]}
            onPress={handleConnectBand}
            disabled={connectingBand}
          >
          <Text style={styles.bandBtnText}>
            {connectingBand ? "Connecting..." : "Connect"}
          </Text>
          </TouchableOpacity>
        )}

        {/* Disconnect Button */}
        {bandConnected && (
          <TouchableOpacity
            style={[styles.bandBtnDisconnect]}
            onPress={handleDisconnectBand}
          >
          <Text style={styles.bandBtnDisconnectText}>Disconnect</Text>
          </TouchableOpacity>
        )}
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
                            /*setCurrentColor(c.rgb);  */
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

        <View style={styles.card}>
              <Text style={styles.sectionHeader}>Audio Control</Text>
              <View style={styles.row}>
                <Text style={styles.label}>Audio</Text>
                <Switch
                  value={audio}
                  onValueChange={audioToggle}
                  trackColor={{ false: '#D6DBDF', true: '#4A7FFB' }}
                  thumbColor="#fff"
                />
              </View>
              <Text style={[styles.label, { marginTop: 20 }]}>Volume</Text>
              <Slider
                style={styles.slider}
                value={volume}
                onValueChange={handleVolumeChange}
                minimumValue={0}
                maximumValue={1}
                minimumTrackTintColor="#4A7FFB"
                maximumTrackTintColor="#D6DBDF"
              />
        </View>

        {/* ---------------- SOUND SELECTION CARD ---------------- */}
        <View style={styles.card}>
          <Text style={styles.sectionHeader}>Button Sound Selection</Text>

          {/* Sound Selector */}
          <View style={styles.row}>
            <Text style={styles.label}>Select Button:</Text>
            <Picker
              selectedValue={selectedSoundButton}
              style={styles.picker}
              onValueChange={(val) => setSelectedSoundButton(val)}
            >
              <Picker.Item label="All" value="All" />
              <Picker.Item label="Button 1" value="1" />
              <Picker.Item label="Button 2" value="2" />
              <Picker.Item label="Button 3" value="3" />
              <Picker.Item label="Button 4" value="4" />
            </Picker>
          </View>

          {/* Sound Options */}
          <View style={styles.row}>
            <Text style={styles.label}>Categories:</Text>
            <Picker
              selectedValue={selectedCategory}
              style={styles.picker}
              onValueChange={(val) => {
                setSelectedCategory(val as CategoryKey); 
                setSelectedSound(soundBank[val as CategoryKey].sounds[0].label);
              }}
            >
              {Object.entries(soundBank).map(([key, cat]) => (
                <Picker.Item key={key} label={cat.label} value={key} />
              ))}
            </Picker>
            </View>
          
            {/* Sound Picker */}
            <View style={styles.row}>
              <Text style={styles.label}>Sound:</Text>
              <Picker
                selectedValue={selectedSound}
                style={styles.picker}
                onValueChange={(val) => {
                  setSelectedSound(val);
                  handleSoundChange(val);
                }}
              >
                {soundBank[selectedCategory].sounds.map((s) => (
                  <Picker.Item key={s.id} label={s.label} value={s.label} />
                ))}
              </Picker>
            </View>
            <TouchableOpacity style={styles.previewBtn} onPress={handlePlayPreview}>
              <Text style={styles.buttonText}>Play Sound</Text>
            </TouchableOpacity>
        </View>

        {/* ---------------- SENSORY PAD CARD ---------------- */}
                        
        <View style={styles.card}>
          <Text style={styles.sectionHeader}>Sensory Pad</Text>
            {/* Mode Selector */}
            <View style={styles.modeContainer}>
              {[
                { id: 1, label: "Vibration OFF" },
                { id: 2, label: "Constant Vib" },
                { id: 3, label: "Increasing Vib" },
              ].map((option) => (
              <TouchableOpacity
              key={option.id}
              style={[
                styles.modeBtn,
                sensoryMode === option.id && styles.modeBtnActive,
              ]}
              onPress={() => sendModeFrame(option.id)}
            >
              <Text
                style={[
                  styles.modeText,
                  sensoryMode === option.id && styles.modeTextActive,
                ]}
            >
            {option.label}
          </Text>
              </TouchableOpacity>
          ))}
        </View>

        {/* Conditional UI for Sliders */}
        {sensoryMode === 2 && (
        <>
          <Text style={styles.label}>Intensity</Text>
          <Slider
            style={styles.slider}
            value={constantIntensity}
            onValueChange={(v) => sendConstantIntensity(Math.round(v))}
            minimumValue={0}
            maximumValue={100}
            minimumTrackTintColor="#4A7FFB"
            maximumTrackTintColor="#D6DBDF"
          />
        </>
      )}

      {sensoryMode === 3 && (
        <>
          <Text style={styles.label}>Intensity Ranges</Text>
          <MultiSlider
            values={[minIntensity, maxIntensity]}
            min={0}
            max={100}
            step={1}
            sliderLength={300}
            onValuesChange={(vals) => {
              const [min, max] = vals.map(v => Math.round(v));
              setMinIntensity(min);
              setMaxIntensity(max);
              sendIncreasingValues(min, max);
            }}
            selectedStyle={{ backgroundColor: "#4A7FFB" }}
            unselectedStyle={{ backgroundColor: "#D6DBDF" }}
            markerStyle={{
            height: 24,
            width: 24,
            borderRadius: 12,
            backgroundColor: "#4A7FFB",
            }}
            containerStyle={{ alignSelf: "center", marginTop: 20 }}
            trackStyle={{ height: 6, borderRadius: 3 }}
          />
          
          <View style={{ flexDirection: "row", justifyContent: "space-between" }}>
            <Text>{minIntensity}%</Text>
            <Text>{maxIntensity}%</Text>
          </View>
        </>
      )}
        </View>
    </ScrollView>
  );
}

const styles = StyleSheet.create({
  container: {
    flexGrow: 1,
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

  modeContainer: {
  flexDirection: "row",
  justifyContent: "space-between",
  marginVertical: 10,
},
modeBtn: {
  flex: 1,
  paddingVertical: 10,
  backgroundColor: "#EAECEE",
  borderRadius: 12,
  marginHorizontal: 4,
  alignItems: "center",
},
modeBtnActive: {
  backgroundColor: "#4A7FFB",
},
modeText: {
  fontSize: 14,
  color: "#34495E",
  fontWeight: "500",
},
modeTextActive: {
  color: "#FFFFFF",
  fontWeight: "600",
},
bandRow: {
  flexDirection: "row",
  justifyContent: "space-between",
  alignItems: "center",
  marginTop: 10,
},

subLabel: {
  fontSize: 14,
  color: "#7F8C8D",
  marginTop: 4,
},

bandBtn: {
  backgroundColor: "#4A7FFB",
  paddingVertical: 10,
  paddingHorizontal: 18,
  borderRadius: 12,
},

bandBtnText: {
  color: "#fff",
  fontSize: 16,
  fontWeight: "600",
},

bandBtnDisconnect: {
  backgroundColor: "#E74C3C",
  paddingVertical: 10,
  paddingHorizontal: 18,
  borderRadius: 12,
},

bandBtnDisconnectText: {
  color: "#fff",
  fontSize: 16,
  fontWeight: "600",
},

soundRow: {
  flexDirection: "row",
  flexWrap: "wrap",
  justifyContent: "space-around",
  marginTop: 10,
},

soundBtn: {
  backgroundColor: "#ECF0F1",
  paddingVertical: 10,
  paddingHorizontal: 14,
  borderRadius: 12,
  margin: 5,
},

soundBtnActive: {
  backgroundColor: "#4A7FFB",
},

soundBtnText: {
  fontSize: 14,
  color: "#2C3E50",
  fontWeight: "500",
},

soundBtnTextActive: {
  color: "#FFFFFF",
  fontWeight: "700",
},

previewBtn: {
  marginTop: 20,
  backgroundColor: "#C9D6FF",
  paddingVertical: 12,
  borderRadius: 12,
  alignItems: "center",
},

previewBtnText: {
  color: "#2C3E50",
  fontWeight: "600",
  fontSize: 16,
},

});