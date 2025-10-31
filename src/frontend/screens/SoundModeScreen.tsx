import React from 'react';
import { View, Text, StyleSheet } from 'react-native';

export default function SoundModeScreen() {
  return (
    <View style={styles.container}>
      <Text style={styles.header}>LED Mode</Text>
      <Text>This is where your LED firmware controls will go.</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: { flex: 1, backgroundColor: '#F9FAFF', padding: 24 },
  header: { fontSize: 22, fontWeight: '700', color: '#2C3E50', marginBottom: 10 },
});
