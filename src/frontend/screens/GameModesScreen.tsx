import React from 'react';
import { View, Text, StyleSheet } from 'react-native';
console.log("🎮 [TOP] GameModesScreen file loaded");

export default function GameModesScreen() {
  console.log("🎮 GameModesScreen loaded!");
  return (
    <View style={styles.container}>
      <Text>Modes Page!</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    justifyContent: 'center',
    alignItems: 'center',
  },
});
