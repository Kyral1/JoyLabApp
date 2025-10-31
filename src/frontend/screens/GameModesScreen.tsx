import React from 'react';
import { View, Text, StyleSheet, TouchableOpacity } from 'react-native';
import type { NativeStackScreenProps } from '@react-navigation/native-stack';
import type { GameModesStackParamList } from '../navigation/GameModesStack';

type Props = NativeStackScreenProps<GameModesStackParamList, 'GameModesHome'>;

export default function GameModesScreen({ navigation }: Props) {
  return (
    <View style={styles.container}>
      <Text style={styles.header}>Modes</Text>

      <TouchableOpacity style={styles.modeButton} onPress={() => navigation.navigate('LEDMode')}>
        <Text style={styles.modeText}>LED Mode</Text>
      </TouchableOpacity>

      <TouchableOpacity style={styles.modeButton} onPress={() => navigation.navigate('SoundMode')}>
        <Text style={styles.modeText}>Sound Mode</Text>
      </TouchableOpacity>

      <TouchableOpacity style={styles.modeButton} onPress={() => navigation.navigate('DualMode')}>
        <Text style={styles.modeText}>Dual Mode</Text>
      </TouchableOpacity>
    </View>
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
  modeButton: {
    backgroundColor: '#FFFFFF',
    borderRadius: 20,
    paddingVertical: 20,
    paddingHorizontal: 25,
    marginVertical: 10,
    shadowColor: '#000',
    shadowOpacity: 0.1,
    shadowRadius: 6,
    elevation: 3,
  },
  modeText: { 
    fontSize: 18, 
    fontWeight: '500', 
    color: '#2C3E50' 
  },
});
