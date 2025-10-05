import React from 'react';
import { View, Text, StyleSheet } from 'react-native';
console.log("📊 [TOP] StatisticsScreen file loaded");

export default function StatisticsScreen() {
  console.log("📊 StatisticsScreen loaded!");
  return (
    <View style={styles.container}>
      <Text>Stats</Text>
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
