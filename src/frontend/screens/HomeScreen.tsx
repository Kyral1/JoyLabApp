import React from 'react';
import { View, Text, StyleSheet } from 'react-native';

export default function GameModesScreen() {
  return (
    <View style={styles.container}>
      <Text>Game Modes will go here!</Text>
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
